/*	GEMFMLIB.C	03/15/84 - 06/16/85	Gregg Morris		*/
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
*	Copyright (C) 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include <portab.h>
#include <machine.h>
#include <struct.h>
#include <basepage.h>
#include <obdefs.h>
#include <taddr.h>
#include <gemlib.h>
#include <gem.h>

#define FORWARD 0
#define BACKWARD 1
#define DEFLT 2

#define BACKSPACE 0x0E08			/* backspace		*/
#define SPACE 0x3920				/* ASCII <space>	*/
#define UP 0x4800				/* up arrow		*/
#define DOWN 0x5000				/* down arrow		*/
#define LEFT 0x4B00				/* left arrow		*/
#define RIGHT 0x4D00				/* right arrow		*/
#define DELETE 0x5300				/* keypad delete	*/
#define TAB 0x0F09				/* tab			*/
#define BACKTAB 0x0F00				/* backtab		*/
#define RETURN 0x1C0D				/* carriage return	*/


						/* in WMLIB.C		*/
EXTERN WORD		w_drawdesk();
EXTERN WORD		w_update();
						/* in GEMRSLIB.C	*/
EXTERN BYTE		*rs_str();
/* --------------- added for metaware compiler --------------- */
EXTERN VOID   		wm_update();		/* in WMLIB.C		*/
EXTERN VOID		get_ctrl();		/* in INPUT.C 		*/
EXTERN VOID		get_mown();
EXTERN VOID 		fq();
EXTERN VOID 		ct_chgown();		/* in CTRL.C		*/
EXTERN VOID 		ct_mouse();
EXTERN WORD 		ob_fs();		/* in OBLIB.C		*/
EXTERN VOID 		ob_change();
EXTERN WORD 		ob_find();
EXTERN WORD 		get_par();		/* in OBJOP.C		*/
EXTERN WORD		gr_watchbox();		/* in GRLIB.C		*/
EXTERN VOID 		ev_button();		/* in EVLIB.C		*/
EXTERN WORD 		ev_multi();
EXTERN VOID		gsx_sclip();		/* in GRAF.C		*/
EXTERN VOID 		ob_edit();		/* in OBED.C 		*/
EXTERN VOID		merge_str();		/* in OPTIMIZE.C	*/
EXTERN VOID 		sound();		
EXTERN WORD 		fm_alert();		/* in FMALT.C		*/
/* ---------------------------------------------------------- */

EXTERN GRECT		gl_rfull;
EXTERN GRECT		gl_rscreen;

EXTERN LONG		gl_mntree;

EXTERN THEGLO		D;

GLOBAL LONG		ad_g2loc;
MLOCAL	WORD		ml_ocnt = 0;
MLOCAL	LONG		ml_mnhold;
MLOCAL	GRECT		ml_ctrl;
MLOCAL	PD		*ml_pmown;

MLOCAL WORD	ml_alrt[] = 
		{AL00CRT,AL01CRT,AL02CRT,AL03CRT,AL04CRT,AL05CRT};
MLOCAL WORD	ml_pwlv[] = 
		{0x0102,0x0102,0x0102,0x0101,0x0002,0x0001};

	VOID
fm_own(beg_ownit)
	WORD		beg_ownit;
{

	if (beg_ownit)
	{
	  wm_update(TRUE);
	  if (ml_ocnt == 0)
	  {
	    ml_mnhold = gl_mntree;
	    gl_mntree = 0x0L;
	    get_ctrl(&ml_ctrl);
	    get_mown(&ml_pmown);
	    ct_chgown(rlr, &gl_rscreen);
	  }
	  ml_ocnt++;
	}
	else
	{
	  ml_ocnt--;
	  if (ml_ocnt == 0)
	  {
	    ct_chgown(ml_pmown, &ml_ctrl);
	    gl_mntree = ml_mnhold;
	  }
	  wm_update(FALSE);
	}
}


/*
*	Routine to find the next editable text field, or a field that
*	is marked as a default return field.
*/
	WORD
find_obj(tree, start_obj, which)
	REG LONG	tree;
	WORD		start_obj;
	WORD		which;
{
	REG WORD	obj, flag, state, inc;
	WORD		theflag;

	obj = 0;
	flag = EDITABLE;
	inc = 1;
	switch(which)
	{
	  case BACKWARD:
		inc = -1;
						/* fall thru		*/
	  case FORWARD:
		obj = start_obj + inc;
		break;
	  case DEFLT:
		flag = DEFAULT;
		break;
	}

	while ( obj >= 0 )
	{
	  state = ob_fs(tree, obj, &theflag);
	  if ( !(theflag & HIDETREE) &&
	       !(state & DISABLED) )
	  {
	    if (theflag & flag)
	      return(obj);
	  }
	  if (theflag & LASTOB)
	    obj = -1;
	  else
	    obj += inc;
	}
	return(start_obj);
}


	WORD
