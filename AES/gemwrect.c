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

#define TOP 0
#define LEFT 1
#define RIGHT 2
#define BOTTOM 3
						/* in OPTIMIZE.A86	*/
EXTERN 		min();
EXTERN 		max();
						/* in WMLIB.C		*/
EXTERN WORD	w_getsize();


EXTERN VOID     everyobj();			/* in OBJOP.C		*/
						/* added for hc compiler*/
    
EXTERN THEGLO	D;

GLOBAL ORECT	*rul;
GLOBAL ORECT	gl_mkrect;


	VOID
or_start()
{
	REG WORD	i;

	rul = (ORECT *) 0;
	for(i=0; i<NUM_ORECT; i++)
	{
	  D.g_olist[i].o_link = rul;
	  rul = &D.g_olist[i];
	}

}


	ORECT
*get_orect()
{
	ORECT		*po;

	if ((po = rul) != 0)
	  rul = rul->o_link;
	return(po);
}


	ORECT
*mkpiece(tlrb, new, old)
	WORD		tlrb;
	REG ORECT	*new, *old;
{
	REG ORECT	*rl;

	rl = get_orect();
	rl->o_link = old;
						/* do common calcs	*/
	rl->o_x = old->o_x;
	rl->o_w = old->o_w;
	rl->o_y = max(old->o_y, new->o_y);
	rl->o_h = min(old->o_y + old->o_h, new->o_y + new->o_h) - rl->o_y;
						/* use override calcs	*/
	switch(tlrb)
	{
	  case TOP:
		rl->o_y = old->o_y;
		rl->o_h = new->o_y - old->o_y;
	  	break;
	  case LEFT:
		rl->o_w = new->o_x - old->o_x;
	  	break;
	  case RIGHT:
		rl->o_x = new->o_x + new->o_w;
		rl->o_w = (old->o_x + old->o_w) - (new->o_x + new->o_w);
	  	break;
	  case BOTTOM:
		rl->o_y = new->o_y + new->o_h;
		rl->o_h = (old->o_y + old->o_h) - (new->o_y + new->o_h);
	  	break;
	}
	return(rl);
}


	ORECT
*brkrct(new, r, p)
	REG ORECT	*new, *r, *p;
{
	REG WORD	i;
	WORD		have_piece[4];
						/* break up rectangle r	*/
						/*   based on new,	*/
						/*   adding new orects	*/
						/*   to list p		*/

	if ( (new->o_x < r->o_x + r->o_w) &&
	     (new->o_x + new->o_w > r->o_x) &&
	     (new->o_y < r->o_y + r->o_h) &&
	     (new->o_y + new->o_h > r->o_y) )
	{
						/* there was overlap	*/
						/*   so we need new	*/
						/*   rectangles		*/
	  have_piece[TOP] = ( new->o_y > r->o_y );
	  have_piece[LEFT] = ( new->o_x > r->o_x );
	  have_piece[RIGHT] = ( new->o_x + new->o_w < r->o_x + r->o_w );
	  have_piece[BOTTOM] = ( new->o_y + new->o_h < r->o_y + r->o_h );


	  for(i=0; i<4; i++)
	  {
	    if ( have_piece[i] )
	      p = (p->o_link = mkpiece(i, new, r));
	  }
						/* take out the old guy	*/
	  p->o_link = r->o_link;
	  r->o_link = rul;
	  rul = r;
	  return(p);
	}
	return(NULLPTR);
}


	VOID
mkrect(tree, wh)
	LONG		tree;			/* place holder for everyobj */
	WORD		wh;
{
	REG WINDOW	*pwin;
	ORECT		*new;
	REG ORECT	*r, *p;

	pwin = &D.w_win[wh];
						/* get the new rect	*/
						/*   that is used for	*/
						/*   breaking other 	*/
						/*   this windows rects	*/
	new = &gl_mkrect;
						/* 			*/
	r = (p = &pwin->w_rlist)->o_link;
						/* redo rectangle list	*/
	while ( r )
	{
	  if ( ( p = brkrct(new, r, p)) != 0  )
	  {
						/* we broke a rectangle	*/
						/*   which means this	*/
						/*   can't be blt	*/
	    pwin->w_flags |=  VF_BROKEN;
	    r = p->o_link;
	  }
	  else
	    r = (p = r)->o_link;
	}
}



	VOID
newrect(tree, wh)
	LONG		tree;
	WORD		wh;
{
	REG WINDOW	*pwin;
	REG ORECT	*r, *new;
	ORECT		*r0;

	pwin = &D.w_win[wh];
	r0 = pwin->w_rlist;
						/* dump rectangle list	*/
	if ( r0 )
	{
	  for (r=r0; r->o_link; r=r->o_link);
	  r->o_link = rul;
	  rul = r0;
	}
						/* zero the rectangle	*/
						/*   list		*/
	pwin->w_rlist = 0x0;
						/* start out with no	*/
						/*   broken rectangles	*/
	pwin->w_flags &= ~VF_BROKEN;
						/* if no size then	*/
						/*   return		*/
	w_getsize(WS_TRUE, wh, &gl_mkrect.o_x);
	if ( !(gl_mkrect.o_w && gl_mkrect.o_h) )
	  return;
						/* init. a global orect	*/
						/*   for use during 	*/
						/*   mkrect calls	*/
	gl_mkrect.o_link = (ORECT *) 0x0;
						/* break other window's	*/
						/*   rects with our 	*/
						/*   current rect	*/
	everyobj(tree, ROOT, wh, mkrect, 0, 0, MAX_DEPTH);
						/* get an orect in this	*/
						/*   windows list 	*/
	new = get_orect();
	new->o_link  = (ORECT *) 0x0;
	w_getsize(WS_TRUE, wh, &new->o_x);
	pwin->w_rlist = new;
}


