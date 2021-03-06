/**
 *  \file       test_CollectorAct.c
 *  \brief      Unit test for state machine module.
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
#include <string.h>
#include "unity.h"
#include "Collector.h"
#include "CollectorAct.h"
#include "Mock_rkhsma.h"
#include "Mock_rkhsm.h"
#include "rkhfwk_cast.h"
#include "signals.h"
#include "events.h"
#include "Mock_topic.h"
#include "Mock_rkhtrc_record.h"
#include "Mock_rkhtrc_filter.h"
#include "Mock_rkhtmr.h"
#include "Mock_rkhfwk_dynevt.h"
#include "Mock_device.h"
#include "Mock_rkhassert.h"
#include "Mock_Config.h"
#include "Mock_StatQue.h"
#include "Mock_GStatus.h"
#include "Mock_ffile.h"
#include "Mock_geoMgr.h"
#include "Mock_Trace.h"
#include "Mock_Backup.h"
#include "Mock_Flowmeter.h"
#include "AgroIoTVersion.h"
#include "Mock_bsp.h"

/* ----------------------------- Local macros ------------------------------ */
#define GEO_INVALID_GEOSTAMP    \
    { \
        {GEO_INVALID_UTC}, {RMC_StatusInvalid}, \
        {GEO_INVALID_LATITUDE}, {GEO_INVALID_LATITUDE_IND}, \
        {GEO_INVALID_LONGITUDE}, {GEO_INVALID_LONGITUDE_IND}, \
        {GEO_INVALID_SPEED}, {GEO_INVALID_COURSE}, \
        {GEO_INVALID_DATE} \
    }

/* ------------------------------- Constants ------------------------------- */
RKHROM RKH_SCMP_T DevStatus_Active, Mapping_Active;
RKHROM RKH_SBSC_T DevStatus_DevNotConnected, DevStatus_DevConnected,
                  Mapping_Stopped, Mapping_Running;
RKHROM RKH_SCHOICE_T Mapping_C1, Mapping_C2, Mapping_C3;
static const Geo invalidPosition = GEO_INVALID_GEOSTAMP;

/* ---------------------------- Local data types --------------------------- */
typedef struct DevA DevA;
struct DevA
{
    Device base;
    int x;
    int y;
};

typedef struct EvtDevAData EvtDevAData;
struct EvtDevAData
{
    EvtDevData base;
    int x;
    int y;
};

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
RKH_SMA_CREATE(Collector, collector, 0, HCAL, NULL, Collector_init, NULL);
RKH_SMA_DEF_PTR(collector);
RKH_SM_CONST_CREATE(mapping, 0, HCAL, &Mapping_Active, NULL, NULL);

static Collector *me;
static RKH_EVT_T evtObj, *evt;
const RKHSmaVtbl rkhSmaVtbl =  /* Instantiate it because rkhsma is mocked */
{
    rkh_sma_activate,
    rkh_sma_dispatch,
    rkh_sma_post_fifo,
    rkh_sma_post_lifo
};

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
setUp(void)
{
    me = RKH_DOWNCAST(Collector, collector);
    evt = &evtObj;
}

void
tearDown(void)
{
}

void
test_ConstructorWithDigInPolActiveLow(void)
{
    Config_getDigInPolarity_ExpectAndReturn(ActiveLow);
    rkh_sma_ctor_Expect(RKH_UPCAST(RKH_SMA_T, me), &me->vtbl);

    Collector_ctor();

    TEST_ASSERT_NOT_NULL(me->vtbl.task);
    TEST_ASSERT_EQUAL(me, me->itsMapping.itsCollector);
    TEST_ASSERT_NULL(me->dev);
    TEST_ASSERT_EQUAL_MEMORY(&invalidPosition, 
                             &me->status.data.position, 
                             sizeof(Geo));
    TEST_ASSERT_EQUAL(MAPPING_STOP, me->status.data.devData.a.x);
    TEST_ASSERT_EQUAL(0xff, me->status.data.ioStatus.digIn);
    TEST_ASSERT_EQUAL(0, me->status.data.ioStatus.digOut);
    TEST_ASSERT_EQUAL(LINE_BATT, me->status.data.batChrStatus);
    TEST_ASSERT_EQUAL(0, me->itsMapping.nStoreLastSync);
    TEST_ASSERT_EQUAL(0, me->itsMapping.nStoreLastSync);
    TEST_ASSERT_EQUAL(0, me->status.data.devData.h.hoard);
    TEST_ASSERT_EQUAL(0, me->status.data.devData.h.pqty);
    TEST_ASSERT_EQUAL(0, me->status.data.devData.h.flow);
}

