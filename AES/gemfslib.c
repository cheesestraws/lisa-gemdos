/*  	GEMFSLIB.C	5/14/84 - 07/16/85	Lee Lorenzen		*/
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
#include <obdefs.h>
#include <taddr.h>
#include <dos.h>
#include <gemlib.h>
#include <gem.h>


#define NM_NAMES (F9NAME-F1NAME+1)
#define NAME_OFFSET F1NAME
#define LEN_FTITLE 18				/* BEWARE, requires change*/
						/*  in GEM.RSC		*/
						/* in DOS.C		*/
EXTERN	LONG	dos_alloc();
EXTERN	BYTE	*scasb();

/* ---------- added for metaware compiler ---------- */
EXTERN VOID 	dos_sdta();			/* in DOS.C		*/
EXTERN WORD 	dos_gdrv();
EXTERN WORD 	dos_sfirst();
EXTERN WORD 	dos_snext();
EXTERN VOID 	dos_free();
EXTERN VOID	sound();			/* in OPTIMIZE.C	*/
EXTERN WORD 	wildcmp();
EXTERN VOID 	ins_char();
EXTERN VOID	fs_sset();
EXTERN VOID	fs_sget();
EXTERN VOID 	fmt_str();
EXTERN VOID  	unfmt_str();
EXTERN WORD 	min();				/* in OPTIMOPT.A86 	*/
EXTERN WORD 	max();
EXTERN WORD 	inside();
EXTERN WORD 	strcmp();
EXTERN WORD 	strchk();
EXTERN WORD 	mul_div();			/* in GSX2.A86		*/
EXTERN VOID 	ob_change();			/* in OBLIB.C		*/
EXTERN VOID 	ob_actxywh();
EXTERN VOID 	ob_draw();
EXTERN VOID 	ob_relxywh();
EXTERN VOID 	bb_screen();			/* in GRAF.C		*/
EXTERN VOID 	gsx_gclip();
EXTERN VOID 	gsx_sclip();
EXTERN VOID 	fm_dial();			/* in FMLIB.C		*/
EXTERN WORD	fm_do();
EXTERN VOID 	fm_own();
EXTERN VOID 	gsx_mxmy();			/* in GSXIF.C		*/
EXTERN VOID 	gsx_mfset();
EXTERN WORD 	gr_slidebox();			/* in GRLIB.C		*/
EXTERN WORD 	inf_what();
/* ----------------------------------------------------- */

EXTERN WORD	gl_hbox;

EXTERN LONG	ad_armice;
EXTERN LONG	ad_hgmice;

EXTERN LONG	ad_dta;
EXTERN BYTE	gl_dta[128];

EXTERN UWORD	gl_bvdisk;

EXTERN THEGLO	D;

GLOBAL BYTE	gl_fsobj[4] = {FTITLE, FILEBOX, SCRLBAR, 0x0};
GLOBAL LONG	ad_fstree;
GLOBAL LONG	ad_fsnames;
GLOBAL LONG	ad_fsdta;
GLOBAL GRECT	gl_rfs;

GLOBAL LONG	ad_tmp1;
GLOBAL BYTE	gl_tmp1[LEN_FSNAME];
GLOBAL LONG	ad_tmp2;
GLOBAL BYTE	gl_tmp2[LEN_FSNAME];

GLOBAL WORD	gl_shdrive;
GLOBAL WORD	gl_fspos;



/*
*	LONG string compare, TRUE == strings the same
*/

	WORD
LSTCMP(lst, rst)
	LONG		lst, rst;
{
	WORD		i;
	BYTE		l;

	i = 0;
	while ((l = LBGET(lst+i)) != 0)
	{ 
	  if 	(l != LBGET(rst+i))
	    return(FALSE);
	  i++;
	}
	if (LBGET(rst+i))
	  return(FALSE);
	return(TRUE);
}


/*
*	Routine to back off the end of a file string.
*/
	BYTE
