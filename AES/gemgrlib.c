/*	GEMGRLIB.C	4/11/84 - 09/20/85	Gregg Morris		*/
/*	merge High C vers. w. 2.2 		8/21/87		mdf	*/ 

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
						/* in GSX.EXE		*/
EXTERN VOID	gsx_moff();
EXTERN VOID	gsx_mon();

/* ------------- added for metaware compiler  ------------- */
EXTERN		dsptch();			/* in ASM.A86 		*/
EXTERN WORD     ev_multi();			/* in EVLIB.C 		*/
EXTERN VOID     gsx_sclip();			/* in GRAF.C		*/
EXTERN VOID     gsx_attr();
EXTERN VOID     gsx_xcbox();
EXTERN VOID     gsx_xbox();
EXTERN WORD     max();				/* in OPTIMOPT.A86	*/
EXTERN VOID     r_set();
EXTERN WORD     rc_equal();
EXTERN VOID     wm_update();			/* in WMLIB.C		*/
EXTERN VOID     rc_constrain();			/* in OPTIMIZE.C	*/
EXTERN VOID     ob_actxywh();			/* in OBLIB.C		*/
EXTERN VOID     ob_change();
EXTERN VOID     ob_relxywh();
EXTERN WORD     mul_div();			/* in GSX2.A86		*/
/* --------------------------------------------------------- */

EXTERN GRECT	gl_rscreen;
EXTERN GRECT	gl_rzero;

EXTERN WORD	xrat, yrat, button, kstate;

/*
*	Routine to watch the mouse while the button is down and
*	it stays inside/outside of the specified rectangle.
*	Return TRUE as long as the mouse is down.  Block until the
*	mouse moves into or out of the specified rectangle.
*/
	WORD
gr_stilldn(out, x, y, w, h)
	WORD		out, x, y, w, h;
{
	WORD		rets[6];
					/* compiler had better	*/
					/*   put the values out,*/
					/*   x, y, w, h in the	*/
					/*   right order on the	*/
					/*   stack to form a	*/
					/*   MOBLK		*/
	dsptch();
	if ( MU_BUTTON & ev_multi(MU_KEYBD | MU_BUTTON | MU_M1, &out, 
		NULLPTR, 0x0L, 0x0001ff00L, 0x0L, &rets[0]) )
	  return(FALSE);
	return(TRUE);
} /* gr_stilldn */

	VOID
gr_setup(color)
	WORD		color;
{
	gsx_sclip(&gl_rscreen);
	gsx_attr(FALSE, MD_XOR, color);
}

	VOID
gr_clamp(xorigin, yorigin, wmin, hmin, pneww, pnewh)
	WORD		xorigin, yorigin;
	WORD		wmin, hmin;
	WORD		*pneww, *pnewh;
{
	*pneww = max(xrat - xorigin + 1, wmin);
	*pnewh = max(yrat - yorigin + 1, hmin);
}

	VOID
gr_scale(xdist, ydist, pcnt, pxstep, pystep)
	REG WORD	xdist, ydist;
	WORD		*pcnt;
	REG		*pxstep, *pystep;
{
	REG WORD	i;
	REG WORD	dist;


	gr_setup(BLACK);

	dist = (xdist + ydist) / 2;

	for(i=0; dist; i++)
	  dist /= 2;

	if ((*pcnt = i) != 0)
	{
	  *pxstep = max(1, xdist / i);
	  *pystep = max(1, ydist / i);
	}
	else
	  *pxstep = *pystep = 1;
}


	VOID
gr_stepcalc(orgw, orgh, pt, pcx, pcy, pcnt, pxstep, pystep)
	WORD		orgw, orgh;
	REG GRECT	*pt;
	REG WORD	*pcx, *pcy;
	WORD		*pcnt, *pxstep, *pystep;
{
	*pcx = (pt->g_w/2) - (orgw/2);
	*pcy = (pt->g_h/2) - (orgh/2);

	gr_scale(*pcx, *pcy, pcnt, pxstep, pystep);

	*pcx += pt->g_x;
	*pcy += pt->g_y;
}


	VOID
