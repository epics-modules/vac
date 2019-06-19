/* devDigitelPump.c
 * DigitelPump Vacuum Gauge Controller
 * INP has form @asyn(port address) station
 * TYPE has ("devType")
 * port is the asyn serial port name
 * address is the controller address (0-255 for MPC/QPC)
 * station for Digitel500/1500 is no. of spts (0-3)
 * station for MPC is either 1 or 2 for the two pump controllers
 *         and 1 - 4 for the QPC
 * devType is "MPC" or "D500" or "D1500 or QPC"
 *
 *  Author: Mohan Ramanathan
 *  July 2007
 *  A common one for Digitel 500, Digitel 1500 and MPC/LPC/SPC/QPC
 *
 *   revision:  01
 *   01-07-2010   Fixed the code to work with D500 bitbus bug
 *
 *   revision: 02
 *   10-02-2014   Fixed connecton problems with MOXA terminal server
 *        will work correctly when MOXA is rebooted now
 *
 *   revision 03
 *   Marty Smith
 *   01-25-2018  Added QPC which has 4 setpoints and added MODL and VERS
 *      fields to the digitel record for MPC/QPC model and firmware vers
 *      this should also work for the QPC (model without Ethernet option).
 *      The record's TYPE field also shows the model, however, this relies
 *      on the proper substitution being made in the database (DEV) and is
 *      prone to human error.
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include <dbScan.h>
#include <dbDefs.h>
#include <dbAccess.h>
#include <dbCommon.h>
#include <alarm.h>
#include <errlog.h>
#include <link.h>
#include <recGbl.h>
#include <recSup.h>
#include <devSup.h>
#include <epicsString.h>
#include <asynDriver.h>
#include <asynEpicsUtils.h>
#include <asynOctet.h>
#include <digitelRecord.h>
#include <epicsExport.h>

#include "choiceDigitel.h"
#include "devDigitelPump.h"

#define DigitelPump_BUFFER_SIZE 330
#define DigitelPump_SIZE 50
#define DigitelPump_TIMEOUT 1.0
#define MAX_CONSEC_ERRORS 2

typedef struct devDigitelPumpPvt {
    asynUser    *pasynUser;
    asynOctet   *pasynOctet;
    void        *octetPvt;
    asynStatus  status;
    char        recBuf[DigitelPump_BUFFER_SIZE];
    char        sendBuf[DigitelPump_SIZE];
    char        *PortName;
    int         devType;
    cmdType     command;
    char        cmdPrefix[5];
    int         pumpNo;
    int         noSPT;
    int         cv1;
    int         cv2;
    int         errCount;
} devDigitelPumpPvt;


static long init(struct dbCommon *prec);
static long readWrite_dg(digitelRecord *pr);

static void buildCommand(devDigitelPumpPvt *pPvt, char *pvalue);

static void devDigitelPumpCallback(asynUser *pasynUser);
static void devDigitelPumpProcess(asynUser *pasynUser,
    char *readBuffer, int *nread);

static digiteldset devDigitelPump = {
    {
        5,
        NULL,
        NULL,
        init,
        NULL
    },
    readWrite_dg
};
epicsExportAddress(dset, devDigitelPump);


static long init(struct dbCommon *prec)
{
    digitelRecord *pr = (digitelRecord *) prec;
    asynUser *pasynUser=NULL;
    asynStatus status;
    asynInterface *pasynInterface;
    devDigitelPumpPvt *pPvt=NULL;
    DBLINK *plink  = &pr->inp;
    int address;
    char *port, *userParam;
    int station;

    /* Allocate private structure */
    pPvt = calloc(1, sizeof(devDigitelPumpPvt));

    /* Create an asynUser */
    pasynUser = pasynManager->createAsynUser(devDigitelPumpCallback, 0);
    pasynUser->userPvt = pr;

    /* Parse input link */
    status = pasynEpicsUtils->parseLink(pasynUser, plink, &port, &address,
        &userParam);
    if (status != asynSuccess) {
        errlogPrintf("devDigitelPump::init %s bad link %s\n",
            pr->name, pasynUser->errorMessage);
        goto bad;
    }

    strcpy(pPvt->cmdPrefix, "");
    pPvt->PortName = port;
    pPvt->devType = pr->type;
    sscanf(userParam, "%d", &station);
    if (pPvt->devType != 3) {
        pPvt->noSPT = 3;
    }
    else {
        pPvt->noSPT = 4; /* For QPC */
    }

    /* set ERR to non zero for initialization for Digitel 500/1500 */
    if (pPvt->devType && pPvt->devType != 3)
        pPvt->errCount = 3;

/*
 * devType needed to be set to device as follows in record "TYPE" field:
 *      MPC   = 0
 *      D500  = 1
 *      D1500 = 2
 *      QPC   = 3
 *
 *  for digitel 500/1500 the address is "0"
 *  for MPC and QPC the default address is 5 and has to be set between 0-255
 *
 */

/*
 *  All MPC and QPC commands are of the form :  "~ AA XX d cc\r"
 *    AA = Address from 00 - FF
 *    XX = 2 character Hex Command
 *    d  = parameter or data comma seperated
 *    cc = 2 character checksum Hex values
 *
 *  The checksum is to be calculated starting from the character after the
 *  start character and ending with the space after the data/parm field.
 *  Add the sum and divide by 0x100 or decimal 256 and reminder in hex is
 *  two character checksum ( just and it with 0xff). Follow the checksum
 *  with a terminator of CR only.
 *
 *  The QPC has 4 setpoints (stations) one for each pump
 */

