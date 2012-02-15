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
#include <stdlib.h>
#include <windows.h>
#include <commctrl.h>
#include <math.h>

#include "ini.h"
#include "settings.h"
#include "resource.h"
#include "commport.h"

#include "dlgSettings.h"
#include "dlgAbout.h"

#define IDT_TIMER1       9000
#define IDT_TIMER1_DELAY 500
#define IDB_RELAY        9001
#define IDE_TL           9002
#define IDE_TH           9003
#define IDB_TLOGIC       9004
#define IDB_TSET         9005
#define IDC_TRACKBAR     9006
#define RELAY_ON         "U K L J U C E N"
#define RELAY_OFF        "I S K L J U C E N"

HINSTANCE hInst;
HMENU     hmMain;

BOOL bManualCtrl=FALSE;

FLOAT fTemp1  = 0;
FLOAT fTemp2  = 0;
UCHAR ucPWM   = 0;
BOOL  bRelay  = FALSE;
UCHAR ucTH    = 30;
UCHAR ucTL    = 23;
BOOL  bTLogic = TRUE;

FLOAT fLastTemp1  = 0;
FLOAT fLastTemp2  = 0;
UCHAR ucLastPWM   = 0;
BOOL  bLastRelay  = FALSE;

HWND hSTCTemp1;
HWND hSTCTemp2;
HWND hSTCRelay;
HWND hSTCPWM;

HWND hEDTTempL;
HWND hEDTTempH;
HWND hBTNTLogic;
HWND hBTNTemp;

HWND hBTNRelay;
HWND hTBPWM;

VOID ProcessStart(HWND hWnd);
VOID ProcessStop(HWND hWnd);

VOID DrawControls(HWND hWnd);
VOID DrawGauge(HDC hdcGauge, INT iX, INT iY, UCHAR ucValue);
VOID DrawTerm (HDC hdcTerm, INT iX, INT iY, UCHAR ucValue);
VOID DrawHeater(HDC hdcHeater, INT iX, INT iY, BOOL bHeat);

LRESULT CALLBACK winMainProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
VOID    CALLBACK Timer1Proc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

INT WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
  WNDCLASSEX wcWinMain;
  HWND hWinMain;
  MSG msgWinMain;
  
  wcWinMain.cbSize        = sizeof(WNDCLASSEX);
  wcWinMain.style         = CS_HREDRAW | CS_VREDRAW;
  wcWinMain.lpfnWndProc   = winMainProc;
  wcWinMain.cbClsExtra    = 0;
  wcWinMain.cbWndExtra    = 0;
  wcWinMain.hInstance     = hInstance;
  wcWinMain.hIcon         = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON_MAIN));
  wcWinMain.hIconSm       = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON_MAIN));
  wcWinMain.hCursor       = LoadCursor(NULL,IDC_ARROW);
  wcWinMain.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
  wcWinMain.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU_MAIN);
  wcWinMain.lpszClassName = "ARTClass";
  
  if(!RegisterClassEx(&wcWinMain))
  {
    MessageBox(NULL, "RegisterClass Error", "Error", MB_ICONERROR);
    return 0;
  }
  
  hWinMain = CreateWindow("ARTClass",
          "Automatska Regulacija Temperature",
          WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
          100,
          100,
          560,
          355,
          NULL,
          NULL,
          hInstance,
          NULL);
  ShowWindow( hWinMain, iCmdShow );
  UpdateWindow( hWinMain );
  
  while (GetMessage(&msgWinMain, NULL, 0, 0))
  {
    TranslateMessage(&msgWinMain);
    DispatchMessage (&msgWinMain);
  }
}

