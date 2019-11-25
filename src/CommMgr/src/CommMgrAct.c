/**
 *  \file       CommMgr.c
 *  \brief      Active object's action implementations.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "rkhtmr.h"
#include "signals.h"
#include "events.h"
#include "CommMgr.h"
#include "CommMgrAct.h"
#include "CommMgrActRequired.h"
#include "YFrame.h"
#include "topic.h"

/* ----------------------------- Local macros ------------------------------ */
#define WaitTime0	RKH_TIME_SEC(60)

/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static SendEvt evSendObj;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ............................ Effect actions ............................. */
void 
CommMgr_ToIdleExt0(CommMgr *const me, RKH_EVT_T *pe)
{
	/*init();*/
	RKH_TR_FWK_AO(me);
	RKH_TR_FWK_QUEUE(&RKH_UPCAST(RKH_SMA_T, me)->equeue);
	RKH_TR_FWK_STATE(me, &Idle);
	RKH_TR_FWK_STATE(me, &Active);
	RKH_TR_FWK_STATE(me, &WaitSync);
	RKH_TR_FWK_STATE(me, &Current);
	RKH_TR_FWK_STATE(me, &SendingStatus);
	RKH_TR_FWK_STATE(me, &ReceivingStatusAck);
	RKH_TR_FWK_STATE(me, &History);
	RKH_TR_FWK_STATE(me, &SendingEndOfHist);
	RKH_TR_FWK_STATE(me, &SendingHist);
	RKH_TR_FWK_STATE(me, &ReceivingMsgAck);
	RKH_TR_FWK_SIG(evNetConnected);
	RKH_TR_FWK_SIG(evNetDisconnected);
	RKH_TR_FWK_SIG(evSent);
	RKH_TR_FWK_SIG(evReceived);
	RKH_TR_FWK_SIG(evRecvFail);
	RKH_TR_FWK_SIG(evSendFail);
	RKH_TR_FWK_SIG(evGStatus);
	RKH_TR_FWK_TIMER(&me->tmEvtObj0.tmr);
	
    topic_subscribe(status, RKH_UPCAST(RKH_SMA_T, me));
    me->isPendingStatus = 0;
    RKH_SET_STATIC_EVENT(&evSendObj, evSend);
}

void 
CommMgr_IdleToActiveExt1(CommMgr *const me, RKH_EVT_T *pe)
{
	/*activateSync();*/
}

void 
CommMgr_CurrentToWaitSyncExt5(CommMgr *const me, RKH_EVT_T *pe)
{
	/*sendMsgFail();*/
}

void 
CommMgr_CurrentToWaitSyncExt6(CommMgr *const me, RKH_EVT_T *pe)
{
	/*recvFail();*/
}

void 
CommMgr_ReceivingStatusAckToC0Ext9(CommMgr *const me, RKH_EVT_T *pe)
{
	/*parseRecv();*/
}

void 
CommMgr_C0ToCurrentFinalExt12(CommMgr *const me, RKH_EVT_T *pe)
{
	/*parseError();*/
}

void 
CommMgr_HistoryToWaitSyncExt13(CommMgr *const me, RKH_EVT_T *pe)
{
	/*recvFail();*/
}

void 
CommMgr_HistoryToWaitSyncExt14(CommMgr *const me, RKH_EVT_T *pe)
{
	/*sendMsgFail();*/
}

void 
CommMgr_ToC1Ext16(CommMgr *const me, RKH_EVT_T *pe)
{
	/*checkHist();*/
}

void 
CommMgr_SendingHistToC2Ext18(CommMgr *const me, RKH_EVT_T *pe)
{
	/*nextSend();*/
}

void 
CommMgr_ReceivingMsgAckToC3Ext19(CommMgr *const me, RKH_EVT_T *pe)
{
	/*parseRecv();*/
}

void 
CommMgr_C1ToSendingHistExt20(CommMgr *const me, RKH_EVT_T *pe)
{
	/*sendStartOfHist();*/
}

void 
CommMgr_C3ToHistoryFinalExt24(CommMgr *const me, RKH_EVT_T *pe)
{
	/*parseError();*/
}

void 
CommMgr_C4ToC1Ext27(CommMgr *const me, RKH_EVT_T *pe)
{
	/*checkHist();*/
}

void 
CommMgr_ActiveToActiveLoc0(CommMgr *const me, RKH_EVT_T *pe)
{
    GStatusEvt *realEvt; 

	/*updateStatus();*/
    realEvt = RKH_DOWNCAST(GStatusEvt, pe);
    me->status = realEvt->status;
    me->isPendingStatus = 1;
}

/* ............................. Entry actions ............................. */
void 
CommMgr_enWaitSync(CommMgr *const me)
{
	RKH_SET_STATIC_EVENT(&me->tmEvtObj0, evTout0);
	RKH_TMR_INIT(&me->tmEvtObj0.tmr, RKH_UPCAST(RKH_EVT_T, &me->tmEvtObj0), NULL);
	RKH_TMR_ONESHOT(&me->tmEvtObj0.tmr, RKH_UPCAST(RKH_SMA_T, me), WaitTime0);
}

void 
CommMgr_enSendingStatus(CommMgr *const me)
{
	/*sendStatus();*/
    ruint len;

    len = YFrame_header(&me->status, evSendObj.buf, 0, YFRAME_SGP_TYPE);
    YFrame_data(&me->status, &evSendObj.buf[len], YFRAME_SGP_TYPE);
    topic_publish(tcpConnection, 
                  RKH_UPCAST(RKH_EVT_T, &evSendObj), 
                  RKH_UPCAST(RKH_SMA_T, me));
}

void 
CommMgr_enReceivingStatusAck(CommMgr *const me)
{
	/*receive();*/
}

void 
CommMgr_enSendingEndOfHist(CommMgr *const me)
{
	/*sendEndOfHist();*/
}

void 
CommMgr_enSendingHist(CommMgr *const me)
{
	/*sendOneMsg();*/
}

void 
CommMgr_enReceivingMsgAck(CommMgr *const me)
{
	/*receive();*/
}

/* ............................. Exit actions .............................. */
void 
CommMgr_exWaitSync(CommMgr *const me)
{
	rkh_tmr_stop(&me->tmEvtObj0.tmr);
}

/* ................................ Guards ................................. */
rbool_t 
CommMgr_isCondC0ToHistory11(CommMgr *const me, RKH_EVT_T *pe)
{
	/*return (isAck()) ? true : false;*/
}

rbool_t 
CommMgr_isCondC1ToSendingHist20(CommMgr *const me, RKH_EVT_T *pe)
{
	/*return (isThereMsg()) ? true : false;*/
}

rbool_t 
CommMgr_isCondC2ToSendingEndOfHist23(CommMgr *const me, RKH_EVT_T *pe)
{
	/*return (isEndOfBlock()) ? true : false;*/
}

rbool_t 
CommMgr_isCondC3ToC425(CommMgr *const me, RKH_EVT_T *pe)
{
	/*return (isAck()) ? true : false;*/
}

rbool_t 
CommMgr_isCondC4ToCurrent26(CommMgr *const me, RKH_EVT_T *pe)
{
	/*return (isPending()) ? true : false;*/
}

/* ---------------------------- Global functions --------------------------- */
/* ------------------------------ End of file ------------------------------ */