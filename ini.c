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
#include "ini.h"

CHAR m_strFileName[255];

VOID ini_setfile(CHAR *strFileName)
{
    memset(m_strFileName, 0x00, 255);
    memcpy(m_strFileName, strFileName, strlen(strFileName));
}

VOID ini_read_string(CHAR *strSection, CHAR *strKey, CHAR *strDefValue, CHAR *strResult)
{
  GetPrivateProfileString(strSection, strKey, strDefValue, strResult, 255, m_strFileName);
}

VOID ini_write_string(CHAR *strSection, CHAR *strKey, CHAR *strValue)
{
  WritePrivateProfileString(strSection, strKey, strValue, m_strFileName);
}

VOID ini_read_int(CHAR *strSection, CHAR *strKey, INT iDefValue, INT *iResult)
{
  *iResult = GetPrivateProfileInt(strSection, strKey, iDefValue, m_strFileName);
}

VOID ini_write_int(CHAR *strSection, CHAR *strKey, INT iValue)
{
  CHAR buffer[sizeof(INT)*8+1];
  itoa(iValue, buffer, 10);
  WritePrivateProfileString(strSection, strKey, buffer, m_strFileName);
}