void
test_ConstructorWithDigInPolActiveHigh(void)
{
    Config_getDigInPolarity_ExpectAndReturn(ActiveHigh);
    rkh_sma_ctor_Expect(RKH_UPCAST(RKH_SMA_T, me), &me->vtbl);

    Collector_ctor();

    TEST_ASSERT_NOT_NULL(me->vtbl.task);
    TEST_ASSERT_EQUAL(me, me->itsMapping.itsCollector);
    TEST_ASSERT_NULL(me->dev);
    TEST_ASSERT_EQUAL_MEMORY(&invalidPosition, 
                             &me->status.data.position, 
                             sizeof(Geo));
    TEST_ASSERT_EQUAL(MAPPING_STOP, me->status.data.devData.a.x);
    TEST_ASSERT_EQUAL(0, me->status.data.ioStatus.digIn);
    TEST_ASSERT_EQUAL(0, me->status.data.ioStatus.digOut);
    TEST_ASSERT_EQUAL(LINE_BATT, me->status.data.batChrStatus);
    TEST_ASSERT_EQUAL(0, me->status.data.devData.h.hoard);
    TEST_ASSERT_EQUAL(0, me->status.data.devData.h.pqty);
    TEST_ASSERT_EQUAL(0, me->status.data.devData.h.flow);
}

void
test_Init(void)
{
    rkh_trc_ao_Ignore();
    rkh_trc_ao_Ignore();
    rkh_trc_obj_Ignore();
    rkh_trc_state_Ignore();
    rkh_trc_state_Ignore();
    rkh_trc_state_Ignore();
    rkh_trc_state_Ignore();
    rkh_trc_state_Ignore();
    rkh_trc_state_Ignore();
    rkh_trc_state_Ignore();
    rkh_trc_state_Ignore();
    rkh_trc_state_Ignore();
    rkh_trc_symFil_Ignore();
    topic_subscribe_Expect(Status, RKH_UPCAST(RKH_SMA_T, me));
    rkh_sm_init_Expect(RKH_UPCAST(RKH_SM_T, &me->itsMapping));
    Flowmeter_init_Expect(&me->flowmeter);

    Collector_init(me, evt);
}

void
test_UpdatePosition(void)
{
    GeoEvt event;

    Collector_updatePosition(me, RKH_UPCAST(RKH_EVT_T, &event));
    TEST_ASSERT_EQUAL_MEMORY(&me->status.data.position, &event.position,
                             sizeof(Geo));
}

void
test_UpdateInvalidPosition(void)
{
    GeoEvt event;
    Geo *pos;
    char backupCode[COURSE_LENGTH + 1], reset[LAT_IND_LENGTH + 1];

    me->backupInfo.error = Backup_NoInit;
    sprintf(backupCode, "%02d", me->backupInfo.error);
    sprintf(reset, "%X", ResetSrcWDG);
    
    pos = &me->status.data.position;
    event.position = invalidPosition;
    bsp_getResetSource_ExpectAndReturn(ResetSrcWDG);

    Collector_updateInvPosition(me, RKH_UPCAST(RKH_EVT_T, &event));

    TEST_ASSERT_EQUAL_STRING(GEO_INVALID_UTC, pos->utc);
    TEST_ASSERT_EQUAL_STRING(GEO_INVALID_LATITUDE, pos->latitude);
    TEST_ASSERT_EQUAL_STRING(GEO_INVALID_LONGITUDE, pos->longitude);
    TEST_ASSERT_EQUAL_STRING(agroIoTVersion, pos->speed);
    TEST_ASSERT_EQUAL_STRING(backupCode, pos->course);
    TEST_ASSERT_EQUAL_STRING(reset, pos->latInd);
}

