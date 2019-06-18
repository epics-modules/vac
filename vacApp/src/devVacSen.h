/* devVacSen.h
*
*  Author: Mohan Ramanathan
*  July 2007  
*   A common one for GP307, GP350 and Televac MM200
*
*  July 2014
*   Added CC-10  from Televac to the code
*
*  March 2016
*   Added MX200  from televac
*
*/


typedef enum {cmdRead, cmdControl } cmdType;

static const char * const readCmdString[] = {
  /* GP307: */
    "PCS", "DGS", "DS IG1", "DS IG2", "DS CG1", "DS CG2", "", "", "", "",

  /* GP350: */
    "PC S", "DGS", "RD 1", "RD 2", "RD A", "RD B", "", "", "", "",

  /* MM200: */
    "RY", "R", "R", "R", "SP", "SP", "SP", "SP", "", "",

  /* CC10: */
    "S5", "S1", "R2", "R3", "R4", "", "", "", "", "",

  /* MX200: */
    "S5", "S1", "S1", "S1", "R7", "R7", "R7", "R7", "", ""
};

static const char * const ctlCmdString[] = {
  /* GP307: */
    "IG1 OFF", "IG1 ON", "IG2 OFF", "IG2 ON", "DG OFF", "DG ON","","","","",

  /* GP350: */
    "F1 0", "F1 1", "F2 0", "F2 1", "DG0 OFF", "DG1 ON","","","","",
};