gr_xor(clipped, cnt, cx, cy, cw, ch, xstep, ystep, dowdht)
	WORD		clipped;
	WORD		cnt;
	WORD		cx, cy, cw, ch;
	REG WORD	xstep, ystep;
	WORD		dowdht;
{
	do
	{
	  if (clipped)
	    gsx_xcbox(&cx);
	  else
	    gsx_xbox(&cx);
	  cx -= xstep;
	  cy -= ystep;
	  if (dowdht)
	  {
	    cw += (2 * xstep);
	    ch += (2 * ystep);
	  }
	} while (cnt--);
}


	VOID
gr_draw(have2box, po, poff)
	WORD		have2box;
	REG GRECT	*po;
	REG GRECT	*poff;
{
	GRECT		t;

	gsx_xbox(po);
	if (have2box)
	{
	  r_set(&t, po->g_x + poff->g_x, po->g_y + poff->g_y,
		      po->g_w + poff->g_w, po->g_h + poff->g_h );
	  gsx_xbox(&t);
	}
}


	WORD
gr_wait(po, poff)
	REG GRECT	*po;
	REG GRECT	*poff;
{
	REG WORD	have2box;
	REG WORD	down;
	
	have2box = !rc_equal(&gl_rzero, poff);
						/* draw/erase old	*/
	gsx_moff();
	gr_draw(have2box, po, poff);
	gsx_mon();
						/* wait for change	*/
	down = gr_stilldn(TRUE, xrat, yrat, 1, 1);
						/* draw/erase old	*/
	gsx_moff();
	gr_draw(have2box, po, poff);
	gsx_mon();
						/* return exit event	*/
	return(down);
}


/*
*	Stretch the free corner of an XOR box(w,h) that is pinned at
*	another corner based on mouse movement until the button comes
*	up.  Also draw a second box offset from the stretching box.
*/
	VOID
gr_rubwind(xorigin, yorigin, wmin, hmin, poff, pwend, phend)
	WORD		xorigin, yorigin;
	WORD		wmin, hmin;
	GRECT		*poff;
	WORD		*pwend, *phend;
{
	WORD		down;
	GRECT		o;

	
	wm_update(TRUE);
	gr_setup(BLACK);
	
	r_set(&o, xorigin, yorigin, 0, 0);
						/* clamp size of rubber	*/
						/*   box to no smaller	*/
						/*   than wmin, hmin	*/
	do
	{
	  gr_clamp(o.g_x, o.g_y, wmin, hmin, &o.g_w, &o.g_h);
	  down = gr_wait(&o, poff);
	} while (down);
	*pwend = o.g_w;
	*phend = o.g_h;
	wm_update(FALSE);
} /* gr_rubwind */


/*
*	Stretch the free corner of a XOR box(w,h) that is pinned at
*	another corner based on mouse movement until the button comes
*	up.  This is also called a rubber-band box.
*/
	VOID
gr_rubbox(xorigin, yorigin, wmin, hmin, pwend, phend)
	WORD		xorigin, yorigin;
	WORD		wmin, hmin;
	WORD		*pwend, *phend;
{
	gr_rubwind(xorigin, yorigin, wmin, hmin, &gl_rzero, pwend, phend);
}



/*
*	Drag a moving XOR box(w,h) that tracks relative to the mouse
*	until the button comes up.  The starting x and y represent
*	the location of the upper left hand corner of the rectangle
*	relative to the mouse position.  This relative distance should
*	be maintained.  A constraining rectangle is also given.  The
*	box should not be able to be dragged out of the contraining
*	rectangle.
*/
	VOID
gr_dragbox(w, h, sx, sy, pc, pdx, pdy)
	WORD		w, h;
	REG WORD	sx, sy;
	GRECT		*pc;
	WORD		*pdx, *pdy;
{
	WORD		offx, offy, down;
	GRECT		o;

	wm_update(TRUE);
	gr_setup(BLACK);

	gr_clamp(sx+1, sy+1, 0, 0, &offx, &offy);
	r_set(&o, sx, sy, w, h);

						/* get box's x,y from	*/
						/*   mouse's x,y then	*/
						/*   constrain result	*/
	do
	{
	  o.g_x = xrat - offx;
	  o.g_y = yrat - offy;
	  rc_constrain(pc, &o);
	  down = gr_wait(&o, &gl_rzero);
	} while (down);
	*pdx = o.g_x;
	*pdy = o.g_y;
	wm_update(FALSE);
} /* gr_dragbox */


	VOID