void
test_UpdateDigOut(void)
{
    DigOutChangedEvt event;

    event.status = 99;
    Collector_updateDigOut(me, RKH_UPCAST(RKH_EVT_T, &event));
    TEST_ASSERT_EQUAL(event.status, me->status.data.ioStatus.digOut);
}

void
test_StartAndStopUpdatingStatusTmr(void)
{
    rkh_tmr_init__Expect(&me->updateStatusTmr.tmr,
                         RKH_UPCAST(RKH_EVT_T, &me->updateStatusTmr));
    rkh_tmr_start_Expect(&me->updateStatusTmr.tmr,
                         RKH_UPCAST(RKH_SMA_T, me),
                         UPDATING_STATUS_TIME, UPDATING_STATUS_TIME);
    Collector_enActive(me);

    rkh_tmr_stop_ExpectAndReturn(&me->updateStatusTmr.tmr, 0);
    Collector_exActive(me);
}

void
test_PublishCurrStatusWithNoDevAndMappingStopped(void)
{
    GStatusEvt event;

    me->itsMapping.sm.state = RKH_CAST(RKH_ST_T, &Mapping_Stopped);
    rkh_fwk_ae_ExpectAndReturn(sizeof(GStatusEvt), evGStatus, me,
                               RKH_UPCAST(RKH_EVT_T, &event));
    topic_publish_Expect(GeneralStatus,
                         RKH_UPCAST(RKH_EVT_T, &event),
                         RKH_UPCAST(RKH_SMA_T, me));

    Collector_publishCurrStatus(me, RKH_UPCAST(RKH_EVT_T, &event));
    TEST_ASSERT_EQUAL_HEX32(MAPPING_STOP, event.status.devData.a.x);
}

void
test_PublishCurrStatusWithNoDevAndMappingRunning(void)
{
    GStatusEvt event;

    rkh_fwk_ae_ExpectAndReturn(sizeof(GStatusEvt), evGStatus, me,
                               RKH_UPCAST(RKH_EVT_T, &event));
    topic_publish_Expect(GeneralStatus,
                         RKH_UPCAST(RKH_EVT_T, &event),
                         RKH_UPCAST(RKH_SMA_T, me));

    Collector_publishCurrStatus(me, RKH_UPCAST(RKH_EVT_T, &event));
}

void
test_PublishCurrStatusWithDevConnected(void)
{
    GStatusEvt event;

    rkh_fwk_ae_ExpectAndReturn(sizeof(GStatusEvt), evGStatus, me,
                               RKH_UPCAST(RKH_EVT_T, &event));
    topic_publish_Expect(GeneralStatus,
                         RKH_UPCAST(RKH_EVT_T, &event),
                         RKH_UPCAST(RKH_SMA_T, me));

    Collector_publishCurrStatus(me, RKH_UPCAST(RKH_EVT_T, &event));
}

void
test_ActiveLowDigInOnEntryDevNotConnected(void)
{
    me->status.data.ioStatus.digIn = 0xfe;
    Config_getDigInPolarity_ExpectAndReturn(ActiveLow);
    rkh_sma_post_lifo_Expect(RKH_UPCAST(RKH_SMA_T, me), 0, me);
    rkh_sma_post_lifo_IgnoreArg_e();

    Collector_initAndTestDevNull(me);
    TEST_ASSERT_NULL(me->dev);
}

