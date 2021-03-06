/*
 *  --------------------------------------------------------------------------
 *
 *                                Framework RKH
 *                                -------------
 *
 *            State-machine framework for reactive embedded systems
 *
 *                      Copyright (C) 2010 Leandro Francucci.
 *          All rights reserved. Protected by international copyright laws.
 *
 *
 *  RKH is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any
 *  later version.
 *
 *  RKH is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with RKH, see copying.txt file.
 *
 *  Contact information:
 *  RKH site: http://vortexmakes.com/que-es/
 *  RKH GitHub: https://github.com/vortexmakes/RKH
 *  RKH Sourceforge: https://sourceforge.net/projects/rkh-reactivesys/
 *  e-mail: lf@vortexmakes.com
 *  ---------------------------------------------------------------------------
 */

/**
 *  \file       assert.c
 *  \brief      RKH assert function for STM32
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2017.04.14  DaBa  v2.4.05  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 *  DaBa  Dario Bali�a       db@vortexmakes.com
 */
/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include "rkh.h"
#include "bsp.h"
#include "trace_msd.h"

RKH_THIS_MODULE

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
rkh_assert(RKHROM char * const file, int line)
{
	/* Before to exit gets high trace output peripherals for
	 * complete stream transmission as soon as possible,
	 * and also handle assertion into peripheral's isr.
	 */
    /* UART output on UART5+DMA1_S7 */
	HAL_NVIC_SetPriority(UART5_IRQn, 0, 0);
    HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 0, 0);

    /* MassStorage file on USBOTG */
    HAL_NVIC_SetPriority(OTG_HS_IRQn, 0, 0);

    RKH_TR_FWK_ASSERT((RKHROM char *)file, line);
    rkh_fwk_exit();
	RKH_TRC_FLUSH();
	RKH_TRC_CLOSE();
    RKH_DIS_INTERRUPT();
	bsp_reset();
}

/* ------------------------------ File footer ------------------------------ */
