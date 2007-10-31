/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef __LOGTYPES_H
#define __LOGTYPES_H
namespace novatel{

enum BINARY_LOG_TYPE
{
  POSB_LOG_TYPE = 1,
  CLKB_LOG_TYPE = 2,
  TM1B_LOG_TYPE = 3,
  MKTB_LOG_TYPE = 4,
  MKPB_LOG_TYPE = 5,
  SPHB_LOG_TYPE = 6,
  DOPB_LOG_TYPE = 7,
  NAVB_LOG_TYPE = 8,
  DCSB_LOG_TYPE = 9,
  RTCM_LOG_TYPE = 10,
  RNGB_LOG_TYPE = 11,      /* obsolete */
  SATB_LOG_TYPE = 12,
  RCSB_LOG_TYPE = 13,
  REPB_LOG_TYPE = 14,
  RALB_LOG_TYPE = 15,
  IONB_LOG_TYPE = 16,
  UTCB_LOG_TYPE = 17,
  ALMB_LOG_TYPE = 18,
  CTSB_LOG_TYPE = 19,
  EPHB_LOG_TYPE = 20,
  SVPB_LOG_TYPE = 21,
  KPHB_LOG_TYPE = 22,
  RQGB_LOG_TYPE = 24,      /* obsolete */
  CMSB_LOG_TYPE = 25,      /* obsolete */
  PXYB_LOG_TYPE = 26,
  GGAB_LOG_TYPE = 27,
  SVCB_LOG_TYPE = 28,
  CONSOLEDATA_LOG_TYPE = 29,  /* bin logs to support      */
  COM1DATA_LOG_TYPE = 30,  /* pass through data logging  */
  COM2DATA_LOG_TYPE = 31,  /* by P. Fenton Mar 94      */
  RGEB_LOG_TYPE = 32,      /* new rngb that has new cstatus and is smaller */
  RGEC_LOG_TYPE = 33,      /* new compressed rngb that has new cstatus and is smaller */
  VLHB_LOG_TYPE = 34,      /* velocity, latency, heading */
  RT20B_LOG_TYPE = 35,     /* matched obs rt20 (posb) */
  SVDB_LOG_TYPE = 36,      /* another sat ECEF and other related range data */
  P20B_LOG_TYPE = 37,
  RTCAB_LOG_TYPE = 38,
  CDSB_LOG_TYPE = 39,      /* new version of CMSB with RTCA */
  MONB_LOG_TYPE = 40,
  RTCM3_LOG_TYPE = 41,
  RTCM9_LOG_TYPE = 42,
  RTCM16_LOG_TYPE = 43,
  RTCM59_LOG_TYPE = 44,
  PVLB_LOG_TYPE = 45,
  DDSB_LOG_TYPE = 46,
  VXYB_LOG_TYPE = 47,
  ETSB_LOG_TYPE = 48,
  PVAB_LOG_TYPE = 49,
  PAVB_LOG_TYPE = 50,
  CLMB_LOG_TYPE = 51,
  RBTB_LOG_TYPE = 52,      /* Raw navigation data */
  SBTB_LOG_TYPE = 53,      /* strange numbers for compatibility with GSV */
  FRMB_LOG_TYPE = 54,
  WBRB_LOG_TYPE = 55,
  RVSB_LOG_TYPE = 56,
  DLLB_LOG_TYPE = 57,
  VERB_LOG_TYPE = 58,

  BSLB_LOG_TYPE = 59,
  RPSB_LOG_TYPE = 60,
  RTKB_LOG_TYPE = 61,
  RTKOB_LOG_TYPE = 62,
  PRTKB_LOG_TYPE = 63,
  OPTB_LOG_TYPE = 64,
  RGED_LOG_TYPE = 65,      // Same as RGEC with a coded pseudorange SD
  RASB_LOG_TYPE = 66,

  WRCB_LOG_TYPE = 67,
  SNOB_LOG_TYPE = 68,

  RTCM1819_LOG_TYPE = 69,
  RTCM2021_LOG_TYPE = 70,
  RTCM22_LOG_TYPE = 71,
  ATTB_LOG_TYPE = 72,
  SBLB_LOG_TYPE = 73,
  AGCB_LOG_TYPE = 74,      // hidden
  ACPB_LOG_TYPE = 75,      // hidden

  FRWB_LOG_TYPE = 79,

  // LogPak PDC specific logs
  PDCB_LOG_TYPE = 76,
  PDCDBG1B_LOG_TYPE = 1999,

  // WAAS specific
  WALB_LOG_TYPE = 81,
  WUTCB_LOG_TYPE = 82,
  CORB_LOG_TYPE = 83,
  MPMB_LOG_TYPE = 95,
  CRLB_LOG_TYPE = 96,

  SFDB_LOG_TYPE = 98,

  // ISM Specific
  ISMRB_LOG_TYPE = 124,