LRESULT CALLBACK winMainProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  HDC hdcMain;
  PAINTSTRUCT psMain;
  CHAR cBuffer[128];
  UCHAR ucTL, ucTH;
  
  switch (uMsg)
  {
    case WM_QUIT:
      PostQuitMessage (0);
      return 0;
      
    case WM_DESTROY:
      ProcessStop(hWnd);
      DestroyMenu(hmMain);
      PostQuitMessage(WM_QUIT);
      break;
      
    case WM_CREATE:
      ini_setfile(".\\settings.ini");
      LoadSettings();
      InitCommonControls();
      hInst = (HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE );
      hmMain = GetMenu (hWnd);
      if ( COMMPortCount()>0){
        EnableMenuItem( hmMain, IDM_RUN_START, MF_BYCOMMAND | MF_ENABLED);
      }
      DrawControls(hWnd);
      break;
      
    case WM_PAINT:
      hdcMain = BeginPaint( hWnd, &psMain );
      DrawTerm  ( hdcMain, 45, 35, (UCHAR)fTemp1 );
      DrawTerm  ( hdcMain, 100, 35, (UCHAR)fTemp2 );
      DrawHeater( hdcMain, 162, 35, bRelay);
      DrawGauge ( hdcMain, 162, 195, ucPWM );
      EndPaint  ( hWnd, &psMain );
      break;
      
   case WM_HSCROLL:
     if(LOWORD(wParam)==TB_ENDTRACK){
        KillTimer( hWnd, IDT_TIMER1 );
        ucPWM = (UCHAR)SendMessage(hTBPWM, TBM_GETPOS, 0, 0);
        device_set_manual( &bRelay, &ucPWM );
        sprintf(cBuffer, "%d%% [%d]", (INT)(((FLOAT)ucPWM/255)*100), ucPWM);
        SetWindowText(hSTCPWM, cBuffer);
        SetTimer( hWnd, IDT_TIMER1, IDT_TIMER1_DELAY, (TIMERPROC)Timer1Proc );
     }
     break;
      
    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
        case IDM_FILE_EXIT:
          PostQuitMessage(WM_QUIT);
          break;
        case IDM_HELP_ABOUT:
          dlgAbout_Show(hWnd);
          break;
        case IDM_SETTINGS:
          dlgSettings_Show(hWnd);
          break;
        case IDM_RUN_START:
          ProcessStart(hWnd);
          break;
        case IDM_RUN_STOP:
          ProcessStop(hWnd);
          break;
        case IDM_RUN_AUTO:
          KillTimer( hWnd, IDT_TIMER1 );
          CheckMenuItem ( hmMain, IDM_RUN_AUTO,   MF_CHECKED);
          CheckMenuItem ( hmMain, IDM_RUN_MANUAL, MF_UNCHECKED );
          EnableWindow(hBTNRelay, FALSE);
          EnableWindow(hTBPWM, FALSE);
          bManualCtrl = FALSE;
          device_set_auto();
          SetTimer( hWnd, IDT_TIMER1, IDT_TIMER1_DELAY, (TIMERPROC) Timer1Proc);
          break;
        case IDM_RUN_MANUAL:
          KillTimer( hWnd, IDT_TIMER1 );
          CheckMenuItem ( hmMain, IDM_RUN_AUTO,   MF_UNCHECKED);
          CheckMenuItem ( hmMain, IDM_RUN_MANUAL, MF_CHECKED );
          EnableWindow(hBTNRelay, TRUE);
          EnableWindow(hTBPWM, TRUE);
          bManualCtrl = TRUE;
          SetTimer( hWnd, IDT_TIMER1, IDT_TIMER1_DELAY, (TIMERPROC) Timer1Proc);
          break;
        case IDB_TLOGIC:
          bTLogic = !bTLogic;
          break;
        case IDB_TSET:
          KillTimer( hWnd, IDT_TIMER1 );
          SendMessage( hEDTTempL, WM_GETTEXT, 4, (LPARAM)cBuffer );
          ucTL = (UCHAR)atoi(cBuffer);
          SendMessage( hEDTTempH, WM_GETTEXT, 4, (LPARAM)cBuffer );
          ucTH = (UCHAR)atoi(cBuffer);
          device_set_temp_level( &ucTL, &ucTH, &bTLogic);
          SetTimer( hWnd, IDT_TIMER1, IDT_TIMER1_DELAY, (TIMERPROC) Timer1Proc);
          break;
        case IDB_RELAY:
          KillTimer( hWnd, IDT_TIMER1 );
          bRelay = !bRelay;
          SetWindowText(hBTNRelay, bRelay?RELAY_ON:RELAY_OFF);
          SendMessage(hBTNRelay, BM_SETCHECK, bRelay?BST_CHECKED:BST_UNCHECKED, 0);
          device_set_manual( &bRelay, &ucPWM );
          SetTimer( hWnd, IDT_TIMER1, IDT_TIMER1_DELAY, (TIMERPROC)Timer1Proc );
          break;
        default:
          break;
      }
      break;
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

