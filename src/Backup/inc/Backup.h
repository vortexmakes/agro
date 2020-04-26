/**
 *  \file       Backup.h
 *  \brief      Specification of system status backup
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __BACKUP_H__
#define __BACKUP_H__

/* ----------------------------- Include files ----------------------------- */
#include "GStatus.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
#define BACKUP_DIR_NAME             "frames"
#define BACKUP_MAXNUMFILES          20
#define BACKUP_MAXNUMREGPERFILE     15600 /* 13[h]*60[m/h]*60[s/m]/3[s] */
#define BACKUP_SIZEOF_REG           sizeof(GStatus)
#define BACKUP_NUMWRITES            1200

/* ------------------------------- Data types ------------------------------ */
typedef enum FileState FileState;
enum FileState
{
    BackupOpen, BackupClosed
};

typedef struct Backup Backup;
struct Backup
{
    int nFiles;
    int oldest;
    int newest;
    char current[12];
    FileState state;
    uint32_t nWrites;
};

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
int Backup_init(Backup *info);
void Backup_getInfo(Backup *info);
int Backup_store(GStatus *status);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */