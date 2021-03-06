/**
 *  \file       bsp.c
 *  \brief      BSP for 80x86 OS win32
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.23  DaBa  v0.0.01  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Bali�a       db@vortexmakes.com
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include "rkh.h"
#include "bsp.h"
#include "getopt.h"
#include "trace_io_cfg.h"
#include "wserial.h"
#include "wserdefs.h"

#include "signals.h"
#include "modcmd.h"
#include "modmgr.h"
#include "conmgr.h"
#include "modpwr.h"
#include "mTime.h"
#include "gps.h"
#include "dIn.h"
#include "dOut.h"
#include "tplfsm.h"
#include "genled.h"
#include "gsmlog.h"

RKH_THIS_MODULE

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
#define ESC                 0x1B
#define TRK_CFG_OPTIONS     "st:f:p:m:g:c:h"

#define YIPIES_TEST_FRAME   "!0|12359094043105600,120000,-38.0050660,-057.5443696," \
							"000.000,000,050514,00FF,0000,00,00,FFFF,FFFF,FFFF,+0"

#define TEST_TX_PACKET       YIPIES_TEST_FRAME
#define TEST_RX_PACKET      "o---- Pong"

#define NUM_AN_SAMPLES_GET  10
#define NUM_DI_SAMPLES_GET  8

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
SERIAL_T serials[ NUM_CHANNELS ] =
{
	{	"COM1",	115200, 8, PAR_NONE, STOP_1, 0 },
	{	"COM2",	9600, 8, PAR_NONE, STOP_1, 0 },
	{   "COM3",	9600, 8, PAR_NONE, STOP_1, 0 }
};

/* ---------------------------- Local variables ---------------------------- */
static rui8_t bsp;
static ModCmdRcvHandler gsmCmdParser;
static GpsRcvHandler    gpsParser;
static SIMSelect_t      simSelect;
static char *opts = (char *)TRK_CFG_OPTIONS;
static const char *helpMessage =
{
    "\nOption usage:\n"
	"\t -s silence\n"
    "\t -f File name for binary trace output\n"
    "\t -t ipaddr of TCP trace client\n"
    "\t -p port of TCP trace client\n"
    "\t -m GSM Module Serial Port\n"
    "\t -g GPS Module Serial Port\n"
	"\t -c Tpsens Serial Port\n"
    "\t -h (help)\n"
};

static RKH_ROM_STATIC_EVENT(e_Term, evTerminate);
static RKH_ROM_STATIC_EVENT(e_Open, evOpen);
static RKH_ROM_STATIC_EVENT(e_Close, evClose);
static RKH_ROM_STATIC_EVENT(e_Ok, evOk);
static RKH_ROM_STATIC_EVENT(e_Recv, evRecv);
static SendEvt e_Send;

static void gsm_rx_isr(unsigned char byte);
static void gsm_tx_isr(void);
static void gps_rx_isr(unsigned char byte);
static void gps_tx_isr(void);

void tplink_rx_isr(unsigned char byte);
void tplink_tx_isr(void);

static SERIAL_CBACK_T gsm_ser_cback =
{ gsm_rx_isr, NULL, NULL, gsm_tx_isr, NULL, NULL, NULL };
static SERIAL_CBACK_T gps_ser_cback =
{ gps_rx_isr, NULL, NULL, gps_tx_isr, NULL, NULL, NULL };
static SERIAL_CBACK_T tplink_cback =
{ tplink_rx_isr, NULL, NULL, tplink_tx_isr, NULL, NULL, NULL };

static char gpsCurrStatus = -1;

static char gsmDebug;


/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static void
printBanner(void)
{
    printf("Tracking.\n\n");
    printf("RKH version      = %s\n", RKH_RELEASE);
    printf("Port version     = %s\n", rkhport_get_version());
    printf("Port description = %s\n\n", rkhport_get_desc());
	printf("Description: \n\n"
		"Tracking y monitoreo remoto de vehicular\n");

    printf("1.- Press ESC to quit \n\n\n");
}

static void
processCmdLineOpts(int argc, char **argv)
{
    int c;

    while ((c = getopt(argc, argv, opts)) != EOF)
        switch (c)
        {
            case 'm':
                strcpy(serials[GSM_PORT].com_name, optarg);
                break;

            case 'g':
                strcpy(serials[GPS_PORT].com_name, optarg);
                break;

			case 'c':
				strcpy(serials[TPSENS_PORT].com_name, optarg);
				break;

			case 's':
                trace_io_silence();
				break;

            case 'f':
                trace_io_setFileName(optarg);
                break;

            case 't':
                trace_io_setTcpIpAddr(optarg);
                break;

            case 'p':
                trace_io_setTcpPort((short)atoi(optarg));
                break;

            case '?':
            case 'h':
                printf(helpMessage);
                break;
        }
}

static
void
gsm_rx_isr( unsigned char byte )
{
    gsmCmdParser(byte);
    gsmlog_write(byte);

	if(gsmDebug)
		putchar(byte);
}

static
void
gsm_tx_isr( void )
{
}

static
void
gps_rx_isr( unsigned char byte )
{
    if(gpsParser != NULL)
        gpsParser(byte);

//	putchar(byte);
}

static
void
gps_tx_isr( void )
{
}

/* ---------------------------- Global functions --------------------------- */
void
bsp_init(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printBanner();

    processCmdLineOpts(argc, argv);
	
    modPwr_init();
	dIn_init();
	dOut_init();
}

void
bsp_close(void)
{
    gsmlog_close();

    bsp_serial_close(GSM_PORT);
    bsp_serial_close(GPS_PORT);
    //bsp_serial_close(TPSENS_PORT);
}

