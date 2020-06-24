/*	GEMEVLIB.C	1/28/84 - 09/12/85	Lee Jay Lorenzen	*/
/*	merge High C vers. w. 2.2 & 3.0		8/20/87		mdf	*/ 

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

#include <portab.h>
#include <machine.h>
#include <struct.h>
#include <basepage.h>
#include <obdefs.h>
#include <gemlib.h>

						/* in ASYNC88.C		*/
EXTERN EVSPEC	iasync();
EXTERN EVSPEC	acancel();
EXTERN UWORD	apret();
EXTERN EVSPEC	mwait();

/* ---------- added for metaware compiler ---------- */
EXTERN VOID 	chkkbd();			/* in DISP.C		*/
EXTERN VOID 	forker();
EXTERN UWORD 	dq();				/* in INPUT.C		*/
EXTERN WORD	downorup();
EXTERN WORD	in_mrect();
EXTERN VOID	ap_rdwr();			/* in APLIB.C		*/
/* ------------------------------------------------- */

EXTERN WORD	xrat, yrat, button, kstate, mclick, mtrans;
EXTERN WORD	pr_button, pr_xrat, pr_yrat, pr_mclick;

EXTERN PD	*gl_mowner;

GLOBAL WORD	gl_dcrates[5] = {450, 330, 275, 220, 165};
GLOBAL WORD	gl_dcindex;
GLOBAL WORD	gl_dclick;
GLOBAL WORD	gl_ticktime;

#if MULTIAPP
EXTERN SHELL	sh[];
#endif

/*
*	Stuff the return array with the mouse x, y, button, and keyboard
*	state.
*/
	VOID
ev_rets(rets)
	WORD		rets[];
{
	if (mtrans)
	{
	  rets[0] = pr_xrat;
	  rets[1] = pr_yrat;
	}
	else
	{
	  rets[0] = xrat;
	  rets[1] = yrat;
	}
	rets[2] = button;
	rets[3] = kstate;
	mtrans = 0;
}


/*
*	Routine to block for a certain async event and return a
*	single return code.
*/
	WORD
ev_block(code, lvalue)
	WORD		code;
	LONG		lvalue;
{
	mwait( iasync(code, lvalue) );
	return( apret(code) );
}


/*
*	Wait for the mouse buttons to reach the state where:
*		((bmask & (bstate ^ button)) == 0) != bflag
*	Clicks is how many times to wait for it to reach the state, but
*	the routine should return how many times it actually reached the
*	state before some time interval.
*
*	High bit of bflgclks determines whether to return when
*	state is entered or left. This is called bflag.
*	The default case is flag = 0 and we are waiting to ENTER the
*	indicated state. If bflag = 1 then we are waiting to LEAVE
*	the state.
*/
	UWORD
ev_button(bflgclks, bmask, bstate, rets)
	WORD		bflgclks;
	UWORD		bmask;
	UWORD		bstate;
	WORD		rets[];
{
	WORD		ret;
	LONG		parm;

	parm = HW(bflgclks) | LW((bmask << 8) | bstate);
	ret = ev_block(MU_BUTTON, parm);
	ev_rets(&rets[0]);
	return(ret);
}


/*
*	Wait for the mouse to leave or enter a specified rectangle.
*/
	UWORD
ev_mouse(pmo, rets)
	REG MOBLK	*pmo;
	WORD		rets[];
{
	WORD		ret;

	ret = ev_block(MU_M1, ADDR(pmo));
	ev_rets(&rets[0]);
	return(ret);
}


/*
*	Routine to wait a specified number of milli-seconds.
*/
	VOID
ev_timer(count)
	LONG		count;
{
	ev_block(MU_TIMER, count / gl_ticktime);
}


/*
*	Do a multi-wait on the specified events.
*/
	WORD
