/*	GEMWMLIB.C 	4/23/84 - 07/11/85	Lee Lorenzen		*/
/*	merge High C vers. w. 2.2 		8/24/87		mdf	*/ 
/*	fix wm_delete bug			10/8/87		mdf	*/

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

#define DESKWH	0x0

#if MULTIAPP
#define NUM_MWIN NUM_WIN+1
#define INACROOT NUM_WIN
EXTERN WORD	shrunk;
EXTERN SHELL	sh[];
EXTERN LONG	ad_armice;
EXTERN LONG 	ad_sysmenu;
#endif
#if SINGLAPP
#define NUM_MWIN NUM_WIN
#endif

WORD		w_bldactive();

EXTERN PD	*fpdnm();
						/* in GEMFLAG.C		*/
EXTERN WORD	unsync();
EXTERN WORD	tak_flag();
						/* in GSXIF.C		*/
EXTERN WORD	gsx_gclip();
EXTERN VOID	gsx_sclip();
EXTERN VOID	gsx_mret();
						/* in OBLIB.C		*/
EXTERN VOID	everyobj();
EXTERN VOID	ob_delete();
EXTERN VOID	ob_draw();
EXTERN VOID	ob_order();
EXTERN WORD	ob_find();
						/* in WRECT88.C		*/
EXTERN ORECT	*get_orect();
EXTERN VOID	newrect();

EXTERN LONG	ad_stdesk;
EXTERN LONG	gl_mntree;
EXTERN LONG	menu_tree[];

EXTERN GRECT	gl_rfull;

GLOBAL LONG	gl_newdesk;			/* current desktop back-*/
						/* ground pattern.	*/
GLOBAL WORD	gl_newroot;			/* current object w/in	*/
						/* gl_newdesk.		*/
GLOBAL LONG	desk_tree[NUM_PDS];		/* list of object trees	*/
						/* for the desktop back-*/
						/* ground pattern.	*/
GLOBAL WORD	desk_root[NUM_PDS];		/* starting object to	*/
						/* draw within desk_tree.*/

GLOBAL OBJECT	W_TREE[NUM_MWIN];
GLOBAL OBJECT	W_ACTIVE[NUM_ELEM];

#if MULTIAPP
#define WSTRSIZE 81
GLOBAL BYTE	W_NAMES[NUM_WIN][WSTRSIZE];
GLOBAL BYTE	W_INFOS[NUM_WIN][WSTRSIZE];
GLOBAL PD	*W_INACTIVE[NUM_WIN];		/* pds of inactive windows*/
GLOBAL PD	*gl_newmenu = (PD *)0x0;
GLOBAL PD	*gl_lastnpd;
GLOBAL WORD	proc_msg[8];
#endif

GLOBAL WORD	gl_watype[NUM_ELEM] =
{
	G_IBOX,		/* W_BOX	*/
	G_BOX,      	/* W_TITLE	*/
	G_BOXCHAR,	/* W_CLOSER	*/
	G_BOXTEXT,	/* W_NAME	*/
	G_BOXCHAR,	/* W_FULLER	*/
	G_BOXTEXT,	/* W_INFO	*/
	G_IBOX,		/* W_DATA	*/
	G_IBOX,		/* W_WORK	*/
	G_BOXCHAR,	/* W_SIZER	*/
	G_BOX,		/* W_VBAR	*/
	G_BOXCHAR,	/* W_UPARROW	*/
	G_BOXCHAR,	/* W_DNARROW	*/
	G_BOX,      	/* W_VSLIDE	*/
	G_BOX,      	/* W_VELEV	*/
	G_BOX,      	/* W_HBAR	*/
	G_BOXCHAR,	/* W_LFARROW	*/
	G_BOXCHAR,	/* W_RTARROW	*/
	G_BOX,		/* W_HSLIDE	*/
	G_BOX		/* W_HELEV	*/
} ;

GLOBAL LONG	gl_waspec[NUM_ELEM] =
{
	0x00011101L,	/* W_BOX	*/
	0x00011101L,	/* W_TITLE	*/
	0x12011101L,	/* W_CLOSER	*/
	0x0L,		/* W_NAME	*/
	0x07011101L,	/* W_FULLER	*/
	0x0L,		/* W_INFO	*/
	0x00001101L,	/* W_DATA	*/
	0x00001101L,	/* W_WORK	*/
	0x06011101L,	/* W_SIZER	*/
	0x00011101L,	/* W_VBAR	*/
	0x0C011101L,	/* W_UPARROW	*/
	0x0D011101L,	/* W_DNARROW	*/
	0x00011111L,	/* W_VSLIDE	*/
	0x00011101L,	/* W_VELEV	*/
	0x00011101L,	/* W_HBAR	*/
	0x0F011101L,	/* W_LFARROW	*/
	0x0E011101L,	/* W_RTARROW	*/
	0x00011111L,	/* W_HSLIDE	*/
	0x00011101L	/* W_HELEV	*/
} ;

GLOBAL TEDINFO	gl_aname;
GLOBAL TEDINFO	gl_ainfo;

GLOBAL TEDINFO	gl_asamp =
{
	0x0L, 0x0L, 0x0L, IBM, MD_REPLACE, TE_LEFT, SYS_FG, 0x0, 1, 80, 80
};


/* ---------- added for metaware compiler ---------- */
GLOBAL VOID	wm_update();
GLOBAL VOID 	wm_calc();
EXTERN VOID	rc_copy();			/* in OPTIMOPT.A86	*/
EXTERN WORD 	rc_equal();
EXTERN VOID 	r_set();
EXTERN VOID	bfill();
EXTERN WORD 	min();
EXTERN WORD 	max();
#if I8086
EXTERN VOID 	movs();
#endif
EXTERN WORD 	rc_intersect();			/* in OPTIMIZE.C	*/
EXTERN VOID 	rc_union();
EXTERN WORD 	mul_div();			/* in GSX2.A86		*/
EXTERN VOID	ct_chgown();			/* in CTRL.C		*/
EXTERN VOID	ap_rdwr();			/* in APLIB.C		*/
EXTERN VOID	bb_screen();			/* in GRAF.C		*/
EXTERN VOID  	gsx_moff();			/* in GSXIF.C		*/
EXTERN VOID 	gsx_mon();
EXTERN VOID 	mn_bar();			/* in MNLIB.C		*/
EXTERN VOID	or_start();			/* in WRECT.C		*/
EXTERN VOID	ev_block();			/* in EVLIB.C		*/
EXTERN VOID	fm_own();			/* in FMLIB.C		*/
/* ------------------------------------------------- */

EXTERN WORD	gl_wbox;
EXTERN WORD	gl_hbox;

EXTERN WORD	gl_width;
EXTERN WORD	gl_height;

EXTERN GRECT	gl_rscreen;
EXTERN GRECT	gl_rfull;
EXTERN GRECT	gl_rzero;


EXTERN THEGLO	D;

GLOBAL WORD	wind_msg[8];

GLOBAL LONG	ad_windspb;

GLOBAL WORD	gl_wtop;
GLOBAL LONG	gl_wtree;
GLOBAL LONG	gl_awind;


	VOID
w_nilit(num, olist)
	REG WORD	num;
	REG OBJECT	olist[];
{
	while( num-- )
	{
	  olist[num].ob_next = olist[num].ob_head = olist[num].ob_tail = NIL;
	}
}


