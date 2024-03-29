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
#ifndef SETTINGS_H
#define	SETTINGS_H

#ifdef	__cplusplus
extern "C" {
#endif

volatile CHAR iniCOMMPort[128];
volatile INT  iniCOMMBaudRate;
volatile INT  iniCOMMDataBits;
volatile INT  iniCOMMParity;
volatile INT  iniCOMMStopBits;

VOID LoadSettings(VOID);

#ifdef	__cplusplus
}
#endif

#endif	/* SETTINGS_H */

