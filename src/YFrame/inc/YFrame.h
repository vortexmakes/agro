/**
 *  \file       YFrame.h
 *  \brief      Specifies the interface of Yipies frame (remote protocol)
 *              module
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.02  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __YFRAME_H__
#define __YFRAME_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkhtype.h"
#include "GStatus.h"
#include "YCommand.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
typedef enum TypeOfResp TypeOfResp;
enum TypeOfResp
{
    TypeOfRespAck,
    TypeOfRespCmd,
    TypeOfRespUnknown,
    TypeOfRespEmpty,        /* indicates an empty frame */
    NumTypeOfResp
};

#define YFRAME_SGP_TYPE        0
#define YFRAME_MGP_TYPE        1

/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
rInt YFrame_getFlags(GStatusType *from, rui8_t *flags, rInt type);
ruint YFrame_header(GStatusType *from, char *to, rInt nFrames, rInt type);
ruint YFrame_data(GStatusType *from, char *to, rInt type);
ruint YFrame_multipleTail(char *to);
TypeOfResp YFrame_parse(char *from, ruint size, YCommand *cmd);
ruint YFrame_getCmdAck(YCommand *from, char *to);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