/*
*	Routine to add a child object to a parent object.  The child
*	is added at the end of the parent's current sibling list.
*	It is also initialized.
*/
	VOID
w_obadd(olist, parent, child)
	REG OBJECT	olist[];
	REG WORD	parent, child;
{
	REG WORD	lastkid;

	if ( (parent != NIL) &&
	     (child != NIL) )
	{
	  olist[child].ob_next = parent;

	  lastkid = olist[parent].ob_tail;
	  if (lastkid == NIL)
	    olist[parent].ob_head = child;
	  else
	    olist[lastkid].ob_next = child;
	  olist[parent].ob_tail = child;
	}
}


	VOID
w_setup(ppd, w_handle, kind)
	PD		*ppd;
	WORD		w_handle;
	WORD		kind;
{
	REG WINDOW	*pwin;

	pwin = &D.w_win[w_handle];
	pwin->w_owner = ppd;
	pwin->w_flags = VF_INUSE;
	pwin->w_kind = kind;
	pwin->w_hslide = pwin->w_vslide = 0;	/* slider at left/top	*/
	pwin->w_hslsiz = pwin->w_vslsiz = -1;	/* use default size	*/
}


	WORD
*w_getxptr(which, w_handle)
	WORD		which;
	REG WORD	w_handle;
{
	switch(which)
	{
	  case WS_CURR:
	  case WS_TRUE:
		return( &W_TREE[w_handle].ob_x );
	  case WS_PREV:
		return( &D.w_win[w_handle].w_xprev );
	  case WS_WORK:
		return( &D.w_win[w_handle].w_xwork );
	  case WS_FULL:
		return( &D.w_win[w_handle].w_xfull );
	}
}

/* Get the size (x,y,w,h) of the window				*/

	VOID
w_getsize(which, w_handle, pt)
	REG WORD	which;
	WORD		w_handle;
	REG GRECT	*pt;
{
	rc_copy(w_getxptr(which, w_handle), pt);
	if ( (which == WS_TRUE) && pt->g_w && pt->g_h)
	{
	  pt->g_w += 2;
	  pt->g_h += 2;
	}
}


	VOID
w_setsize(which, w_handle, pt)
	WORD		which;
	WORD		w_handle;
	GRECT		*pt;
{
	rc_copy(pt, w_getxptr(which, w_handle));
}


        VOID
w_adjust(parent, obj, x, y, w, h)
	WORD		parent;
	REG WORD	obj;
	WORD		x, y, w, h;
{
	rc_copy(&x, &W_ACTIVE[obj].ob_x);
	W_ACTIVE[obj].ob_head = W_ACTIVE[obj].ob_tail = NIL;
	w_obadd(&W_ACTIVE[ROOT], parent, obj);
}


	VOID
w_hvassign(isvert, parent, obj, vx, vy, hx, hy, w, h)
	WORD		isvert;
	REG WORD	parent, obj;
	WORD		vx, vy, hx, hy, w, h;
{
	if ( isvert )
 	  w_adjust(parent, obj, vx, vy, gl_wbox, h);
	else
 	  w_adjust(parent, obj, hx, hy, w, gl_hbox);
}


/*
*	Walk the list and draw the parts of the window tree owned by
*	this window.
*/

do_walk(wh, tree, obj, depth, pc)
	REG WORD	wh;
	LONG		tree;
	WORD		obj;
	WORD		depth;
	REG GRECT	*pc;
{
	REG ORECT	*po;
	GRECT		t;
	
	if ( wh == NIL )
	  return(TRUE);
						/* clip to screen	*/
	if (pc)
	  rc_intersect(&gl_rfull, pc);
	else
	  pc = &gl_rfull;
						/* walk owner rect list	*/
	for(po=D.w_win[wh].w_rlist; po; po=po->o_link)
	{
	  rc_copy(&po->o_x, &t);
						/* intersect owner rect	*/
						/*   with clip rect's	*/
	  if ( rc_intersect(pc, &t) )
	  {
						/*  set clip and draw	*/
	    gsx_sclip(&t);
	    ob_draw(tree, obj, depth);
	  }
	}
}


/*
*	Draw the desktop background pattern underneath the current
*	set of windows.
*/

        VOID
w_drawdesk(pc)
	REG GRECT	*pc;
{
	REG LONG	tree;
	REG WORD	depth;
	REG WORD	root;
	GRECT		pt;
	
	rc_copy(pc, &pt);
	if (gl_newdesk)
	{
	  tree = gl_newdesk;
	  depth = MAX_DEPTH;
	  root = gl_newroot;
	}
	else
	{
	  tree = gl_wtree;
	  depth = 0;
	  root = ROOT;
	}
						/* account for drop	*/
						/*   shadow		*/
						/* BUGFIX in 2.1	*/
	pt.g_w += 2;
	pt.g_h += 2;

	do_walk(DESKWH, tree, root, depth, pc);
}


        VOID
w_cpwalk(wh, obj, depth, usetrue)
	REG WORD	wh;
	WORD		obj;
	WORD		depth;
	WORD		usetrue;
{
	GRECT		c;
						/* start with window's	*/
						/*   true size as clip	*/
	if ( usetrue )
	  w_getsize(WS_TRUE, wh, &c);
	else
	{
						/* use global clip	*/
	  gsx_gclip(&c);
						/* add in drop shadow	*/
	  c.g_w += 2;
	  c.g_h += 2;
	}
	w_bldactive(wh);
	do_walk(wh, gl_awind, obj, depth, &c);
}


/*
*	Build an active window and draw the all parts of it but clip
*	these parts with the owner rectangles and the passed in
*	clip rectangle.
*/

	VOID
w_clipdraw(wh, obj, depth, usetrue)
	WORD		wh;
	WORD		obj;
	WORD		depth;
	WORD		usetrue;
{
	WORD		i;

	if ( (usetrue == 2) ||
	     (usetrue == 0) )
	{
	  for(i=W_TREE[ROOT].ob_head; i>ROOT; i=W_TREE[i].ob_next)
	  {
	    if ( (i != wh) &&
	         (D.w_win[i].w_owner == D.w_win[wh].w_owner) &&
	         (D.w_win[i].w_flags & VF_SUBWIN) &&
	         (D.w_win[wh].w_flags & VF_SUBWIN) )
	      w_cpwalk(i, obj, depth, TRUE);
	  }
	}
						/* build active tree	*/
	w_cpwalk(wh, obj, depth, usetrue);
}


	VOID 
w_strchg(w_handle, obj, pstring)
	REG WORD	w_handle;
	REG WORD	obj;
	REG LONG	pstring;
{
#if MULTIAPP
	if ( obj == W_NAME )
	{
	  LBCOPY( ADDR(W_NAMES[w_handle]), pstring, WSTRSIZE);
	  W_NAMES[w_handle][WSTRSIZE-1] = 0;
	  gl_aname.te_ptext = pstring;
	}
	else
	{
	  LBCOPY( ADDR(W_INFOS[w_handle]), pstring, WSTRSIZE);
	  W_INFOS[w_handle][WSTRSIZE-1] = 0;
	  gl_ainfo.te_ptext = pstring;
	}
#endif
#if SINGLAPP
	if ( obj == W_NAME )
	  gl_aname.te_ptext = D.w_win[w_handle].w_pname = pstring;
	else
	  gl_ainfo.te_ptext = D.w_win[w_handle].w_pinfo = pstring;
#endif	
	w_clipdraw(w_handle, obj, MAX_DEPTH, 1);
}


	VOID
w_barcalc(isvert, space, sl_value, sl_size, min_sld, ptv, pth)
	WORD		isvert;
	REG WORD	space;
	REG WORD	sl_value, sl_size;
	REG WORD	min_sld;
	GRECT		*ptv, *pth;
{
	if (sl_size == -1)
	  sl_size = min_sld;
	else
	  sl_size = max(min_sld, mul_div(sl_size, space, 1000) );

	sl_value = mul_div(space - sl_size, sl_value, 1000);
	if (isvert)
	  r_set(ptv, 3, sl_value, gl_wbox-6, sl_size);
	else
	  r_set(pth, sl_value, 2, sl_size, gl_hbox-4);
}


	VOID
w_bldbar(kind, istop, w_bar, sl_value, sl_size, x, y, w, h)
	UWORD		kind;
	WORD		istop;
	WORD		w_bar, sl_value, sl_size;
	REG WORD	x, y, w, h;
{
	WORD		isvert, obj;
	UWORD		upcmp, dncmp, slcmp;		
	REG WORD	w_up;
	WORD		w_dn, w_slide, space, min_sld;

	isvert = (w_bar == W_VBAR);
	if ( isvert )
	{
	  upcmp = UPARROW;
	  dncmp = DNARROW;
	  slcmp = VSLIDE;
	  w_up = W_UPARROW;
	  w_dn = W_DNARROW;
	  w_slide = W_VSLIDE;
	  min_sld = gl_hbox;
	}
	else
	{
	  upcmp = LFARROW;
	  dncmp = RTARROW;
	  slcmp = HSLIDE;
	  w_up = W_LFARROW;
	  w_dn = W_RTARROW;
	  w_slide = W_HSLIDE;
	  min_sld = gl_wbox;
	}

	w_hvassign(isvert, W_DATA, w_bar, x, y, x, y, w, h);
	x = y = 0;
	if ( istop )
	{
	  if (kind & upcmp)
	  {
	    w_adjust(w_bar, w_up, x, y, gl_wbox, gl_hbox);
	    if ( isvert )
	    {
	      y += (gl_hbox - 1);
	      h -= (gl_hbox - 1);
	    }
	    else
	    {
	      x += (gl_wbox - 1);
	      w -= (gl_wbox - 1);
	    }
	  }
	  if ( kind & dncmp )
	  {
	    w -= (gl_wbox - 1);
	    h -= (gl_hbox - 1);
	    w_hvassign(isvert, w_bar, w_dn, x, y + h - 1, 
			x + w - 1, y, gl_wbox, gl_hbox);
	  }
	  if ( kind & slcmp )
	  {
	    w_hvassign(isvert, w_bar, w_slide, x, y, x, y, w, h);
	    space = (isvert) ? h : w;

	    w_barcalc(isvert, space, sl_value, sl_size, min_sld, 
		  &W_ACTIVE[W_VELEV].ob_x, &W_ACTIVE[W_HELEV].ob_x);

	    obj = (isvert) ? W_VELEV : W_HELEV;
	    W_ACTIVE[obj].ob_head = W_ACTIVE[obj].ob_tail = NIL;
	    w_obadd(&W_ACTIVE[ROOT], w_slide, obj);
	  }
	}
}

	WORD
w_top()
{
	return( (gl_wtop != NIL) ? gl_wtop : DESKWH );
}

	VOID
w_setactive()
{
	GRECT		d;
	REG WORD	wh;
	PD		*ppd;

	wh = w_top();
	w_getsize(WS_WORK, wh, &d);
	ppd = D.w_win[wh].w_owner;
						/* BUGFIX 2.1		*/
						/*  don't chg own if null*/
	if (ppd != NULLPTR)
	  ct_chgown(ppd, &d);
}

	WORD
w_bldactive(w_handle)
	REG WORD	w_handle;
{
	WORD		istop, issub;
	REG WORD	kind;
	REG WORD	havevbar;
	REG WORD	havehbar;
	GRECT		t;
	REG WORD	tempw;
	WORD		offx, offy;
	WINDOW		*pw;

	if (w_handle == NIL)
	  return(TRUE);

	pw = &D.w_win[w_handle];
						/* set if it is on top	*/
	istop = (gl_wtop == w_handle);
						/* get the kind of windo*/
	kind = pw->w_kind;
	w_nilit(NUM_ELEM, &W_ACTIVE[0]);
						/* start adding pieces	*/
						/*   & adjusting sizes	*/
#if MULTIAPP
	gl_aname.te_ptext = ADDR(W_NAMES[w_handle]);
	gl_ainfo.te_ptext = ADDR(W_INFOS[w_handle]);
#endif
#if SINGLAPP
	gl_aname.te_ptext = pw->w_pname;
	gl_ainfo.te_ptext = pw->w_pinfo;
#endif
	gl_aname.te_just = TE_CNTR;
	issub = ( (pw->w_flags & VF_SUBWIN) &&
		  (D.w_win[gl_wtop].w_flags & VF_SUBWIN) );
	w_getsize(WS_CURR, w_handle, &t);
	rc_copy(&t, &W_ACTIVE[W_BOX].ob_x);
	offx = t.g_x;
	offy = t.g_y;
						/* do title area	*/
	t.g_x = t.g_y = 0;
	if ( kind & (NAME | CLOSER | FULLER) )
	{
	  w_adjust(W_BOX, W_TITLE, t.g_x, t.g_y, t.g_w, gl_hbox);
	  tempw = t.g_w;
	  if ( (kind & CLOSER) &&
	       ( istop || issub ) )
	  {
	    w_adjust(W_TITLE, W_CLOSER, t.g_x, t.g_y, gl_wbox, gl_hbox);
	    t.g_x += gl_wbox;
	    tempw -= gl_wbox;
	  }
	  if ( (kind & FULLER) &&
	       ( istop || issub ) )
	  {
	    tempw -= gl_wbox;
	    w_adjust(W_TITLE, W_FULLER, t.g_x + tempw, t.g_y, 
			gl_wbox, gl_hbox);
	  }
	  if ( kind & NAME )
	  {
	    w_adjust(W_TITLE, W_NAME, t.g_x, t.g_y, tempw, gl_hbox);
	    W_ACTIVE[W_NAME].ob_state = (istop || issub) ? NORMAL : DISABLED;

/* APPLE  no pattern in window title
	    gl_aname.te_color = (istop && (!issub)) ? WTS_FG : WTN_FG;
*/
	  }
	  t.g_x = 0;
	  t.g_y += (gl_hbox - 1);
	  t.g_h -= (gl_hbox - 1);
	}
						/* do info area		*/
	if ( kind & INFO )
	{
	  w_adjust(W_BOX, W_INFO, t.g_x, t.g_y, t.g_w, gl_hbox);
	  t.g_y += (gl_hbox - 1);
	  t.g_h -= (gl_hbox - 1);
	}
						/* do data area		*/
	w_adjust(W_BOX, W_DATA, t.g_x, t.g_y, t.g_w, t.g_h);
						/* do work area		*/
	t.g_x++;
	t.g_y++;
	t.g_w -= 2;
	t.g_h -= 2;
	havevbar = kind & (UPARROW | DNARROW | VSLIDE | SIZER);
	havehbar = kind & (LFARROW | RTARROW | HSLIDE | SIZER);
	if ( havevbar )
	  t.g_w -= (gl_wbox - 1);
	if ( havehbar )
	  t.g_h -= (gl_hbox - 1);

	t.g_x += offx;
	t.g_y += offy;

	t.g_x = t.g_y = 1;
	w_adjust(W_DATA, W_WORK, t.g_x, t.g_y, t.g_w, t.g_h);
						/* do vert. area	*/
	if ( havevbar )
	{
	  t.g_x += t.g_w;
	  w_bldbar(kind, istop || issub, W_VBAR, pw->w_vslide, 
			pw->w_vslsiz, t.g_x, 0, 
			t.g_w + 2, t.g_h + 2);
	}
						/* do horiz area	*/
	if ( havehbar )
	{
	  t.g_y += t.g_h;
	  w_bldbar(kind, istop || issub, W_HBAR, pw->w_hslide,
			pw->w_hslsiz, 0, t.g_y, 
			t.g_w + 2, t.g_h + 2);
	}
						/* do sizer area	*/
	if ( (havevbar) &&
	     (havehbar) )
	{
	  w_adjust(W_DATA, W_SIZER, t.g_x, t.g_y, gl_wbox, gl_hbox);
	  W_ACTIVE[W_SIZER].ob_spec = 
		(istop && (kind & SIZER)) ? 0x06011100L: 0x00011100L;
	}
}

	VOID
