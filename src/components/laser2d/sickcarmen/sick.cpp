/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
/* AlexB: This code was copied directly from CARMEN, but modified over time */
/* Tobi: Modified to make it work for 500kbps over RS422 */

#include "carmen_conversion.h"

static char sick_info_message[5000];

/** Serial includes **/
#include <termios.h>

/*****  DIRK WAS HERE - START ******/
#include <sys/utsname.h>
#if __linux
#include <linux/serial.h>
#include <linux/version.h>
#endif
/*****  DIRK WAS HERE - END ******/

#include "sick.h"

/******************** Stuff imported from global carmen files ********************/

//#define sick_test_alloc(X) do {if ((void *)(X) == NULL) sick_die("Out of memory in %s, (%s, line %d).\n", __FUNCTION__, __FILE__, __LINE__); } while (0)

// void 
// sick_die(char* fmt, ...) {
//   va_list args;

//   va_start(args, fmt);
//   vfprintf(stderr, fmt, args);
//   va_end(args);
//   fflush(stderr);
//   exit(-1);
// }

/******************** end Stuff imported from global carmen files ********************/


/*****  DIRK WAS HERE - START ******/
/*extern void shutdown_laser(int x);*/
/*****  DIRK WAS HERE - END ******/

/* fancy serial functions */

int iParity(parity_t par)
{
  if(par == N)
    return(IGNPAR);
  else
    return(INPCK);
}

int iSoftControl(int flowcontrol)
{
  if(flowcontrol)
    return(IXON);
  else
    return(IXOFF);
}

int cDataSize(int numbits)
{
  switch(numbits) {
  case 5:
    return(CS5);
    break;
  case 6:
    return(CS6);
    break;
  case 7:
    return(CS7);
    break;
  case 8:
    return(CS8);
    break;
  default:
    return(CS8);
    break;
  }
}

int cStopSize(int numbits)
{
  if(numbits == 2)
    return(CSTOPB);
  else
    return(0);
}

int cFlowControl(int flowcontrol)
{
  if(flowcontrol)
    return(CRTSCTS);
  else
    return(CLOCAL);
}

int cParity(parity_t par)
{
  if(par != N) {
    if(par == O)
      return(PARENB | PARODD);
    else
      return(PARENB);
  }
  else
    return(0);
}

int cBaudrate(int baudrate)
{
  switch(baudrate) {
  case 0:
    return(B0);
    break;
  case 300:
    return(B300);
    break;
  case 600:
    return(B600);
    break;
  case 1200:
    return(B1200);
    break;
  case 2400:
    return(B2400);
    break;
  case 4800:
    return(B4800);
    break;
  case 9600:
    return(B9600);
    break;
  case 19200:
    return(B19200);
    break;
  case 38400:
    return(B38400);
    break;
  case 57600:
    return(B57600);
    break;
  case 115200:
    return(B115200);
    break;
  case 500000:
    /* to use 500k you have to change the entry of B460800 in you kernel:
       /usr/src/linux/drivers/usb/serial/ftdi_sio.h:
       ftdi_8U232AM_48MHz_b460800 = 0x0006    */
    return(B460800);
    break;
  default:
    return(B9600);
    break;
  }
}

void sick_set_serial_params(sick_laser_p laser)
{
    // printf("TRACE(sick_set_serial_params)\n");
    struct termios  ctio;
  
  tcgetattr(laser->dev.fd, &ctio); /* save current port settings */
  ctio.c_iflag = iSoftControl(laser->dev.swf) | iParity(laser->dev.parity);
  ctio.c_oflag = 0;
  ctio.c_cflag = CREAD | cFlowControl(laser->dev.hwf || laser->dev.swf) |
    cParity(laser->dev.parity) | cDataSize(laser->dev.databits) | 
    cStopSize(laser->dev.stopbits);
  ctio.c_lflag = 0;
  ctio.c_cc[VTIME] = 0;     /* inter-character timer unused */
  ctio.c_cc[VMIN] = 0;      /* blocking read until 0 chars received */
  cfsetispeed(&ctio, (speed_t)cBaudrate(laser->dev.baudrate));
  cfsetospeed(&ctio, (speed_t)cBaudrate(laser->dev.baudrate));
  tcflush(laser->dev.fd, TCIFLUSH);
  tcsetattr(laser->dev.fd, TCSANOW, &ctio);
}

/*****  DIRK WAS HERE - START ******/

int
kernel_minimum_version( int a, int b, int c )
{
  struct utsname        uts;
  int                   ca, cb, cc; 
  uname(&uts);
  sscanf( uts.release, "%d.%d.%d", &ca, &cb, &cc );
  if (ca*65536+cb*256+cc>=a*65536+b*256+c) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int sick_set_baudrate(sick_laser_p laser, int brate )
{    
    struct termios  ctio;
  
#if __linux
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,20)
  struct serial_struct  serinfo;
#endif
#endif /* __linux */
  
  tcgetattr(laser->dev.fd, &ctio); /* save current port settings */
  
#if __linux
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,20)

  if (brate==500000 && kernel_minimum_version(2,4,20)) {
    cfsetispeed ( &ctio, (speed_t) cBaudrate(38400) );
    cfsetospeed ( &ctio, (speed_t) cBaudrate(38400) );
    
    serinfo.reserved_char[0] = 0;
    if (ioctl(laser->dev.fd, TIOCGSERIAL, &serinfo) < 0) 
    {
        sprintf(sick_info_message, "%s: cannot get serial info", sick_info_message);
        close(laser->dev.fd);
        return -1;
    }
    
    // Tobi: Changing this line made the 500kps mode work!
    // from: 
    // serinfo.flags = (serinfo.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
    // to:
    serinfo.flags |= ASYNC_SPD_CUST;
    serinfo.custom_divisor =  serinfo.baud_base / brate;
    
    if (ioctl(laser->dev.fd, TIOCSSERIAL, &serinfo) < 0) 
    {
        sprintf(sick_info_message, "%s: cannot set serial info", sick_info_message);
        close(laser->dev.fd);
        return -1;
    }
  } else {
    
    cfsetispeed ( &ctio, (speed_t) cBaudrate(brate) );
    cfsetospeed ( &ctio, (speed_t) cBaudrate(brate) );
    
    serinfo.reserved_char[0] = 0;
    if (ioctl(laser->dev.fd, TIOCGSERIAL, &serinfo) < 0) 
    {
        sprintf(sick_info_message, "%s: cannot get serial info", sick_info_message);
        close(laser->dev.fd);
        return -1;
    }
    
    serinfo.flags &= ~ASYNC_SPD_CUST;
    serinfo.custom_divisor = 0;
    
    if (ioctl(laser->dev.fd, TIOCSSERIAL, &serinfo) < 0) 
    {
        sprintf(sick_info_message, "%s: cannot set serial info", sick_info_message);
        close(laser->dev.fd);
        return -1;
    }
    
  }
#else 
  cfsetispeed(&ctio, (speed_t)cBaudrate(brate));
  cfsetospeed(&ctio, (speed_t)cBaudrate(brate));
#endif
#endif /* __linux */
  tcflush(laser->dev.fd, TCIFLUSH);
  tcsetattr(laser->dev.fd, TCSANOW, &ctio);

  return 0;
}

/*****  DIRK WAS HERE - END ******/

int sick_serial_connect(sick_laser_p laser)
{
  if((laser->dev.fd = open(laser->dev.ttyport.c_str(), O_RDWR | O_NOCTTY, 0)) < 0)
    return(-1);
  sick_set_serial_params(laser);
  return(laser->dev.fd);
}

/* sick_compute_checksum - Compute the CRC checksum of a segment of data. */

static int sick_compute_checksum(unsigned char *CommData, int uLen)
{
  unsigned char abData[2] = {0, 0}, uCrc16[2] = {0, 0};
  
  while(uLen--) {
    abData[0] = abData[1];
    abData[1] = *CommData++;
    if(uCrc16[0] & 0x80) {
      uCrc16[0] <<= 1;
      if(uCrc16[1] & 0x80)
        uCrc16[0] |= 0x01;
      uCrc16[1] <<= 1;
      uCrc16[0] ^= CRC16_GEN_POL0;
      uCrc16[1] ^= CRC16_GEN_POL1;
    } 
    else {
      uCrc16[0] <<= 1;
      if(uCrc16[1] & 0x80)
        uCrc16[0] |= 0x01;
      uCrc16[1] <<= 1;
    }
    uCrc16[0] ^= abData[0];
    uCrc16[1] ^= abData[1];
  }
  return (((int)uCrc16[0]) * 256 + ((int)uCrc16[1]));
}

int sick_read_data(sick_laser_p laser, unsigned char *data, double timeout)
{
  static int val, i, j, l, pos, chk1, chk2;
  double start_time = carmen_get_time_ms();

  l    = BUFFER_SIZE;
  pos  = 0;
  chk1 = FALSE;
  chk2 = FALSE;
  while(carmen_get_time_ms() - start_time < timeout) {
    val = carmen_serial_numChars(laser->dev.fd);
    if(val > 0) {
      if(pos + val >= BUFFER_SIZE)
	return(0);
      if(pos + val >= l + 6)
	val = l + 6 - pos;
      read(laser->dev.fd, &(data[pos]), val);
      pos += val;
      if(!chk1 && pos > 2) {
	if(data[0] != STX || data[1] != LID) {
	  for(i = 1; i < pos - 1; i++) {
	    if(data[i] == STX && data[i+1] == LID) {
	      for(j = i; j < pos; j++) {
		data[j - i] = data[j];
	      }
	      pos -= i;
	      chk1 = TRUE;
	      break;
	    }
	  }
	  if(!chk1)
	    pos = 0;
	} 
	else
	  chk1 = TRUE;
      }
      if(!chk2 && pos > 4) {
	l = data[3] * 256 + data[2];
	chk2 = TRUE;
      }
    }
    if(pos == l + 6)
      return(l + 6);
    usleep(1000);
  }
  return(0);
}

int sick_write_command(sick_laser_p laser, unsigned char command,
		       unsigned char *argument, int arg_length)
{
  unsigned char  buffer[MAX_COMMAND_SIZE];
  int pos = 0, i, check, length, loop, answer = 0, counter = 0;
  int val = 0;

  /* SYNC CHARS */
  buffer[pos++] = 0x02;
  /* ADDRESS    */
  buffer[pos++] = 0x00;
  /* MESSAGE LENGTH */
  length = 1 + arg_length;
  buffer[pos++] = length & 0x00ff;
  buffer[pos++] = length / 256;	/* I wonder if that works */
  /* COMMAND */
  buffer[pos++] = command;
  /* ARGUMENT */
  if(arg_length > 0)
    for(i=0; i < arg_length; i++)
      buffer[pos++] = argument[i];
  /* CHECKSUM */
  check = sick_compute_checksum(buffer, length + 4);
  buffer[pos++] = check & 0x00ff;
  buffer[pos++] = check / 256;
  carmen_serial_writen(laser->dev.fd, (char*)buffer, pos);

  /* wait for acknowledgement */
  loop = 1;
  answer = INI;
  while(loop) {
    counter++;
    val = carmen_serial_numChars(laser->dev.fd);
    if(val > 0) {
      read(laser->dev.fd, &buffer, val);
      switch(buffer[0]) {
      case 0x02:
	answer = STX;
	break;
      case 0x06:
	answer = ACK;
	break;
      case 0x10:
	answer = DLE;
	break;
      case 0x15:
	answer = NAK;
	break;
      default:
	answer = UKN;
      }
      loop = 0;
    } 
    else if(counter > 5) {
      answer = TIO;
      loop = 0;
    }
    usleep(1000); 
  }
  return answer;
}

void sick_request_status(sick_laser_p laser)
{
  sick_write_command(laser, 0x31, NULL, 0);
}

void sick_request_sensor(sick_laser_p laser)
{
  static unsigned char args[2] = {0x05, 0xb4};

  sick_write_command(laser, 0x30, args, 2);
}