  MSGB_LOG_TYPE = 1024,
  HDRB_LOG_TYPE = 1025,
  GRPB_LOG_TYPE = 1026,
  DIRB_LOG_TYPE = 1027,
  SCHB_LOG_TYPE = 1028,
  LPSTATUSB_LOG_TYPE = 1029,
  SITELOGB_LOG_TYPE = 1030,
  METB_LOG_TYPE = 1031,
  BATB_LOG_TYPE = 1032,
  PSNB_LOG_TYPE = 1033,
  PDCVERB_LOG_TYPE = 1034,
  STATUSB_LOG_TYPE = 1035,
  PROJECTB_LOG_TYPE = 1036,
  GROUPB_LOG_TYPE = 1037,

  DICB_LOG_TYPE = 15400,
  TAGB_LOG_TYPE = 15401,

  // Add new logs here synchronizing numbers with GPSCARD (MINOS1&2) tree


  // leaving the type number for the old RBT, SBT, and FRM binary logs the same.
  IRBTB_LOG_TYPE = 102,    /* Raw navigation data */
  ISBTB_LOG_TYPE = 103,    /* strange numbers for compatibility with GSV */
  IFRMB_LOG_TYPE = 104,

  // Softsurv format observation blocks
  SSOBS_L1L2_LOG_TYPE = 20001,
  SSOBS_L1_LOG_TYPE = 10001,
  SSOBS_GISMO_LOG_TYPE = 10000,  // now obsolete

  // GLONASS logs
  GEPB_LOG_TYPE = 77,
  GALB_LOG_TYPE = 78,
  CALB_LOG_TYPE = 87,
  GCLB_LOG_TYPE = 88,

  // OEM4 logs
  IONUTCB_LOG_TYPE = 8 ,
  CLOCKMODELB_LOG_TYPE = 16 ,
  RAWGPSSUBFRAMEB_LOG_TYPE = 25 ,
  CHANDEBUGB_LOG_TYPE = 32 ,
  VERSIONB_LOG_TYPE = 37 ,
  RAWEPHEMB_LOG_TYPE = 41 ,
  BESTPOSB_LOG_TYPE = 42 ,
  RANGEB_LOG_TYPE = 43 ,
  PSRPOSB_LOG_TYPE = 47 ,
  SATVISB_LOG_TYPE = 48 ,
  PROPAGATEDCLOCKMODELB_LOG_TYPE = 71 ,
  ALMANACB_LOG_TYPE = 73 ,
  RAWALMB_LOG_TYPE = 74 ,
  TRACKSTATB_LOG_TYPE = 83 ,
  SATSTATB_LOG_TYPE = 84 ,
  RXSTATUSB_LOG_TYPE = 93 ,
  RXSTATUSEVENTB_LOG_TYPE = 94 ,
  MATCHEDPOSB_LOG_TYPE = 96 ,
  BESTVELB_LOG_TYPE = 99 ,
  PSRVELB_LOG_TYPE = 100 ,
  TIMEB_LOG_TYPE = 101 ,
  TIMESYNCB_LOG_TYPE = 492 ,
  RANGEPNB_LOG_TYPE = 126 ,
  RXCONFIGB_LOG_TYPE = 128 ,
  RANGECMPB_LOG_TYPE = 140 ,
  RTKPOSB_LOG_TYPE = 141 ,
  NAVIGATEB_LOG_TYPE = 161 ,
  AVEPOSB_LOG_TYPE = 172 ,
  REFSTATIONB_LOG_TYPE = 175 ,
  PASSCOM1B_LOG_TYPE = 233 ,
  PASSCOM2B_LOG_TYPE = 234 ,
  PASSCOM3B_LOG_TYPE = 235 ,

  // Zeiss logs supported
  ZMESB_LOG_TYPE = 201,
  ZPOSB_LOG_TYPE = 202,
  ZSTNB_LOG_TYPE = 203,
  ZCFGB_LOG_TYPE = 204,
  ZEPHB_LOG_TYPE = 205,
  ZTAGB_LOG_TYPE = 206,

  //INS Logs
  BESTGPSPOSB_LOG_TYPE = 423,
  BESTGPSVELB_LOG_TYPE = 506,
  BESTLEVERARMB_LOG_TYPE = 507,
  INSATTB_LOG_TYPE = 263,
  INSATTSB_LOG_TYPE = 319,
  INSCOVB_LOG_TYPE = 264,
  INSCOVSB_LOG_TYPE = 320,
  INSPOSB_LOG_TYPE = 265,
  INSPOSSB_LOG_TYPE = 321,
  INSPVAB_LOG_TYPE = 507,
  INSPVASB_LOG_TYPE = 508,
  INSSPDB_LOG_TYPE = 266,
  INSSPDSB_LOG_TYPE = 323,
  INSVELB_LOG_TYPE = 267,
  INSVELSB_LOG_TYPE = 324,
  RAWIMUB_LOG_TYPE = 268,
  RAWIMUSB_LOG_TYPE = 325,
  VEHICLEBODYROTATIONB = 642,

};

typedef enum BINARY_LOG_TYPE BINARY_LOG_TYPE;

}//namespace

#endif