ap_sendmsg(ap_msg, type, towhom, w3, w4, w5, w6, w7)
	REG WORD	ap_msg[];
	WORD		type;
	PD		*towhom;
	WORD		w3, w4, w5, w6, w7;
{
	ap_msg[0] = type;
	ap_msg[1] = rlr->p_pid;
	ap_msg[2] = 0;
	ap_msg[3] = w3;
	ap_msg[4] = w4;
	ap_msg[5] = w5;
	ap_msg[6] = w6;
	ap_msg[7] = w7;
	ap_rdwr(MU_SDMSG, towhom, 16, ADDR(&ap_msg[0]));
}



/*
*	Walk down ORECT list and accumulate the union of all the owner
*	rectangles.
*/
	WORD	
w_union(po, pt)
	REG ORECT	*po;
	REG GRECT	*pt;
{
	if (!po)
	  return(FALSE);

	rc_copy(&po->o_x, pt);

	po = po->o_link;
	while (po)
	{
	  rc_union(&po->o_x, pt);
	  po = po->o_link;
	}
	return(TRUE);
}



	VOID
w_redraw(w_handle, pt)
	REG WORD	w_handle;
	GRECT		*pt;
{
	GRECT		t, d;
	PD		*ppd;

	ppd = D.w_win[w_handle].w_owner;
#if MULTIAPP
	if ( !sh[ppd->p_pid].sh_isgem )		/* get out if not a gemapp*/
	  return;
#endif
						/* make sure work rect	*/
						/*   and word rect	*/
						/*   intersect		*/
	rc_copy(pt, &t);
	w_getsize(WS_WORK, w_handle, &d);
	if ( rc_intersect(&t, &d) )
	{
						/* make sure window has	*/
						/*   owns a rectangle	*/
	  if ( w_union(D.w_win[w_handle].w_rlist, &d) )
	  {
						/* intersect redraw	*/
						/*   rect with union	*/
						/*   of owner rects	*/
	    if ( rc_intersect(&d, &t) )
	      ap_sendmsg(wind_msg, WM_REDRAW, ppd,
			w_handle, t.g_x, t.g_y, t.g_w, t.g_h);
	  }
	}
}


/*
*	Routine to fix rectangles in preparation for a source to
*	destination blt.  If the source is at -1, then the source
*	and destination left fringes need to be realigned.
*/
	WORD
w_mvfix(ps, pd)
	REG GRECT	*ps;
	REG GRECT	*pd;
{
	REG WORD	tmpsx;
	
	tmpsx = ps->g_x;
	rc_intersect(&gl_rfull, ps);
	if (tmpsx == -1)
	{
	  pd->g_x++;
	  pd->g_w--;
	  return(TRUE);
	}
	return(FALSE);
}

/*
*	Call to move top window.  This involves BLTing the window if
*	none of it that is partially off the screen needs to be redraw,
*	else the whole desktop to just updated.  All uncovered portions
*	of the desktop are redrawn by later by calling w_update.
*/
	WORD	
w_move(w_handle, pstop, prc)
	REG WORD	w_handle;
	REG WORD	*pstop;
	GRECT		*prc;
{
	GRECT		s;			/* source		*/
	GRECT		d;			/* destination		*/
	REG GRECT	*pc;
	REG WORD	sminus1, dminus1;

	w_getsize(WS_PREV, w_handle, &s);
	s.g_w += 2;
	s.g_h += 2;
	w_getsize(WS_TRUE, w_handle, &d);
						/* set flags for when	*/
						/*   part of the source	*/
						/*   is off the screen	*/
	if ( ( (s.g_x + s.g_w > gl_width) && (d.g_x < s.g_x) )  ||
	     ( (s.g_y + s.g_h > gl_height) && (d.g_y < s.g_y) )   )
	{
	  rc_union(&s, &d);
	  *pstop = DESKWH;
	}
	else
	{
	  *pstop = w_handle;
	}
						/* intersect with full	*/
						/*   screen and align	*/
						/*   fringes if -1 xpos	*/
	sminus1 = w_mvfix(&s, &d);
	dminus1 = w_mvfix(&d, &s);
						/* blit what we can	*/
	if ( *pstop == w_handle )
	{
	  gsx_sclip(&gl_rfull);
	  bb_screen(S_ONLY, s.g_x, s.g_y, d.g_x, d.g_y, s.g_w, s.g_h);
						/* cleanup left edge	*/
	  if (sminus1 != dminus1)
	  {
	    if (dminus1)
	      s.g_x--;
	    if (sminus1)
	    {
	      d.g_x--;
	      d.g_w = 1;
	      gsx_sclip(&d);
	      w_clipdraw(gl_wtop, 0, 0, 0);
	    }
	  }
	  pc = &s;
	}
	else
	{
	  pc = &d;
	}
						/* clean up the rest	*/
						/*   by returning	*/
						/*   clip rect		*/
	rc_copy(pc, prc);
	return( (*pstop == w_handle) );
}


/*
*	Draw windows from top to bottom.  If top is 0, then start at
*	the topmost window.  If bottom is 0, then start at the 
*	bottomost windwo.  For the first window drawn, just do the
*	insides, since DRAW_CHANGE has already drawn the outside
*	borders.
*/

	VOID
