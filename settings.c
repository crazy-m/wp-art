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
#include "settings.h"
#include "ini.h"

VOID LoadSettings(VOID)
{
  ini_read_string( "COMM", "Port",    "", (CHAR *)iniCOMMPort);
  ini_read_int   ( "COMM", "BaudRate", 2, &iniCOMMBaudRate   ); // 9600
  ini_read_int   ( "COMM", "DataBits", 4, &iniCOMMDataBits   ); // 8
  ini_read_int   ( "COMM", "Parity",   0, &iniCOMMParity     ); // None
  ini_read_int   ( "COMM", "StopBits", 0, &iniCOMMStopBits   ); // 1
}

VOID SaveSettings(VOID)
{
  ini_write_string( "COMM", "Port",     (CHAR *)iniCOMMPort  );
  ini_write_int   ( "COMM", "BaudRate", (INT)iniCOMMBaudRate );
  ini_write_int   ( "COMM", "DataBits", (INT)iniCOMMDataBits );
  ini_write_int   ( "COMM", "Parity",   (INT)iniCOMMParity   );
  ini_write_int   ( "COMM", "StopBits", (INT)iniCOMMStopBits );
}