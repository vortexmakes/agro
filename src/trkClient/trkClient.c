/**
 *  \file       trkClient.c
 *  \brief      Yipies Tracking Client.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.06.05  DaBa  v1.0.00   Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Balina db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include <string.h>
#include "rkh.h"
#include "rkhfwk_pubsub.h"
#include "rkhtmr.h"
#include "bsp.h"
#include "signals.h"
#include "events.h"
#include "topics.h"
#include "trkClient.h"
#include "epoch.h"
#include "date.h"
#include "conMgr.h"

/* ----------------------------- Local macros ------------------------------ */
#define SEND_TIME    RKH_TIME_MS(5000)
#define RECV_TIME    RKH_TIME_MS(5000)
#define TEST_FRAME \
    "!0|12359094043105600,120000,-38.0050660,-057.5443696," \
    "000.000,000,050514,00FF,0000,00,00,FFFF,FFFF,FFFF,+0"

#define TEST_FRAME_FLAGS         "12"
#define TEST_FRAME_HEADER        "!0|"
#define TEST_MULTIFRAME_QTY      5
#define TEST_MULTIFRAME_HEADER   "!1|"
#define TEST_FRAME_TAIL          "1"
#define MULTIFRAME_COUNT         2

/*#define TEST_FRAME_TAIL     "00FF,0000,00,00,FFFF,FFFF,FFFF,+0" */

/* ......................... Declares active object ........................ */
typedef struct TrkClient TrkClient;

/* ................... Declares states and pseudostates .................... */
RKH_DCLR_BASIC_STATE Client_Disconnected, Client_Send, Client_Receive,
                     Client_Idle;
RKH_DCLR_COMP_STATE Client_Connected;
RKH_DCLR_COND_STATE Client_CheckResp;

/* ........................ Declares initial action ........................ */
static void init(TrkClient *const me, RKH_EVT_T *pe);

/* ........................ Declares effect actions ........................ */
static void updateGeoStamp(TrkClient *const me, RKH_EVT_T *pe);
static void sendIo(TrkClient *const me, RKH_EVT_T *pe);
static void sendTime(TrkClient *const me, RKH_EVT_T *pe);
static void sendSensor(TrkClient *const me, RKH_EVT_T *pe);
static void sendOk(TrkClient *const me, RKH_EVT_T *pe);
static void sendFail(TrkClient *const me, RKH_EVT_T *pe);
static void doRecv(TrkClient *const me, RKH_EVT_T *pe);

/* ......................... Declares entry actions ........................ */
static void idleEntry(TrkClient *const me);
static void recvEntry(TrkClient *const me);
static void recvFail(TrkClient *const me);

/* ......................... Declares exit actions ......................... */
static void idleExit(TrkClient *const me);

/* ............................ Declares guards ............................ */
rbool_t checkResp(TrkClient *const me, RKH_EVT_T *pe);

/* ........................ States and pseudostates ........................ */
RKH_CREATE_BASIC_STATE(Client_Disconnected, NULL, NULL, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(Client_Disconnected)
RKH_TRREG(evNetConnected, NULL, NULL, &Client_Connected),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(Client_Connected, NULL, NULL, RKH_ROOT,
                             &Client_Idle, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(Client_Connected)
RKH_TRINT(evGeoStamp,        NULL, updateGeoStamp),
RKH_TRINT(evGeoStampInvalid, NULL, NULL),
RKH_TRREG(evNetDisconnected, NULL, NULL, &Client_Disconnected),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Client_Idle, idleEntry, idleExit, &Client_Connected,
                       NULL);
RKH_CREATE_TRANS_TABLE(Client_Idle)
RKH_TRREG(evTimeout,       NULL, sendTime, &Client_Send),
/*RKH_TRREG(evSensorData,    NULL, sendSensor, &Client_Send), */
RKH_TRREG(evIoChg,         NULL, sendIo, &Client_Send),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Client_Send, NULL, NULL, &Client_Connected, NULL);
RKH_CREATE_TRANS_TABLE(Client_Send)
RKH_TRREG(evSent,     NULL, sendOk, &Client_Receive),
RKH_TRREG(evSendFail, NULL, sendFail, &Client_Idle),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Client_Receive, recvEntry, NULL, &Client_Connected,
                       NULL);