*fs_back(pstr, pend)
	REG BYTE	*pstr;
	REG BYTE	*pend;
{
						/* back off to last	*/
						/*   slash		*/
	while ( (*pend != ':') &&
		(*pend != '\\') &&
		(pend != pstr) )
	  pend--;
						/* if a : then insert	*/
						/*   a backslash	*/
	if (*pend == ':')
	{
	  pend++;
	  ins_char(pend, 0, '\\', 64);
	}
	return(pend);
}


/*
*	Routine to back up a path and return the pointer to the beginning
*	of the file specification part
*/
	BYTE
*fs_pspec(pstr, pend)
	REG BYTE	*pstr;
	REG BYTE	*pend;
{
	pend = fs_back(pstr, pend);
	if (*pend == '\\')
	  pend++;
	else
	{
	  strcpy("A:\\*.*", pstr);
	  pstr[0] += (BYTE) dos_gdrv();
	  pend = pstr + 3;
	}
	return(pend);
}

/*
*	Routine to compare based on type and then on name if its a file
*	else, just based on name
*/

	WORD
fs_comp()
{
	WORD		chk;

	if ( (gl_tmp1[0] == ' ') &&
	     (gl_tmp2[0] == ' ') )
	{
	  chk = strchk( scasb(&gl_tmp1[0], '.'), 
			scasb(&gl_tmp2[0], '.') );
	  if ( chk )
	    return( chk );
	}
	return ( strchk(&gl_tmp1[0], &gl_tmp2[0]) );
}


	WORD
fs_add(thefile, fs_index)
	WORD		thefile;
	WORD		fs_index;
{
	WORD		len;

	len = LSTCPY(ad_fsnames + (LONG) fs_index, 
			ad_fsdta - (LONG) 1);
	D.g_fslist[thefile] = (BYTE *) fs_index;
	fs_index += len + 2;

	return(fs_index);
}


/*
*	Make a particular path the active path.  This involves
*	reading its directory, initializing a file list, and filling
*	out the information in the path node.  Then sort the files.
*/
	WORD
fs_active(ppath, pspec, pcount)
	LONG		ppath;
	BYTE		*pspec;
	WORD		*pcount;
{
	WORD		ret, thefile, len;
	WORD		fs_index;
	REG WORD	i, j, gap;
	BYTE		*temp;
	
	gsx_mfset(ad_hgmice);

	thefile = 0;
	fs_index = 0;
	len = 0;

	if (gl_shdrive)
	{
	  strcpy("\007 A:", &gl_dta[29]);
	  for(i=0; i<16; i++)
	  {
	    if ( (gl_bvdisk >> i) & 0x0001 )
	    {
	      gl_dta[31] = 'A' + (15 - i);
	      fs_index = fs_add(thefile, fs_index);
	      thefile++;
	    }
	  }
	}
	else
	{
	  dos_sdta(ad_dta);
	  ret = dos_sfirst(ppath, F_SUBDIR);
	  while ( ret )
	  {
						/* if it is a real file	*/
						/*   or directory then	*/
						/*   save it and set	*/
						/*   first byte to tell	*/
						/*   which		*/
	    if (gl_dta[30] != '.')
	    {
	      gl_dta[29] = (gl_dta[21] & F_SUBDIR) ? 0x07 : ' ';
	      if ( (gl_dta[29] == 0x07) ||
		   (wildcmp(pspec, &gl_dta[30])) )
	      {
		fs_index = fs_add(thefile, fs_index);
	        thefile++;
	      }
	    }
	    ret = dos_snext();

	    if (thefile >= NM_FILES)
	    {
	      ret = FALSE;
	      sound(TRUE, 660, 4);
	    }
	  }
	}
	*pcount = thefile;
						/* sort files using shell*/
						/*   sort on page 108 of */
						/*   K&R C Prog. Lang.	*/
	for(gap = thefile/2; gap > 0; gap /= 2)
	{
	  for(i = gap; i < thefile; i++)
	  {
	    for (j = i-gap; j >= 0; j -= gap)
	    {
	      LSTCPY(ad_tmp1, ad_fsnames + (LONG) D.g_fslist[j]);
	      LSTCPY(ad_tmp2, ad_fsnames + (LONG) D.g_fslist[j+gap]);
	      if ( fs_comp() <= 0 )
		break;
	      temp = D.g_fslist[j];
	      D.g_fslist[j] = D.g_fslist[j+gap];
	      D.g_fslist[j+gap] = temp;
	    }
	  }
	}
	gsx_mfset( ad_armice );
	return(TRUE);
}


