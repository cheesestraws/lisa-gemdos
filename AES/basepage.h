/*	BASEPAGE.H	1/28/84 - 12/15/84	Lee Jay Lorenzen	*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1986			Digital Research Inc.
*	-------------------------------------------------------------
*/

						/* in BASE88.C		*/
EXTERN PD	*rlr, *drl, *nrl;
EXTERN EVB	*eul, *dlr, *zlr;

#if I8086
EXTERN UWORD	elinkoff;
#else
EXTERN LONG	elinkoff;
#endif
EXTERN BYTE	indisp;
EXTERN WORD	fpt, fph, fpcnt;		/* forkq tail, head, 	*/
						/*   count		*/
EXTERN SPB	wind_spb;
EXTERN CDA	*cda;
EXTERN WORD	curpid;