w_update(bottom, pt, top, moved, usetrue)
	REG WORD	bottom;
	REG GRECT	*pt;
	REG WORD	top;
	WORD		moved;
	WORD		usetrue;
{
	REG WORD	i, ni;
	REG WORD	done;
	
						/* limit to screen	*/
	rc_intersect(&gl_rfull, pt);
	gsx_moff();
						/* update windows from	*/
						/*   top to bottom	*/
	if (bottom == DESKWH)
	  bottom = W_TREE[ROOT].ob_head;
						/* if there are windows	*/
	if (bottom != NIL)
	{
						/* start at the top	*/ 
	  if (top == DESKWH) 
	    top = W_TREE[ROOT].ob_tail;
						/* draw windows from	*/
						/*   top to bottom	*/
	  do
	  {
						
	    if ( !((moved) && (top == gl_wtop)) )
	    {
						/* set clip and draw	*/
						/*   a window's border	*/
	      gsx_sclip(pt);
	      					/* CHANGED 1/10/86 LKW	*/
/*	      w_clipdraw(top, 0, MAX_DEPTH, 2);	!* from FALSE to 2	*!
*/
	      w_cpwalk(top, 0, MAX_DEPTH, usetrue);
						/* let appl. draw inside*/
	      w_redraw(top, pt);
	    }
						/* scan to find prev	*/
	    i = bottom;
	    done = (i == top);
	    while (i != top)
	    {
	      ni = W_TREE[i].ob_next;
	      if (ni == top)
		top = i;
	      else
		i = ni;
	    }
	  }
	  while( !done );
	}
	gsx_mon();
}

#if SINGLAPP
	VOID
w_setmen(pid)
	WORD		pid;
{
	WORD		npid;

	npid = menu_tree[pid] ? pid : 0;
	if ( gl_mntree != menu_tree[npid] )
	  mn_bar(menu_tree[npid], TRUE, npid);

	npid = desk_tree[pid] ? pid : 0;
	if (gl_newdesk != desk_tree[npid] )
	{
	  gl_newdesk = desk_tree[npid];
	  gl_newroot = desk_root[npid];
	  w_drawdesk(&gl_rscreen);
	}
}

/*
*	Routine to draw menu of top most window as the current menu bar.
*/
	VOID
w_menufix()
{
	WORD		pid;

	pid = D.w_win[w_top()].w_owner->p_pid;
	w_setmen(pid);
}
#endif

#if MULTIAPP
	VOID
w_setmen(pid)
	WORD		pid;
{
	GRECT		c;
	WORD		npid;
	LONG 		tree;
	
			/* code to find best available menu goes here */
	npid = pid;
	tree = menu_tree[npid];
	
	if (sh[npid].sh_isacc && !tree)
	{
	  tree = gl_mntree;
	  npid = gl_mnppd->p_pid;
	}
	if (!tree)
	{
	  tree = ad_sysmenu;
	  npid = 1; /* scrmgr */
	}
	
	mn_bar(tree, TRUE, npid);

	if (desk_tree[npid])
	{
	  gl_newdesk = desk_tree[npid];
	  gl_newroot = desk_root[npid];
	}
	else
	  gl_newdesk = 0x0L;

	w_drawdesk(&gl_rscreen);
}


	VOID
w_newmenu(owner)
	PD 	*owner;
{
	gl_newmenu = owner;
}

/*
*	Routine to draw menu of top most window as the current menu bar.
*/
	VOID
w_menufix(rlr)
	PD	*rlr;
{
	WORD		pid;

	if (gl_newmenu && (rlr == gl_newmenu))  /* is there a possible menu */
	{					/* change and is the new ap */
	   gl_newmenu = (PD *)0x0;		/* loaded		    */
	   w_setmen(rlr->p_pid);
	}
}

	WORD
w_clswin()
{
	WORD		i;
						/* close any open winds	*/
	wm_update(TRUE);
	for(i=1; i < NUM_WIN; i++)
	{
	  if (D.w_win[i].w_owner == rlr)
	  {
	    if (D.w_win[i].w_flags & VF_INTREE)
	      ob_delete(gl_wtree, i);
	    wm_delete(i);
	  }
	}
	wm_update(FALSE);
}

/*
*	Routine to take all windows belonging to a particular
*	process, save them on a list, and delete them from
*	the window tree.
*	IF deletions first,
*/
	VOID
oldwfix(npd, isdelete)
	PD		*npd;		/* pd of old process 	*/
	WORD		isdelete;
{
	WORD		ii, next;
	PD		*owner;

	for(ii=W_TREE[ROOT].ob_head; ii>ROOT; ii=next)
	{
	  next = W_TREE[ii].ob_next;
	  owner = D.w_win[ii].w_owner;
				/* if not an acc or dosnext	   */
				/* then make window inactive      */
	  if ( (!sh[owner->p_pid].sh_isacc) &&
	       (sh[owner->p_pid].sh_isgem) &&
	       !(D.w_win[ii].w_flags & VF_KEEPWIN) )
	  {
	    if (isdelete)
	    {
	      ob_delete(gl_wtree, ii);
	      w_obadd(&W_TREE[ROOT], INACROOT, ii);
  	      D.w_win[ii].w_flags &= ~VF_INTREE;
	    }
	    else
	      ap_sendmsg(proc_msg, WM_UNTOPPED, owner, ii, 0, 0, 0, 0);
	  }
	}
}
	VOID
newwfix(npd)
	PD		*npd;		/* pd of new process 	*/
{
	WORD		ii, next;

	for(ii=W_TREE[INACROOT].ob_head; (ii != INACROOT)&&(ii != NIL);
		ii=next)
	{
	  next = W_TREE[ii].ob_next;
	  if(D.w_win[ii].w_owner == npd)
	  {
	    D.w_win[ii].w_flags |= VF_INTREE;
	    ob_delete(gl_wtree, ii);
	    w_obadd(&W_TREE[ROOT], ROOT, ii);
	  }
	}
}

	WORD
w_windfix(npd)
	PD		*npd;		/* pd of new process 	*/
{
	WORD		ii, jj;
	WORD		wh, old;

/* to send all untopped	*/
	oldwfix(npd, FALSE);
	for (ii=0; ii<NUM_ACCS; ii++)
	  dsptch();
/* */
	oldwfix(npd, TRUE);
	if (npd)
	  newwfix(npd);
	gl_wtop = W_TREE[0].ob_tail;
	for (ii=0; ii<NUM_ACCS; ii++)
	  dsptch();

						/* if not a dos app	*/
						/* don't draw borders	*/
	if (! sh[npd->p_pid].sh_isgem)
	  return(TRUE);
	else
	  gl_newmenu = npd;

	wm_update(TRUE);
	everyobj(gl_wtree, ROOT, NIL, newrect, 0, 0, MAX_DEPTH);
	wh = gl_wtop;
	gsx_sclip(&gl_rfull);
	ob_draw(gl_wtree, ROOT, 0);
	w_setactive();
	wm_update(FALSE);

	for (ii=0; ii<NUM_ACCS; ii++)
	  dsptch();
	wm_update(TRUE);
	while (wh != NIL)
	{
	  w_cpwalk(wh, 0, MAX_DEPTH, TRUE);
	  w_redraw(wh, &gl_rfull);
	  old=wh;
	  wh=W_TREE[ROOT].ob_head;
	  if (wh==old)
	    break;	  
	  for (wh=W_TREE[ROOT].ob_head; W_TREE[wh].ob_next != old;
	    wh = W_TREE[wh].ob_next);
	}
	gsx_mfset(ad_armice);
	wm_update(FALSE);
	return(TRUE);
}
#endif

/*
*	Draw the tree of windows given a major change in the some 
*	window.  It may have been sized, moved, fulled, topped, or closed.
*	An attempt should be made to minimize the amount of
*	redrawing of other windows that has to occur.  W_REDRAW()
*	will actually issue window redraw requests based on
*	the rectangle that needs to be cleaned up.
*/

	WORD