int sick_set_laser_baudrate(sick_laser_p laser, int brate)
{
  unsigned char args[1];
  int result;

  if(brate == 500000)
    args[0] = 0x48;
  else if(brate == 38400)
    args[0] = 0x40;
  else if(brate == 19200)
    args[0] = 0x41;
  else
    args[0] = 0x42;
  result = sick_write_command(laser, 0x20, args, 1);
  return (result == ACK);
}

int sick_set_config_mode(sick_laser_p laser)
{ 
  unsigned char data[MAX_COMMAND_SIZE], args[MAX_COMMAND_SIZE];
  int i, result;

  for(i = 0; i < 8; i++)
    args[i + 1] = (unsigned char)laser->dev.passwd[i];
  args[0] = 0x00;
  carmen_serial_ClearInputBuffer(laser->dev.fd);
  result = sick_write_command(laser, 0x20, args, 9);
  if(result == ACK)
    return(sick_read_data(laser, data, MAX_TIME_FOR_CONFIG));
  else
    return(FALSE);
}

int sick_set_lms_resolution(sick_laser_p laser)
{
    unsigned char args[4];
    int result;

    if(laser->settings.angle_range == 100) {
        args[0] = 0x64; args[1] = 0x00;
        if(laser->settings.angle_resolution == RES_1_00_DEGREE) {
            args[2] = 0x64; args[3] = 0x00;
        } 
        else if(laser->settings.angle_resolution == RES_0_50_DEGREE) {
            args[2] = 0x32; args[3] = 0x00;
        } 
        else {
            args[2] = 0x19; args[3] = 0x00;
        }
    } 
    else {
        args[0] = 0xB4; args[1] = 0x00;
        if(laser->settings.angle_resolution == RES_1_00_DEGREE) {
            args[2] = 0x64; args[3] = 0x00;
        } else {
            args[2] = 0x32; args[3] = 0x00;
        }
    }
    result = sick_write_command(laser, 0x3B, args, 4);
    return(result == ACK);
}

int sick_request_lms_config(sick_laser_p laser)
{
  int result;
  
  result = sick_write_command(laser, 0x74, NULL, 0);
  return(result == ACK);
}

int sick_set_lms_config(sick_laser_p laser, unsigned char *data, int len)
{
  int result;

  if(len == 32) {
    result = sick_write_command(laser, 0x77, data, len);
    return(result == ACK);
  } 
  else
    return(FALSE);
}

int sick_parse_conf_data(sick_laser_p laser, unsigned char *buf, int length)
{
  int check, i;
  unsigned char data[32];
  double start_time = carmen_get_time_ms();
  int ret;
  
  if(length < 4)
    return(FALSE);
  else
    check = sick_compute_checksum(buf, length - 2);
  if((length != 42 && length != 40) || buf[4] != 0xF4 ||
     buf[length - 2] != (check & 0x00ff) || buf[length - 1] != (check / 256))
    return(FALSE);
  for(i = 0; i < 32; i++)
    data[i] = buf[i + 5];
  if((laser->settings.range_res == CM && data[6] != 0) ||
     (laser->settings.range_res == MM && data[6] != 1) ||
     (laser->settings.range_res == DM && data[6] != 2) ||
     (laser->settings.range_dist == RANGE80M  && data[5] != 1) ||
     (laser->settings.range_dist == RANGE160M && data[5] != 3) ||
     (laser->settings.range_dist == RANGE320M && data[5] != 5)) {

    /* Set LASER in config-mode ... */
    do {} while ( (!(ret=sick_set_config_mode(laser))) && (carmen_get_time_ms()-start_time < MAX_TIME_FOR_CONFIG) );
    if ( ret == FALSE )
    {
        sprintf(sick_info_message,"%s\n\tCouldn't enter config mode",sick_info_message);
        return FALSE;
    }

    /* fix the laser sunlight problem */
    //    data[4] |= 1;

    switch(laser->settings.range_dist) {
    case RANGE80M:
      data[5] = 1; 
      break;
    case RANGE160M:
      data[5] = 3;
      break;
    case RANGE320M:
      data[6] = 5;
      break;
    default:
      data[6] = 1;
      break;
    }
    switch(laser->settings.range_res) {
    case CM:
      data[6] = 0;
      break;
    case MM:
      data[6] = 1;
      break;
    case DM:
      data[6] = 2;
      break;
    default:
      data[6] = 0;
      break;
    }
    /*     Set LMS configuration ... */
    start_time = carmen_get_time_ms();
    do {} while (!(ret=sick_set_lms_config(laser, data, 32)) && (carmen_get_time_ms()-start_time < MAX_TIME_FOR_CONFIG));
    if ( ret == FALSE )
    {
        sprintf(sick_info_message,"%s\n\tCouldn't set lms config",sick_info_message);
        return FALSE;
    }
  }
  return(TRUE);
}

