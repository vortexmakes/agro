/**
 *  \file       Collector.h
 *  \brief      Specifies the interface of Collector module.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.11.22  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __COLLECTOR_H__
#define __COLLECTOR_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkhsma.h"
#include "rkhsm.h"
#include "rkhtmr.h"
#include "GStatus.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
#define UPDATING_STATUS_TIME   RKH_TIME_MS(1500)

/* ................................ Signals ................................ */
/* ................................. Events ................................ */
/* ........................ Declares active object ......................... */
RKH_SMA_DCLR(collector);
RKH_SM_DCLR(mapping);

/* ................... Declares states and pseudostates .................... */
RKH_DCLR_COMP_STATE Mapping_Active;

typedef struct Device Device;
typedef struct Collector Collector;
typedef struct Mapping Mapping;

struct Mapping
{
    RKH_SM_T sm;                /* Orthogonal region */
    Collector *itsCollector;
};

struct Collector
{
    RKH_SMA_T base;
    RKHSmaVtbl vtbl;            /* Virtual table */
    GStatus status;
    Device *dev;
    Mapping itsMapping;   /* Mapping orthogonal region */
    RKHTmEvt updateStatusTmr;
};

/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */