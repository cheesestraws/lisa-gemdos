/*	GEMLIB.H	03/15/84 - 06/07/85	Lee Lorenzen		*/
/*	EVNTLIB.H	03/15/84 - 05/16/84	Lee Lorenzen		*/
/*	2.0		10/30/85 - 10/30/85	Lowell Webster		*/

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

#if MULTIAPP
#define	PR_EXITED	0
#define	PR_SWITCH	1
#define	PR_FAILURE	-1
#define	PR_OKGEM	0
#define	PR_TERMINATE	1
#define	PR_STOPPED	2
#define SIZE_DESKTOP	0x20000L		/* 128K		*/
#define DEF_CHNLSIZE	0x40000L		/* 256K		*/
#define SHRINK 0x01		/* state vars for sh_state 	*/
#define ABORT 0x02
#define ACCNODE struct accnode
ACCNODE
{
	WORD	acc_swap;
	BYTE	acc_name[12];
};
#endif
						/* mu_flags		*/
#define MU_KEYBD 0x0001	
#define MU_BUTTON 0x0002
#define MU_M1 0x0004
#define MU_M2 0x0008
#define MU_MESAG 0x0010
#define MU_TIMER 0x0020
#define MU_SDMSG 0x0040
#define MU_MUTEX 0x0080

typedef struct moblk
{
	WORD		m_out;
	WORD		m_x;
	WORD		m_y;
	WORD		m_w;
	WORD		m_h;
} MOBLK ;


/*	MENULIB.H	05/04/84 - 11/01/84	Lowell Webster	*/

#define WID_DESKACC 21
	
#define MTH 1					/* menu thickness	*/

#define THESCREEN 0
#define THEBAR 1
#define THEACTIVE 2

/*!!!*//* Sozobon C has problems with the following macro, so I changed this... */
#if I8086
#define THEMENUS ( LWGET(OB_TAIL(THESCREEN)) )
#endif
#if MC68K
#define THEMENUS ( (WORD) *((WORD *)( OB_TAIL(THESCREEN) )) )
#endif

/*** STATE DEFINITIONS FOR menu_state ***********************************/

#define INBAR	1	/* mouse position	outside menu bar & mo dn */
			/* multi wait		mo up | in menu bar	*/
			/* moves		-> 5  ,  ->2		*/

#define OUTTITLE 2  	/* mouse position	over title && mo dn	*/
			/* multiwait		mo up | out title rect	*/
			/* moves		-> 5  ,	->1 ->2  ->3	*/

#define OUTITEM 3 	/* mouse position	over item && mo dn	*/
			/* multi wait		mo up | out item rect	*/
			/* moves		-> 5  , ->1 ->2 ->3 ->4	*/

#define INBARECT 4	/* mouse position	out menu rect && bar && mo dn*/			/* multi wait	mo up | in menu rect | in menu bar */
			/* moves	-> 5  ,	-> 3	     , -> 2	*/



/*	APPLLIB.H	05/05/84 - 10/16/84	Lee Lorenzen		*/

#define SCR_MGR 0x0001			/* pid of the screen manager*/

#define AP_MSG 0

#define NP_RUNIT 1

#define MN_SELECTED 10

#define WM_REDRAW 20
#define WM_TOPPED 21
#define WM_CLOSED 22
#define WM_FULLED 23
#define WM_ARROWED 24
#define WM_HSLID 25
#define WM_VSLID 26
#define WM_SIZED 27
#define WM_MOVED 28
#define WM_NEWTOP 29		/* not used as of 4/16/86	*/
#define WM_UNTOPPED 30		/* added 10/28/85 LKW		*/

#define AC_OPEN 40
#define AC_CLOSE 41
#define AC_ABORT 42		/* added 3/27/86 LKW		*/

#define CT_UPDATE 50		/* not used as of 4/16/86	*/


/*	FORMLIB.H	05/05/84 - 10/16/84	Gregg Morris		*/

#define FMD_START 0
#define FMD_GROW 1
#define FMD_SHRINK 2
#define FMD_FINISH 3

/*	SCRPLIB.H	05/05/84 - 01/05/85	Lee Lorenzen		*/

/*	FSELLIB.H	05/05/84 - 01/07/85	Lee Lorenzen		*/

#define RET_CODE int_out[0]

#define FS_IPATH addr_in[0]
#define FS_ISEL addr_in[1]

#define FS_BUTTON int_out[1]

#define NM_FILES 100

#define LEN_FSNAME 16


/*	RSRCLIB.H	05/05/84 - 09/13/84	Lowell Webster		*/


#define RS_PFNAME addr_in[0]		/* rs_init, 			*/
#define RS_TYPE int_in[0]
#define RS_INDEX int_in[1]
#define RS_INADDR addr_in[0]
#define RS_OUTADDR addr_out[0]		


#define RMODE_RD 0
#define RMODE_WR 1
#define RMODE_RW 2

#define SMODE 0				/* seek mode is absolute offset	*/
#define F_ATTR 0			/* file attribute for creating	*/

typedef struct rssofln
{
	WORD	rss_offset;
	WORD	rss_length;
}RSSOFLN;