/*
 *  All commands sent to digitel 500/1500 end with "\r"
 *  the read commands are "RD\r", "RC\r" and "RSx\r"
 *  the control commands are of the form:
 *  "Mx\r" where x is between 1-9.
 *  "SNPxxyy\r" where N is 1-3 P=1-4 and xxyy if the value.
 *
 *  The digitel 500 upon power up and also when communication is initiallized
 *  automatically sets it up to send exception reporting and timed reporting
 *  at 10 minute intervals.  These needs to be turned off with "SL3\r" and
 *  "SL4\r" commands.  We check for ERR and then send these reset commands.
 *
 *  Digitel 500/1500 uses  a unique scheme for data back.
 *  It echos all the characters sent and then starts with a linefeed.
 *  All monitors end with a linefeed & carriage return (\n\r).
 *  For control commands it ends with just "*".
 *
 *  In the case of BITBUS BUGS for D500 the device strips out the command echos.
 *  So the reply from the device in this case is no echos of commands.
 */

    /*  MPC and QPC controller */
    if ( pPvt->devType == 0 || pPvt->devType == 3) {
        if (address < 0 || address > 255) {
            errlogPrintf("devDigitelPump::init %s address out of range %d\n",
                pr->name, address);
            goto bad;
        }
        if (pPvt->devType == 0) {
            if (station < 1 || station > 2) {
                errlogPrintf("devDigitelPump::init %s MPC station out of range %d\n",
                    pr->name, station);
                goto bad;
            }
        }
        if (pPvt->devType == 3) {
            /* Station here refers to which setpoint in the QPC for this pump */
            if (station < 1 || station > 4) {
                errlogPrintf("devDigitelPump::init %s QPC station out of range %d\n",
                    pr->name, station);
                goto bad;
            }
        }

        sprintf(pPvt->cmdPrefix, "~ %02X", address);
        pPvt->pumpNo = station;
    }
    else {
        /*  Digitel 500/1500 controller*/
        /* Station here refers to the setpoint number used; 0 if not used otherwise
         * only 1 and 2 are valid for Digitels */
        if (station < 0 || station > 3) {
            errlogPrintf("devDigitelPump::init %s Digitel station out of range %d\n",
                pr->name, station);
            goto bad;
        }
        pPvt->noSPT = station;
    }

    status = pasynManager->connectDevice(pasynUser, pPvt->PortName, 0);
    if (status!=asynSuccess) {
        printf("can't connect to serial port %s %s\n", pPvt->PortName,
            pasynUser->errorMessage);
        goto bad;
    }

    pasynInterface = pasynManager->findInterface(pasynUser, asynOctetType, 1);
    if (!pasynInterface) {
        printf("%s driver not supported\n", asynOctetType);
        goto bad;
    }

    pPvt->pasynOctet = (asynOctet *)pasynInterface->pinterface;
    pPvt->octetPvt = pasynInterface->drvPvt;
    pPvt->pasynUser = pasynUser;
    pPvt->pasynOctet->flush(pPvt->octetPvt, pPvt->pasynUser);

    pr->dpvt = pPvt;

    asynPrint(pasynUser, ASYN_TRACE_FLOW,
        "devDigitelPump::init name=%s port=%s address=%d Device=%d station=%d\n",
        pr->name, pPvt->PortName, address, pPvt->devType, station);

    return 0;

bad:
    if (pasynUser)
        pasynManager->freeAsynUser(pasynUser);
    if (pPvt)
        free(pPvt);
    pr->pact = 1;
    return -1;
}


static void buildCommand(devDigitelPumpPvt *pPvt, char *pvalue)
{
    asynUser *pasynUser = pPvt->pasynUser;
    dbCommon *pr = (dbCommon *)pasynUser->userPvt;
    int chkSum =0;
    unsigned int i;

    memset(pPvt->sendBuf, 0, DigitelPump_SIZE);
    strcpy(pPvt->sendBuf, pPvt->cmdPrefix);
    strcat(pPvt->sendBuf, pvalue);

    /* For MPC and QPC only*/
    if (pPvt->devType == 0 || pPvt->devType == 3) {
        strcat(pPvt->sendBuf, " ");

        /* Now calculate the checksum. */
        for (i=1; i<strlen(pPvt->sendBuf); i++)
            chkSum += pPvt->sendBuf[i];
        chkSum &= 0xff;
        sprintf(&pPvt->sendBuf[i], "%2.2X", chkSum);
    }
    asynPrint(pPvt->pasynUser, ASYN_TRACEIO_DEVICE,
        "devDigitelPump::buildCommand %s len=%d string=|%s|\n",
        pr->name, (int) strlen(pPvt->sendBuf), pPvt->sendBuf);

    return;
}


static long readWrite_dg(digitelRecord *pr)
{
    devDigitelPumpPvt *pPvt = (devDigitelPumpPvt *)pr->dpvt;
    asynUser *pasynUser = pPvt->pasynUser;
    asynStatus status;
    char pvalue[30];
    int t1;
    int t2;
    int t3;
    int pumpType;
    float val1;
    float val2;

    /* Init this value first, it is checked later */
    memset(pvalue, 0, sizeof(pvalue));
    if (!pr->pact) {
        memset(pPvt->sendBuf, 0, DigitelPump_SIZE);
        /*
         *  We have to check to see if any record fields changed.
         *  We cannot send all the changes so for each processing only one command
         *  change will be sent.
         *  If no record change was done then read the values from the record....
         */
       if (pr->flgs) {
            /* This command for the display on the device was not implemented for the
             * QPC by the vendor but will be in firmware version 1.35; although for
             * the QPC it is not necessarily needed as the display shows everything
             * anyway.
             */
            if (pr->flgs & MOD_DSPL) { /* Change device display */
                if (pPvt->devType && pPvt->devType != 3){
                    sprintf(pvalue, "M%d", 3+pr->dspl);
                }
                else if(pPvt->devType == 0) {
                    /* command[0] = Change the displays main parameter on device */
                    sprintf(pvalue, " %s %d, %s", ctlCmdString[0],
                        pPvt->pumpNo, displayStr[pr->dspl]);
                }
                else { /* Don't use for QPC */
                    *pvalue = '\0';
                }
            }
            else if (pr->flgs & MOD_MODS) {
                /* Standby or Operate mode setting (Mode  Select) */
                if (pPvt->devType && pPvt->devType != 3)
                    sprintf(pvalue, "M%d", 1+pr->mods);
                else
                    sprintf(pvalue, " %s %d", ctlCmdString[1+pr->mods], pPvt->pumpNo);
            }
            else if (pr->flgs & MOD_KLCK) {
                /* Lock/unlock device keypad; don't use for the QPC model
                 * as it is for remote/local mode and you will loose remote
                 * mode if the keypad is locked
                 */
                if (pPvt->devType && pPvt->devType != 3) {
                    sprintf(pvalue, "M%d", 8+pr->klck);
                }
                else if (pPvt->devType == 0) { /* MPC ONLY */
                    /* Establish Local communication mode for QPC
                     * so never send this commnd to the QPC devType == 3
                     * will not allow remote communication to be established again
                     *
                     * Should this also allow the Digitel 500/1500 to use this command?
                     */
                    sprintf(pvalue, " %s %d", ctlCmdString[3+pr->klck], pPvt->pumpNo);
                }
                else { /* Don't use for QPC */
                    *pvalue = '\0';
                }
            }
            else if ( (pr->flgs & MOD_BAKE) && pr->bkin) {
                if (pPvt->devType && pPvt->devType != 3) {
                    sprintf(pvalue, "M%d", 7-pr->baks);
                }
                else { /* Don't use for QPC */
                    *pvalue = '\0';
                }
            }
            else if (pr->flgs & MOD_SETP) {
                /* Set point/hysteresis commands */
                if (pPvt->devType && pPvt->devType != 3) { /* Digitel ONLY */
                    switch (pPvt->noSPT) {
                    case 3:
                        if ((pr->spfg & MOD_SP3S) && pr->bkin) {
                            /* format Snmxe-0x converted to Snmxy */
                            sprintf(pvalue, "S31%.0e", pr->sp3s);
                            pvalue[4] = pvalue[7];
                            pvalue[5] = '\0';
                        }
                        else if ((pr->spfg & MOD_S3HS) && pr->bkin) {
                            sprintf(pvalue, "S32%.0e", pr->sp3s);
                            pvalue[4] = pvalue[7];
                            pvalue[5] = '\0';
                        }
                        else if ((pr->spfg & MOD_S3MS) && pr->bkin) {
                            sprintf(pvalue, "S33%d0%d0", pr->s3ms, 1-pr->s3vr);
                        }
                        else if ((pr->spfg & MOD_S3VS) && pr->bkin) {
                            sprintf(pvalue, "S33%d0%d0", pr->s3mr, 1-pr->s3vs);
                        }
                        else { /* prevent fall-through from above statements */

                    case 2:
                        if (pr->spfg & MOD_SP2S ) {
                            sprintf(pvalue, "S21%.0e", pr->sp2s);
                            pvalue[4] = pvalue[7];
                            pvalue[5] = '\0';
                        }
                        else if (pr->spfg & MOD_S2HS ) {
                            sprintf(pvalue, "S22%.0e", pr->s2hs);
                            pvalue[4] = pvalue[7];
                            pvalue[5] = '\0';
                        }
                        else if (pr->spfg & MOD_S2MS ) {
                            sprintf(pvalue, "S23%d0%d0", pr->s2ms, 1-pr->s2vr);
                        }
                        else if (pr->spfg & MOD_S2VS ) {
                            sprintf(pvalue, "S23%d0%d0", pr->s2mr, 1-pr->s2vs);
                        }
                        else { /* prevent fall-through from above statements */

                    case 1:
                        if (pr->spfg & MOD_SP1S ) {
                            sprintf(pvalue, "S11%.0e", pr->sp1s);
                            pvalue[4] = pvalue[7];
                            pvalue[5] = '\0';
                        }
                        else if (pr->spfg & MOD_S1HS ) {
                            sprintf(pvalue, "S12%.0e", pr->s1hs);
                            pvalue[4] = pvalue[7];
                            pvalue[5] = '\0';
                        }
                        else if (pr->spfg & MOD_S1MS ) {
                            sprintf(pvalue, "S13%d0%d0", pr->s1ms, 1-pr->s1vr);
                        }
                        else if (pr->spfg & MOD_S1VS ) {
                            sprintf(pvalue, "S13%d0%d0", pr->s1mr, 1-pr->s1vs);
                        }
                    }}} /* The above code may be an instance of Pigeon's Device:
                         * http://pigeonsnest.co.uk/stuff/pigeons-device.html */
                }
                else { /* Only for MPC and QPC */
                /* Odd pump number uses odd setpoint number for device
                 * Even pump number uses even setpoint number for device
                 * Set point command 3D variables
                 * Command: ~ ADDR 3D Setpoint,Pump#,XX,YY CRC\r
                 * From table: XX and YY are record field values for ON set
                 * point and OFF set point respectively.
                 * Pump#  Setpoint   XX      YY
                 *   1       1      sp1s    s1hr
                 *   2       2      sp2s    s2hr
                 * Pumps 3 & 4 are for QPC ONLY
                 *   3       3      sp3s    s3hr
                 *   4       4      sp4s    s4hr
                 *
                 * In the QPC user manual Rev G both the 3C and 3D commands were
                 * replaced by the 3B command which this driver currently does not
                 * support; however, the 3C & 3D commands I am told are still
                 * made available for bakward compatilibility.
                 *
                 * Now if we send a on/off set point we must send both and they
                 * must be in the range 1.00E-11 - 1.00E-04 for both. Also the
                 * off pressure must be 20% greater than the on pressure or the
                 * controller will automatically set the off pressure to 20%
                 * greater than the on pressure. THIS IS NOT CHECKED HERE.
                 * Because of the set point range noted above I added a check
                 * here to make sure that we don't send a bad set point value.
                 *
                 * Set points 1 & 2 are for MPC and QPC while set points 3 & 4
                 * are for the QPC ONLY.
                 */

               /* Set point 1 */
                    if (pr->spfg & MOD_SP1S) { /* ON Pressure */
                        t1 = pPvt->pumpNo; /* This is defined in the st.cmd */
                        if (pr->sp1s < 1e-4 || pr->sp1s > 1e-11){
                            val1 = pr->sp1s;
                            val2 = pr->s1hr;
                        }
                        else {
                            val1 = 0;
                        }
                    }
                    else if (pr->spfg & MOD_S1HS) { /* OFF Pressure */
                        t1 = pPvt->pumpNo;
                        if (pr->s1hs < 1e-4 || pr->s1hs > 1e-11){
                            val1 = pr->sp1r;
                            val2 = pr->s1hs;
                        }
                        else {
                            val1 = 0;
                        }
                    }

               /* Set point 2 */
                    else if (pr->spfg & MOD_SP2S) { /* ON Pressure */
                        if (pPvt->devType != 3) {
                            t1 = 2 + pPvt->pumpNo;
                        }
                        else {
                            t1 = pPvt->pumpNo;
                        }
                        if (pr->sp2s < 1e-4 || pr->sp2s > 1e-11){
                            val1 = pr->sp2s;
                            val2 = pr->s2hr;
                        }
                        else {
                            val1 = 0;
                        }
                    }
                    else if (pr->spfg & MOD_S2HS) { /* OFF Pressure */
                        if (pPvt->devType != 3) {
                            t1 = 2 + pPvt->pumpNo;
                        }
                        else {
                            t1 = pPvt->pumpNo;
                        }
                        if (pr->s2hs < 1e-4 || pr->s2hs > 1e-11){
                            val1 = pr->sp2r;
                            val2 = pr->s2hs;
                        }
                        else {
                            val1 = 0;
                        }
                    }

               /* Set point 3 */
                    else if ( (pr->spfg & MOD_SP3S)&&(pPvt->devType == 3) ) {
                        t1 = pPvt->pumpNo;
                        if (pr->sp3s < 1e-4 || pr->sp3s > 1e-11){
                            val1 = pr->sp3s;
                            val2 = pr->s3hr;
                        }
                        else {
                            val1 = 0;
                        }
                    }
                    else if ( (pr->spfg & MOD_S3HS)&&(pPvt->devType == 3) ) {
                        t1 = pPvt->pumpNo;
                        if (pr->s3hs < 1e-4 || pr->s3hs > 1e-11){
                            val1 = pr->sp3r;
                            val2 = pr->s3hs;
                        }
                        else {
                            val1 = 0;
                       }
                    }

               /* Set point 4 */
                    else if ( (pr->spfg & MOD_SP4S)&&(pPvt->devType == 3) ) {
                        t1 = pPvt->pumpNo;
                        if (pr->sp4s < 1e-4 || pr->sp4s > 1e-11){
                            val1 = pr->sp4s;
                            val2 = pr->s4hr;
                        }
                        else {
                            val1 = 0;
                        }
                    }
                    else if ( (pr->spfg & MOD_S4HS)&&(pPvt->devType == 3) ) {
                        t1 = pPvt->pumpNo;
                        if (pr->s4hs < 1e-4 || pr->s4hs > 1e-11){
                            val1 = pr->sp4r;
                            val2 = pr->s4hs;
                        }
                        else {
                            val1 = 0;
                        }
                    }
                    if (val1 != 0){
                        /* ctlCmdString[5] = Set supply set point */
                        sprintf(pvalue, " %s %d,%d,%7.1E,%7.1E", ctlCmdString[5],
                            t1, pPvt->pumpNo, val1, val2);
                    }
                    else {
                        val1 = 0;
                        asynPrint(pPvt->pasynUser, ASYN_TRACEIO_DEVICE,
                            "devDigitelPump::readWrite_dg Invalid set "
                            "point value for record %s SP%d \n",
                            pr->name, pPvt->pumpNo);
                    }
                }
                pr->spfg = 0;
            }
            pr->flgs = 0;

            /* Then send the command via buildCommand */
            pPvt->command = cmdControl;
            buildCommand(pPvt, pvalue);
        }

        else if (pPvt->errCount != 0 && pPvt->devType && pPvt->devType != 3) {
            /* if record has error for Digitel then send the reset command */
            pPvt->command = cmdReset;
        }
        else {
            pPvt->command = cmdRead;
        }

        asynPrint(pPvt->pasynUser, ASYN_TRACEIO_DEVICE,
              "devDigitelPump::readWrite_dg %s command %d len=%d string=|%s|\n",
              pr->name, pPvt->command, (int) strlen(pPvt->sendBuf), pPvt->sendBuf);

        pr->pact = 1;
        status = pasynManager->queueRequest(pasynUser, 0, 0);
        if (status != asynSuccess) {
            asynPrint(pasynUser, ASYN_TRACE_ERROR,
                "devDigitelPump::readWrite %s ERROR calling queueRequest\n"
                "    status=%d, error=%s\n",
                pr->name, status, pasynUser->errorMessage);
            status = -1;
            pr->pact = 0;
            recGblSetSevr(pr, COMM_ALARM, INVALID_ALARM);
        }
        return status;
    }

    /*
     * Now process the data back from the device during callback
     * check the error count.
     * if ERR = 0 then process normally.
     * If ERR > allowed Error then set the invalid read alarm and return.
     * if ERR > 0  & <= allowed Error then skip writing to fields.
     */
    pr->err = pPvt->errCount;
    if (pr->err > MAX_CONSEC_ERRORS) {
        recGblSetSevr(pr, READ_ALARM, INVALID_ALARM);
        pr->udf=0;
        return 0;
    }

    if ((pr->err > 0) && (pr->err <= MAX_CONSEC_ERRORS)) {
        pr->udf=0;
        return 0;
    }

/* now decode the returned data and load in database fields. */

/* set to safe value initially ( mostly 0) */
    pr->modr = 0;
    pr->cmor = 0;
    pr->bakr = 0;
    pr->set1 = 0;
    pr->set2 = 0;
    pr->set3 = 0;
    pr->set4 = 0;
    pr->ptyp = 0;
    pr->s1mr = 0;
    pr->s1vr = 1;
    pr->s2mr = 0;
    pr->s2vr = 1;
    pr->s3mr = 0;
    pr->s3vr = 1;
    pr->s3br = 0;
    pr->s4mr = 0;
    pr->s4vr = 1;
    pr->val = 9.9e+9;

/*  for Digitel commands */
    if (pPvt->devType && pPvt->devType != 3) {
        /*  Decode the time online, pump voltage & current */
        strncpy(pvalue, &pPvt->recBuf[0], 22);
        sscanf(pvalue, "%d %d:%d %lfV%leI", &t1, &t2, &t3, &pr->volt, &pr->crnt);
        /* Time online in minutes.*/
        pr->tonl = t1*1440 + t2*60 + t3;

        /*   Decode the status of High Voltage */
        if (pPvt->recBuf[23] == 'H')
            pr->modr = 1;

        /*   Decode the status of Cooldown */
        if (pPvt->recBuf[24] == 'C')
            pr->cmor = 1;

        /*   Decode the status of Bakeout option */
        if (pPvt->recBuf[25] == 'B')
            pr->bakr = 1;

        /*   Decode the status of setpoints if exists */
        if (pPvt->recBuf[26] == '1' && pPvt->noSPT >= 1)
            pr->set1 = 1;

        if (pPvt->recBuf[27] == '2' && pPvt->noSPT >= 2)
            pr->set2 = 1;

        if (pPvt->recBuf[28] == '3' && pPvt->noSPT == 3)
            pr->set3 = 1;

        /* Get the accumulated power, current, pumpsize  cooldown cycle */
        strncpy(pvalue, &pPvt->recBuf[30], 13);
        sscanf(pvalue, "%dP %dI %dC %dS", &t1, &t2, &t3, &pumpType);
        pr->accw = t1 * 0.444;
        pr->acci = t2 * 1.11;
        pr->cool = t3;

        /*  Check to make sure the pump size is correct.. */
        if (pumpType <1 || pumpType >32)
            pumpType =1;
        /* If controller is Digitel 1500 */
        if(pr->type ==2)
            pumpType *= 4;

        switch(pumpType) {
        case 1:
            pr->ptyp = 0;
            break;
        case 2:
            pr->ptyp = 1;
            break;
        case 4:
            pr->ptyp = 2;
            break;
        case 8:
            pr->ptyp = 3;
            break;
        case 16:
            pr->ptyp = 4;
            break;
        case 32:
            pr->ptyp = 5;
            break;
        }

        /*  calculate the pressure from the size of the pump. (Torr) */
        if ( (pr->modr == 1) && (pr->volt !=0) )
            pr->val = 0.005 * pr->crnt/pumpType;

        /*  Decode the Setpoints if enabled ..... */
        switch (pPvt->noSPT) {
        case 3:
            strncpy(pvalue, &pPvt->recBuf[120], 18);
            if (pvalue[0] == 'E' && pvalue[1] == 'R') {
                pr->bkin = 0;
            } else {
                pr->bkin = 1;
                sscanf(pvalue, "%le %le ", &pr->sp3r, &pr->s3hr);
                if (pvalue[14] == '1')
                    pr->s2mr = 1;
                if (pvalue[16] == '1')
                    pr->s2vr = 0;
                if (pvalue[17] == '1')
                    pr->s3br = 1;

                strncpy(pvalue, &pPvt->recBuf[139], 2);
                sscanf(pvalue, "%lf", &pr->s3tr);
            }

        case 2:
            strncpy(pvalue, &pPvt->recBuf[90], 18);
            sscanf(pvalue, "%le %le ", &pr->sp2r, &pr->s2hr);
            if (pvalue[14] == '1')
                pr->s2mr = 1;
            if (pvalue[16] == '1')
                pr->s2vr = 0;

        case 1:
            strncpy(pvalue, &pPvt->recBuf[60], 18);
            sscanf(pvalue, "%le %le ", &pr->sp1r, &pr->s1hr);
            if (pvalue[14] == '1')
                pr->s1mr = 1;
            if (pvalue[16] == '1')
                pr->s1vr = 0;
        }
    }

    else { /* for MPC and QPC */
        /*  get the status first and the mode and cool down mode properly. */
        strncpy(pvalue, &pPvt->recBuf[0], 20);
        if (strncmp(pvalue, "RUNNING", 7) == 0)
            pr->modr = 1;
        else if (strncmp(pvalue, "COOL DOWN", 9) ==0)
            pr->cmor = 1;

        /*  read the pressure */
        strncpy(pvalue, &pPvt->recBuf[30], 8);
        sscanf(pvalue, "%e", &val1);
        pr->val = val1;

        /*  read the current */
        strncpy(pvalue, &pPvt->recBuf[60], 7);
        sscanf(pvalue, "%e", &val2);
        pr->crnt = val2;

        /*  read the voltage */
        strncpy(pvalue, &pPvt->recBuf[90], 4);
        sscanf(pvalue, "%d", &t1);
        pr->volt = t1;

        /* When pump is turned off */
        if (pr->volt < 1000 && pr->crnt < 1e-6)
            pr->val = 9.9e9;

        /*
         * read the Pump Size - the record has a list of old pump sizes in
         * 30,60,120,220,400,700 l/s so change the value to fit one of these.
         *
         * For the QPC the pump size may be set on the front panel display
         * to any number up to 4 digits. Not sure at this time if it allows
         * 0000 - 9999 inclusive or not but there must be a finite number of
         * ion pump sizes.
         */
        strncpy(pvalue, &pPvt->recBuf[120], 4);
        sscanf(pvalue, "%d ", &pumpType);
        if (pumpType < 45)
            pr->ptyp = 0;
        else if (pumpType < 75)
            pr->ptyp = 1;
        else if (pumpType < 170)
            pr->ptyp = 2;
        else if (pumpType < 300)
            pr->ptyp = 3;
        else if (pumpType < 500)
            pr->ptyp = 4;
        else
            pr->ptyp = 5; /* This is 700 L/s in record dbd file */

        /* Even pump number goes with even setpoints */
        /* Odd pump number goes with odd setpoints */
        /*  read Setpoint 1 or 2 */
        /* Format is n,s,X.XE-XX,Y.YE-YY,ST
         * Where: n = Set point number (1-8)
         *        s = Supply number (1-4)
         * X.XXE-XX = On Point pressure
         * Y.YYE-YY = Off Point pressure
         *       ST = 1 (active) or 0 (inactive)
         */
        strncpy(pvalue, &pPvt->recBuf[150], 25);
        sscanf(pvalue, "%d,%d,%e,%e,%d", &t1, &t2, &val1, &val2, &t3);
        pr->sp1r = val1;
        pr->s1hr = val2;
        pr->set1 = t3;

        /*  read Setpoint 3 or 4 */
        strncpy(pvalue, &pPvt->recBuf[180], 25);
        sscanf(pvalue, "%d,%d,%e,%e,%d", &t1, &t2, &val1, &val2, &t3);
        pr->sp2r = val1;
        pr->s2hr = val2;
        pr->set2 = t3;

        /*  read Setpoint 5 or 6 */
        strncpy(pvalue, &pPvt->recBuf[210], 25);
        sscanf(pvalue, "%d,%d,%e,%e,%d", &t1, &t2, &val1, &val2, &t3);
        pr->sp3r = val1;
        pr->s3hr = val2;
        pr->set3 = t3;

        /* Read set point 4 */
        strncpy(pvalue, &pPvt->recBuf[240], 25);
        sscanf(pvalue, "%d,%d,%e,%e,%d", &t1, &t2, &val1, &val2, &t3);
        pr->sp4r = val1;
        pr->s4hr = val2;
        pr->set4 = t3;

        /* Currently not using setpoints 7 & 8 which are digital */

        /*  read Model Number */
        strncpy(pr->modl, &pPvt->recBuf[278], 4);
        /*  read Firmware Version */
        if (pPvt->devType == 3){
            strncpy(pr->vers, &pPvt->recBuf[318], 5);
        }
        else {
            strncpy(pr->vers, &pPvt->recBuf[318], 8);
        }
     }

    pr->lval = log10(pr->val);
    if (pr->val < 1e-12 )
        pr->lval = -12;
    pr->udf=0;

    return 0;
}


