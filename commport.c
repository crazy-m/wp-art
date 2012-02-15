/*
 * This file is part of ART application.
 *
 * I/O Control Board is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 *  I/O Control Board is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with I/O Control Board. If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <windows.h>
#include "commport.h"
#include "settings.h"

HANDLE hCOMMPort; 

INT COMMPortCount(VOID)
{
  INT   numOfComm = -1;
  HKEY  hKey;
  
  if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),
                   0,
                   KEY_QUERY_VALUE,
                   &hKey) != ERROR_SUCCESS)
    return -1;
  
  RegQueryInfoKey(hKey,NULL,NULL,NULL,NULL,NULL,NULL,
          (LPDWORD)&numOfComm,NULL,NULL,NULL,NULL);
 
  RegCloseKey(hKey);
  
  return numOfComm;
}

BOOL COMMPortName(DWORD dwIndex, CHAR *strPortName)
{
  HKEY  hKey;
  CHAR  Name[48];
  DWORD szName;
  UCHAR PortName[48];
  DWORD szPortName;
   
  if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),
                   0,
                   KEY_READ,
                   &hKey) != ERROR_SUCCESS)
    return FALSE;
  
  szName = sizeof(Name);
  szPortName = sizeof(PortName);
  
  if (RegEnumValue(hKey,
            dwIndex,
            Name,
            &szName,
            NULL,
            NULL,
            PortName,
            &szPortName) == ERROR_SUCCESS)
    strcpy(strPortName,(char*)PortName);
    
  RegCloseKey(hKey);
  return TRUE;
}

BOOL COMMPortOpen(CHAR *strPortName)
{
  if((hCOMMPort = CreateFile(strPortName, GENERIC_READ | GENERIC_WRITE, 0, 0,
          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0))==INVALID_HANDLE_VALUE)
    return FALSE;
  return TRUE;
}

BOOL COMMPortConfig(VOID)
{
  DCB          dcbCOMMConfig   = {0};
  COMMTIMEOUTS ctoCOMMTimeouts = {0};
  
  switch (iniCOMMBaudRate)
  {
    case 0:
      dcbCOMMConfig.BaudRate = CBR_2400;
      break;
    case 1:
      dcbCOMMConfig.BaudRate = CBR_4800;
      break;
    case 2:
      dcbCOMMConfig.BaudRate = CBR_9600;
      break;
    case 3:
      dcbCOMMConfig.BaudRate = CBR_19200;
      break;
    case 4:
      dcbCOMMConfig.BaudRate = CBR_38400;
      break;
    case 5:
      dcbCOMMConfig.BaudRate = CBR_57600;
      break;
    case 6:
      dcbCOMMConfig.BaudRate = CBR_115200;
      break;
    default:
      //Fall back to 9600
      dcbCOMMConfig.BaudRate = CBR_9600;
  }
  
  dcbCOMMConfig.ByteSize = (BYTE)( iniCOMMDataBits + 4 );
  dcbCOMMConfig.Parity   = (BYTE)  iniCOMMParity;
  dcbCOMMConfig.StopBits = (BYTE)  iniCOMMStopBits;
  /*
  dcbCOMMConfig.fBinary = TRUE;
  dcbCOMMConfig.fParity = TRUE;
  dcbCOMMConfig.fOutxCtsFlow = FALSE;
  dcbCOMMConfig.fOutxDsrFlow = FALSE;
  dcbCOMMConfig.fDtrControl = DTR_CONTROL_DISABLE;
  dcbCOMMConfig.fDsrSensitivity = FALSE;
  dcbCOMMConfig.fTXContinueOnXoff = TRUE;
  dcbCOMMConfig.fOutX = FALSE;
  dcbCOMMConfig.fInX = FALSE;
  dcbCOMMConfig.fErrorChar = FALSE;
  dcbCOMMConfig.fNull = FALSE;
  dcbCOMMConfig.fRtsControl = RTS_CONTROL_DISABLE;
  dcbCOMMConfig.fAbortOnError = FALSE;
  */
  if (!SetCommState(hCOMMPort, &dcbCOMMConfig))
    return FALSE;

  ctoCOMMTimeouts.ReadIntervalTimeout         = 0;
  ctoCOMMTimeouts.ReadTotalTimeoutConstant    = 0;
  ctoCOMMTimeouts.ReadTotalTimeoutMultiplier  = 0;
  ctoCOMMTimeouts.WriteTotalTimeoutConstant   = 0;
  ctoCOMMTimeouts.WriteTotalTimeoutMultiplier = 0;
  
  if(!SetCommTimeouts(hCOMMPort, &ctoCOMMTimeouts))
    return FALSE;
  
  return TRUE;
}

BOOL COMMPortRead(UCHAR *strBuffer, DWORD szBuffer, DWORD *dwBytes)
{
  return ReadFile(hCOMMPort, strBuffer, szBuffer, dwBytes, NULL);
}

BOOL COMMPortWrite(UCHAR *strBuffer, DWORD szBuffer, DWORD *dwBytes)
{
  return WriteFile(hCOMMPort, strBuffer, szBuffer, dwBytes, NULL);
}

BOOL COMMPortClose(VOID)
{
  CloseHandle(hCOMMPort);
  return TRUE;
}

BOOL COMMPortTest(VOID)
{
  SaveSettings();
  if(!COMMPortOpen((CHAR *)iniCOMMPort) || !COMMPortConfig() || !COMMPortClose())
    return FALSE;
  return TRUE;
}