void
test_ActiveHighDigInOnEntryDevNotConnected(void)
{
    me->status.data.ioStatus.digIn = 0xfe;
    Config_getDigInPolarity_ExpectAndReturn(ActiveHigh);
    rkh_sma_post_lifo_Expect(RKH_UPCAST(RKH_SMA_T, me), 0, me);
    rkh_sma_post_lifo_IgnoreArg_e();

    Collector_initAndTestDevNull(me);
    TEST_ASSERT_NULL(me->dev);
}

void
test_IdleActiveLowDigInOnEntryDevNotConnected(void)
{
    me->status.data.ioStatus.digIn = 0xff;
    Config_getDigInPolarity_ExpectAndReturn(ActiveLow);
    rkh_sma_post_lifo_Expect(RKH_UPCAST(RKH_SMA_T, me), 0, me);
    rkh_sma_post_lifo_IgnoreArg_e();

    Collector_initAndTestDevNull(me);
    TEST_ASSERT_NULL(me->dev);
}

void
test_IdleActiveHighDigInOnEntryDevNotConnected(void)
{
    me->status.data.ioStatus.digIn = 0;
    Config_getDigInPolarity_ExpectAndReturn(ActiveHigh);
    rkh_sma_post_lifo_Expect(RKH_UPCAST(RKH_SMA_T, me), 0, me);
    rkh_sma_post_lifo_IgnoreArg_e();

    Collector_initAndTestDevNull(me);
    TEST_ASSERT_NULL(me->dev);
}

void
test_ActiveLowDigInInDevNotConnected(void)
{
    DigInChangedEvt event;

    event.status = 0xfe;
    Config_getDigInPolarity_ExpectAndReturn(ActiveLow);
    rkh_sma_post_lifo_Expect(RKH_UPCAST(RKH_SMA_T, me), 0, me);
    rkh_sma_post_lifo_IgnoreArg_e();

    Collector_updateDigInTestDevNull(me, RKH_UPCAST(RKH_EVT_T, &event));
    TEST_ASSERT_EQUAL(event.status, me->status.data.ioStatus.digIn);
}

void
test_IdleActiveLowDigInInDevNotConnected(void)
{
    DigInChangedEvt event;

    event.status = 0xff;
    Config_getDigInPolarity_ExpectAndReturn(ActiveLow);
    rkh_sma_post_lifo_Expect(RKH_UPCAST(RKH_SMA_T, me), 0, me);
    rkh_sma_post_lifo_IgnoreArg_e();

    Collector_updateDigInTestDevNull(me, RKH_UPCAST(RKH_EVT_T, &event));
    TEST_ASSERT_EQUAL(event.status, me->status.data.ioStatus.digIn);
}

void
test_ActiveHighDigInInDevNotConnected(void)
{
    DigInChangedEvt event;

    event.status = 0xfe;
    Config_getDigInPolarity_ExpectAndReturn(ActiveHigh);
    rkh_sma_post_lifo_Expect(RKH_UPCAST(RKH_SMA_T, me), 0, me);
    rkh_sma_post_lifo_IgnoreArg_e();

    Collector_updateDigInTestDevNull(me, RKH_UPCAST(RKH_EVT_T, &event));
    TEST_ASSERT_EQUAL(event.status, me->status.data.ioStatus.digIn);
}

void
test_IdleActiveHighDigInInDevNotConnected(void)
{
    DigInChangedEvt event;

    event.status = 0;
    Config_getDigInPolarity_ExpectAndReturn(ActiveHigh);
    rkh_sma_post_lifo_Expect(RKH_UPCAST(RKH_SMA_T, me), 0, me);
    rkh_sma_post_lifo_IgnoreArg_e();

    Collector_updateDigInTestDevNull(me, RKH_UPCAST(RKH_EVT_T, &event));
    TEST_ASSERT_EQUAL(event.status, me->status.data.ioStatus.digIn);
}

