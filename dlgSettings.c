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
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "commport.h"
#include "settings.h"

#include "dlgSettings.h"


BOOL CALLBACK dlgSettingsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

VOID dlgSettings_OnButtonSave(VOID);
VOID dlgSettings_OnInit(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

CHAR *strTmp;
INT  iTmp;
INT  iPortNum;

static HWND hCBPortName;
static HWND hCBBaudRate;
static HWND hCBDataBits;
static HWND hCBParity;
static HWND hCBStopBits;
static HWND hBTNPortTest;

CONST CHAR strBaudRate[7][10] = {"2400","4800","9600","19200","38400","57600","115200"};
CONST CHAR strDataBits[5][10] = {"4","5","6","7","8"};
CONST CHAR strParity[5][10]   = {"None","Odd","Even","Mark","Space"};
CONST CHAR strStopBits[3][10] = {"1","1.5","2"};

INT dlgSettings_Show(HWND hWndParent)
{
  DialogBox( (HINSTANCE)GetWindowLong( hWndParent,GWL_HINSTANCE ),
          MAKEINTRESOURCE(IDD_DLG_SETTINGS), hWndParent, (DLGPROC)dlgSettingsProc );
  return 0;
}

BOOL CALLBACK dlgSettingsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg)
  {
    case WM_INITDIALOG:
      dlgSettings_OnInit( hWnd, uMsg, wParam, lParam );
      return TRUE;
    case WM_CLOSE:
      EndDialog( hWnd, 0 );
      break;
    case WM_COMMAND:
      switch(wParam)
      {
        case IDC_BTN_SETTINGS_SAVE:
          dlgSettings_OnButtonSave();
          EndDialog( hWnd, 0 );
          break;
        case IDC_BTN_SETTINGS_CANCEL:
          EndDialog( hWnd, 0 );
          break;
        case IDC_BTN_PORT_TEST:
          if ( COMMPortTest() )
          {
            MessageBox( hWnd, TEXT("Test COMM porta je uspio!"),
                    TEXT("COMM Port Test"), MB_OK | MB_ICONINFORMATION );
          }
          else
          {
            MessageBox( hWnd, TEXT("Test COMM porta nije uspio"),
                    TEXT("COMM Port Test"), MB_OK | MB_ICONERROR );
          }
          break;
      }
      break;
  }
  return FALSE;
}

VOID dlgSettings_OnButtonSave(VOID)
{
  strTmp = (CHAR *)malloc( 255*sizeof(CHAR) );
  if ( iPortNum>0 ){
    SendMessage( hCBPortName, WM_GETTEXT, 255, (LPARAM)strTmp );
    strcpy( (CHAR *)iniCOMMPort, strTmp );
    iniCOMMBaudRate = SendMessage( hCBBaudRate, CB_GETCURSEL, 0, 0 );
    iniCOMMDataBits = SendMessage( hCBDataBits, CB_GETCURSEL, 0, 0 );
    iniCOMMParity   = SendMessage( hCBParity,   CB_GETCURSEL, 0, 0 );
    iniCOMMStopBits = SendMessage( hCBStopBits, CB_GETCURSEL, 0, 0 );
  }
  free( strTmp );
  SaveSettings();
  LoadSettings();
}

VOID dlgSettings_OnInit(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  INT i;
  CHAR strPortName[10];
  
  hCBPortName   = GetDlgItem( hWnd, IDC_CBO_PORT_NAME     );
  hCBBaudRate   = GetDlgItem( hWnd, IDC_CBO_PORT_BAUDRATE );
  hCBDataBits   = GetDlgItem( hWnd, IDC_CBO_PORT_DATABITS );
  hCBParity     = GetDlgItem( hWnd, IDC_CBO_PORT_PARITY   );
  hCBStopBits   = GetDlgItem( hWnd, IDC_CBO_PORT_STOPBITS );
  hBTNPortTest  = GetDlgItem( hWnd, IDC_BTN_PORT_TEST     );
  
  LoadSettings();
  
  strTmp   = (CHAR *)malloc( 128*sizeof(CHAR) );
  iPortNum = COMMPortCount();

  if (iPortNum > 0){
    
    for (i=0;i<iPortNum;i++){
      COMMPortName( i, strPortName );
      SendMessage( hCBPortName, CB_ADDSTRING, 0, (LPARAM)strPortName );
    }
    
    for(i=0;i<(sizeof(strBaudRate)/10*sizeof(CHAR));i++)
      SendMessage( hCBBaudRate, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)strBaudRate[i]) );
    
    for(i=0;i<(sizeof(strDataBits)/10*sizeof(CHAR));i++)
      SendMessage( hCBDataBits, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)strDataBits[i]) );
    
    for(i=0;i<(sizeof(strParity)/10*sizeof(CHAR));i++)
      SendMessage( hCBParity, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)strParity[i]) );
    
    for(i=0;i<(sizeof(strStopBits)/10*sizeof(CHAR));i++)
      SendMessage( hCBStopBits, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)strStopBits[i]) );
    
    iTmp = SendMessage( hCBPortName, CB_FINDSTRINGEXACT, 0, (LPARAM)iniCOMMPort );
    if( strTmp!="" && iTmp!=CB_ERR ){
      SendMessage( hCBPortName, CB_SETCURSEL, iTmp, 0 );
    }
    else
    {
      MessageBox( hWnd,
              "Naznaceni port nije dostupan,\r\nkoristim prvi na listi.\r\n",
              "Postavke COMM porta", MB_OK | MB_ICONINFORMATION );
      SendMessage( hCBPortName, CB_SETCURSEL, 0, 0 );
    }
    
    SendMessage( hCBBaudRate, CB_SETCURSEL, iniCOMMBaudRate, 0 );
    SendMessage( hCBDataBits, CB_SETCURSEL, iniCOMMDataBits, 0 );
    SendMessage( hCBParity,   CB_SETCURSEL, iniCOMMParity,   0 );
    SendMessage( hCBStopBits, CB_SETCURSEL, iniCOMMStopBits, 0 );
    
    EnableWindow( hCBPortName,  1 );
    EnableWindow( hCBBaudRate,  1 );
    EnableWindow( hCBDataBits,  1 );
    EnableWindow( hCBParity,    1 );
    EnableWindow( hCBStopBits,  1 );
    EnableWindow( hBTNPortTest, 1);
  }
  else
  {
    MessageBox( hWnd, "Nisu detektovani COMM portovi na sistemu\r\n",
            "Postavke COMM porta", MB_ICONINFORMATION | MB_OK );
    // EndDialog( hWnd, 0 );
  }
  free(strTmp);
}