/*
*	Routine to adjust the scroll counters by one in either
*	direction, being careful not to overrun or underrun the
*	tail and heads of the list
*/
	WORD
fs_1scroll(curr, count, touchob)
	REG WORD	curr, count, touchob;
{
	REG WORD	newcurr;

	newcurr = (touchob == FUPAROW) ? (curr - 1) : (curr + 1);
	if (newcurr < 0)
	  newcurr++;
	if ( (count - newcurr) < NM_NAMES )
	  newcurr--;
	return( (count > NM_NAMES) ? newcurr : curr );
}


/*
*	Routine to take the filenames that will appear in the window, 
*	based on the current scrolled position, and point at them 
*	with the sub-tree of G_STRINGs that makes up the window box.
*/
	VOID
fs_format(tree, currtop, count)
	REG LONG	tree;
	WORD		currtop;
	WORD		count;
{
	REG WORD	i, cnt;
	REG WORD	y, h, th;
	LONG		adtext, tlen;
						/* build in real text	*/
						/*   strings		*/
	gl_fspos = currtop;			/* save new position	*/
	cnt = min(NM_NAMES, count - currtop);
	for(i=0; i<NM_NAMES; i++)
	{
	  if (i < cnt)
	  {
	    LSTCPY(ad_tmp2,  ad_fsnames + (LONG) D.g_fslist[currtop+i]);
	    fmt_str(&gl_tmp2[1], &gl_tmp1[1]);
	    gl_tmp1[0] = gl_tmp2[0];
	  }
	  else
	  {
	    gl_tmp1[0] = ' ';
	    gl_tmp1[1] = NULL;
	  }
	  fs_sset(tree, NAME_OFFSET+i, ad_tmp1, &adtext, &tlen);
	  LWSET(OB_TYPE(NAME_OFFSET+i), 
		((gl_shdrive) ? G_BOXTEXT : G_FBOXTEXT) );
	  LWSET(OB_STATE(NAME_OFFSET+i), NORMAL);
	}
						/* size and position the*/
						/*   elevator		*/
	y = 0;
	th = h = LWGET(OB_HEIGHT(FSVSLID));  
	if ( count > NM_NAMES)
	{
	  h = mul_div(NM_NAMES, h, count);
	  h = max(gl_hbox/2, h);		/* min size elevator	*/
	  y = mul_div(currtop, th-h, count-NM_NAMES);
	}
	LWSET(OB_Y(FSVELEV), y);
	LWSET(OB_HEIGHT(FSVELEV), h);
}


/*
*	Routine to select or deselect a file name in the scrollable 
*	list.
*/
	VOID
fs_sel(sel, state)
	WORD		sel;
	WORD		state;
{
	if (sel)
	  ob_change(ad_fstree, F1NAME + sel - 1, state, TRUE);
}


/*
*	Routine to handle scrolling the directory window a certain number
*	of file names.
*/
	WORD
