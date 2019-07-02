/**
 *  \file       test_rfile.c
 *  \brief      Unit test for rfile module.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.07.01  LeFr  v1.0.00
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "unity.h"
#include "ffdata.h"
#include "rfile.h"
#include "Mock_ffile.h"
#include "Mock_devflash.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
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
test_RestoreDirBothPageBad(void)
{
    ffui8_t result;
    const FFILE_T *dir;
    PageRes mainPage, backPage;

    mainPage.result = PAGE_BAD;
    mainPage.checksum = 0;
    backPage.result = PAGE_BAD;
    backPage.checksum = 0;

    devflash_setInvalidPage_Expect();
    devflash_verify_page_ExpectAndReturn(RF_DIR_MAIN_PAGE, mainPage);
    devflash_verify_page_ExpectAndReturn(RF_DIR_BACK_PAGE, backPage);

    dir = rfile_restore_directory(&result);
    TEST_ASSERT_EQUAL(DIR_BAD, result);
    TEST_ASSERT_EQUAL_MEMORY(defdir, dir, sizeof(FFILE_T) * NUM_FLASH_FILES);
}

/* ------------------------------ End of file ------------------------------ */
