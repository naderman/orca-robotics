#ifndef ORCA2_LASER2D_SICK_ACFR_BASICS_H
#define ORCA2_LASER2D_SICK_ACFR_BASICS_H

namespace laser2d {

typedef  void*(*FunciThread)(void* )  ;


#define  uchar   unsigned char
#define  uint16b unsigned short int
#define  int16b  short int
#define  int16   short int
#define  uint    unsigned int
#define  uint8   uint8_t 
#define  uint16  uint16_t 
#define  uint8b  unsigned char
#define  uint32  unsigned int
#define  int32  int


//#define SIMU

#define PIs180  (3.141592/180.0)
struct threadData	{	int *pflag ; int priority ; } ;	
#define T5segs  50

struct TaskXX {	char active,priority,saving ; int xx[5] ; char name[15] ; char *DevSer ; } ;

#define XXprintf printf

extern void Msg(char *s) ;
extern void PushMsg(int h,int x,int y,char *msg);
extern void GlobalMsg(char *s);

// -------------------------------------------------------------------
extern char *GivemeStrEntry(char *head) ;
// -------------------------------------------------------------------
//extern char* SpecifySerialPortString(unsigned np) ;
//extern int LimiteInteger(int x,int x1,int x2) ;


// --------------------------------------------------------
#define xuint8   unsigned char 
#define xint8    signed   char 
#define xuint16  unsigned short int 
#define xint16   signed   short int  
#define xuint32  unsigned long int 
#define xint32   signed long int  
#define xuint    unsigned int  

// ...........................................

//#define _Millsecs2UteTimeUnits  10

//#include "..\includeL\common.h"
//#include "..\includeL\CmdServer.h" 
//#include "..\includeL\clx.h"
//#include "..\includeL\lib1.h"
//#include "..\includeL\printer_service.h" 
//#include "..\includeL\wvars.h"
//#include "..\includeL\wvars_x.h"
//#include "..\includeL\pt.h"
//#include "..\includeL\SequenceSrvc.h"
#include "seriallib.h" 
//#include "..\includeL\serial_libZ.h" 

} // namespace


#endif