int sick_set_lms_range(sick_laser_p laser)
{
  int l = 0;
  unsigned char data[BUFFER_SIZE];

  if(sick_request_lms_config(laser)) {
    usleep(100000);
    l = sick_read_data(laser, data, MAX_TIME_FOR_GETTING_CONF);
    return(sick_parse_conf_data(laser, data, l));
  } 
  else
    return(FALSE);
}

int sick_start_continuous_mode(sick_laser_p laser)
{
  unsigned char lmsarg[1] = {0x24}, pls180arg[1] = {0x20};
  unsigned char pls360arg[1] = {0x24};
  int result = 0;
  double start_time = carmen_get_time_ms();

  do {
    if(laser->settings.type == LMS)
      result = sick_write_command(laser, 0x20, lmsarg, 1);
    else if(laser->settings.type == PLS &&
	    laser->settings.angle_resolution == RES_1_00_DEGREE)
      result = sick_write_command(laser, 0x20, pls180arg, 1);
    else if(laser->settings.type == PLS &&
	    laser->settings.angle_resolution == RES_0_50_DEGREE)
      result = sick_write_command(laser, 0x20, pls360arg, 1);

    if ( carmen_get_time_ms() - start_time > MAX_TIME_FOR_ACK )
        return -1;
  } while(result != ACK);
  return 0;
}

int sick_stop_continuous_mode(sick_laser_p laser)
{
  unsigned char args[1] = {0x25};
  int result;
  double start_time = carmen_get_time_ms();

  do {
    result = sick_write_command(laser, 0x20, args, 1);
    if ( carmen_get_time_ms() - start_time > MAX_TIME_FOR_ACK )
        return -1;
  } while(result != ACK);
  return 0;
}

/* sick_test_baudrate - Test a combination of baudrate and parity 
   of the laser. */

int sick_testBaudrate(sick_laser_p laser, int brate )
{
    // printf("TRACE(sick_testBaudrate)\n");
    
    unsigned char data[BUFFER_SIZE], ReqLaser[2] = {5, 180};
  int response;

  sick_set_baudrate(laser, brate);
  
  response = sick_write_command(laser, 0x30, ReqLaser, 2);
  if(response == NAK) {
      sprintf( sick_info_message, "%s\n\tsickTestBaudrate -- Could not send command to laser", sick_info_message );    
      return FALSE;
  }
  if(sick_read_data(laser, data, MAX_TIME_FOR_TESTING_BAUDRATE))
  {
      return TRUE;
  }

  sprintf( sick_info_message, "%s\n\tsickTestBaudrate -- Timeout trying to read baudrate", sick_info_message );
  return FALSE;
}

