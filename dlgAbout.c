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
#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "dlgAbout.h"

BOOL CALLBACK dlgAboutProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT dlgAbout_Show(HWND hWndParent)
{
  DialogBox( (HINSTANCE)GetWindowLong( hWndParent, GWL_HINSTANCE ),
          MAKEINTRESOURCE(IDD_DLG_ABOUT), hWndParent, (DLGPROC)dlgAboutProc );
  return 0;
}

BOOL CALLBACK dlgAboutProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg)
  {
    case WM_INITDIALOG:
      return TRUE;
    case WM_CLOSE:
      EndDialog(hWnd,0);
      break;
    case WM_COMMAND:
      switch(wParam)
      {
        case IDC_BTN_ABOUT_OK:
          EndDialog(hWnd,0);
          break;
      }
      break;
  }
  return FALSE;
}