VOID CALLBACK Timer1Proc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
  CHAR buffer[255];
  
  if( !device_get_info(&fTemp1, &fTemp2, &bRelay, &ucPWM) )
    ProcessStop(hWnd);
  
  if (!bManualCtrl){
    if(bLastRelay!=bRelay)
    {
      SetWindowText(hBTNRelay, bRelay?RELAY_ON:RELAY_OFF);
      SendMessage(hBTNRelay, BM_SETCHECK, bRelay?BST_CHECKED:BST_UNCHECKED, 0);
    }
    if(ucLastPWM!=ucPWM)
    {
      SendMessage(hTBPWM, TBM_SETPOS, TRUE, (LPARAM)(ucPWM));
    }
  }
  
  if(fLastTemp1!=fTemp1)
  {
    sprintf(buffer,"%.2f", fTemp1);
    SetWindowText(hSTCTemp1, buffer);
    fLastTemp1 = fTemp1;
  }
  
  if(fLastTemp2!=fTemp2)
  {
    sprintf(buffer,"%.2f", fTemp2);
    SetWindowText(hSTCTemp2, buffer);
    fLastTemp2 = fTemp2;
  }
  
  if(bLastRelay!=bRelay)
  {
    sprintf(buffer,"%s", (bRelay?RELAY_ON:RELAY_OFF));
    SetWindowText(hSTCRelay, buffer);
    bLastRelay = bRelay;
  }
  
  if(ucLastPWM!=ucPWM)
  {
    sprintf(buffer, "%d%% [%d]", (INT)(((FLOAT)ucPWM/255)*100), ucPWM);
    SetWindowText(hSTCPWM, buffer);
    ucLastPWM = ucPWM;
  }

  InvalidateRect( hWnd, NULL, FALSE);
}

VOID ProcessStart(HWND hWnd)
{
  CHAR cBuffer[128];
  if( COMMPortCount()>0 )
  {
    if (device_open()){
      Sleep(2000);
      device_set_auto();
      device_get_temp_level( &ucTL, &ucTH, &bTLogic );
      EnableMenuItem( hmMain, IDM_RUN_START,  MF_GRAYED | MF_DISABLED);
      EnableMenuItem( hmMain, IDM_RUN_STOP,   MF_ENABLED);
      EnableMenuItem( hmMain, IDM_RUN_AUTO,   MF_ENABLED);
      CheckMenuItem ( hmMain, IDM_RUN_AUTO,   MF_CHECKED);
      EnableMenuItem( hmMain, IDM_RUN_MANUAL, MF_ENABLED );
      EnableMenuItem( hmMain, IDM_SETTINGS,   MF_GRAYED | MF_DISABLED);
      EnableWindow(hEDTTempL, TRUE);
      EnableWindow(hEDTTempH, TRUE);
      EnableWindow(hBTNTLogic, TRUE);
      EnableWindow(hBTNTemp, TRUE);
      sprintf(cBuffer,"%d",ucTL);
      SetWindowText(hEDTTempL, cBuffer);
      sprintf(cBuffer,"%d",ucTH);
      SetWindowText(hEDTTempH, cBuffer);
      SendMessage(hBTNTLogic, BM_SETCHECK, bTLogic?BST_CHECKED:BST_UNCHECKED, 0);
      bManualCtrl=FALSE;
      SetTimer( hWnd, IDT_TIMER1, IDT_TIMER1_DELAY, (TIMERPROC) Timer1Proc);
    }
  }
}