int sick_detect_baudrate(sick_laser_p laser)
{
    
    if(sick_testBaudrate(laser, 9600)) {
      sprintf( sick_info_message, "%s\n\tdetected connected baudrate: 9600", sick_info_message );
      return(9600);
  } 
  else 
  {
    if(sick_testBaudrate(laser, 19200)) {
      sprintf( sick_info_message, "%s\n\tdetected connected baudrate: 19200", sick_info_message );
      return(19200);
    } 
    else 
    {
        if(sick_testBaudrate(laser, 38400)) {
            sprintf( sick_info_message, "%s\n\tdetected connected baudrate: 38400", sick_info_message );
            return(38400);
      } 
      else
      {
	       if(sick_testBaudrate(laser, 500000)) {
                sprintf( sick_info_message, "%s\n\tdetected connected baudrate: 500000", sick_info_message );
                    
                return(500000);
            } 
	        else 
            {
               sprintf( sick_info_message, "%s\n\tdetected connected baudrate failed", sick_info_message );
               return(0);
	        }
      }
    }
  }

}

int sick_check_baudrate(sick_laser_p laser, int brate)
{
  if(sick_testBaudrate(laser, brate)) {
    sprintf( sick_info_message, "%s\n\tsick_check_baudrate(%d) succeeded", sick_info_message, brate );
    return(TRUE);
  } 
  else {
    sprintf( sick_info_message, "%s\n\tsick_check_baudrate(%d) failed", sick_info_message, brate );
    return(FALSE);
  }
}

int sick_connect_device(sick_laser_p laser)
{
    // printf("TRACE(sick_connect_device)\n");

    sprintf(sick_info_message, "%s\tconnect TTY %-14s", sick_info_message, laser->dev.ttyport.c_str());
    
    sick_serial_connect(laser);
    if(laser->dev.fd == -1) 
    {
        sprintf( sick_info_message, "%s: failed.", sick_info_message );
        return -1;
    }
    sprintf(sick_info_message, "%s\n\tset port param %6d:%d%c%d", 
            sick_info_message,
            laser->dev.baudrate, laser->dev.databits,
            (laser->dev.parity == N ? 'N' : laser->dev.parity == E ? 'E' : 'O'),
            laser->dev.stopbits);
    sick_set_serial_params(laser);

    return 0;
}

int sick_make_sure_laser_works(sick_laser_p laser)
{  
    int brate = 0;
    int detected_baud_rate = 0;
    double start_time = carmen_get_time_ms();

    /* make sure the baudrate is set correctly, change it if necessary */
    if(laser->settings.detect_baudrate) 
    {
        brate = sick_detect_baudrate(laser);
        if(!brate)
        {
            strcpy( sick_info_message, "Couldn't detect baud rate" );
            return -1;
        }
        detected_baud_rate = 1;
    } 
    else if(!sick_check_baudrate(laser, laser->settings.start_baudrate)) 
    {
        strcpy( sick_info_message, "communication does not work!" );
        return -1;
    }
    
    if(brate != laser->settings.set_baudrate) {
        while(!sick_set_config_mode(laser)) {
            if ( (carmen_get_time_ms()-start_time) > MAX_TIME_FOR_CONFIG ) {
                sprintf( sick_info_message, "%s\n\tCouldn't set config mode", sick_info_message );
                return -1;
            }
        }
        
        while(!sick_set_laser_baudrate(laser, laser->settings.set_baudrate)) {
            if ( (carmen_get_time_ms()-start_time) > MAX_TIME_FOR_CONFIG ) {
                sprintf( sick_info_message, "%s\n\tCouldn't set laser baudrate", sick_info_message );
                return -1;
            }
        }
        sick_set_baudrate(laser, laser->settings.set_baudrate);
        brate = laser->settings.set_baudrate;
    }
  
    /* set the resolution of the blue lasers */
    if(laser->settings.type == LMS) {
        start_time = carmen_get_time_ms();
        while(!sick_set_lms_resolution(laser)) {
            if ( (carmen_get_time_ms()-start_time) > MAX_TIME_FOR_CONFIG ) {
                sprintf( sick_info_message, "%s\n\tCouldn't set lms resolution", sick_info_message );
                return -1;
            }
        }
        usleep(100000);
        start_time = carmen_get_time_ms();
        while(!sick_set_lms_range(laser))
        {
            if ( (carmen_get_time_ms()-start_time) > MAX_TIME_FOR_CONFIG ) {
                sprintf( sick_info_message, "%s\n\tCouldn't set lms range", sick_info_message );
                return -1;
            }
        }
    }
  
    /* set the laser to continuous mode */
    if ( sick_start_continuous_mode(laser) )
    {
        sprintf(sick_info_message,"%s\n\tCouldn't start continuous mode",sick_info_message);
        return -1;
    }
                                     
    if ( !detected_baud_rate )
        brate = laser->settings.set_baudrate;

    sprintf( sick_info_message, "Started laser\n\t\ttype %s\n\t\tbaud-rate %6d\n\t\tangle range %3d\n\t\tangle resolution %1.2f",
             (laser->settings.type == PLS) ? "PLS" : "LMS", 
             brate,
             laser->settings.angle_range,
             (laser->settings.angle_resolution == RES_1_00_DEGREE ? 1.0 :
              laser->settings.angle_resolution == RES_0_50_DEGREE ? 0.5 :  
              0.25) );
    return 0;
}