draw_change(w_handle, pt)
	REG WORD	w_handle;
	REG GRECT	*pt;
{
	GRECT		c, pprev;
	REG GRECT	*pw;
	REG WORD	start;
	WORD		stop, moved;
	WORD		oldtop, clrold, diffbord, wasclr;

	wasclr = !(D.w_win[w_handle].w_flags & VF_BROKEN);
						/* save old size	*/
	w_getsize(WS_CURR, w_handle, &c);
	w_setsize(WS_PREV, w_handle, &c);
						/* set new size's	*/
	w_setsize(WS_CURR, w_handle, pt);
	pw = (GRECT *) w_getxptr(WS_WORK, w_handle);
	wm_calc(WC_WORK, D.w_win[w_handle].w_kind, 
			pt->g_x, pt->g_y, pt->g_w, pt->g_h, 
			&pw->g_x, &pw->g_y, &pw->g_w, &pw->g_h);
						/* update rect. lists	*/
	everyobj(gl_wtree, ROOT, NIL, newrect, 0, 0, MAX_DEPTH);
						/* remember oldtop	*/
	oldtop = gl_wtop;
	gl_wtop = W_TREE[ROOT].ob_tail;
						/* if new top then	*/
						/*   change men		*/
	if (gl_wtop != oldtop)
#if SINGLAPP
	  w_menufix();
#endif
#if MULTIAPP
	{
	  if (gl_wtop == NIL)
	    w_setmen(1);
	  else
	    w_newmenu(D.w_win[gl_wtop].w_owner);
	}
#endif
						/* set ctrl rect and	*/
						/*   mouse owner	*/
	w_setactive();
						/* init. starting window*/
	start = w_handle;
						/* stop at the top	*/
	stop = DESKWH;
						/* set flag to say we	*/
						/*   haven't moved 	*/
						/*   the top window	*/
	moved = FALSE;
						/* if same upper left	*/
						/*   corner and not	*/
						/*   zero size window	*/
						/*   then its a size or	*/
						/*   top request, else	*/
						/*   its a move, grow,	*/
						/*   open or close.	*/
	if ( (!rc_equal(&gl_rzero, pt)) &&
	      (pt->g_x == c.g_x) && 
	      (pt->g_y == c.g_y) )
	{
						/* size or top request	*/
	  if ( (pt->g_w == c.g_w) && (pt->g_h == c.g_h) )
	  {
	  					/* sizes of prev and 	*/
						/*  current are the same*/
						/*  so its a top request*/

						/* return if this isn't	*/
						/*   a top request 	*/
	    if ( (w_handle != W_TREE[ROOT].ob_tail) ||
		 (w_handle == oldtop) )

	      return(TRUE);
						/* say when borders will*/
						/*   change		*/
	    diffbord = !( (D.w_win[oldtop].w_flags & VF_SUBWIN) &&
		          (D.w_win[gl_wtop].w_flags & VF_SUBWIN) );
						/* draw oldtop covered	*/
						/*   with deactivated 	*/
						/*   borders		*/
	    if (oldtop != NIL)
	    {
	      if (diffbord)
	        w_clipdraw(oldtop, 0, MAX_DEPTH, 2);
	      clrold = !(D.w_win[oldtop].w_flags & VF_BROKEN);
	    }
	    else
	      clrold = TRUE;
						/* if oldtop isn't 	*/
						/*   overlapped and new	*/
						/*   top was clear then	*/
						/*   just draw activated*/
						/*   borders		*/
	    if ( (clrold) && 
		 (wasclr) )
	    {
	      w_clipdraw(gl_wtop, 0, MAX_DEPTH, 1);
	      return(TRUE);
	    }
	  }
	  else
	  					/* size change		*/
	  {
						/* stop before current	*/
						/*   window if shrink	*/
						/*   was a pure subset	*/
	    if ( (pt->g_w <= c.g_w) && (pt->g_h <= c.g_h) )
	    {
	      stop = w_handle;
	      w_clipdraw(gl_wtop, 0, MAX_DEPTH, 2);
	      moved = TRUE;
	    }
						/* start at bottom if	*/
						/*   a shrink occurred	*/
	    if ( (pt->g_w < c.g_w) || (pt->g_h < c.g_h) )
	      start = DESKWH;
						/* update rect. is the	*/
						/*   union of two sizes	*/
						/*   + the drop shadow	*/
	    c.g_w = max(pt->g_w, c.g_w) + 2; 
	    c.g_h = max(pt->g_h, c.g_h) + 2; 
	  }
	}
	else
	{
						/* move or grow or open	*/
						/*   or close		*/
	  if ( !(c.g_w && c.g_h) ||
		( (pt->g_x <= c.g_x) && 
		  (pt->g_y <= c.g_y) &&
		  (pt->g_x+pt->g_w >= c.g_x+c.g_w) && 
		  (pt->g_y+pt->g_h >= c.g_y+c.g_h)))
	  {
						/* a grow that is a 	*/
						/*  superset or an open	*/
	    rc_copy(pt, &c);
	  }
	  else
	  {
						/* move, close or shrink*/
						/* do a move of top guy	*/
	    if ( (pt->g_w == c.g_w) && 
		 (pt->g_h == c.g_h) &&
		 (gl_wtop == w_handle) )
	    {
	      moved = w_move(w_handle, &stop, &c);
	      start = DESKWH;
	    }
						/* check for a close	*/
	    if ( !(pt->g_w && pt->g_h) )
	      start = DESKWH;
						/* handle other moves	*/
						/*   and shrinks	*/
	    if ( start != DESKWH )
	    {
	      rc_union(pt, &c);
	      if ( !rc_equal(pt, &c) )
	        start = DESKWH;
	    }
	  }
	}
						/* update gl_wtop	*/
						/*   after close,	*/
						/*   or open		*/
	if ( oldtop != W_TREE[ROOT].ob_tail )
	{
	  if (gl_wtop != NIL)
	  {
						/* open or close with	*/
						/*   other windows open	*/
	    w_getsize(WS_CURR, gl_wtop, pt);
	    rc_union(pt, &c);
						/* if it was an open	*/
						/*   then draw the	*/
						/*   old top guy	*/
						/*   covered		*/
	    if ( (oldtop != NIL ) &&
		 (oldtop != w_handle) )
	    {
	    					/* BUGFIX 2/20/86 LKW	*/
						/* only an open if prev	*/
						/*  size was zero.	*/
	      w_getsize(WS_PREV, gl_wtop, &pprev);
	      if (rc_equal(&pprev, &gl_rzero))
	        w_clipdraw(oldtop, 0, MAX_DEPTH, 2);	/* */
	    }
	  }
	}
	c.g_w += 2;				/* account for drop shadow*/
	c.g_h += 2;				/* BUGFIX in 2.1	*/
						
						/* update the desktop	*/
						/*   background		*/
	if (start == DESKWH)
	  w_drawdesk(&c);

						/* start the redrawing	*/
	w_update(start, &c, stop, moved, TRUE);
}


/*
*	Walk down ORECT list looking for the next rect that still has
*	size when clipped with the passed in clip rectangle.
*/
	VOID	
w_owns(w_handle, po, pt, poutwds)
	WORD		w_handle;
	REG ORECT	*po;
	GRECT		*pt;
	REG WORD	*poutwds;
{
	while (po)
	{
	  rc_copy(&po->o_x, &poutwds[0]);
	  D.w_win[w_handle].w_rnext = po = po->o_link;
	  if ( (rc_intersect(pt, &poutwds[0])) &&
	       (rc_intersect(&gl_rfull, &poutwds[0]))  )
	    return;
	}
	poutwds[2] = poutwds[3] = 0;
}




