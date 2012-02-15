/*
 * This file is part of ART application.
 *
 * ART is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 *  ART is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ART. If not, see <http://www.gnu.org/licenses/>.
 */
#include <windows.h>

#include "device.h"
#include "commport.h"
#include "settings.h"

BOOL bDeviceInUse=FALSE;

BOOL device_msg_error(CHAR *msgError);

BOOL device_open(VOID)
{
  DWORD dwBytes;
  UCHAR buffer[2];
  INT i=0;
  
  if ( !COMMPortOpen((CHAR *)iniCOMMPort) || !COMMPortConfig() )
    return FALSE;
  
  for(i=0;i<5;i++)
  {
    if(!COMMPortWrite (  "I"  , 1, &dwBytes))
      return device_msg_error("Ne mogu pisati: I\r\n");
    if(!COMMPortRead  ( buffer, 2, &dwBytes ))
      return device_msg_error("Ne mogu citati: I\r\n");
    if(!strncmp(buffer,"OK",2))
    {
      bDeviceInUse=TRUE;
      return TRUE;
    }
  }
  return FALSE;
}

BOOL device_close(VOID)
{
  COMMPortClose();
  bDeviceInUse=FALSE;
  return TRUE;
}

BOOL device_get_info(FLOAT *fTemp1, FLOAT *fTemp2, BOOL *bRelay, UCHAR *ucPWM)
{
  DWORD dwBytes;
  UCHAR buffer[6];
  
  if (bDeviceInUse)
  {
    if(!COMMPortWrite (  "E"  , 1, &dwBytes))
      return device_msg_error("Ne mogu pisati: E\r\n");
    if(!COMMPortRead  ( buffer, 6, &dwBytes ))
      return device_msg_error("Ne mogu citati: E\r\n");
    *fTemp1 = (FLOAT)(((UINT)(buffer[0]<<8))|buffer[1])/16.0;
    *fTemp2 = (FLOAT)(((UINT)(buffer[2]<<8))|buffer[3])/16.0;
    *bRelay = (BOOL)(buffer[4]);
    *ucPWM  = buffer[5];
  }
  return TRUE;
}

BOOL device_set_auto(VOID)
{
  DWORD dwBytes;
  if (bDeviceInUse)
  {
    if( !COMMPortWrite ( "A", 1, &dwBytes ) )
      return device_msg_error("Ne mogu pisati: AUTO\r\n");
  }
  return TRUE;
}

BOOL device_set_manual(CONST BOOL *bRelay, CONST UCHAR *ucPWM)
{
  DWORD dwBytes;
  UCHAR buffer[3];
  if(bDeviceInUse)
  {
    buffer[0] = 'R';
    buffer[1] = (UCHAR)(*bRelay);
    buffer[2] = *ucPWM;
    if( !COMMPortWrite ( buffer, 3, &dwBytes ) )
      return device_msg_error("Ne mogu pisati: MANUAL\r\n");
  }
  return TRUE;
}

BOOL device_set_temp_level(CONST UCHAR *ucTL, CONST UCHAR *ucTH, CONST BOOL *bTLogic)
{
  DWORD dwBytes;
  UCHAR buffer[5];
  if(bDeviceInUse)
  {
    buffer[0]='L';
    buffer[1]=*ucTH;
    buffer[2]=(UCHAR)(*bTLogic);
    buffer[3]=*ucTL;
    buffer[4]=(UCHAR)(*bTLogic);
    if( !COMMPortWrite ( buffer, 5, &dwBytes ) )
      return device_msg_error("Ne mogu pisati: L\r\n");
  }
  return TRUE;
}

BOOL device_get_temp_level(UCHAR *ucTL, UCHAR *ucTH, BOOL *bTLogic)
{
  DWORD dwBytes;
  UCHAR buffer[3];
  
  if (bDeviceInUse)
  {
    if(!COMMPortWrite (  "V"  , 1, &dwBytes))
      return device_msg_error("Ne mogu pisati: R\r\n");
    if(!COMMPortRead  ( buffer, 3, &dwBytes ))
      return device_msg_error("Ne mogu citati: R\r\n");
    *ucTH = buffer[0];
    *ucTL = buffer[1];
    *bTLogic = buffer[2];
  }
  return TRUE;
}

BOOL device_msg_error(CHAR *msgError)
{
  MessageBox( NULL, msgError, "Greska na uredjaju", MB_OK | MB_ICONERROR);
  return FALSE;
}