int sick_start_laser(sick_laser_p laser)
{
    // printf("TRACE(sick_start_laser)\n");
    
    int ret;

    strcpy( sick_info_message, "" );

    /* open the serial port */
    printf("Opening serial port %s ...\n", laser->dev.ttyport.c_str() );
    ret = sick_connect_device(laser);
    if ( ret == 0 )
    {
        ret = sick_make_sure_laser_works(laser);
    }

    if ( ret )
    {
        // we failed
        if ( laser->dev.fd != -1 )
            close(laser->dev.fd);
    }
    else
    {
        printf("Opened. %d\n",ret);
    }
    return ret;
}

/* sick_valid_packet - This function returns 1 if a valid packet is
   detected in a chunk of data.  An offset and packet length are 
   returned. */

int sick_valid_packet(unsigned char *data, long size,
                      long *offset, long *len)
{
  int i, check, packet_size = 0, theo_size = 0;

  for(i = 0; i < size; i++) {
    if(packet_size == 0 && *data == 0x02)
      packet_size++;
    else if(packet_size == 1 && *data == 0x80)
      packet_size++;
    else if(packet_size == 1)
      packet_size = 0;
    else if(packet_size == 2) {
      theo_size = data[0];
      packet_size++;
    } 
    else if(packet_size == 3) {
      theo_size += (data[0] << 8) + 6;
      if(size >= theo_size + (i - packet_size)) {	// check the crc
	check = data[theo_size - 3 - 2];
	check += data[theo_size - 3 - 1] << 8;                                
	if(check != sick_compute_checksum(data - 3, theo_size - 2)) {
	  i -= 2;
	  data -= 2;
	  packet_size = 0;
	}
	else {
	  *offset = i - packet_size;
	  *len = theo_size;
	  return 1;
	}
      } 
      else
	packet_size = 0;
    }
    data++;
  }
  return 0;
}

/* sick_process_packet - Interpret packets received from the laser.  If
   the packets contain laser data, expand the data into a useful form. */

static void sick_process_packet(sick_laser_p laser, unsigned char *packet)
{
  int i = 0, LoB = 0, HiB = 0, bit14, bit15, numMeasurements;
  float conversion = 1.0;

  if(packet[0] == 0xb0) 
  {
    /* Check the number of measurements */
    numMeasurements = ((packet[2] << 8) + packet[1]) & 0x3FFF;

    /* Extract the distance conversion factor */
    bit14 = packet[2] & 0x40;
    bit15 = packet[2] & 0x80;
    if(laser->settings.type == LMS) {
      if(!bit15)
	if(!bit14)
	  conversion = 1.0;
	else
	  conversion = 0.1;
      else
	conversion = 10.0;
    }

    /* Compute range values */
    laser->new_reading = 1;

    for (i = 0; i < numMeasurements; i++) 
    {
      LoB = packet[i * 2 + 3]; 
      HiB = packet[i * 2 + 4];
      laser->range[i] = ((HiB & 0x1f) * 256 + LoB) * conversion;
      laser->glare[i] = (HiB & 0x20) / 32;
      laser->wfv[i] = (HiB & 0x40) / 64;
      laser->sfv[i] = (HiB & 0x80) / 128;
    }
    laser->timestamp = carmen_get_time_ms();
  }
}

