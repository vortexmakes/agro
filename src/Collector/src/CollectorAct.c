/**
 *  \file       CollectorAct.c
 *  \brief      Active object's action implementations.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkhsma.h"
#include "rkhsm.h"
#include "rkhfwk_cast.h"
#include "Collector.h"
#include "CollectorAct.h"
#include "CollectorActRequired.h"
#include "topic.h"
#include "signals.h"
#include "events.h"
#include "rkhtmr.h"
#include "rkhfwk_dynevt.h"

RKH_MODULE_NAME(CollectorAct)

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
RKH_DCLR_SM_CONST_GLOBAL(mapping);
RKH_SM_T *collectorMapping;

/* ---------------------------- Local variables ---------------------------- */
static RKH_EVT_T evMappingObj;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static void 
dispatch(RKH_SMA_T *me, void *arg)
{
    Mapping *region;
    
    region = &(RKH_DOWNCAST(Collector, me)->itsMapping);
    rkh_sm_dispatch((RKH_SM_T *)me, (RKH_EVT_T *)arg);
    rkh_sm_dispatch(RKH_UPCAST(RKH_SM_T, region), (RKH_EVT_T *)arg);
}

bool
testDevNullJobCond(Collector *const me)
{
    return (me->status.ioStatus.digIn != 0xff) ? true : false;
}

/* ---------------------------- Global functions --------------------------- */
void 
Collector_ctor(void)
{
    Collector *me;

    me = RKH_DOWNCAST(Collector, collector);
    me->vtbl = rkhSmaVtbl;
    me->vtbl.task = dispatch;
    me->status.ioStatus.digIn = 0xff;
    me->status.ioStatus.digOut = 0;
    rkh_sma_ctor(RKH_UPCAST(RKH_SMA_T, me), &me->vtbl);

    me->itsMapping.itsCollector = me;
    RKH_SM_INIT(&me->itsMapping, 
                mapping, 0, HCAL, 
                Mapping_Active, NULL, NULL);
    collectorMapping = (RKH_SM_T *)&(me->itsMapping);
    RKH_SET_STATIC_EVENT(&evMappingObj, evMapping);
}

/* ............................ Effect actions ............................. */
void
Collector_init(Collector *const me, RKH_EVT_T *pe)
{
	(void)pe;

    RKH_TR_FWK_AO(me);
    RKH_TR_FWK_AO(collectorMapping);
    RKH_TR_FWK_QUEUE(&RKH_UPCAST(RKH_SMA_T, me)->equeue);
    RKH_FILTER_OFF_SMA(collectorMapping);

    topic_subscribe(status, RKH_UPCAST(RKH_SMA_T, me));
    rkh_sm_init(RKH_UPCAST(RKH_SM_T, &me->itsMapping));
}

void 
Collector_updatePosition(Collector *const me, RKH_EVT_T *pe)
{
    me->status.position = RKH_DOWNCAST(GeoEvt, pe)->position;
}

void 
Collector_updateDigOut(Collector *const me, RKH_EVT_T *pe)
{
    me->status.ioStatus.digOut = RKH_DOWNCAST(DigOutChangedEvt, pe)->status;
}

void 
Collector_publishCurrStatus(Collector *const me, RKH_EVT_T *pe)
{
    GStatusEvt *evt;

    evt = RKH_ALLOC_EVT(GStatusEvt, evGStatus, me);
    evt->status = me->status;
    topic_publish(status, 
                  RKH_UPCAST(RKH_EVT_T, evt), 
                  RKH_UPCAST(RKH_SMA_T, me));
}

void 
Collector_updateDigInTestDevNull(Collector *const me, RKH_EVT_T *pe)
{
    me->status.ioStatus.digIn = RKH_DOWNCAST(DigInChangedEvt, pe)->status;
    if (testDevNullJobCond(me))
    {
        RKH_SMA_POST_LIFO(RKH_UPCAST(RKH_SMA_T, me), 
                          RKH_UPCAST(RKH_EVT_T, &evMappingObj), me);
    }
}

void 
Collector_testDevNull(Collector *const me, RKH_EVT_T *pe)
{
    if (testDevNullJobCond(me))
    {
        RKH_SMA_POST_LIFO(RKH_UPCAST(RKH_SMA_T, me), 
                          RKH_UPCAST(RKH_EVT_T, &evMappingObj), me);
    }
}

/* ............................. Entry actions ............................. */
void
Collector_enActive(Collector *const me)
{
    RKH_SET_STATIC_EVENT(&me->updateStatusTmr, evTimeout);
    RKH_TMR_INIT(&me->updateStatusTmr.tmr, 
                 RKH_UPCAST(RKH_EVT_T, &me->updateStatusTmr), 
                 NULL);
    RKH_TMR_PERIODIC(&me->updateStatusTmr.tmr, 
                     RKH_UPCAST(RKH_SMA_T, me), 
                     0, UPDATING_STATUS_TIME);
}

void
Collector_initDevNull(Collector *const me)
{
    me->dev = (Device *)0;
}

/* ............................. Exit actions .............................. */
void
Collector_exActive(Collector *const me)
{
    rkh_tmr_stop(&me->updateStatusTmr.tmr);
}

/* ................................ Guards ................................. */
/* ------------------------------ End of file ------------------------------ */