/*
*	Start the window manager up by initializing internal variables.
*/
	VOID
wm_start()
{
	REG WORD	i;
	REG ORECT	*po;
	REG LONG	tree;
	PD		*ppd;

#if MULTIAPP
	mn_init();
#endif
						/* init default owner	*/
						/*  to be screen mgr.	*/
	ppd = fpdnm(NULLPTR, SCR_MGR);
						/* init owner rects.	*/
	or_start();
						/* init window extent	*/
						/*   objects		*/
	bfill(NUM_MWIN * sizeof(OBJECT), 0, &W_TREE[ROOT]);
	w_nilit(NUM_MWIN, &W_TREE[ROOT]);
	for(i=0; i<NUM_MWIN; i++)
	{
	  D.w_win[i].w_flags = 0x0;
	  D.w_win[i].w_rlist = (ORECT *) 0x0;
	  W_TREE[i].ob_type = G_IBOX;
#if MULTIAPP
	  W_NAMES[i][0] = 0;
#endif
	}
	W_TREE[ROOT].ob_type = G_BOX;
	tree = ad_stdesk;
	W_TREE[ROOT].ob_spec = LLGET(OB_SPEC(ROOT));
						/* init window element	*/
						/*   objects		*/
	bfill(NUM_ELEM * sizeof(OBJECT), 0, &W_ACTIVE[ROOT]);
	w_nilit(NUM_ELEM, &W_ACTIVE[0]);
	for(i=0; i<NUM_ELEM; i++)
	{
	  W_ACTIVE[i].ob_type = gl_watype[i];
	  W_ACTIVE[i].ob_spec = gl_waspec[i];
	}
	W_ACTIVE[ROOT].ob_state = SHADOWED;
						/* init rect. list	*/
	D.w_win[0].w_rlist = po = get_orect();
	po->o_link = (ORECT *) 0x0;
	po->o_x = XFULL;
	po->o_y = YFULL;
	po->o_w = WFULL;
	po->o_h = HFULL;
	w_setup(ppd, DESKWH, NONE);
	w_setsize(WS_CURR, DESKWH, &gl_rscreen);
	w_setsize(WS_PREV, DESKWH, &gl_rscreen);
	w_setsize(WS_FULL, DESKWH, &gl_rfull);
	w_setsize(WS_WORK, DESKWH, &gl_rfull);
						/* init global vars	*/
	gl_wtop = NIL;
	gl_wtree = ADDR(&W_TREE[ROOT]);
	gl_awind = ADDR(&W_ACTIVE[0]);
	gl_newdesk = 0x0L;
						/* init tedinfo parts	*/
						/*   of title and info	*/
						/*   lines		*/
	movs(sizeof(TEDINFO), &gl_asamp, &gl_aname);
	movs(sizeof(TEDINFO), &gl_asamp, &gl_ainfo);
	gl_aname.te_just = TE_CNTR;
	W_ACTIVE[W_NAME].ob_spec = ADDR(&gl_aname);
	W_ACTIVE[W_INFO].ob_spec = ADDR(&gl_ainfo);
}


/*
*	Allocates a window for the calling application of the appropriate
*	size and returns a window handle.
*
*/

	WORD
wm_create(kind, pt)
	WORD		kind;
	GRECT		*pt;
{
	REG WORD	i;

	for(i=0; (D.w_win[i].w_flags & VF_INUSE) && i<NUM_WIN; i++);
	if ( i < NUM_WIN )
	{
	  w_setup(rlr, i, kind);
	  w_setsize(WS_CURR, i, &gl_rzero);
	  w_setsize(WS_PREV, i, &gl_rzero);
	  w_setsize(WS_FULL, i, pt);
	  return(i);
	}
	return(-1);
}


/*
*	Opens or closes a window.
*/
	VOID
wm_opcl(wh, pt, isadd)
	REG WORD	wh;
	REG GRECT	*pt;
	WORD		isadd;
{
 	GRECT		t;

	rc_copy(pt, &t);
	wm_update(TRUE);
	if (isadd)
	{
	  D.w_win[wh].w_flags |= VF_INTREE;
	  w_obadd(&W_TREE[ROOT], ROOT, wh);
	}
	else
	{
	  ob_delete(gl_wtree, wh);
	  D.w_win[wh].w_flags &= ~VF_INTREE;
	}
	draw_change(wh, &t);
	if (isadd)
	  w_setsize(WS_PREV, wh, pt);
	wm_update(FALSE);
}

/*
*	Opens a window from a created but closed state.
*/
	VOID
wm_open(w_handle, pt)
	WORD		w_handle;
	GRECT		*pt;
{
	wm_opcl(w_handle, pt, TRUE);
}


/*
*	Closes a window from an open state.
*/

	VOID
wm_close(w_handle)
	WORD		w_handle;
{
	wm_opcl(w_handle, &gl_rzero, FALSE);
}


/*
*	Frees a window and its handle up for use by 
*	by another application or by the same application.
*/

	VOID
wm_delete(w_handle)
	WORD		w_handle;
{
	newrect(gl_wtree, w_handle);		/* give back recs.	*/
	w_setsize(WS_CURR, w_handle, &gl_rscreen);
	w_setsize(WS_PREV, w_handle, &gl_rscreen);
	w_setsize(WS_FULL, w_handle, &gl_rfull);
	w_setsize(WS_WORK, w_handle, &gl_rfull);
	D.w_win[w_handle].w_flags = 0x0;	/*&= ~VF_INUSE;	*/
	D.w_win[w_handle].w_owner = NULLPTR;
#if MULTIAPP
	W_INACTIVE[w_handle] = NULLPTR;
#endif
}


/*
*	Gives information about the current window to the application
*	that owns it.
*/
	VOID
wm_get(w_handle, w_field, poutwds)
	REG WORD	w_handle;
	WORD		w_field;
	REG WORD	*poutwds;
{
	REG WORD	which;
	GRECT		t;
	REG ORECT	*po;

	which = -1;
	switch(w_field)
	{
	  case WF_WXYWH:
		which = WS_WORK;
		break;
	  case WF_CXYWH:
		which = WS_CURR;
		break;
	  case WF_PXYWH:
		which = WS_PREV;
		break;
	  case WF_FXYWH:
		which = WS_FULL;
		break;
	  case WF_HSLIDE:
		poutwds[0] = D.w_win[w_handle].w_hslide;
		break;
	  case WF_VSLIDE:
		poutwds[0] = D.w_win[w_handle].w_vslide;
		break;
	  case WF_HSLSIZ:
		poutwds[0] = D.w_win[w_handle].w_hslsiz;
		break;
	  case WF_VSLSIZ:
		poutwds[0] = D.w_win[w_handle].w_vslsiz;
		break;
	  case WF_TOP:
		poutwds[0] = w_top();
		break;
	  case WF_FIRSTXYWH:
	  case WF_NEXTXYWH:
	        w_getsize(WS_WORK, w_handle, &t);
		po = (w_field == WF_FIRSTXYWH) ? D.w_win[w_handle].w_rlist : 
						 D.w_win[w_handle].w_rnext;
		w_owns(w_handle, po, &t, &poutwds[0]);
		break;
	  case WF_SCREEN:
		gsx_mret(&poutwds[0], &poutwds[2]);
		break;
	  case WF_TATTRB:
		poutwds[0] = D.w_win[w_handle].w_flags >> 3;
		break;
	}
	if (which != -1)
	  w_getsize(which, w_handle, &poutwds[0]);
}

	WORD
