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
#ifndef INI_H
#define	INI_H

#ifdef	__cplusplus
extern "C" {
#endif

VOID ini_setfile      (CHAR *strFileName);
VOID ini_read_string  (CHAR *strSection, CHAR *strKey, CHAR *strDefValue, CHAR *strResult);
VOID ini_write_string (CHAR *strSection, CHAR *strKey, CHAR *strValue);
VOID ini_read_int     (CHAR *strSection, CHAR *strKey, INT iDefValue, INT *iResult);
VOID ini_write_int    (CHAR *strSection, CHAR *strKey, INT iValue);

#ifdef	__cplusplus
}
#endif

#endif	/* INI_H */