fs_nscroll(tree, psel, curr, count, touchob, n)
	REG LONG	tree;
	REG WORD	*psel;
	WORD		curr, count, touchob, n;
{
	REG WORD	i, newcurr, diffcurr;
	WORD		sy, dy, neg;
	GRECT		r[2];
						/* single scroll n times*/
	newcurr = curr;
	for (i=0; i<n; i++)
	  newcurr = fs_1scroll(newcurr, count, touchob);
						/* if things changed 	*/
						/*   then redraw	*/
	diffcurr = newcurr - curr;
	if (diffcurr)
	{
	  curr = newcurr;
	  fs_sel(*psel, NORMAL);
	  *psel = 0;
	  fs_format(tree, curr, count);
	  gsx_gclip(&r[1].g_x);
	  ob_actxywh(tree, F1NAME, &r[0].g_x);

	  if (( neg = (diffcurr < 0)) != 0 )
	    diffcurr = -diffcurr;

	  if (diffcurr < NM_NAMES)
	  {
	    sy = r[0].g_y + (r[0].g_h * diffcurr);
	    dy = r[0].g_y;

	    if (neg)
	    {
	      dy = sy;
	      sy = r[0].g_y;
	    }

	    bb_screen(S_ONLY, r[0].g_x, sy, r[0].g_x, dy, r[0].g_w, 
				r[0].g_h * (NM_NAMES - diffcurr) );
	    if ( !neg )
	      r[0].g_y += r[0].g_h * (NM_NAMES - diffcurr);
	  }
	  else
	    diffcurr = NM_NAMES;

	  r[0].g_h *= diffcurr;
	  for(i=0; i<2; i++)
	  {
	    gsx_sclip(&r[i].g_x);
	    ob_draw(tree, ((i) ? FSVSLID : FILEBOX), MAX_DEPTH);
	  }
	}
	return(curr);
}


/*
*	Routine to call when a new directory has been specified.  This
*	will activate the directory, format it, and display ir[0].
*/
	WORD
fs_newdir(ftitle, fpath, pspec, tree, pcount, pos)
	LONG		ftitle;
	LONG		fpath;
	BYTE		*pspec;
	LONG		tree;
	WORD		*pcount;
	WORD		pos;
{
	BYTE		*ptmp;
	WORD		len;
					/* BUGFIX 2.1 added len calculation*/
					/*  so FTITLE doesn't run over into*/
					/*  F1NAME.			*/
	ob_draw(tree, FSDIRECT, MAX_DEPTH);
	fs_active(fpath, pspec, pcount);
	if (pos+ NM_NAMES > *pcount)	/* in case file deleted		*/
	  pos = max(0, *pcount - NM_NAMES);
	fs_format(tree, pos, *pcount);
	len = LSTRLEN(ADDR(pspec));
	len = (len > LEN_FTITLE) ? LEN_FTITLE : len;
	LBSET(ftitle, ' ');
	ftitle++;
	LBCOPY(ftitle, ADDR(pspec), len);
	ftitle += len;
	LBSET(ftitle, ' ');
	ftitle++;
	LBSET(ftitle, NULL);
	ptmp = &gl_fsobj[0];
	while(*ptmp)
	  ob_draw(tree, *ptmp++, MAX_DEPTH);
	return(TRUE);
}



/*
*	File Selector input routine that takes control of the mouse
*	and keyboard, searchs and sort the directory, draws the file 
*	selector, interacts with the user to determine a selection
*	or change of path, and returns to the application with
*	the selected path, filename, and exit button.
*/
	WORD