void
test_UpdateDigInInDevConnected(void)
{
    DigInChangedEvt event;

    event.status = 0xfc;
    Collector_updateDigIn(me, RKH_UPCAST(RKH_EVT_T, &event));
    TEST_ASSERT_EQUAL(event.status, me->status.data.ioStatus.digIn);
}

void
test_UpdateDevDataAndJobCondTrue(void)
{
    EvtDevAData event;
    Device device;

    event.base.dev = &device;
    device_update_ExpectAndReturn(event.base.dev, 
                                  RKH_UPCAST(RKH_EVT_T, &event), false);
    device_updateRaw_Expect(event.base.dev);
    device_test_ExpectAndReturn(event.base.dev, 1);
    rkh_sma_post_lifo_Expect(RKH_UPCAST(RKH_SMA_T, me), 0, me);
    rkh_sma_post_lifo_IgnoreArg_e();

    Collector_updateAndTestDevData(me, RKH_UPCAST(RKH_EVT_T, &event));
    TEST_ASSERT_EQUAL(event.base.dev, me->dev);
}

void
test_UpdateDevDataAndJobCondFalse(void)
{
    EvtDevAData event;
    Device device;

    event.base.dev = &device;
    device_update_ExpectAndReturn(event.base.dev, 
                                  RKH_UPCAST(RKH_EVT_T, &event), false);
    device_updateRaw_Expect(event.base.dev);
    device_test_ExpectAndReturn(event.base.dev, 0);
    rkh_sma_post_lifo_Expect(RKH_UPCAST(RKH_SMA_T, me), 0, me);
    rkh_sma_post_lifo_IgnoreArg_e();

    Collector_updateAndTestDevData(me, RKH_UPCAST(RKH_EVT_T, &event));
    TEST_ASSERT_EQUAL(event.base.dev, me->dev);
}

void
test_UpdateDevDataAndStoreStatus(void)
{
    EvtDevAData event;
    Device device;

    event.base.dev = &device;
    device_update_ExpectAndReturn(event.base.dev, 
                                  RKH_UPCAST(RKH_EVT_T, &event), true);
    device_updateRaw_Expect(event.base.dev);
    GStatus_setChecksum_Expect(&me->status);
    StatQue_put_ExpectAndReturn(0, 0);
    StatQue_put_IgnoreArg_elem();
    Backup_store_ExpectAndReturn(&me->status, Backup_Ok);
    device_test_ExpectAndReturn(event.base.dev, 0);
    rkh_sma_post_lifo_Expect(RKH_UPCAST(RKH_SMA_T, me), 0, me);
    rkh_sma_post_lifo_IgnoreArg_e();

    Collector_updateAndTestDevData(me, RKH_UPCAST(RKH_EVT_T, &event));
    TEST_ASSERT_EQUAL(event.base.dev, me->dev);
}

void
test_StartAndStopSyncStoppedTmr(void)
{
    Mapping *region;

    me->base.sm.state = RKH_CAST(RKH_ST_T, &DevStatus_DevNotConnected);
    region = &me->itsMapping;

    Config_getConnTime_ExpectAndReturn(60);
    rkh_tmr_init__Expect(&region->syncStoppedTmr.tmr,
                         RKH_UPCAST(RKH_EVT_T, &region->syncStoppedTmr));
    rkh_tmr_start_Expect(&region->syncStoppedTmr.tmr,
                         RKH_UPCAST(RKH_SMA_T, me),
                         RKH_TIME_SEC(60), 0);
    bsp_workStatusLed_Expect(SEQ_NO_LIT);

    Mapping_enStopped(region);
    TEST_ASSERT_EQUAL_HEX32(MAPPING_STOP, me->status.data.devData.a.x);

    rkh_tmr_stop_ExpectAndReturn(&region->syncStoppedTmr.tmr, 0);
    Mapping_exStopped(region);
}