fm_inifld(tree, start_fld)
	LONG		tree;
	WORD		start_fld;
{
						/* position cursor on	*/
						/*   the starting field	*/
	if (start_fld == 0)
	  start_fld = find_obj(tree, 0, FORWARD);
	return( start_fld );
}


	WORD
fm_keybd(tree, obj, pchar, pnew_obj)
	LONG		tree;
	WORD		obj;
	WORD		*pchar;
	WORD		*pnew_obj;
{
	WORD		direction;
						/* handle character	*/
	direction = -1;
	switch( *pchar )
	{
	  case RETURN:
		obj = 0;
		direction = DEFLT;
		break;
	  case BACKTAB:
	  case UP:
		direction = BACKWARD;
		break;
	  case TAB:
	  case DOWN:
	        direction = FORWARD;
		break;
	}

	if (direction != -1)
	{
	  *pchar = 0x0;
	  *pnew_obj = find_obj(tree, obj, direction);
	  if ( (direction == DEFLT) &&
 	       (*pnew_obj != 0) )
	  {
	    ob_change(tree, *pnew_obj, 
			LWGET(OB_STATE(*pnew_obj)) | SELECTED, TRUE);
	    return(FALSE);
	  }
	}

	return(TRUE);
}



	WORD
fm_button(tree, new_obj, clks, pnew_obj)
	REG LONG	tree;
	REG WORD	new_obj;
	WORD		clks;
	WORD		*pnew_obj;
{
	REG WORD	tobj;
	WORD		orword;
	WORD		parent, state, flags;
	WORD		cont, junk, tstate, tflags;
	WORD		rets[6];

	cont = TRUE;
	orword = 0x0;

	state = ob_fs(tree, new_obj, &flags);
						/* handle touchexit case*/
						/*   if double click,	*/
						/*   then set high bit	*/
	if (flags & TOUCHEXIT)
	{
	  if (clks == 2)
	    orword = 0x8000;
	  cont = FALSE;
	}

						/* handle selectable case*/
	if ( (flags & SELECTABLE) &&
	    !(state & DISABLED) ) 
	{
		 				/* if its a radio button*/
	  if (flags & RBUTTON)
	  {
						/* check siblings to	*/
						/*   find and turn off	*/
						/*   the old RBUTTON	*/
	    parent = get_par(tree, new_obj, &junk);
	    tobj = LWGET(OB_HEAD(parent));
	    while ( tobj != parent )
	    {
	      tstate = ob_fs(tree, tobj, &tflags);
	      if ( (tflags & RBUTTON) &&
		   ( (tstate & SELECTED) || 
		     (tobj == new_obj) ) )
	      {
	        if (tobj == new_obj)
		  state = tstate |= SELECTED;
		else
		  tstate &= ~SELECTED;
		ob_change(tree, tobj, tstate, TRUE);
	      }
	      tobj = LWGET(OB_NEXT(tobj));
	    }
	  }
	  else
	  {					/* turn on new object	*/
	    if ( gr_watchbox(tree, new_obj, state ^ SELECTED, state) )
	      state ^= SELECTED;
	  }
						/* if not touchexit 	*/
						/*   then wait for 	*/
						/*   button up		*/
	  if ( (cont) &&
	       (flags & (SELECTABLE | EDITABLE)) )
	    ev_button(1, 0x00001, 0x0000, &rets[0]);
	}
						/* see if this selection*/
						/*   gets us out	*/
	if ( (state & SELECTED) &&
	     (flags & EXIT) )
	  cont = FALSE;
						/* handle click on 	*/
						/*   another editable	*/
						/*   field		*/
	if ( (cont) &&
	     ( (flags & HIDETREE) ||
	       (state & DISABLED) ||
	       !(flags & EDITABLE) ) )
	  new_obj = 0;

	*pnew_obj = new_obj | orword;
	return( cont );
}


/*
*	ForM DO routine to allow the user to interactively fill out a 
*	form.  The cursor is placed at the starting field.  This routine
*	returns the object that caused the exit to occur
*/
	WORD
