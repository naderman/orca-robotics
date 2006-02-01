#ifndef TELEOP_EVDEVUTIL__H
#define TELEOP_EVDEVUTIL__H

// Functions to help debug things.
void printInputType( const char *dev );
void printDeviceInfo( const char *dev );

int devSupportsAbsoluteAxes( const char *dev, bool &supportsAbsoluteAxes );
int devIsUSB( const char *dev, bool &isUSB );

// returns 0 on success
// Caller should have allocated at least nameSize to name.
int devName( const char *dev, char *name, int nameSize=256 );

#endif
