/**
 *  \file gps.h
 *
 *	This module contains functions for manipulating GPS serial port and
 *	process NMEA and Ublox frames
 */
/* -------------------------- Development history -------------------------- */
/*
 *  2018.09.06  DaBa  v1.0.00   Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Bali�a db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __GPS_H__
#define __GPS_H__

/* ----------------------------- Include files ----------------------------- */
/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
#define DEG_LENGTH			7
#define MIN_LENGTH			8
    
#define UTC_LENGTH          6
#define STATUS_LENGTH       1
//#define LATDEG_LENGTH       DEG_LENGTH
//#define LATMIN_LENGTH       MIN_LENGTH
#define LATITUDE_LENGTH      10
#define LAT_IND_LENGTH      1
#define LONGDEG_LENGTH      DEG_LENGTH
#define LONGMIN_LENGTH      MIN_LENGTH

#define LONGITUDE_LENGTH    11

#define LONG_IND_LENGTH     1
#define SPEED_LENGTH        7
#define COURSE_LENGTH       3
#define DATE_LENGTH         6

/* ------------------------------- Data types ------------------------------ */
typedef struct
{
    char utc[UTC_LENGTH+1];
    char status[STATUS_LENGTH+1];
    char latitude[LATITUDE_LENGTH+1];
    char latInd[LAT_IND_LENGTH+1];
    char longitude[LONGITUDE_LENGTH+1];
    char longInd[LONG_IND_LENGTH+1];
    char speed[SPEED_LENGTH+1];
    char course[COURSE_LENGTH+1];
    char date[DATE_LENGTH+1];
} GeoStamp;

typedef void (*GpsRcvHandler)(unsigned char c);

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
GpsRcvHandler gps_parserInit(void);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