void
test_StoreStatus(void)
{
    int n;
    Mapping *region;

    region = &me->itsMapping;
    region->nStoreLastSync = n = 20;
    GStatus_setChecksum_Expect(&region->itsCollector->status);
    StatQue_put_ExpectAndReturn(0, 0);
    StatQue_put_IgnoreArg_elem();
    Backup_store_ExpectAndReturn(&region->itsCollector->status, Backup_Ok);

    Mapping_storeStatus(region, (RKH_EVT_T *)0);
    TEST_ASSERT_EQUAL(n + 1, me->itsMapping.nStoreLastSync);
}

void
test_StoreStatusAndSyncWithoutDeviceConnected(void)
{
    int n;
    Mapping *region;

    region = &me->itsMapping;
    region->itsCollector->dev = (Device *)0;
    region->nStoreLastSync = n = 20;
    GStatus_setChecksum_Expect(&region->itsCollector->status);
    StatQue_put_ExpectAndReturn(0, 0);
    StatQue_put_IgnoreArg_elem();
    Backup_store_ExpectAndReturn(&region->itsCollector->status, Backup_Ok);
    device_clear_Expect(region->itsCollector->dev);

    Mapping_storeStatusAndSync(region, (RKH_EVT_T *)0);
    TEST_ASSERT_EQUAL(n + 1, me->itsMapping.nStoreLastSync);
}

void
test_StoreStatusAndSync(void)
{
    int n;
    Mapping *region;
    Device device;

    region = &me->itsMapping;
    region->itsCollector->dev = &device;
    region->nStoreLastSync = n = 20;
    GStatus_setChecksum_Expect(&region->itsCollector->status);
    StatQue_put_ExpectAndReturn(0, 0);
    StatQue_put_IgnoreArg_elem();
    Backup_store_ExpectAndReturn(&region->itsCollector->status, Backup_Ok);
    device_clear_Expect(region->itsCollector->dev);

    Mapping_storeStatusAndSync(region, (RKH_EVT_T *)0);
    TEST_ASSERT_EQUAL(n + 1, me->itsMapping.nStoreLastSync);
}

void
test_syncDir(void)
{
    Mapping *region;

    region = &me->itsMapping;
    me->itsMapping.nStoreLastSync = 20;
    ffile_sync_Expect();
    Mapping_syncDir(region, (RKH_EVT_T *)0);
    TEST_ASSERT_EQUAL(0, me->itsMapping.nStoreLastSync);
}

void
test_StartAndStopSyncRunningTmr(void)
{
    Mapping *region;

    me->base.sm.state = RKH_CAST(RKH_ST_T, &DevStatus_DevNotConnected);
    region = &me->itsMapping;

    Config_getMapTimeOnRunning_ExpectAndReturn(3);
    rkh_tmr_init__Expect(&region->syncRunningTmr.tmr,
                         RKH_UPCAST(RKH_EVT_T, &region->syncRunningTmr));
    rkh_tmr_start_Expect(&region->syncRunningTmr.tmr,
                         RKH_UPCAST(RKH_SMA_T, me),
                         RKH_TIME_SEC(3), 0);
    bsp_workStatusLed_Expect(SEQ_LIT);

    Mapping_enRunning(region);
    TEST_ASSERT_EQUAL_HEX32(MAPPING_RUNNING, me->status.data.devData.a.x);

    rkh_tmr_stop_ExpectAndReturn(&region->syncRunningTmr.tmr, 0);
    Mapping_exRunning(region);
}

void
test_IsSyncDirOnStopped(void)
{
    rbool_t result;
    Mapping *region;

    region = &me->itsMapping;
    region->nStoreLastSync = 80;
    Config_getMaxNumStoreOnStopped_ExpectAndReturn(240);
    result = Mapping_isSyncDirOnStopped(RKH_UPCAST(RKH_SM_T, region),
                                        (RKH_EVT_T *)0);
    TEST_ASSERT_EQUAL(RKH_FALSE, result);

    region->nStoreLastSync = 240;
    Config_getMaxNumStoreOnStopped_ExpectAndReturn(240);
    result = Mapping_isSyncDirOnStopped(RKH_UPCAST(RKH_SM_T, region),
                                        (RKH_EVT_T *)0);
    TEST_ASSERT_EQUAL(RKH_TRUE, result);
}