typedef struct imofln			/* image block structure	*/
{
	WORD	rim_offset;
	WORD	rim_length;
}IMOFLN;

typedef struct rstree
{
	WORD	rst_first;
	WORD	rst_count;
}RSTREE;

#define RES_TREE	8		/* # of long tree pointers	*/
					/*  reserved in the global array*/

			/* these must coincide w/ rshdr */
#define RT_VRSN 0
#define RT_OB 1
#define RT_TEDINFO 2
#define RT_ICONBLK 3
#define RT_BITBLK 4
#define RT_FREESTR 5
#define RT_STRING 6
#define RT_IMAGEDATA 7
#define	RT_FREEIMG 8
#define RT_TRINDEX 9

#define NUM_RTYPES 10

#define R_NOBS 10
#define R_NTREE 11
#define R_NTED 12
#define R_NICON 13
#define R_NBITBLK 14
#define R_NSTRING 15
#define R_IMAGES 16

#define NUM_RN 7

#define RS_SIZE 17				/* NUM_RTYPES + NUM_RN	*/

#define HDR_LENGTH ((RS_SIZE+1)*2)		/* in bytes	*/


typedef struct rshdr
{
	WORD		rsh_vrsn;	/* must same order as RT_	*/
	WORD		rsh_object;
	WORD		rsh_tedinfo;
	WORD		rsh_iconblk;	/* list of ICONBLKS		*/
	WORD		rsh_bitblk;
	WORD		rsh_frstr;	
	WORD		rsh_string;
	WORD		rsh_imdata;	/* image data			*/
	WORD		rsh_frimg;	
	WORD		rsh_trindex;
	WORD		rsh_nobs;	/* counts of various structs	*/
	WORD		rsh_ntree;
	WORD		rsh_nted;
	WORD		rsh_nib;
	WORD		rsh_nbb;
	WORD		rsh_nstring;
	WORD		rsh_nimages;
	WORD		rsh_rssize;	/* total bytes in resource	*/
}RSHDR;


/* type definitions for use by an application when calling	*/
/*  rsrc_gaddr and rsrc_saddr					*/

#define R_TREE 0
#define R_OBJECT 1
#define R_TEDINFO 2
#define R_ICONBLK 3
#define R_BITBLK 4
#define R_STRING 5		/* gets pointer to free strings	*/
#define R_IMAGEDATA 6		/* gets pointer to free images	*/
#define R_OBSPEC 7
#define R_TEPTEXT 8		/* sub ptrs in TEDINFO	*/
#define R_TEPTMPLT 9
#define R_TEPVALID 10
#define R_IBPMASK 11		/* sub ptrs in ICONBLK	*/
#define R_IBPDATA 12
#define R_IBPTEXT 13
#define R_BIPDATA 14		/* sub ptrs in BITBLK	*/
#define R_FRSTR 15		/* gets addr of ptr to free strings	*/
#define R_FRIMG 16		/* gets addr of ptr to free images	*/


/*	WINDLIB.H	05/05/84 - 01/26/85	Lee Lorenzen		*/

#define VF_INUSE 0x0001
#define VF_BROKEN 0x0002
#define VF_INTREE 0x0004
#define VF_SUBWIN 0x0008
#define VF_KEEPWIN 0x0010

typedef struct window
{
	WORD		w_flags;
	PD		*w_owner;
	WORD		w_kind;
	LONG		w_pname;
	LONG		w_pinfo;
	WORD		w_xfull;
	WORD		w_yfull;
	WORD		w_wfull;
	WORD		w_hfull;
	WORD		w_xwork;
	WORD		w_ywork;
	WORD		w_wwork;
	WORD		w_hwork;
	WORD		w_xprev;
	WORD		w_yprev;
	WORD		w_wprev;
	WORD		w_hprev;
	WORD		w_hslide;
	WORD		w_vslide;
	WORD		w_hslsiz;
	WORD		w_vslsiz;
	ORECT		*w_rlist;	/* owner rect. list	*/
	ORECT		*w_rnext;	/* used for search first*/
					/*   search next	*/
} WINDOW;

#define NUM_ORECT (NUM_WIN * 10)	/* is this enough???	*/

#define WS_FULL 0
#define WS_CURR 1
#define WS_PREV 2
#define WS_WORK 3
#define WS_TRUE 4

#define XFULL 0
#define YFULL gl_hbox
#define WFULL gl_width
#define HFULL (gl_height - gl_hbox)

#define NAME 0x0001
#define CLOSER 0x0002
#define FULLER 0x0004
#define MOVER 0x0008
#define INFO 0x0010
#define SIZER 0x0020
#define UPARROW 0x0040
#define DNARROW 0x0080
#define VSLIDE 0x0100
#define LFARROW 0x0200
#define RTARROW 0x0400
#define HSLIDE 0x0800
#define HOTCLOSE 0x1000		/* added 11/12/85	LKW		*/