VOID ProcessStop(HWND hWnd)
{
  EnableMenuItem( hmMain, IDM_RUN_START,   MF_ENABLED);
  EnableMenuItem( hmMain, IDM_RUN_STOP,    MF_GRAYED | MF_DISABLED);
  EnableMenuItem( hmMain, IDM_RUN_AUTO,    MF_GRAYED | MF_DISABLED);
  EnableMenuItem( hmMain, IDM_RUN_MANUAL,  MF_GRAYED | MF_DISABLED);
  EnableMenuItem( hmMain, IDM_SETTINGS,    MF_ENABLED);
  CheckMenuItem ( hmMain, IDM_RUN_AUTO,   MF_CHECKED);
  CheckMenuItem ( hmMain, IDM_RUN_MANUAL, MF_UNCHECKED );
  EnableWindow(hEDTTempL, FALSE);
  EnableWindow(hEDTTempH, FALSE);
  EnableWindow(hBTNTLogic, FALSE);
  EnableWindow(hBTNTemp, FALSE);
  EnableWindow(hBTNRelay, FALSE);
  EnableWindow(hTBPWM, FALSE);
  KillTimer( hWnd, IDT_TIMER1 );
  fTemp1=0;fTemp2=0;ucPWM=0;bRelay=FALSE;
  SetWindowText(hSTCTemp1, "00.00");
  SetWindowText(hSTCTemp2, "00.00");
  SetWindowText(hEDTTempL, "0");
  SetWindowText(hEDTTempH, "0");
  SetWindowText(hSTCRelay, RELAY_OFF);
  InvalidateRect(hWnd,NULL,FALSE);
  device_set_auto();
  device_close();
}

VOID DrawControls(HWND hWnd)
{
    HWND hCtrl;
    HFONT hfCtrlFont;
    hfCtrlFont = CreateFont( 14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
              ANSI_CHARSET, OUT_DEVICE_PRECIS, CLIP_MASK, ANTIALIASED_QUALITY, 
              DEFAULT_PITCH, "MS Outlook");
    hCtrl = CreateWindow("Button", "Temperature:",
            WS_CHILDWINDOW | WS_VISIBLE | BS_GROUPBOX,
            10, 10, 130, 290, hWnd, NULL, hInst, NULL);
    SendMessage(hCtrl, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
      
    hCtrl = CreateWindow("Button", "Grijac:",
            WS_CHILDWINDOW | WS_VISIBLE | BS_GROUPBOX,
            150, 10, 225, 160, hWnd, NULL, hInst, NULL);
    SendMessage(hCtrl, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    
    hCtrl = CreateWindow("Button", "Ventilator:",
            WS_CHILDWINDOW | WS_VISIBLE | BS_GROUPBOX,
            150, 175, 225, 125, hWnd, NULL, hInst, NULL);
    SendMessage(hCtrl, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    
    hSTCTemp1 = CreateWindow("Static", "00.00",
            WS_CHILDWINDOW | WS_VISIBLE | SS_CENTERIMAGE | SS_CENTER | SS_SUNKEN,
            35, 265, 35, 20, hWnd, NULL, hInst, NULL);
    SendMessage(hSTCTemp1, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    
    hSTCTemp2 = CreateWindow("Static", "00.00",
            WS_CHILDWINDOW | WS_VISIBLE | SS_CENTERIMAGE | SS_CENTER | SS_SUNKEN,
            90, 265, 35, 20, hWnd, NULL, hInst, NULL);
    SendMessage(hSTCTemp2, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    
    hSTCRelay = CreateWindow("Static", RELAY_OFF,
            WS_CHILDWINDOW | WS_VISIBLE | SS_CENTERIMAGE | SS_CENTER | SS_SUNKEN,
            162, 135, 200, 20, hWnd, NULL, hInst, NULL);
    SendMessage(hSTCRelay, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    
    hCtrl = CreateWindow("Button", "Temp. granice:",
            WS_CHILDWINDOW | WS_VISIBLE | BS_GROUPBOX,
            385, 10, 160, 135, hWnd, NULL, hInst, NULL);
    SendMessage(hCtrl, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    
    hCtrl = CreateWindow("Static", "Donja granica:",
            WS_CHILDWINDOW | WS_VISIBLE | SS_CENTERIMAGE | SS_LEFT ,
            395, 30, 80, 15, hWnd, NULL, hInst, NULL);
    SendMessage(hCtrl, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    
    hEDTTempL = CreateWindowEx(WS_EX_CLIENTEDGE, "Edit", "00",
            WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_DISABLED | ES_CENTER | ES_NUMBER,
            500, 27, 30, 20, hWnd, NULL, hInst, NULL);
    SendMessage(hEDTTempL, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    
    hCtrl = CreateWindow("Static", "Gornja granica:",
            WS_CHILDWINDOW | WS_VISIBLE | SS_CENTERIMAGE | SS_LEFT ,
            395, 55, 80, 15, hWnd, (HMENU) IDE_TL, hInst, NULL);
    SendMessage(hCtrl, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    
    hEDTTempH = CreateWindowEx(WS_EX_CLIENTEDGE, "Edit", "00",
            WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_DISABLED | ES_CENTER | ES_NUMBER,
            500, 53, 30, 20, hWnd, (HMENU) IDE_TH, hInst, NULL);
    SendMessage(hEDTTempH, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    
    hBTNTLogic = CreateWindow("Button", "Okidaj na jedan senzor",
            WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_DISABLED | BS_AUTOCHECKBOX,
            400, 85, 130, 15, hWnd, (HMENU)IDB_TLOGIC, hInst, NULL);
    SendMessage(hBTNTLogic, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    
    hBTNTemp = CreateWindow("Button", "Postavi",
            WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_DISABLED ,
            420, 110, 80, 25, hWnd, (HMENU) IDB_TSET, hInst, NULL);
    SendMessage(hBTNTemp, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    
    hCtrl = CreateWindow("Button", "Rucna kontrola:",
            WS_CHILDWINDOW | WS_VISIBLE | BS_GROUPBOX,
            385, 150, 160, 150, hWnd, NULL, hInst, NULL);
    SendMessage(hCtrl, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    
    hCtrl = CreateWindow("Static", "Grijac:",
            WS_CHILDWINDOW | WS_VISIBLE | SS_CENTERIMAGE | SS_LEFT ,
            395, 170, 80, 15, hWnd, NULL, hInst, NULL);
    SendMessage(hCtrl, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    
    hBTNRelay = CreateWindow("Button", RELAY_OFF,
            WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_DISABLED | BS_AUTOCHECKBOX,
            410, 190, 110, 15, hWnd, (HMENU)IDB_RELAY, hInst, NULL);
    SendMessage(hBTNRelay, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    
    hCtrl = CreateWindow("Static", "Ventilator:",
            WS_CHILDWINDOW | WS_VISIBLE | SS_CENTERIMAGE | SS_LEFT ,
            395, 210, 80, 15, hWnd, NULL, hInst, NULL);
    SendMessage(hCtrl, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    hTBPWM = CreateWindow("msctls_trackbar32", "",
            WS_CHILDWINDOW | WS_VISIBLE | WS_DISABLED | TBS_BOTH ,
            400, 225, 130, 40, hWnd, (HMENU) IDC_TRACKBAR, hInst, NULL);
    SendMessage(hTBPWM, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
    SendMessage(hTBPWM, TBM_SETRANGE, TRUE,  (LPARAM) MAKELONG(0, 255));
    
    hSTCPWM = CreateWindow("Static", "0% [0]",
            WS_CHILDWINDOW | WS_VISIBLE | SS_CENTERIMAGE | SS_CENTER | SS_SUNKEN,
            400, 265, 130, 20, hWnd, NULL, hInst, NULL);
    SendMessage(hSTCPWM, WM_SETFONT, (WPARAM)hfCtrlFont, TRUE);
}

VOID DrawGauge(HDC hdcGauge, INT iX, INT iY, UCHAR ucValue)
{
  HDC hdcMemDC;
  HBRUSH hbrGauge;
  HBRUSH hbrOldBrush;
  HBITMAP hbiGauge;
  HBITMAP hbiOldBG;
  BITMAP bmpGauge;
  
  FLOAT fAngle;
  INT iPoints[]={3};
  
  hbrGauge = CreateSolidBrush(RGB(220,0,0));
  
  hbiGauge = (HBITMAP)LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BMP_GAUGE)); 
  
  hdcMemDC = CreateCompatibleDC(hdcGauge);
  
  GetObject( hbiGauge, sizeof(BITMAP), &bmpGauge);
  hbiOldBG    = (HBITMAP)SelectObject(hdcMemDC, hbiGauge);
  hbrOldBrush = (HBRUSH) SelectObject(hdcMemDC, hbrGauge);
  
  fAngle = (FLOAT) (153.0*ucValue/255);
  
  POINT p[3]=
  {
      {101+115.0*sin((fAngle-76.5)/120.0),148-115.0*cos((fAngle-76.5)/120.0)},
      {101+ 85.0*sin((fAngle-81.5)/120.0),148- 85.0*cos((fAngle-81.5)/120.0)},
      {101+ 85.0*sin((fAngle-72.5)/120.0),148- 85.0*cos((fAngle-72.5)/120.0)}
  };
  
  PolyPolygon(hdcMemDC, p, iPoints, 1);
  
  BitBlt(hdcGauge, iX, iY, bmpGauge.bmWidth, bmpGauge.bmHeight,
          hdcMemDC, 0, 0, SRCCOPY);
  SelectObject(hdcMemDC, hbiOldBG);
  SelectObject(hdcMemDC, hbrOldBrush);
  DeleteDC(hdcMemDC);
  DeleteObject(hbiOldBG);
  DeleteObject(hbrOldBrush);
  DeleteObject(hbiGauge);
}

VOID DrawTerm(HDC hdcTerm, INT iX, INT iY, UCHAR ucValue)
{
  HDC hdcMemDC;
  HBITMAP hbiTerm;
  HBITMAP hbiOldBG;
  BITMAP bmpTerm;
  HFONT hFont, hOldFont;
  INT i;
  CHAR buffer[4];
    
  SetBkMode(hdcTerm, TRANSPARENT);
  hdcMemDC = CreateCompatibleDC(hdcTerm);
  
  hbiTerm  = (HBITMAP)LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BMP_TERM));
  hbiOldBG = (HBITMAP)SelectObject(hdcMemDC, hbiTerm);
  GetObject( hbiTerm, sizeof(BITMAP), &bmpTerm);
  
  hFont = CreateFont(10, 0, 0, 0,
          FW_THIN, FALSE, FALSE, FALSE,
          ANSI_CHARSET, OUT_DEFAULT_PRECIS,
          CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
          DEFAULT_PITCH | FF_ROMAN,
          "MS Outlook");
  hOldFont = (HFONT)SelectObject(hdcTerm, hFont);
  
  for(i=0;i<=50;i++)
  {
    MoveToEx(hdcTerm, iX-5, (iY+10)+(4*i), (LPPOINT) NULL);
    LineTo(hdcTerm, iX, (iY+10)+(4*i));
  }
  
  for(i=0;i<=10;i++)
  {
    MoveToEx(hdcTerm, iX-8, (iY+10)+(20*i), (LPPOINT) NULL);
    LineTo(hdcTerm, iX, (iY+10)+(20*i));
    sprintf(buffer,"%3d",100-i*10);
    TextOut(hdcTerm, iX-22, (iY+5)+(20*i), buffer, sizeof(buffer));
  }
  
  BitBlt(hdcTerm, iX, iY, bmpTerm.bmWidth, 10, hdcMemDC, 0, 0, SRCCOPY);
  StretchBlt(hdcTerm, iX, iY+10, bmpTerm.bmWidth, (100-ucValue)*2,
          hdcMemDC, 0, 10, bmpTerm.bmWidth, 1, SRCCOPY);
  StretchBlt(hdcTerm, iX, iY+10+(100-ucValue)*2, bmpTerm.bmWidth, ucValue*2,
          hdcMemDC, 0, 11, bmpTerm.bmWidth, 1, SRCCOPY);
  BitBlt(hdcTerm, iX, iY+10+200, bmpTerm.bmWidth, 10, hdcMemDC, 0, 12, SRCCOPY);
  
  SelectObject(hdcMemDC, hbiOldBG);
  SelectObject(hdcTerm, hOldFont);
  DeleteDC(hdcMemDC);
  DeleteObject(hbiOldBG);
  DeleteObject(hbiTerm);
}

VOID DrawHeater(HDC hdcHeater, INT iX, INT iY, BOOL bHeat)
{
  HDC hdcMemDC;
  HBITMAP hbiHeater;
  HBITMAP hbiOldBG;
  BITMAP bmpHeater;
  
  hbiHeater = (HBITMAP)LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BMP_HEAT)); 
  
  hdcMemDC = CreateCompatibleDC(hdcHeater);
  
  GetObject( hbiHeater, sizeof(BITMAP), &bmpHeater);
  hbiOldBG    = (HBITMAP)SelectObject(hdcMemDC, hbiHeater);
  
  BitBlt(hdcHeater, iX, iY, bmpHeater.bmWidth, bmpHeater.bmHeight/2, hdcMemDC, 0, (bHeat?0:90), SRCCOPY);
  
  SelectObject(hdcMemDC, hbiOldBG);
  DeleteDC(hdcMemDC);
  DeleteObject(hbiOldBG);
  DeleteObject(hbiHeater);
}