fm_do(tree, start_fld)
	REG LONG	tree;
	WORD		start_fld;
{
	REG WORD	edit_obj;
	WORD		next_obj;
	WORD		which, cont;
	WORD		idx;
	WORD		rets[6];
						/* grab ownership of 	*/
						/*   screen and mouse	*/
	fm_own(TRUE);
						/* flush keyboard	*/
	fq();
						/* set clip so we can	*/
						/*   draw chars, and	*/
						/*   invert buttons	*/
	gsx_sclip(&gl_rfull);
						/* determine which is 	*/
						/*   the starting field	*/
						/*   to edit		*/
	next_obj = fm_inifld(tree, start_fld);
	edit_obj = 0;
						/* interact with user	*/
	cont = TRUE;
	while(cont)
	{
						/* position cursor on	*/
						/*   the selected 	*/
						/*   editting field	*/
	  if ( (next_obj != 0) &&
	       (edit_obj != next_obj) )
	  {
	    edit_obj = next_obj;
	    next_obj = 0;
	    ob_edit(tree, edit_obj, 0, &idx, EDINIT);
	  }
						/* wait for mouse or key */
	  which = ev_multi(MU_KEYBD | MU_BUTTON, NULLPTR, NULLPTR,
			 0x0L, 0x0002ff01L, 0x0L, &rets[0]);
						/* handle keyboard event*/
	  if (which & MU_KEYBD)
	  {
	    cont = fm_keybd(tree, edit_obj, &rets[4], &next_obj);
	    if (rets[4])
	      ob_edit(tree, edit_obj, rets[4], &idx, EDCHAR);
	  }
						/* handle button event	*/
	  if (which & MU_BUTTON)
	  {
	    next_obj = ob_find(tree, ROOT, MAX_DEPTH, rets[0], rets[1]);
 	    if (next_obj == NIL)
	    {
	      sound(TRUE, 440, 2);
	      next_obj = 0;
	    }
	    else
	      cont = fm_button(tree, next_obj, rets[5], &next_obj);
	  }
						/* handle end of field	*/
						/*   clean up		*/
	  if ( (!cont) ||
	       ((next_obj != 0) && 
		(next_obj != edit_obj)) )

	  {
	    ob_edit(tree, edit_obj, 0, &idx, EDEND);
	  }
	}
						/* give up mouse and	*/
						/*   screen ownership	*/
	fm_own(FALSE);
						/* return exit object	*/
	return(next_obj);
}


/*
*	Form DIALogue routine to handle visual effects of drawing and
*	undrawing a dialogue
*/
	WORD 
fm_dial(fmd_type, pt)
	REG WORD	fmd_type;
	REG GRECT	*pt;
{
						/* adjust tree position	*/
	gsx_sclip(&gl_rscreen);
	switch( fmd_type )
	{
	  case FMD_START:
						/* grab screen sync or	*/
						/*   some other mutual	*/
						/*   exclusion method	*/
		break;

	  case FMD_FINISH:
						/* update certain 	*/
						/*   portion of the	*/
						/*   screen		*/
		w_drawdesk(pt);
		w_update(0, pt, 0, FALSE, FALSE);
		break;
	}
	return(TRUE);
}

	WORD
fm_show(string, pwd, level)
	WORD		string;
	UWORD		*pwd;
	WORD		level;
{
	BYTE		*alert;
	LONG		ad_alert;

	ad_alert = (LONG) ADDR( alert = rs_str(string) );
	if (pwd)
	{
	  merge_str(&D.g_loc2[0], alert, pwd);
	  ad_alert = ad_g2loc;
	}
	return( fm_alert(level, ad_alert) );
}


				/* TRO 9/20/84	- entered from dosif	*/
				/* when a DOS error occurs		*/
	WORD
eralert(n, d)	
	WORD		n;		/* n = alert #, 0-5 	*/	
	WORD		d;		/* d = drive code	*/
{
	WORD		ret, level;
	WORD		*drive_let;
	WORD		**pwd;
	BYTE		drive_a[2];

	drive_a[0] = 'A' + d;
	drive_let = (WORD *) &drive_a[0];

	level = (ml_pwlv[n] & 0x00FF);
	pwd = (ml_pwlv[n] & 0xFF00) ? &drive_let : 0;

	ct_mouse(TRUE);
	ret = fm_show(ml_alrt[n], pwd, level);
	ct_mouse(FALSE);

	return (ret != 1);
}


	WORD
fm_error(n)
	WORD		 n;		/* n = dos error number */
{
	WORD		ret, string;

	if (n > 63)
	  return(FALSE);

	switch (n)
	{
	  case 2:
	  case 18:	
	  case 3:
		string = AL18ERR;
		break;
	  case 4:
		string = AL04ERR;
		break;
	  case 5:
		string = AL05ERR;
		break;
	  case 15:
		string = AL15ERR;
		break;
	  case 16:
		string = AL16ERR;
		break;
	  case 8:
	  case 10:
	  case 11:
		string = AL08ERR;
		break;
	  default:
		string = ALXXERR;
		break;
	}

	ret = fm_show(string, ((string == ALXXERR) ? &n : 0), 1);

	return (ret != 1);
}

