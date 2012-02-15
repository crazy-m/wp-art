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
#ifndef COMMPORT_H
#define	COMMPORT_H

#ifdef	__cplusplus
extern "C" {
#endif

INT  COMMPortCount(VOID);

BOOL COMMPortName(DWORD dwIndex, CHAR *strPortName);
BOOL COMMPortOpen(CHAR *strPortName);
BOOL COMMPortConfig(VOID);
BOOL COMMPortWrite(UCHAR *strBuffer, DWORD szBuffer, DWORD *dwBytes);
BOOL COMMPortRead(UCHAR *strBuffer, DWORD szBuffer, DWORD *dwBytes);
BOOL COMMPortClose(VOID);

#ifdef	__cplusplus
}
#endif

#endif	/* COMMPORT_H */