RKH_CREATE_TRANS_TABLE(Client_Receive)
RKH_TRINT(evTimeout,  NULL, doRecv),
RKH_TRREG(evReceived, NULL, NULL, &Client_CheckResp),
RKH_TRREG(evRecvFail, NULL, recvFail, &Client_Idle),
RKH_END_TRANS_TABLE

RKH_CREATE_COND_STATE(Client_CheckResp);
RKH_CREATE_BRANCH_TABLE(Client_CheckResp)
RKH_BRANCH(checkResp,   NULL,   &Client_Idle),
RKH_BRANCH(ELSE,        NULL,   &Client_Receive),
RKH_END_BRANCH_TABLE

/* ............................. Active object ............................. */
struct TrkClient
{
    RKH_SMA_T ao;       /* Base structure */
    RKH_TMR_T timer;
    GeoStamp geo;
};

RKH_SMA_CREATE(TrkClient, trkClient, 6, HCAL, &Client_Disconnected, init, NULL);
RKH_SMA_DEF_PTR(trkClient);

/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static RKH_STATIC_EVENT(e_tout, evTimeout);
static RKH_ROM_STATIC_EVENT(evRecvObj, evRecv);
static SendEvt evSendObj;
static char *testFrame = TEST_FRAME;
static ruint din;
static ruint dout;
static CBOX_STR cbox;
static ruint cntSingle;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ............................ Initial action ............................. */
static void
init(TrkClient *const me, RKH_EVT_T *pe)
{
    (void)pe;

    ConnectionTopic_subscribe(me);
    tpGeo_subscribe(me);
    tpIoChg_subscribe(me);
    tpSensor_subscribe(me);

    RKH_TR_FWK_AO(me);
    RKH_TR_FWK_TIMER(&me->timer);

    RKH_TR_FWK_QUEUE(&RKH_UPCAST(RKH_SMA_T, me)->equeue);
    RKH_TR_FWK_STATE(me, &Client_Disconnected);
    RKH_TR_FWK_STATE(me, &Client_Connected);
    RKH_TR_FWK_STATE(me, &Client_Idle);
    RKH_TR_FWK_STATE(me, &Client_Send);
    RKH_TR_FWK_STATE(me, &Client_Receive);

    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &evSendObj), evSend);
    RKH_TMR_INIT(&me->timer, &e_tout, NULL);

    cntSingle = MULTIFRAME_COUNT;
}

/* ............................ Effect actions ............................. */
static void
updateGeoStamp(TrkClient *const me, RKH_EVT_T *pe)
{
    me->geo = (((GeoStampEvt *)pe)->position);
}

static void
sendFrame(TrkClient *const me)
{
    char buff[20];

    char *p;

    p = (char *)evSendObj.buf;

    sprintf(p, "%s", TEST_FRAME_HEADER);
    strcat(p, TEST_FRAME_FLAGS);
    strcat(p, ConMgr_Imei());
    strcat(p, ",");
    strcat(p, me->geo.utc);
    strcat(p, ",");
    strcat(p, me->geo.latInd);
    strcat(p, me->geo.latitude);
    strcat(p, ",");
    strcat(p, me->geo.longInd);
    strcat(p, me->geo.longitude);
    strcat(p, ",");
    strcat(p, me->geo.speed);
    strcat(p, ",");
    strcat(p, me->geo.course);
    strcat(p, ",");
    strcat(p, me->geo.date);
    strcat(p, ",");

    sprintf(buff, "%02x%02x,", dout, din);
    strcat(p, buff);

    sprintf(buff, "%04x,", cbox.h.hoard);
    strcat(p, buff);

    sprintf(buff, "%02x,", cbox.h.pqty);
    strcat(p, buff);

    sprintf(buff, "%02x,", cbox.hum);
    strcat(p, buff);

    sprintf(buff, "%04x,", cbox.a.x);
    strcat(p, buff);
    sprintf(buff, "%04x,", cbox.a.y);
    strcat(p, buff);
    sprintf(buff, "%04x,", cbox.a.z);
    strcat(p, buff);

    strcat(p, TEST_FRAME_TAIL);

    evSendObj.size = strlen((char *)evSendObj.buf);
    ConnectionTopic_publish(&evSendObj, me);
}