ev_multi(flags, pmo1, pmo2, tmcount, buparm, mebuff, prets)
	REG WORD	flags;
	REG MOBLK	*pmo1;
	MOBLK		*pmo2;
	LONG		tmcount;
	LONG		buparm;
	LONG		mebuff;
	WORD		prets[];
{
	QPB		m;
	REG EVSPEC	which;
	REG WORD	what;
	REG CQUEUE	*pc;
#if MULTIAPP
	WORD		evbuff[8];
	WORD		pid;
	SHELL		*psh;
	LONG		ljunk;

	pid = rlr->p_pid;
	psh = &sh[pid];
	if ( psh->sh_state & SHRINK )		/* shrink accessory	*/
	{
	  if (pr_shrink(pid, TRUE, &ljunk, &ljunk))
	    ap_exit(TRUE); /* if no swap space terminate acc */ 
	  psh->sh_state &= ~SHRINK;
	}
#endif
						/* say nothing has 	*/
						/*   happened yet	*/
	what = 0x0;
		 				/* do a pre-check for a	*/
						/*   keystroke & then	*/
						/*   clear out the forkq*/
	chkkbd();
	forker();
						/*   a keystroke	*/
	if (flags & MU_KEYBD)
	{
						/* if a character is 	*/
						/*   ready then get it	*/
	  pc = &rlr->p_cda->c_q;
	  if ( pc->c_cnt )
	  {
	    prets[4] = (UWORD) dq(pc);
	    what |= MU_KEYBD;
	  }
	}
						/* if we own the mouse	*/
						/*   then do quick chks	*/
	if ( rlr == gl_mowner )
	{
						/* quick check button	*/
	  if (flags & MU_BUTTON)
	  {
	    if ( (mtrans > 1) &&
		 (downorup(pr_button, buparm)) )
	    {
	      what |= MU_BUTTON;
	      prets[5] = pr_mclick;
	    }
	    else
	    {
	      if ( downorup(button, buparm) )
	      {
	        what |= MU_BUTTON;
	        prets[5] = mclick;
	      }
	    }
	  }
						/* quick check mouse rec*/
	  if ( ( flags & MU_M1 ) &&	
	       ( in_mrect(pmo1) ) )
	      what |= MU_M1;
						/* quick check mouse rec*/
	  if ( ( flags & MU_M2 ) &&
	       ( in_mrect(pmo2) ) )
	      what |= MU_M2;
	}
						/* quick check timer	*/
	if (flags & MU_TIMER)
	{
	  if ( tmcount == 0x0L )
	    what |= MU_TIMER;
	}
						/* quick check message	*/
	if (flags & MU_MESAG)
	{
	  if ( rlr->p_qindex > 0 )
	  {
#if MULTIAPP
	    ap_rdwr(MU_MESAG, rlr, 16, ADDR(&evbuff[0]) );
#endif
#if SINGLAPP
	    ap_rdwr(MU_MESAG, rlr, 16, mebuff);
#endif
	    what |= MU_MESAG;
	  }
	}
						/* check for quick out	*/
						/*   if something has	*/
						/*   already happened	*/
	if (what == 0x0)
	{
						/* wait for a keystroke	*/
	  if (flags & MU_KEYBD)
	    iasync( MU_KEYBD, 0x0L );
						/* wait for a button	*/
	  if (flags & MU_BUTTON)
	    iasync( MU_BUTTON, buparm );
						/* wait for mouse rect.	*/
	  if (flags & MU_M1)
	    iasync( MU_M1, ADDR(pmo1) );
						/* wait for mouse rect.	*/
	  if (flags & MU_M2)
	    iasync( MU_M2, ADDR(pmo2) ); 
						/* wait for message	*/
	  if (flags & MU_MESAG)
	  {
	    m.qpb_ppd = rlr;
	    m.qpb_cnt = 16;
#if MULTIAPP
	    m.qpb_buf = ADDR(&evbuff[0]);
#endif
#if SINGLAPP
	    m.qpb_buf = mebuff;
#endif
	    iasync( MU_MESAG, ADDR(&m) );
	  }
						/* wait for timer	*/
	  if (flags & MU_TIMER)
	    iasync( MU_TIMER, tmcount / gl_ticktime );
						/* wait for events	*/
	  which = mwait( flags );
						/* cancel outstanding	*/
						/*   events		*/
	  which |= acancel( flags );
	}
						/* get the returns	*/
	ev_rets(&prets[0]);
						/* do aprets() if 	*/
						/*   something hasn't	*/
						/*   already happened	*/
	if (what == 0x0)
	{
	  what = which;
	  if (which & MU_KEYBD)
	    prets[4] = apret(MU_KEYBD);
	  if (which & MU_BUTTON)
	    prets[5] = apret(MU_BUTTON);
	  if (which & MU_M1)
	    apret(MU_M1);
	  if (which & MU_M2)
	    apret(MU_M2);
	  if (which & MU_MESAG)
	    apret(MU_MESAG);
	  if (which & MU_TIMER)
	    apret(MU_TIMER);
	}
#if MULTIAPP
	if ( (flags & MU_MESAG) && (what & MU_MESAG) )
	{
	  LBCOPY(mebuff, ADDR(&evbuff[0]), 16);
	}
#endif
	 	  	  	  	  	  /* return what happened*/
	return( what );
}


/*
*	Wait for a key to be ready at the keyboard and return it. 
*/
	WORD
ev_dclick(rate, setit)
	WORD		rate, setit;
{
	if (setit)
	{
	  gl_dcindex = rate;
	  gl_dclick = gl_dcrates[gl_dcindex] / gl_ticktime ;
	}
	return( gl_dcindex );
}

#if MULTIAPP
	VOID
ev_mesag(type, ppd, mebuff)
	WORD		type;
	PD		*ppd;
	LONG		mebuff;
{
	WORD		evbuff[8];

	ap_rdwr(type, ppd, 16, ADDR(&evbuff[0]) );
	LBCOPY(mebuff, ADDR(&evbuff[0]), 16);
}
#endif

