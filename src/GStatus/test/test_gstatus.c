/**
 *  \file       test_gtatus.c
 *  \brief      Unit test for general status module.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.08.02  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <string.h>
#include "unity.h"
#include "GStatus.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
static GStatus status0 =
{
    {
        "185124", 
        "A", 
        "37.8402883", 
        "-", 
        "057.6884350", 
        "-", 
        "0.078",
        "", 
        "310119"
    },
    {1, 1, {0, 0, 1}, {0xdddd, 0xffff, 0xffff}, 0},
    {0xff, 0x3f},
    3
};

static GPS_STR status1 =
{
    "185124", 
    "A", 
    "37", 
    "8402883", 
    "-", 
    "057", 
    "6884350", 
    "-", 
    "0.078",
    "", 
    "310119",
    "3fff",
    "3",
    {1, 1, {0, 0, 1}, {0xdddd, 0xffff, 0xffff}, 0}
};

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void 
setUp(void)
{
}

void 
tearDown(void)
{
}

void
test_InvalidArgs(void)
{
    rInt result;
    GPS_STR *oldStatus;
    GStatus *newStatus;

    result = GStatus_toGpsStr((GStatus *)0, (GPS_STR *)0);
    TEST_ASSERT_EQUAL(1, result);

    result = GStatus_toGpsStr((GStatus *)0, oldStatus);
    TEST_ASSERT_EQUAL(1, result);

    result = GStatus_toGpsStr(newStatus, (GPS_STR *)0);
    TEST_ASSERT_EQUAL(1, result);

    result = GStatus_fromGpsStr(oldStatus, (GStatus *)0);
    TEST_ASSERT_EQUAL(1, result);

    result = GStatus_fromGpsStr((GPS_STR *)0, newStatus);
    TEST_ASSERT_EQUAL(1, result);

    result = GStatus_fromGpsStr((GPS_STR *)0, (GStatus *)0);
    TEST_ASSERT_EQUAL(1, result);
}

void
test_ConvertGStatusToGpsStr(void)
{
    rInt result;
    GPS_STR oldStatus;
    char temp[32];

    result = GStatus_toGpsStr(&status0, &oldStatus);
    TEST_ASSERT_EQUAL_STRING(status0.position.utc, oldStatus.utc);
    TEST_ASSERT_EQUAL_STRING(status0.position.status, oldStatus.status);
    sprintf(temp, "%s.%s", oldStatus.latdeg, oldStatus.latmin);
    TEST_ASSERT_EQUAL_STRING(status0.position.latitude, temp);
    TEST_ASSERT_EQUAL_STRING(status0.position.latInd, oldStatus.lat_ind);
    sprintf(temp, "%s.%s", oldStatus.longdeg, oldStatus.longmin);
    TEST_ASSERT_EQUAL_STRING(status0.position.longitude, temp);
    TEST_ASSERT_EQUAL_STRING(status0.position.longInd, oldStatus.long_ind);
    TEST_ASSERT_EQUAL_STRING(status0.position.speed, oldStatus.speed);
    TEST_ASSERT_EQUAL_STRING(status0.position.course, oldStatus.course);
    TEST_ASSERT_EQUAL_STRING(status0.position.date, oldStatus.date);
    sprintf(temp, "%02X%02X", status0.ioStatus.digOut, status0.ioStatus.digIn);
    TEST_ASSERT_EQUAL_STRING(temp, oldStatus.in_out_st);
    sprintf(temp, "%d", status0.batChrStatus);
    TEST_ASSERT_EQUAL_STRING(temp, oldStatus.acbk_st);
    TEST_ASSERT_EQUAL(0, result);
}

void
test_ConvertGpsStrToGStatus(void)
{
    rInt result;
    GStatus newStatus;
    char temp[32];

    result = GStatus_fromGpsStr(&status1, &newStatus);
    TEST_ASSERT_EQUAL_STRING(status1.utc, newStatus.position.utc);
    TEST_ASSERT_EQUAL_STRING(status1.status, newStatus.position.status);
    sprintf(temp, "%s.%s", status1.latdeg, status1.latmin);
    TEST_ASSERT_EQUAL_STRING(temp, newStatus.position.latitude);
    TEST_ASSERT_EQUAL(0, result);
}

/* ------------------------------ End of file ------------------------------ */