fs_input(pipath, pisel, pbutton)
	LONG		pipath, pisel;
	WORD		*pbutton;
{
	REG WORD	touchob, value, fnum;
	WORD		curr, count, sel;
	WORD		mx, my;
	REG LONG	tree;
	LONG		ad_fpath, ad_fname, ad_ftitle, ad_locstr;
	WORD		fname_len, fpath_len, temp_len; 
	WORD		dclkret, cont, firsttime, newname, elevpos;
	REG BYTE	*pstr, *pspec;
	GRECT		pt;
	BYTE		locstr[64];

	LONG		dummy; /*!!!*/

					/* get out quick if path is	*/
					/*   nullptr or if pts to null.	*/
	if (pipath == 0x0L)
	  return(FALSE);
	if ( LBGET(pipath) == NULL)
	  return(FALSE);
						/* get memory for 	*/
						/*   the string buffer	*/
#if SINGLAPP
	ad_fsnames = dos_alloc( LW(LEN_FSNAME * NM_FILES) );
	if (!ad_fsnames)
	  return(FALSE);
#endif
#if MULTIAPP
	ad_fsnames = ADDR(&D.g_fsnames[0]);
#endif	
	tree = ad_fstree;
	ad_locstr = (LONG) ADDR(&locstr[0]);
						/* init strings in form	*/

	dummy = LLGET(OB_SPEC(FTITLE));
	ad_ftitle = LLGET(dummy);

	LSTCPY(ad_ftitle, ADDR(" *.* "));
	dummy=LLGET(OB_SPEC(FSDIRECT));
	if (LSTCMP(pipath, LLGET(dummy)))
	  elevpos = gl_fspos;			/* same dir as last time */	
	else					
	  elevpos = 0;
  	fs_sset(tree, FSDIRECT, pipath, &ad_fpath, &temp_len);
	LSTCPY(ad_tmp1, pisel);
	fmt_str(&gl_tmp1[0], &gl_tmp2[0]);
	fs_sset(tree, FSSELECT, ad_tmp2, &ad_fname, &fname_len);
						/* set clip and start	*/
						/*   form fill-in by	*/
						/*   drawing the form	*/
	gsx_sclip(&gl_rfs);	
	fm_dial(FMD_START, &gl_rfs);
	D.g_dir[0] = NULL;			
	ob_draw(tree, ROOT, 2);
						/* init for while loop	*/
						/*   by forcing initial	*/
						/*   fs_newdir call	*/
	sel = 0;
	newname = gl_shdrive = FALSE;
	cont = firsttime = TRUE;
	while( cont )
	{
	  touchob = (firsttime) ? 0x0 : fm_do(tree, FSSELECT);
	  gsx_mxmy(&mx, &my);
	
	  fpath_len = LSTCPY(ad_locstr, ad_fpath);
	  if ( !strcmp(&D.g_dir[0], &locstr[0]) )
	  {
	    fs_sel(sel, NORMAL);
	    if ( (touchob == FSOK) ||
		 (touchob == FSCANCEL) )
	      ob_change(tree, touchob, NORMAL, TRUE);
	    strcpy(&locstr[0], &D.g_dir[0]);
	    pspec = fs_pspec(&D.g_dir[0], &D.g_dir[fpath_len]);	    
/*	    LSTCPY(ad_fpath, ADDR(&D.g_dir[0])); */
  	    fs_sset(tree, FSDIRECT, ADDR(&D.g_dir[0]), &ad_fpath, &temp_len);
	    pstr = fs_pspec(&locstr[0], &locstr[fpath_len]);	    
	    strcpy("*.*", pstr);
	    fs_newdir(ad_ftitle, ad_locstr, pspec, tree, &count, elevpos);
	    curr = elevpos;
	    sel = touchob = elevpos = 0;
	    firsttime = FALSE;
	  }

	  value = 0;
	  dclkret = ((touchob & 0x8000) != 0);
	  switch( (touchob &= 0x7fff) )
	  {
	    case FSOK:
	    case FSCANCEL:
		cont = FALSE;
		break;
	    case FUPAROW:
	    case FDNAROW:
		value = 1;
		break;
	    case FSVSLID:
		ob_actxywh(tree, FSVELEV, &pt);
		pt.g_x -= 3;
		pt.g_w += 6;
		if ( inside(mx, my, &pt) )
		  goto dofelev;
		touchob = (my <= pt.g_y) ? FUPAROW : FDNAROW;
		value = NM_NAMES;
		break;
	    case FSVELEV:
dofelev:	fm_own(TRUE);
		ob_relxywh(tree, FSVSLID, &pt);
		pt.g_x += 3;		/* APPLE	*/
		pt.g_w -= 6;
		LWSET(OB_X(FSVSLID), pt.g_x);
		LWSET(OB_WIDTH(FSVSLID), pt.g_w);
		value = gr_slidebox(tree, FSVSLID, FSVELEV, TRUE);
		pt.g_x -= 3;
		pt.g_w += 6;
		LWSET(OB_X(FSVSLID), pt.g_x);
		LWSET(OB_WIDTH(FSVSLID), pt.g_w);
		fm_own(FALSE);
		value = curr - mul_div(value, count-NM_NAMES, 1000);
		if (value >= 0)
		  touchob = FUPAROW;
		else
		{
		  touchob = FDNAROW;
		  value = -value;
		}
		break;
	    case F1NAME:
	    case F2NAME:
	    case F3NAME:
	    case F4NAME:
	    case F5NAME:
	    case F6NAME:
	    case F7NAME:
	    case F8NAME:
	    case F9NAME:
		fnum = touchob - F1NAME + 1;
		if ( fnum <= count )
		{
		  if ( (sel) &&
		       (sel != fnum) )
		    fs_sel(sel, NORMAL);
		  if ( sel != fnum)
		  {
		    sel = fnum;
		    fs_sel(sel, SELECTED);
		  }
						/* get string and see	*/
						/*   if file or folder	*/
		  fs_sget(tree, touchob, ad_tmp1);
		  if (gl_tmp1[0] == ' ')
		  {
						/* copy to selection	*/
		    newname = TRUE;
		    if (dclkret)
		      cont = FALSE;
		  }
		  else
		  {
		    if (gl_shdrive)
		    {
						/* prepend in drive name*/
		      if (locstr[1] == ':')
		        locstr[0] = gl_tmp1[2];
		    }
		    else
		    {
						/* append in folder name*/
		      pstr = fs_pspec(&locstr[0], &locstr[fpath_len]);
		      strcpy(pstr - 1, &gl_tmp2[0]);
		      unfmt_str(&gl_tmp1[1], pstr);
		      strcat(&gl_tmp2[0], pstr);
		    }
		    firsttime = TRUE;
		  }
		  gl_shdrive = FALSE;
		}
		break;
	    case FCLSBOX:
		pspec = pstr = fs_back(&locstr[0], &locstr[fpath_len]);
		if (*pstr-- == '\\')
		{
		  firsttime = TRUE;
		  if (*pstr != ':')
		  {
		    pstr = fs_back(&locstr[0], pstr);
		    if (*pstr == '\\')
		      strcpy(pspec, pstr);
		  }
		  else
		  {
		    if (gl_bvdisk)
		      gl_shdrive = TRUE;
		  }
		}
		break;
	    case FTITLE:
		firsttime = TRUE;
		break;
	  }
	  if (firsttime)
	  {
	   /* LSTCPY(ad_fpath, ad_locstr); */
  	    fs_sset(tree, FSDIRECT, ad_locstr, &ad_fpath, &temp_len);
	    D.g_dir[0] = NULL;
	    gl_tmp1[1] = NULL;
	    newname = TRUE;
	  }
	  if (newname)
	  {
	    LSTCPY(ad_fname, ad_tmp1 + 1);
	    ob_draw(tree, FSSELECT, MAX_DEPTH);
	    if (!cont)
	      ob_change(tree, FSOK, SELECTED, TRUE);
	    newname = FALSE;
	  }
	  if (value)
	    curr = fs_nscroll(tree, &sel, curr, count, touchob, value);
	}
						/* return path and	*/
						/*   file name to app	*/
	LSTCPY(pipath, ad_fpath);
	LSTCPY(ad_tmp1, ad_fname);
	unfmt_str(&gl_tmp1[0], &gl_tmp2[0]);
	LSTCPY(pisel, ad_tmp2);
						/* start the redraw	*/
	fm_dial(FMD_FINISH, &gl_rfs);
						/* return exit button	*/
	*pbutton = inf_what(tree, FSOK, FSCANCEL);
#if SINGLAPP
	dos_free(ad_fsnames);
#endif
	return( TRUE );
}

