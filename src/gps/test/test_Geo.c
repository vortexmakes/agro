/**
 *  \file       test_Geo.c
 *  \brief      Unit test for Geo module
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
#include "Geo.h"
#include "Mock_rkhassert.h"
#include "Mock_rkhport.h"
#include "Mock_ssp.h"
#include "Mock_rmc.h"
#include "Mock_ubxm8parser.h"
#include "Mock_SecString.h"
#include "Mock_geoMgr.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
SSP_CREATE_NORMAL_NODE(rootGpsParser);  /* Included just for testing */
SSP_CREATE_BR_TABLE(rootGpsParser)
SSPBR("", 0, 0),
SSP_END_BR_TABLE

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static void
errorHandler(GeoErrorCode errCode)
{
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_RANGE, errCode);
}

static void
setPreconditions(char *attribute, char *value, size_t bufSize)
{
    char *pos;

    strcpy(attribute, value);
    pos = memchr(attribute, '\0', bufSize);
    SecString_strchk_ExpectAndReturn(attribute, bufSize, pos);
}

/* ---------------------------- Global functions --------------------------- */
void
setUp(void)
{
    Geo_init(0);
}

void
tearDown(void)
{
}

void
test_SetErrorHandler(void)
{
    Geo_init(errorHandler);
}

void
test_GetValidLatitude(void)
{
    Geo position;
    char *str, value[] = GEO_INVALID_LATITUDE;

    setPreconditions(position.latitude, value, LATITUDE_LENGTH + 1);
    str = Geo_getLatitude(&position);

    TEST_ASSERT_NOT_NULL(str);
    TEST_ASSERT_EQUAL_STRING(value, str);
}

void
test_AttemptToGetAnInvalidLatitude(void)
{
    Geo position;
    char *str, *pos, value[] = "37.840288333";
    size_t bufSize;

    Geo_init(errorHandler);
    bufSize = LATITUDE_LENGTH + 1;
    memcpy(position.latitude, value, bufSize);
    pos = (char *)0;
    SecString_strchk_ExpectAndReturn(position.latitude, bufSize, pos);

    str = Geo_getLatitude(&position);

    TEST_ASSERT_NULL(str);
}

void
test_AttemptToGetAnInvalidLatitudeWithErrorHandler(void)
{
    Geo position;
    char *str, *pos, value[] = "37.840288333";
    size_t bufSize;

    bufSize = LATITUDE_LENGTH + 1;
    memcpy(position.latitude, value, bufSize);
    pos = (char *)0;
    SecString_strchk_ExpectAndReturn(position.latitude, bufSize, pos);

    str = Geo_getLatitude(&position);

    TEST_ASSERT_NULL(str);
}

void
test_GetValidUtc(void)
{
    Geo position;
    char *str, value[] = GEO_INVALID_UTC;

    setPreconditions(position.utc, value, UTC_LENGTH + 1);
    str = Geo_getUtc(&position);

    TEST_ASSERT_NOT_NULL(str);
    TEST_ASSERT_EQUAL_STRING(value, str);
}

void
test_GetValidLongitude(void)
{
    Geo position;
    char *str, value[] = GEO_INVALID_LONGITUDE;

    setPreconditions(position.longitude, value, LONGITUDE_LENGTH + 1);
    str = Geo_getLongitude(&position);

    TEST_ASSERT_NOT_NULL(str);
    TEST_ASSERT_EQUAL_STRING(value, str);
}

void
test_GetValidSpeed(void)
{
    Geo position;
    char *str, value[] = GEO_INVALID_SPEED;

    setPreconditions(position.speed, value, SPEED_LENGTH + 1);
    str = Geo_getSpeed(&position);

    TEST_ASSERT_NOT_NULL(str);
    TEST_ASSERT_EQUAL_STRING(value, str);
}

void
test_GetValidCourse(void)
{
    Geo position;
    char *str, value[] = GEO_INVALID_COURSE;

    setPreconditions(position.course, value, COURSE_LENGTH + 1);
    str = Geo_getCourse(&position);

    TEST_ASSERT_NOT_NULL(str);
    TEST_ASSERT_EQUAL_STRING(value, str);
}

void
test_GetValidDate(void)
{
    Geo position;
    char *str, value[] = GEO_INVALID_DATE;

    setPreconditions(position.date, value, DATE_LENGTH + 1);
    str = Geo_getDate(&position);

    TEST_ASSERT_NOT_NULL(str);
    TEST_ASSERT_EQUAL_STRING(value, str);
}

void
test_GetValidLatitudeIndicator(void)
{
    Geo position;
    char *str, value[] = GEO_INVALID_LATITUDE_IND;

    setPreconditions(position.latInd, value, LAT_IND_LENGTH + 1);
    str = Geo_getLatInd(&position);

    TEST_ASSERT_NOT_NULL(str);
    TEST_ASSERT_EQUAL_STRING(value, str);
}

void
test_GetValidLongitudeIndicator(void)
{
    Geo position;
    char *str, value[] = GEO_INVALID_LONGITUDE_IND;

    setPreconditions(position.latInd, value, LONG_IND_LENGTH + 1);
    str = Geo_getLatInd(&position);

    TEST_ASSERT_NOT_NULL(str);
    TEST_ASSERT_EQUAL_STRING(value, str);
}

/* ------------------------------ End of file ------------------------------ */