static void
sendMultiFrame(TrkClient *const me)
{
    char buff[10];
    char i;

    char *p;

    p = (char *)evSendObj.buf;

    sprintf(p, "%s%04x|", TEST_MULTIFRAME_HEADER, TEST_MULTIFRAME_QTY);
    strcat(p, ConMgr_Imei());
    for (i = 0; i < TEST_MULTIFRAME_QTY; ++i)
    {
        strcat(p, "|");
        strcat(p, TEST_FRAME_FLAGS);
        strcat(p, ",");
        strcat(p, me->geo.utc);
        strcat(p, ",");
        strcat(p, me->geo.latInd);
        strcat(p, me->geo.latitude);
        strcat(p, ",");
        strcat(p, me->geo.longInd);
        strcat(p, me->geo.longitude);
        strcat(p, ",");
        strcat(p, me->geo.speed);
        strcat(p, ",");
        strcat(p, me->geo.course);
        strcat(p, ",");
        strcat(p, me->geo.date);
        strcat(p, ",");

        sprintf(buff, "%02x%02x,", dout, din);
        strcat(p, buff);

        sprintf(buff, "%04x,", cbox.h.hoard);
        strcat(p, buff);

        sprintf(buff, "%02x,", cbox.h.pqty);
        strcat(p, buff);

        sprintf(buff, "%02x,", cbox.hum);
        strcat(p, buff);

        sprintf(buff, "%04x,", cbox.a.x);
        strcat(p, buff);
        sprintf(buff, "%04x,", cbox.a.y);
        strcat(p, buff);
        sprintf(buff, "%04x,", cbox.a.z);
        strcat(p, buff);

        strcat(p, TEST_FRAME_TAIL);
    }

    strcat(p, "#");

    evSendObj.size = strlen((char *)evSendObj.buf);
    ConnectionTopic_publish(&evSendObj, me);
}

static void
sendIo(TrkClient *const me, RKH_EVT_T *pe)
{
    IoChgEvt *pio;

    pio = RKH_DOWNCAST(IoChgEvt, pe);
    din = pio->din;

    sendFrame(me);
}

static void
sendTime(TrkClient *const me, RKH_EVT_T *pe)
{
    (void)pe;

    if (cntSingle && (--cntSingle == 0))
    {
        cntSingle = MULTIFRAME_COUNT;
        sendMultiFrame(me);
    }
    else
    {
        sendFrame(me);
    }
}

static void
sendSensor(TrkClient *const me, RKH_EVT_T *pe)
{
    SensorData *ps;

    ps = RKH_DOWNCAST(SensorData, pe);
    cbox = ps->cbox;

    sendFrame(me);
}

static void
sendOk(TrkClient *const me, RKH_EVT_T *pe)
{
    (void)me;

    bsp_sendOk();
}

static void
sendFail(TrkClient *const me, RKH_EVT_T *pe)
{
    (void)me;

    bsp_sendFail();
}

static void
doRecv(TrkClient *const me, RKH_EVT_T *pe)
{
    (void)pe;

    ConnectionTopic_publish(&evRecvObj, me);
}

/* ............................. Entry actions ............................. */
static void
idleEntry(TrkClient *const me)
{
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), SEND_TIME);
}

static void
recvEntry(TrkClient *const me)
{
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), RECV_TIME);
}

static void
recvFail(TrkClient *const me)
{
    (void)me;

    bsp_recvFail();
}

/* ............................. Exit actions .............................. */
static void
idleExit(TrkClient *const me)
{
    rkh_tmr_stop(&me->timer);
}

/* ................................ Guards ................................. */
rbool_t
checkResp(TrkClient *const me, RKH_EVT_T *pe)
{
    ReceivedEvt *evt;

    (void)me;

    evt = RKH_DOWNCAST(ReceivedEvt, pe);

    return (evt->size != 0) ? RKH_TRUE : RKH_FALSE;
}

/* ---------------------------- Global functions --------------------------- */
/* ------------------------------ End of file ------------------------------ */