static void devDigitelPumpCallback(asynUser *pasynUser)
{
    dbCommon *pr = (dbCommon *)pasynUser->userPvt;
    devDigitelPumpPvt *pPvt = (devDigitelPumpPvt *)pr->dpvt;
    char readBuffer[DigitelPump_SIZE];
    char responseBuffer[DigitelPump_BUFFER_SIZE];
    rset *prset = (rset *) pr->rset;
    int i, nread;
    char pvalue[30]="";
    char *pstartdata=0;
    static const char *fn = "devDigitelPumpCallback";

    pPvt->pasynUser->timeout = DigitelPump_TIMEOUT;
    memset(responseBuffer, 0, DigitelPump_BUFFER_SIZE);

/*
 *  DigitelPump on normal cycle should get status from all the values.
 *  For commands issued the sendBuf will have a finite value.
 */

/*
 *  The digitel 500 upon power up and also when communication is initiallized
 *  automatically sets it up to send exception reporting and timed reporting
 *  at 10 minute intervals.  These needs to be turned off with SL3 and SL4
 *  commands. The SL commands reply with the standard "*" and in addition
 *  gives some bogues characters like 30 and sometimes #30 followed by LF CR.
 */

    /*  for sending write commands  */
    if (pPvt->command == cmdControl) {
        devDigitelPumpProcess(pasynUser, readBuffer, &nread);

        if (nread < 1 ) {
            asynPrint(pasynUser, ASYN_TRACE_ERROR,
                "%s %s Cmd reply too small=%d\n",
                fn, pr->name, nread);
            goto finish;
        }

        /* for Digitel */
        if (pPvt->devType && pPvt->devType != 3) {
            if (readBuffer[0] !='M' && readBuffer[0] != 'S') {
                asynPrint(pasynUser, ASYN_TRACE_ERROR,
                    "%s %s Cmd reply has error=[%s]\n",
                    fn, pr->name, readBuffer);
                recGblSetSevr(pr, READ_ALARM, INVALID_ALARM);
                goto finish;
            }
        }

        else { /* for MPC and QPC */
            if (readBuffer[3]!='O' || readBuffer[4] != 'K') {
                asynPrint(pasynUser, ASYN_TRACE_ERROR,
                    "%s %s Cmd reply has error=[%s]\n",
                    fn, pr->name, readBuffer);
                recGblSetSevr(pr, READ_ALARM, INVALID_ALARM);
                goto finish;
            }
        }
    }

    /*  for Digitel 500/1500 issue reset commands not for QPC */
    else if (pPvt->command == cmdReset && pPvt->devType && pPvt->devType != 3) {
        strcpy( pPvt->sendBuf, "SL3");
        devDigitelPumpProcess(pasynUser, readBuffer, &nread);
        strcpy( pPvt->sendBuf, "SL4");
        devDigitelPumpProcess(pasynUser, readBuffer, &nread);
    }

    /*   Now start the various reads ......
     *   make sure to check the devType and send the correct set of commands
     *   devType ->  0 = MPC , 1 & 2 Digitel 500 and 1500, 3 = QPC
     *   MPC has 8 commands while Digitel has either 2-5 based on noSPT
     *   The data will be packed into responseBuf

     *   The locations are as follows for MPC and QPC:
     *   responseBuf[0-29]    = Pump Status
     *   responseBuf[30-59]   = Pressure
     *   responseBuf[60-89]   = Current
     *   responseBuf[90-119]  = Voltage
     *   responseBuf[120-149] = Pump Size
     *   responseBuf[150-179] = SetPoint 1 or 2
     *   responseBuf[180-209] = SetPoint 2
     *   responseBuf[210-239] = SetPoint 3 or 4
     *   responseBuf[240-269] = SetPoint 4
     *   responseBuf[270-299] = Model Number
     *   responseBuf[300-319] = Firmware Version
     *
     *   The locations are as follows for Digitel:
     *   responseBuf[0-29]    = Pump Status
     *   responseBuf[30-59]   = AutoRun and Pump Size
     *   responseBuf[60-89]   = SetPoint 1
     *   responseBuf[90-119]  = SetPoint 2
     *   responseBuf[120-149] = SetPoint 3
     */

    /****************************************************
     * See devDigitelPump.h
     * This for loop is for the read commands
     * i    Command   Description
     * 0      OD        Get Supply Status
     * 1      0B        Read Pressure
     * 2      0A        Read Current
     * 3      0C        Read Voltage
     * 4      11        Get Pump Size
     * 5      3C        Get Setpoint
     * 6      3C        Get Setpoint
     * 7      3C        Get Setpoint
     * 8      01        Read Model Number
     * 9      02        Read Firmware Version Level
     *****************************************************/
    for (i=0; i<11; i++) {
        if (pPvt->devType && pPvt->devType != 3) {
            /* Skip once Digitel 500 & 1500 setpoint commands are done */
            if (i > pPvt->noSPT + 1)
                continue;

            /* for Digitel 500/1500 the read commands are offset by 10 */
            strcpy(pvalue, readCmdString[11 + i]);
        }
        else { /* For MPC and QPC commands */
            if ( i < 5){
                /* Good for the following commands:
                 * readCmdString[0] = 0D = Get Supply Status
                 * readCmdString[1] = 0B = Read Pressure
                 * readCmdString[2] = 0A = Read Current
                 * readCmdString[3] = 0C = Read Voltage
                 * readCmdString[4] = 11 = Get pump size
                 */
                sprintf(pvalue, " %s %d", readCmdString[i], pPvt->pumpNo);
            }
            else if (i > 4 && i < 9) {
                /* Good for the following commands:
                 * readCmdString[5] = 3C = Get Set point
                 * readCmdString[6] = 3C = Get Set point
                 * readCmdString[7] = 3C = Get Set point
                 * readCmdString[8] = 3C = Get Set point
                 */
                if (pPvt->devType == 0) {
                    if (i<8) {
                    /* Get the setpoint based on pump being odd/even for MPC */
                        sprintf(pvalue, " %s %d", readCmdString[i],
                            ((i-5)*2 + pPvt->pumpNo));
                    }
                }
                else { /* For QPC ONLY */
                    /* Here we have only 1 set point per pump so only get 1*/
                    if (i==5) {
                        sprintf(pvalue, " %s %d", readCmdString[i],
                            pPvt->pumpNo);
                    }
                }
            }
            else { /* For MPC & QPC model and version commands */
                if ((i > 8 || i < 11) &&
                    (pPvt->devType == 0 || pPvt->devType == 3)) {
                    /* Good for the following commands:
                     * readCmdString[9] = 01 = Get model number
                     * readCmdString[10] = 02 = Get firmware version
                     */
                    sprintf(pvalue, " %s", readCmdString[i]);
                }
            }
        }

        /* Don't send more than one 3C command at a time for a QPC pump */
        if (pPvt->devType == 3 && ((i<6)||(i>8)) ) {
            /* Build the command to send to the device */
            buildCommand(pPvt, pvalue);
            /* send and receive the command */
            devDigitelPumpProcess(pasynUser, readBuffer, &nread);
        }
        else if ( pPvt->devType != 3 && ((i>=6)||(i<=8)) ){  /* MPC */
            /* Build the command to send to the device */
            buildCommand(pPvt, pvalue);
            /* send and receive the command */
            devDigitelPumpProcess(pasynUser, readBuffer, &nread);
        }
        else if (pPvt->devType != 3 && ((i<6)||(i>8)) ) {
            /* Build the command to send to the device */
            buildCommand(pPvt, pvalue);
            /* send and receive the command */
            devDigitelPumpProcess(pasynUser, readBuffer, &nread);
        }

        if (nread < 1) {
            asynPrint(pasynUser, ASYN_TRACE_ERROR,
                "%s %s Read reply too small=%d\n",
                fn, pr->name, nread);
            pPvt->errCount++;
            goto finish;
        }

/*
 *     For MPC check whether status is "OK" for good or ER for error
 *     For Digitel sends out message with the words ERROR at the beginning
 *     Lets look for these and set the alarm on the record if problems.
 *
 *     For MPC lets strip the leading 9 characters all the way to "d"
 *     For MPC the reply "AA OK XX d cc\r" (letters are same as controls)
 *
 *     Digitel echos all the characters sent and then starts with a linefeed.
 *     For digitel the reply
 *		RD  = "RD\r\nDD HH:MM XXXXV x.xE-xI HCB123\n\r"
 *		RC  = "RC\r\nXXP XXI XC XS\n\r"
 *		RSx = "RSx\r\nX.0E-X Y.0E-Y ZZZZ HH\n\r"
 *
 *	For BITBUS and Digitel the driver does not receive the echos
 *		of the commands and strips the leading \n
 */
        /* for Digitel */
        if (pPvt->devType && pPvt->devType != 3) {
            if (readBuffer[4] =='E' || readBuffer[5] =='E' || readBuffer[0] =='E') {
                asynPrint(pasynUser, ASYN_TRACE_ERROR,
                    "%s %s Read reply has error=[%s]\n",
                    fn, pr->name, readBuffer);
                pPvt->errCount++;
                goto finish;
            }

            /*  find the echoed command followed by \n  (\r\n) */
            /*  No "R" as first character for bitbus case  */
            if (readBuffer[0] == 'R') {
                if ( i < 2 )
                    pstartdata = &readBuffer[4];
                else
                    pstartdata = &readBuffer[5];
            }
            else {
                pstartdata = &readBuffer[0];
            }
        }

        else { /* for MPC and QPC */
            if(readBuffer[3]=='O' && readBuffer[4] == 'K') {
                if (nread < 12 ) {
                    strcpy(readBuffer, "OK");
                    pstartdata = &readBuffer[0];
                }
                else {
                    /* strip off the header cmds */
                    pstartdata = &readBuffer[9];
                }
            }
            else {
                asynPrint(pasynUser, ASYN_TRACE_ERROR,
                    "%s %s Read reply has error=[%s]\n",
                    fn, pr->name, readBuffer);
                pPvt->errCount++;
                goto finish;
            }
        }
        asynPrint(pasynUser, ASYN_TRACEIO_DEVICE,
            "%s %s Read reply = [%s]\n",
            fn, pr->name, pstartdata);
        strcpy(&responseBuffer[30*i], pstartdata);
        strcpy(&pPvt->recBuf[30*i], pstartdata);
    }

    /* for successful read set err field=0 */
    pPvt->errCount=0;

finish:
    /*
     *  Process the record. This will result in the readWrite_dg routine
     *  being called again, but with pact=1
     */

    memset(pPvt->recBuf, 0, DigitelPump_BUFFER_SIZE);
    memcpy(pPvt->recBuf, responseBuffer, DigitelPump_BUFFER_SIZE);
    dbScanLock(pr);
    (*prset->process)(pr);
    dbScanUnlock(pr);
}