#define W_BOX 0
#define W_TITLE	1
#define W_CLOSER 2
#define W_NAME	3
#define W_FULLER 4
#define W_INFO 5
#define W_DATA	6
#define W_WORK	7
#define W_SIZER	8
#define W_VBAR 9
#define W_UPARROW 10
#define W_DNARROW 11
#define W_VSLIDE 12
#define W_VELEV	13
#define W_HBAR 14
#define W_LFARROW 15
#define W_RTARROW 16
#define W_HSLIDE 17
#define W_HELEV	18

#define NUM_ELEM 19

#define WC_BORDER 0
#define WC_WORK 1


#define WF_KIND 1
#define WF_NAME 2
#define WF_INFO 3
#define WF_WXYWH 4
#define WF_CXYWH 5
#define WF_PXYWH 6
#define WF_FXYWH 7
#define WF_HSLIDE 8
#define WF_VSLIDE 9
#define WF_TOP 10
#define WF_FIRSTXYWH 11
#define WF_NEXTXYWH 12

#define WF_NEWDESK 14
#define WF_HSLSIZ 15
#define WF_VSLSIZ 16
#define WF_SCREEN 17
#define WF_TATTRB 18

#define WF_SIZTOP 19
						/* window attributes	*/
#define WA_SUBWIN 0x01
#define WA_KEEPWIN 0x02
						/* arrow message	*/
#define WA_UPPAGE 0
#define WA_DNPAGE 1
#define WA_UPLINE 2
#define WA_DNLINE 3
#define WA_LFPAGE 4
#define WA_RTPAGE 5
#define WA_LFLINE 6
#define WA_RTLINE 7

#define G_SIZE 15

#define SH_INPLACE 0
#define SH_ABOVE 1
#define SH_FSTEP 2
#define SH_ACC 3

typedef struct sh_struct
{
	WORD	sh_doexec;			/* if TRUE then do an	*/
						/*   an exec on the	*/
						/*   current command	*/
						/*   else exit and	*/
						/*   return to DOS	*/
	WORD	sh_dodef;			/* if TRUE then use the	*/
						/*   default command	*/
						/*   which is always	*/
						/*   DESKTOP, else use	*/
						/*   the command that	*/
						/*   the app. supplied	*/
	WORD	sh_isdef;			/* if TRUE then using	*/
						/*   default command	*/
						/*   which is always	*/
						/*   DESKTOP, else use	*/
						/*   the command that	*/
						/*   the app. supplied	*/
	WORD	sh_fullstep;			/* if TRUE then make	*/
						/*   AES step-aside 	*/
						/*   otherwise just the */
						/*   DESKTOP		*/
	WORD	sh_isgem;			/* used to signal if	*/
						/*   the currently 	*/
						/*   running appl.	*/
						/*   is a GEM app.	*/
#if MULTIAPP
	WORD	sh_loadable;			/* if TRUE then can make*/
						/*   a pr_load call on	*/
						/*   this channel	*/
	WORD	sh_wh;				/* window handle of fake*/
						/*   window for a dos app*/
	WORD	sh_isacc;			/* if TRUE then accessory*/
	WORD	sh_state;			/* bit vector of states	*/
#endif
	BYTE	sh_desk[14];
	BYTE	sh_cdir[67];
} SHELL;


#define THEGLO struct glstr
THEGLO
{
/* GLOBAL */ UDA	g_intuda[2];			/* must be 1st	*/
/* GLOBAL */ PD		g_intpd[2];
/* GLOBAL */ CDA	g_intcda[2];
/* GLOBAL */ EVB	g_intevb[NUM_IEVBS];

/* GLOBAL */ BYTE	*g_scrap;
/* GLOBAL */ BYTE	*g_loc1;
/* GLOBAL */ BYTE	*g_loc2;
/* GLOBAL */ BYTE	*g_dir;
/* GLOBAL */ BYTE	*g_dta;
/* GLOBAL */ BYTE	*s_cdir;
/* GLOBAL */ BYTE	*s_cmd;

/* GLOBAL */ FPD	g_fpdx[NFORKS];
/* GLOBAL */ ORECT 	g_olist[NUM_ORECT];

/* GLOBAL */ BYTE	g_rawstr[MAX_LEN];
/* GLOBAL */ BYTE	g_tmpstr[MAX_LEN];
/* GLOBAL */ BYTE	g_valstr[MAX_LEN];
/* GLOBAL */ BYTE	g_fmtstr[MAX_LEN];

/* GLOBAL */ BYTE	*g_fslist[NM_FILES];
#if MULTIAPP
/* GLOBAL */ BYTE	g_fsnames[LEN_FSNAME * NM_FILES];
#endif

/* GLOBAL */ UWORD	g_sysglo[G_SIZE];

/* GLOBAL */ WINDOW	w_win[NUM_WIN];
						/* all but 1st 2 pds,	*/
						/*   cdas, and udas	*/
						/*   may not be used	*/
/* GLOBAL */ EVB	g_extevb[NUM_EEVBS];
/* GLOBAL */ UDA	g_extuda[NUM_PDS-2];
/* GLOBAL */ PD		g_extpd[NUM_PDS-2];
/* GLOBAL */ CDA	g_extcda[NUM_PDS-2];
};