gr_2box(flag1, cnt, pt, xstep, ystep, flag2)
	WORD		flag1;
	WORD		cnt;
	GRECT		*pt;
	WORD		xstep, ystep;
	WORD		flag2;
{
	REG WORD	i;

	gsx_moff();
	for (i=0; i<2; i++)
	  gr_xor(flag1, cnt, pt->g_x, pt->g_y, pt->g_w, pt->g_h, 
			xstep, ystep, flag2);
	gsx_mon();
}


/*
*	Draw a moving XOR box(w,h) that moves from a point whose upper
*	left corner is at src_x, src_y to a point at dst_x, dst_y
*/

	VOID
gr_movebox(w, h, srcx, srcy, dstx, dsty)
	WORD		w, h;
	REG WORD	srcx, srcy;
	WORD		dstx, dsty;
{
	REG WORD	signx, signy;
	WORD		cnt, xstep, ystep;
	GRECT		t;

	r_set(&t, srcx, srcy, w, h);

	signx = (srcx < dstx) ? -1 : 1;
	signy = (srcy < dsty) ? -1 : 1;

	gr_scale(signx*(srcx-dstx), signy*(srcy-dsty), &cnt, &xstep, &ystep);

	gr_2box(FALSE, cnt, &t, signx*xstep, signy*ystep, FALSE);
}


/*
*	Draw an small box that moves from the origin x,y to a spot
*	centered within the rectangle and then expands to match the
*	size of the rectangle.
*/
/*
	VOID
gr_growbox(po, pt)
	REG GRECT	*po;
	GRECT		*pt;
{
	WORD		cx, cy;
	WORD		cnt, xstep, ystep;

	gr_stepcalc(po->g_w, po->g_h, pt, &cx, &cy, &cnt, &xstep, &ystep);
	gr_movebox(po->g_w, po->g_h, po->g_x, po->g_y, cx, cy);
	po->g_x = cx;
	po->g_y = cy;
	gr_2box(TRUE, cnt, po, xstep, ystep, TRUE);
}
*/

/*
*	Draw a box that shrinks from the rectangle given down around
*	a small box centered within the rectangle and then moves to the
*	origin point.
*/
/*
	VOID
gr_shrinkbox(po, pt)
	REG GRECT	*po;
	REG GRECT	*pt;
{
	WORD		cx, cy;
	WORD		cnt, xstep, ystep;

	gr_stepcalc(po->g_w, po->g_h, pt, &cx, &cy, &cnt, &xstep, &ystep);
	gr_2box(TRUE, cnt, pt, -xstep, -ystep, TRUE);
	gr_movebox(po->g_w, po->g_h, cx, cy, po->g_x, po->g_y);
}
*/

	WORD
gr_watchbox(tree, obj, instate, outstate)
	REG LONG	tree;
	REG WORD	obj;
	WORD		instate;
	WORD		outstate;
{
	REG WORD	out;
	REG WORD	state;
	GRECT		t;

	gsx_sclip(&gl_rscreen);
	ob_actxywh(tree, obj, &t);
	
	out = FALSE;
	do
	{
	  state = (out) ? outstate : instate;
	  ob_change(tree, obj, state, TRUE);
	  out = !out;
	} while( gr_stilldn(out, t.g_x, t.g_y, t.g_w, t.g_h) );

	return( out );
}


	WORD
gr_slidebox(tree, parent, obj, isvert)
	REG LONG	tree;
	WORD		parent;
	WORD		obj;
	WORD		isvert;
{
	GRECT		t, c;
	WORD		divnd, divis;

	ob_actxywh(tree, parent, &c);
	ob_relxywh(tree, obj, &t);
	gr_dragbox(t.g_w, t.g_h, t.g_x + c.g_x, t.g_y + c.g_y, 
			&c, &t.g_x, &t.g_y); 

	if ( isvert )
	{
	  divnd = t.g_y - c.g_y;
	  divis = c.g_h - t.g_h;
	}
	else
	{
	  divnd = t.g_x - c.g_x;
	  divis = c.g_w - t.g_w;
	}
	if (divis)
	  return( mul_div( divnd, 1000, divis) );
	else
	  return(0);
}


	VOID
gr_mkstate(pmx, pmy, pmstat, pkstat)
	WORD		*pmx, *pmy, *pmstat, *pkstat;
{
	*pmx = xrat;
	*pmy = yrat;
	*pmstat = button;
	*pkstat = kstate;
}