void
bsp_keyParser(int c)
{
    switch(c)
    {
        case ESC:
            RKH_SMA_POST_FIFO(modMgr, &e_Term, &bsp);
            rkhport_fwk_stop();
            break;

		case 'v':
			gsmDebug ^= 1;
			break;

        case 'q':
            dOut_set(0, 1, DOUT_TIME(1000));
            break;

        case 'o':
            printf("Open GPRS Socket\r\n");
            RKH_SMA_POST_FIFO(conMgr, &e_Open, &bsp);
            break;

        case 'c':
            printf("Close GPRS Socket\r\n");
            RKH_SMA_POST_FIFO(conMgr, &e_Close, &bsp);
            break;

		case '1':
			printf("Module Power ON\r\n");
			modPwr_on();
			break;

		case '2':
			printf("Module Power OFF\r\n");
			modPwr_off();
			break;

        case 'r':
            printf("Read GPRS Socket\r\n");
            RKH_SMA_POST_FIFO(conMgr, &e_Recv, &bsp);
            break;

        case 's':
            RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &e_Send), evSend);
            e_Send.size = strlen(TEST_TX_PACKET);

            memcpy(e_Send.buf, (unsigned char *)TEST_TX_PACKET, e_Send.size);

            printf("Write GPRS Socket:\r\n");

            RKH_SMA_POST_FIFO(conMgr, RKH_UPCAST(RKH_EVT_T, &e_Send), &bsp);
            break;

        default:
            break;
    }
}

void
bsp_timeTick(void)
{
    mTime_tick();
	tplfsm_timer_isr();
}

void
bsp_serial_open(int ch)
{
    switch(ch)
    {
        case GSM_PORT:
            gsmCmdParser = ModCmd_init();
			init_serial_hard(ch, &gsm_ser_cback);
			connect_serial(ch);
			modPwr_off();
            gsmlog_open("gsmlog");
			break;

        case GPS_PORT:
            gpsParser = NULL;
			init_serial_hard(ch, &gps_ser_cback);
			connect_serial(ch);
			break;

		case TPSENS_PORT:
			init_serial_hard(ch, &tplink_cback);
			connect_serial(ch);
			break;

		default:
			break;
    }
        
    RKH_TR_FWK_ACTOR(&bsp, "bsp");
}

void
bsp_serial_close(int ch)
{
	disconnect_serial(ch);
	deinit_serial_hard(ch);
}

void
bsp_serial_puts(int ch, char *p)
{
    while(*p!='\0')
    {
        gsmlog_write(*p);
        tx_data(ch, *p);
        ++p;
    }
}

void
bsp_serial_putnchar(int ch, unsigned char *p, ruint ndata)
{
    while(ndata && (ndata-- != 0))
    {
        gsmlog_write(*p);
        tx_data(ch, *p);
        ++p;
    }
}

void 
bsp_GSMModemFound(void)
{
    printf("\r\nGSM Modem Found\r\n"); 

    set_led(LED_GSM, LSTAGE1);
}

void 
bsp_SIMReady(void)
{
    printf("\r\nGSM SIM Ready\r\n"); 

    set_led(LED_GSM, LSTAGE2);
}

void 
bsp_SIMSelect(SIMSelect_t sim)
{
    simSelect = sim;

    printf("\r\nGSM SIM %s\r\n", 
            simSelect == MainSIM ? "Main" : "Secondary");

    set_led(LED_SIM, simSelect ? LSTAGE2 : LSTAGE1);
}

void
bsp_SIMChange(void)
{
    simSelect = (simSelect == MainSIM) ? SecSIM : MainSIM;

    bsp_SIMSelect(simSelect);
}

void
bsp_serial_putchar(int ch, unsigned char c)
{
	if(ch == TPSENS_PORT)
        return;

    gsmlog_write(c);
    tx_data(ch, c);
}

void
bsp_regStatus(Status_t status)
{
    printf("\r\nGSM Network %s\r\n", 
            status == RegisteredSt ? "Registered" : "Unregistered");

    set_led(LED_GSM, status == RegisteredSt ? LSTAGE3 : LSTAGE2);
}

void 
bsp_netStatus(Status_t status)
{
    printf("\r\nGSM Socket %s\r\n", 
            status == ConnectedSt ? "Connected" : "Disconnected");

    set_led(LED_GSM, status == ConnectedSt ? LSTAGE4 : LSTAGE3);
}

void 
bsp_sendOk(void)
{
    //printf("\r\nGSM Socket Send Ok\r\n"); 
	printf("/"); 
    set_led(LED_GSM, LSTAGE4);
}

void 
bsp_sendFail(void)
{
    printf("\r\nGSM Socket Sending Failure\r\n"); 
    set_led(LED_GSM, LSTAGE4);
}

void 
bsp_recvFail(void)
{
    printf("\r\nGSM Socket Receiving Failure\r\n"); 
    set_led(LED_GSM, LSTAGE4);
}

void 
bsp_recvOk(void)
{
    //printf("\r\nGSM Socket Receive Ok\r\n"); 
	printf("."); 
    set_led(LED_GSM, LSTAGE4);
}

void 
bsp_GPSStatus(char status)
{
	if(status != gpsCurrStatus)
		printf("\r\nGPS %s\r\n", status == RMC_StatusValid ? "Active" : "Void" );

	gpsCurrStatus = status;
    set_led(LED_GPS, status == RMC_StatusValid ? LIT : LSTAGE2);
}

void
bsp_gpsParserHandler_set(void *p)
{
    gpsParser = (GpsRcvHandler)p;
}

/* ------------------------------ File footer ------------------------------ */
