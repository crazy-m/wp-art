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
#ifndef DEVICE_H
#define	DEVICE_H

#ifdef	__cplusplus
extern "C" {
#endif
  
BOOL device_open(VOID);
BOOL device_close(VOID);
BOOL device_get_info(FLOAT *fTemp1, FLOAT *fTemp2, BOOL *bRelay, UCHAR *ucPWM);
BOOL device_set_auto(VOID);
BOOL device_set_manual(CONST BOOL *bRelay, CONST UCHAR *ucPWM);
BOOL device_set_temp_level(CONST UCHAR *ucTL, CONST UCHAR *ucTH, CONST BOOL *bTLogic);
BOOL device_get_temp_level(UCHAR *ucTL, UCHAR *ucTH, BOOL *bTLogic);
  
#ifdef	__cplusplus
}
#endif

#endif	/* DEVICE_H */

