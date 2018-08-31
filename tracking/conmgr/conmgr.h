/**
 *  \file       conmgr.h
 *  \brief      Specification of connection and protocol manager.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.15  DaBa  v1.0.01  Initial version
 *  2018.05.07  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Bali�a      db@vortexmakes.com
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __CONMGR_H__
#define __CONMGR_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "events.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */

/**
 * Specifies time waiting for GSM modem first response.
 */
#define SYNC_DELAY_TIME     RKH_TIME_MS(1000)

/**
 * Specifies the maximum tries seeking for GSM modem response. 
 */
#define MAX_SYNC_RETRY      5

/**
 * Specifies time waiting for GSM modem network registration.
 */
#define REGISTRATION_TIME   RKH_TIME_MS(60000)

/**
 * Specifies a time delay after general failure before to restart connection.
 */
#define FAILURE_TRY_DELAY   RKH_TIME_MS(10000)

/**
 * Specifies the maximum tries for setting APN configuration.
 */
#define MAX_CONFIG_RETRY    5

/**
 * Specifies the time waiting for Time Sync Message by Network.
 */
#define WAIT_NETCLOCK_TIME  RKH_TIME_MS(10000)

/**
 * Specifies a time delay after config error before to restart connection.
 */
#define CONFIG_TRY_DELAY    RKH_TIME_MS(3000)

/**
 * Specifies maximum server connection attemps.
 */
#define MAX_CONNECT_RETRY   3

/**
 * Specifies a time delay between server connection attemps.
 */
#define CONNECT_TRY_DELAY   RKH_TIME_MS(5000)

/**
 * Specifies a time delay between server close and reopen.
 */
#define REOPEN_DELAY        RKH_TIME_MS(5000)

/**
 * Specifies connections status´s polling period.
 */
#define CONNSTATUS_PERIOD   RKH_TIME_MS(2000)

/**
 * GSM Network Provider and Connection specific configurations.
 */
/* .................................. APN .................................. */
    #define MOVISTAR_APN_ADDR   "INTERNER.GPRS.UNIFON.COM.AR"  
    #define MOVISTAR_APN_USER   "WAP"
    #define MOVISTAR_APN_PASS   "WAP"

#define CONNECTION_APN          MOVISTAR_APN_ADDR
#define CONNECTION_USER         MOVISTAR_APN_USER
#define CONNECTION_PASSWORD     MOVISTAR_APN_PASS

/* .................................. SIM PIN .............................. */
#define SIM_PIN_NUMBER          9474

/* ................................. Server ................................ */
    #define YIPIES_SERVER       "71.6.135.77"
    #define YIPIES_PORT         "33499"

    #define DOCKLIGHT_SERVER    "181.230.72.86"
    #define DOCKLIGHT_PORT      "33498"

#define CONNECTION_PROT         "TCP"
#define CONNECTION_DOMAIN       YIPIES_SERVER 
#define CONNECTION_PORT         YIPIES_PORT

/* ................................ Signals ................................ */
/* ........................ Declares active object ......................... */
RKH_SMA_DCLR(conMgr);

/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
ReceivedEvt * ConMgr_ReceiveDataGetRef(void);
char * ConMgr_Imei(void);
char * ConMgr_ImeiSNR(void);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
