/*	GEMAPLIB.C	03/15/84 - 08/21/85	Lee Lorenzen		*/
/*	merge High C vers. w. 2.2 & 3.0		8/19/87		mdf	*/ 

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include <portab.h>
#include <machine.h>
#include <struct.h>
#include <basepage.h>
#include <obdefs.h>
#include <gemlib.h>
#include <gem.h>

#include "geminit.h"
#include "gempd.h"
#include "geminput.h"
#include "gemflag.h"
#include "gemevlib.h"

#define TCHNG 0
#define BCHNG 1
#define MCHNG 2
#define KCHNG 3


GLOBAL WORD	gl_play;
GLOBAL WORD	gl_recd;
GLOBAL WORD	gl_rlen;
GLOBAL LONG	gl_rbuf;


/*
*	Routine to initialize the application
*/
WORD ap_init(void)
{
	WORD		pid;
	LONG		scdir;
#if MULTIAPP
	SHELL		*psh;
	UWORD		chseg;
#endif

	pid = rlr->p_pid;
#if MULTIAPP
	psh = &sh[pid];
	if ( psh->sh_state & SHRINK )
	{
	  chseg = dos_gpsp();			/* get seg of acc's PSP	*/
	  dos_stblk(chseg, psh->sh_shrsz);	/* size down acc	*/

	}
#endif
	rs_gaddr(ad_sysglo, R_STRING, STSCDIR, &scdir);
	LBSET(scdir, gl_logdrv);		/* set drive letter	*/
	sc_write(scdir);
	return( pid );
}


/*
*	APplication READ or WRITE
*/
VOID ap_rdwr(WORD code, REG PD *p, WORD length, LONG pbuff)
{
	QPB		m;
						/* do quick version if	*/
						/*   it is standard 16	*/
						/*   byte read and the	*/
						/*   pipe has only 16	*/
						/*   bytes inside it	*/
	if ( (code == MU_MESAG) &&
	     (p->p_qindex == length) &&
	     (length == 16) )
	{
	  LBCOPY(pbuff, p->p_qaddr, p->p_qindex);
	  p->p_qindex = 0;
	}
	else
	{
	  m.qpb_ppd = p;
	  m.qpb_cnt = length;
	  m.qpb_buf = pbuff;
	  ev_block(code, ADDR(&m));
	  }
}

/*
*	APplication FIND
*/
WORD ap_find(LONG pname)
{
	REG PD		*p;
	BYTE		temp[9];

	LSTCPY(ADDR(&temp[0]), pname);
 
	p = fpdnm(&temp[0], 0x0);
	return( ((p) ? (p->p_pid) : (-1)) );
}

/*
*	APplication Tape PLAYer
*/
VOID ap_tplay(REG LONG pbuff, WORD length, WORD scale)
{
	REG WORD	i;
	FPD		f;
	LONG		ad_f;

	ad_f = (LONG) ADDR(&f);

	gl_play = TRUE;
	for(i=0; i<length; i++)
	{
						/* get an event to play	*/
	  LBCOPY(ad_f, pbuff, sizeof(FPD));
	  pbuff += sizeof(FPD);
						/* convert it to machine*/
						/*   specific form	*/
	  switch( ((LONG)(f.f_code)) )
	  {
	    case TCHNG:
		ev_timer( (f.f_data*100L) / scale );
		f.f_code = 0;
		break;
	    case MCHNG:
		f.f_code = (WORD (*)(VOID))mchange;
		break;
	    case BCHNG:
		f.f_code = (WORD (*)(VOID))bchange;
		break;
	    case KCHNG:
		f.f_code = (WORD (*)(VOID))kchange;
		break;
	  }
						/* play it		*/
	  if (f.f_code)
	    forkq(f.f_code, f.f_data);
						/* let someone else	*/
						/*   hear it and respond*/
	  dsptch();
	}
	gl_play = FALSE;
} /* ap_tplay */

/*
*	APplication Tape RECorDer
*/
WORD ap_trecd(REG LONG pbuff, REG WORD length)
{
	REG WORD	i;
	REG WORD	code;
	WORD		(*proutine)(VOID);
	
	
						/* start recording in	*/
						/*   forker()		*/
	cli();
	gl_recd = TRUE;
	gl_rlen = length;
	gl_rbuf = pbuff;
	sti();
	  					/* 1/10 of a second	*/
						/*   sample rate	*/
	while( gl_recd )
	  ev_timer( 100L );
						/* done recording so	*/
						/*   figure out length	*/
	cli();
	gl_recd = FALSE;
	gl_rlen = 0;
	length = ((WORD)(gl_rbuf - pbuff)) / sizeof(FPD);
	gl_rbuf = 0x0L;
	sti();
						/* convert to machine	*/
						/*   independent 	*/
						/*   recording		*/
	for(i=0; i<length; i++)
	{
#if MC68K
	  proutine = (WORD (*)(VOID))LLGET(pbuff);
#endif
#if I8086
	  proutine = (WORD (*)())LWGET(pbuff);
#endif
	  if((LONG)proutine == (LONG)tchange)
	  {
	    code = TCHNG;
	    LLSET(pbuff+sizeof(WORD *), LLGET(pbuff+sizeof(WORD *)) * 
			gl_ticktime);
	  }
	  if((LONG)proutine == (LONG)mchange)
	    code = MCHNG;
	  if((LONG)proutine == (LONG)kchange)
	    code = KCHNG;
	  if((LONG)proutine == (LONG)bchange)
	    code = BCHNG;
	  LWSET(pbuff, code);
	  pbuff += sizeof(FPD);
	}
	return(length);
} /* ap_trecd */


#if MULTIAPP
VOID ap_exit(WORD isgem)
#else
VOID ap_exit(VOID)
#endif
{
	wm_update(TRUE);
#if SINGLAPP
	mn_clsda();
#endif
	if (rlr->p_qindex)
	  ap_rdwr(MU_MESAG, rlr, rlr->p_qindex, ad_valstr);
	gsx_mfset(ad_armice);
	wm_update(FALSE);
	all_run();
#if MULTIAPP
	if (isgem)
	  dos_term();
#endif
}

#if MULTIAPP
/*
*	special abort for accessories
*/
	VOID
ap_accexit()
{
	ap_exit(TRUE);
}
#endif