/* sick_handle_laser - Process any data that is available from the 
   laser. Attempt to detect valid packets in the data. */

void sick_handle_laser(sick_laser_p laser)
{
  int bytes_available_to_read, bytes_read;
  int leftover;

  strcpy(sick_info_message,"");
  
  laser->new_reading = 0;

  /* read as much as we can from the laser (without over-flowing our buffer) */
  bytes_available_to_read = carmen_serial_numChars(laser->dev.fd);
  if(bytes_available_to_read > LASER_BUFFER_SIZE - laser->buffer_position)
    bytes_available_to_read = LASER_BUFFER_SIZE - laser->buffer_position;
  bytes_read = carmen_serial_readn( laser->dev.fd, 
                                    (char*)(laser->buffer + laser->buffer_position), 
                                    bytes_available_to_read);

  /* process at most one laser reading */
  if(bytes_read > 0) 
      laser->buffer_position += bytes_read;

  if ( laser->buffer_position-laser->processed_mark > 0 )
  {
      if(sick_valid_packet(laser->buffer + laser->processed_mark,
                           laser->buffer_position - laser->processed_mark,
                           &(laser->packet_offset), &(laser->packet_length))) 
      {
          sick_process_packet(laser, laser->buffer + laser->processed_mark +
                              laser->packet_offset + 4);
          leftover = laser->buffer_position - laser->processed_mark - 
              laser->packet_length;
          laser->processed_mark += laser->packet_offset + laser->packet_length;

          if(leftover == 0) 
          {
              laser->buffer_position = 0;
              laser->processed_mark = 0;
          }  
      }
  }

  /* shift everything forward in the buffer, if necessary,
     so that processed_mark points to the start of the buffer. */
  if(laser->buffer_position > LASER_BUFFER_SIZE / 2) 
  {
    memmove(laser->buffer, laser->buffer + laser->processed_mark,
	    laser->buffer_position - laser->processed_mark);
    laser->buffer_position = laser->buffer_position - laser->processed_mark;
    laser->processed_mark = 0;
  } 
}

int sick_stop_laser(sick_laser_p laser)
{
    if ( sick_stop_continuous_mode(laser) )
    {
        sprintf( sick_info_message, "Couldn't stop continuous mode!" );
        close(laser->dev.fd);
        return -1;
    }

    sprintf( sick_info_message, "stopped LASER continuous mode.");
    close(laser->dev.fd);
    return 0;
}


char *sick_info()
{
    return sick_info_message;
}








////////////////////////////////////////////////////////////////////////////////

sick_laser_t::sick_laser_t( const laser_settings_t &initSettings )
    : settings(initSettings)
{
    settings.checkValid();

    this->settings = settings;

    // Install settings
    dev.type     = settings.type;
    dev.baudrate = settings.start_baudrate;
    dev.parity   = settings.parity;
    dev.fd       = -1;
    dev.databits = settings.databits;
    dev.stopbits = settings.stopbits;
    dev.hwf      = settings.hwf;
    dev.swf      = settings.swf;
    dev.passwd   = settings.password;
    dev.ttyport  = settings.device_name;

    // Allocate laser
    numvalues       = settings.num_values;
    range           = new double[numvalues];
    glare           = new int[numvalues];
    wfv             = new int[numvalues];
    sfv             = new int[numvalues];
    buffer          = new unsigned char[LASER_BUFFER_SIZE];
    new_reading     = 0;
    buffer_position = 0;
    processed_mark  = 0;
    for(int i = 0; i < settings.num_values; i++) 
    {
        range[i]    = 0.0;
        glare[i]    = 0;
        wfv[i]      = 0;
        sfv[i]      = 0;
    }
    timestamp = 0.0;
}

sick_laser_t::~sick_laser_t()
{
    if (range)  delete[] range;
    if (glare)  delete[] glare;
    if (wfv)    delete[] wfv;
    if (sfv)    delete[] sfv;
    if (buffer) delete[] buffer;
}
