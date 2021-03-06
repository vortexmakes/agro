/**
 *  \file       ffdir.h
 *  \brief
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __FFDIR_H__
#define __FFDIR_H__

/* ----------------------------- Include files ----------------------------- */
#include "rfile.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
enum
{
    DIR_OK, DIR_BAD, DIR_RECOVERY, DIR_BACKUP
};

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
FFILE_T *ffdir_restore(ffui8_t *status);
void ffdir_update(FFILE_T *file);
FFILE_T *ffdir_getFile(FFD_T fd);
void ffdir_getDirty(DirId dir);
FFILE_T *ffdir_clean(void);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
