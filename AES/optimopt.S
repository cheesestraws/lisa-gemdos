/*	OPTIMOPT.S */



*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 


.globl _r_get
.globl _r_set
.globl _scasb
.globl _bfill
.globl _strchk

*
*	VOID
*r_get(pxywh, px, py, pw, ph)
*	WORD		*pxywh;
*	WORD		*px, *py, *pw, *ph;
_r_get:
LD80A:
            link    A6,#0
            movem.l D7/A5,-(SP)
            movea.l $08(A6),A5
            movea.l $0C(A6),A0
            move.w  (A5),(A0)
            movea.l $10(A6),A0
            move.w  $02(A5),(A0)
            movea.l $14(A6),A0
            move.w  $04(A5),(A0)
            movea.l $18(A6),A0
            move.w  $06(A5),(A0)
            tst.l   (SP)+
            movem.l (SP)+,A5
            unlk    A6
            rts


*	VOID
*r_set(pxywh, x, y, w, h)
*	WORD		*pxywh;
*	WORD		x, y, w, h;
_r_set:
LD83E:
            link    A6,#$00
            movem.l D7/A5,-(SP)
            movea.l $08(A6),A5
            move.w  $0C(A6),(A5)
            move.w  $0E(A6),$02(A5)
            move.w  $10(A6),$04(A5)
            move.w  $12(A6),$06(A5)
            tst.l   (SP)+
            movem.l (SP)+,A5
            unlk    A6
            rts


_scasb:
LDB94:
            link    A6,#$00
            movem.l D6-D7/A5,-(SP)
            movea.l $08(A6),A5
            move.b  $0D(A6),D7
            bra     LDBA8
LDBA6:
            addq.l  #1,A5
LDBA8:
            tst.b   (A5)
            beq     LDBB0
            cmp.b   (A5),D7
            bne     LDBA6
LDBB0:
            move.l  A5,D0
            tst.l   (SP)+
            movem.l (SP)+,D7/A5
            unlk    A6
            rts

/*
	VOID
bfill(num, bval, addr)
	REG WORD	num;
	BYTE		bval;
	REG BYTE	*addr;
*/
_bfill:
LDB3E:
            link    A6,#$00
            movem.l D5-D7/A5,-(SP)
            move.w  $08(A6),D7
            move.b  $0B(A6),D6
            movea.l $0C(A6),A5
LDB52:
            move.b  D6,(A5)+
            subq.w  #1,D7
            bne     LDB52
            tst.l   (SP)+
            movem.l (SP)+,D6-D7/A5
            unlk    A6
            rts



/*
*	Return <0 if s<t, 0 if s==t, >0 if s>t
*
*
*	WORD
*strchk(s, t)
*	BYTE		s[], t[];
*/
_strchk:
LDC10:
            link    A6,#$00
            movem.l D6-D7,-(SP)
            clr.w   D7
            bra     LDC30
LDC1C:
            movea.l $08(A6),A0
            move.b  $00(A0,D7.w),D0
            ext.w   D0
            addq.w  #1,D7
            tst.b   D0
            bne     LDC30
            clr.w   D0
            bra     LDC5E
LDC30:
            movea.l $08(A6),A0
            move.b  $00(A0,D7.w),D0
            ext.w   D0
            movea.l $0C(A6),A1
            move.b  $00(A1,D7.w),D1
            ext.w   D1
            cmp.w   D1,D0
            beq     LDC1C
            movea.l $08(A6),A0
            move.b  $00(A0,D7.w),D0
            ext.w   D0
            movea.l $0C(A6),A1
            move.b  $00(A1,D7.w),D1
            ext.w   D1
            sub.w   D1,D0
LDC5E:
            tst.l   (SP)+
            movem.l (SP)+,D7
            unlk    A6
            rts