void
test_IsSyncDirOnRunning(void)
{
    rbool_t result;
    Mapping *region;

    region = &me->itsMapping;
    region->nStoreLastSync = 80;
    Config_getMaxNumStoreOnRunning_ExpectAndReturn(100);
    result = Mapping_isSyncDirOnRunning(RKH_UPCAST(RKH_SM_T, region),
                                        (RKH_EVT_T *)0);
    TEST_ASSERT_EQUAL(RKH_FALSE, result);

    region->nStoreLastSync = 100;
    Config_getMaxNumStoreOnRunning_ExpectAndReturn(100);
    result = Mapping_isSyncDirOnRunning(RKH_UPCAST(RKH_SM_T, region),
                                        (RKH_EVT_T *)0);
    TEST_ASSERT_EQUAL(RKH_TRUE, result);
}

void
test_StoreTrace(void)
{
    TraceEvt event = {2, 4, 6};

    Trace_put_Expect(event.id, event.arg0, event.arg1);
    Collector_storeTrace(me, RKH_UPCAST(RKH_EVT_T, &event));
}

void
test_InitBackup(void)
{
    Backup_init_ExpectAndReturn(&me->backupInfo, Backup_Ok);
    Trace_put_Expect(TraceId_UsbFlashMounted, Backup_Ok, 0);

    Collector_initBackup(me, RKH_UPCAST(RKH_EVT_T, evt));
}

void
test_deinitBackup(void)
{
    Backup_deinit_ExpectAndReturn(&me->backupInfo, Backup_NoInit);
    Trace_put_Expect(TraceId_UsbFlashUnmounted, Backup_NoInit, 0);

    Collector_deinitBackup(me, RKH_UPCAST(RKH_EVT_T, evt));
}

void
test_UpdateFlowmeterData(void)
{
    FlowmeterEvt event;

    event.flow1.nPulses = 32;
    event.flow1.dir = Forward;
    event.flow2.nPulses = 64;
    event.flow2.dir = Reverse;
    Flowmeter_update_Expect(&me->flowmeter, RKH_UPCAST(RKH_EVT_T, &event));
    Flowmeter_updateRaw_Expect(&me->flowmeter, &me->status.data.devData);

    Collector_updateFlowmeter(me, RKH_UPCAST(RKH_EVT_T, &event));
}

void
test_StoreStatusWhenSystemTurns(void)
{
    int n;
    Mapping *region;

    region = &me->itsMapping;
    region->nStoreLastSync = n = 20;
    GStatus_setChecksum_Expect(&region->itsCollector->status);
    StatQue_put_ExpectAndReturn(0, 0);
    StatQue_put_IgnoreArg_elem();
    Backup_store_ExpectAndReturn(&region->itsCollector->status, Backup_Ok);

    Mapping_storeStatus(region, (RKH_EVT_T *)0);
    TEST_ASSERT_EQUAL(n + 1, me->itsMapping.nStoreLastSync);
}

void
test_CleanDeviceAttributesWhenItIsNotMapping(void)
{
    Mapping *region;

    region = &me->itsMapping;
    region->itsCollector->status.data.devData.h.hoard = 0xaa;
    region->itsCollector->status.data.devData.h.pqty = 0x55;
    region->itsCollector->status.data.devData.h.flow = 0xaa;
    rkh_tmr_stop_ExpectAndReturn(&region->syncRunningTmr.tmr, 0);

    Mapping_exRunning(region);
    TEST_ASSERT_EQUAL(0, me->status.data.devData.h.hoard);
    TEST_ASSERT_EQUAL(0, me->status.data.devData.h.pqty);
    TEST_ASSERT_EQUAL(0, me->status.data.devData.h.flow);
}

/* ------------------------------ End of file ------------------------------ */