wm_gsizes(w_field, psl, psz)
	WORD		w_field;
	WORD		*psl;
	WORD		*psz;
{
	if ( (w_field == WF_HSLSIZ) ||
	     (w_field == WF_HSLIDE) )
	{
	  *psl = W_ACTIVE[W_HELEV].ob_x;
	  *psz = W_ACTIVE[W_HELEV].ob_width; 
	  return(W_HBAR);
	}
	if ( (w_field == WF_VSLSIZ) ||
	     (w_field == WF_VSLIDE) )
	{
	  *psl = W_ACTIVE[W_VELEV].ob_y;
	  *psz = W_ACTIVE[W_VELEV].ob_height; 
	  return(W_VBAR);
	}
	return(0);
}


/*
*	Routine to top a window and then make the right redraws happen
*/
	VOID
wm_mktop(w_handle)
	REG WORD	w_handle;
{
	GRECT		t, p;

	if ( w_handle != gl_wtop )
	{
	  ob_order(gl_wtree, w_handle, NIL);
	  w_getsize(WS_PREV, w_handle, &p);
	  w_getsize(WS_CURR, w_handle, &t);
	  draw_change(w_handle, &t);
	  w_setsize(WS_PREV, w_handle, &p);
	}
}


/*
*	Allows application to set the attributes of
*	one of the windows that it currently owns.  Some of the
*	information includes the name, and the scroll bar elevator
*	positions. 
*/

        VOID
wm_set(w_handle, w_field, pinwds)
	REG WORD	w_handle;
	REG WORD	w_field;
	REG WORD	*pinwds;
{
	WORD		which, liketop, i;
	REG WORD	wbar;
	WORD		osl, osz, nsl, nsz;
	GRECT		t;
	WINDOW		*pwin;	

	which = -1;
						/* grab the window sync	*/
	wm_update(TRUE);
	pwin = &D.w_win[w_handle];
	wbar = wm_gsizes(w_field, &osl, &osz); 
	if (wbar)
	{
	  pinwds[0] = max(-1, pinwds[0]);
	  pinwds[0] = min(1000, pinwds[0]);
	}
	liketop = ( ( w_handle == gl_wtop ) ||
		    ( pwin->w_flags & VF_SUBWIN ) );
	switch(w_field)
	{
	  case WF_NAME:
		which = W_NAME;
		break;
	  case WF_INFO:
		which = W_INFO;
		break;
	  case WF_SIZTOP:
		ob_order(gl_wtree, w_handle, NIL);
						/* fall thru	*/
	  case WF_CXYWH:
		draw_change(w_handle, &pinwds[0]);
		break;
	  case WF_TOP:
		if (w_handle != gl_wtop)
		{
		  for(i=W_TREE[ROOT].ob_head; i>ROOT; i=W_TREE[i].ob_next)
		  {
		    if ( (i != w_handle) &&
		         (D.w_win[i].w_owner == rlr) &&
		         (D.w_win[i].w_flags & VF_SUBWIN) &&
		         (pwin->w_flags & VF_SUBWIN) )
		      wm_mktop(i);
		  }
		  wm_mktop(w_handle);
		}
		break;
	  case WF_NEWDESK:
		pwin->w_owner = rlr;
		desk_tree[rlr->p_pid] = gl_newdesk = *(LONG *) &pinwds[0];
		desk_root[rlr->p_pid] = gl_newroot = pinwds[2];
		break;
	  case WF_HSLSIZ:
		pwin->w_hslsiz = pinwds[0];
		break;
	  case WF_VSLSIZ:
		pwin->w_vslsiz = pinwds[0];
		break;
	  case WF_HSLIDE:
		pwin->w_hslide = pinwds[0];
		break;
	  case WF_VSLIDE:
		pwin->w_vslide = pinwds[0];
		break;
	  case WF_TATTRB:
		if (pinwds[0] & WA_SUBWIN)
		  pwin->w_flags |= VF_SUBWIN;
		else
		  pwin->w_flags &= ~VF_SUBWIN;
		if (pinwds[0] & WA_KEEPWIN)
		  pwin->w_flags |= VF_KEEPWIN;
		else
		  pwin->w_flags &= ~VF_KEEPWIN;
		break;
	}
	if ( (wbar) &&
	     (liketop) )
	{
	  w_bldactive(w_handle);
	  wm_gsizes(w_field, &nsl, &nsz); 
	  if ( (osl != nsl) ||
	       (osz != nsz) ||
	       (pwin->w_flags & VF_SUBWIN) )
	  {
	    w_getsize(WS_TRUE, w_handle, &t);
	    do_walk(w_handle, gl_awind, wbar + 3, MAX_DEPTH, &t);
	  }
	}
	if (which != -1)
		w_strchg(w_handle, which, pinwds[0], pinwds[1]);
						/* give up the sync	*/
	wm_update(FALSE);
}


/*
*	Given an x and y location this call will figure out which window
*	the mouse is in.
*/

	WORD
wm_find(x, y)
	WORD		x, y;
{
	return(	ob_find(gl_wtree, 0, 2, x, y) );
}


/*
*	Locks or unlocks the current state of the window tree while an 
*	application is responding to a window update message in his message
*	pipe or is making some other direct screen update based on his current
*	rectangle list.
*/
	VOID
wm_update(beg_update)
	REG WORD	beg_update;
{

	if ( beg_update < 2)
	{
	  if ( beg_update )
	  {
	    if ( !tak_flag(&wind_spb) )
		    ev_block(MU_MUTEX, ad_windspb);

	  }
	  else
	    unsync(&wind_spb);
	}
	else
	{
	  beg_update -= 2;
	  fm_own( beg_update );
	}
}

/*
*	Given a width and height of a Work Area and the Kind of window
*	desired calculate the required window size including the 
*	Border Area.  or...  Given the width and height of a window
*	including the Border Area and the Kind of window desired, calculate
*	the result size of the window Work Area.
*/
	VOID
wm_calc(wtype, kind, x, y, w, h, px, py, pw, ph)
	WORD		wtype;
	REG UWORD	kind;
	WORD		x, y, w, h;
	WORD		*px, *py, *pw, *ph;
{
	REG WORD	tb, bb, lb, rb;

	tb = bb = rb = lb = 1;
	
	if ( kind & (NAME | CLOSER | FULLER) )
	  tb += (gl_hbox - 1);
	if ( kind & INFO )
	  tb += (gl_hbox - 1);

	if ( kind & (UPARROW | DNARROW | VSLIDE | SIZER) )
	  rb += (gl_wbox - 1);
	if ( kind & (LFARROW | RTARROW | HSLIDE | SIZER) )
	  bb += (gl_hbox - 1);
						/* negate values to calc*/
						/*   Border Area	*/
	if ( wtype == WC_BORDER )
	{
	  lb = -lb;
	  tb = -tb;
	  rb = -rb;
	  bb = -bb;						
	}
	*px = x + lb;
	*py = y + tb;
	*pw = w - lb - rb;
	*ph = h - tb - bb;
}