static void devDigitelPumpProcess(asynUser *pasynUser,
    char *readBuffer, int *nread)
{
    dbCommon *pr = (dbCommon *)pasynUser->userPvt;
    devDigitelPumpPvt *pPvt = (devDigitelPumpPvt *)pr->dpvt;
    size_t  nwrite;
    int eomReason;
    static const char *fn = "devDigitelPumpProcess";

    pPvt->pasynUser->timeout = DigitelPump_TIMEOUT;

    /*
     * These are set in startup file:
     * the default EOS character for output to device is "\r"
     *
     * the default EOS character for input from MPC is "\r"
     * the default EOS character for input from Digitel is "\n\r"
     */

    /* All QPC commands should be > 9 chars */
    if (strlen(pPvt->sendBuf) >= 10 && pPvt->devType == 3){
        pPvt->pasynOctet->flush(pPvt->octetPvt, pPvt->pasynUser);
        pPvt->status = pPvt->pasynOctet->write(pPvt->octetPvt, pasynUser,
            pPvt->sendBuf, strlen(pPvt->sendBuf), &nwrite);
        if (pPvt->status!=asynSuccess) {
            asynPrint(pasynUser, ASYN_TRACE_ERROR,
                "%s write failed %s Bytes read: %d, Reason: %d\n",
                fn, pasynUser->errorMessage, (int) nwrite, eomReason);
        }
        else {
            asynPrint(pasynUser, ASYN_TRACEIO_DEVICE,
                "%s %s nwrite=%d output=[%s]\n",
                fn, pr->name, (int) nwrite, pPvt->sendBuf);
        }

        pPvt->status = pPvt->pasynOctet->read(pPvt->octetPvt, pasynUser,
            readBuffer, DigitelPump_SIZE, &nwrite, &eomReason);
        *nread = nwrite;
        if (pPvt->status!=asynSuccess) {
            asynPrint(pasynUser, ASYN_TRACE_ERROR,
                "%s read failed %s Bytes read: %d, Reason: %d\n",
                fn, pasynUser->errorMessage, *nread, eomReason);
        }
        else {
            asynPrint(pasynUser, ASYN_TRACEIO_DEVICE,
                "%s %s nread=%d input=[%s]\n",
                fn, pr->name, *nread, readBuffer);
        }
    }
    else if(strlen(pPvt->sendBuf) < 10 && pPvt->devType == 3) {
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
            "%s sendBuf too small %s command=[%s]\n",
            fn, pasynUser->errorMessage, pPvt->sendBuf);
    }
    else if (pPvt->devType != 3){
        pPvt->pasynOctet->flush(pPvt->octetPvt, pPvt->pasynUser);
        pPvt->status = pPvt->pasynOctet->write(pPvt->octetPvt, pasynUser,
            pPvt->sendBuf, strlen(pPvt->sendBuf), &nwrite);
        if (pPvt->status!=asynSuccess) {
            asynPrint(pasynUser, ASYN_TRACE_ERROR,
                "%s write failed %s Bytes read: %d, Reason: %d\n",
                fn, pasynUser->errorMessage, (int) nwrite, eomReason);
        }
        else {
            asynPrint(pasynUser, ASYN_TRACEIO_DEVICE,
                "%s %s nwrite=%d output=[%s]\n",
                fn, pr->name, (int) nwrite, pPvt->sendBuf);
        }

        pPvt->status = pPvt->pasynOctet->read(pPvt->octetPvt, pasynUser,
            readBuffer, DigitelPump_SIZE, &nwrite, &eomReason);
       *nread = nwrite;
       if (pPvt->status!=asynSuccess) {
            asynPrint(pasynUser, ASYN_TRACE_ERROR,
                "%s read failed %s Bytes read: %d, Reason: %d\n",
                fn, pasynUser->errorMessage, *nread, eomReason);
        }
        else {
            asynPrint(pasynUser, ASYN_TRACEIO_DEVICE,
                "%s %s nread=%d input=[%s]\n",
                fn, pr->name, *nread, readBuffer);
        }
    }

    *nread = nwrite;
    asynPrint(pasynUser, ASYN_TRACEIO_DEVICE,
        "%s %s nread=%d input=[%s]\n",
        fn, pr->name, *nread, readBuffer);
}

