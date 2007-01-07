// ------ serialLib.h ----

namespace laser2d {

// --------------------------------------------------------
extern int SerialWriteJEG(unsigned H,uchar *buff,int L,int *nw);
extern void CloseSerialPortJEG( unsigned *H) ;
extern int OpenSerialPortXJEG(char *name,int speed0, unsigned *H);
extern int SerialReadJEG(unsigned h, uchar *buff, int lMin, int lMax, int TimeOut);
extern int SerialWaitForCharJEG(unsigned h, uchar b ,int timeoutDs,int *ignoredChars );
extern int	ChangeSerialSpeedJEG(unsigned H,int speed);
extern int SerialReadUntilCharXJEG(unsigned h,uchar *string,int Lmax,uchar b,int timeoutDs,int *ignoredChars);
extern int CharsWaitingJEG(unsigned h);
extern int SerialWaitForCharXJEG(unsigned h, uchar b ,int timeoutDs,int *ignoredChars ) ;
// --------------------------------------------------------

}
