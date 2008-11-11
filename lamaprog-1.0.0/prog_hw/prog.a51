;LAMAPROG v 0.2
;Jan Parkman (parkmaj@users.sourceforge.net)
;This software is under GNU license (www.gpl.org)

;suported devices:
;	- i2c eeproms: 24c01 -- 24c512
;	- 8051: at89c2051
;	- general purpose i2c bus (rs232 <-> i2c convertor) 	


;TODO -- revcblock convert data from hexa ascii to number? 

;----------------------------------------------------------------------
;--- EQUs
;base code segment
cbaseseg	equ	0000h


;----------------------------------------------------------------------
;--- Data segment
dseg at 0030h

;----------------------------------------------------------------------
;--- Bit segment
bseg at 21h


;----------------------------------------------------------------------
;--- Code segment
cseg at cbaseseg

	jmp start

org cbaseseg + 0030h


start:
	call 	init
	call 	comver

loop:

	jnb 	RI,loopend
	call 	recvblock

	mov 	r0,#80h
	cjne 	@r0,#':',badcom
	
	inc	r0			;#81h	

	cjne 	@r0,#'v',loopdev	;Version command

	call 	comver
	jmp	loop
			
loopdev:
	cjne 	@r0,#'d',badcom		;Device select
	call 	comdev 
	jmp	loop	


loopend:

	jmp	loop
badcom:			;bad command
	call	sendminus

	jmp 	loop


;--- DEVICES


;!NOT implemented (now)

;----------------------------------------------------------------------
;--- device 'a' -- Atmel 89cxxx chips
; current address is in dptr
dseg

cseg
;in dptr is curent address
pin_p33		equ	p0.2	;on 2051 p3.3
pin_p34		equ	p0.3	;	 p3.4
pin_p35		equ	p0.4	;	 p3.5
pin_p37		equ	p0.5	;	 p3.7
port_data	equ	p2	;	 p1
pin_prog	equ	p0.6	;	 p3.2	(ale)
pin_p5_12v	equ	p3.5	;	 5V/12V (5V if set) 
pin_p0v		equ	p3.4	;	 0V     (0V if set)
pin_xtal1	equ	p0.1	;	 XTAL1 ****
pin_bsy		equ	p0.7	;	 p3.1 

dev_a:	;only 89c2051 now
	call	sendplus
	;init
	mov	port_data,#0ffh
	mov	dph,#0	
	mov	dpl,#0
	setb	pin_p5_12v	;0V
	setb	pin_p0v
	clr	pin_xtal1

deva_loop:
	call	recvblock
	mov	r0,#80h
	cjne	@r0,#':',deva_bad1	;bad command
	inc	r0			;r0=81h (second char)


;--- set address :aAAAA
deva_addr:
	cjne @r0,#'a',deva_read

	clr	pin_xtal1
				;reset counter...
	setb	pin_p5_12v	;rising edge 0 -> 5V 
	setb	pin_p0v
	clr	pin_p0v
	
	mov	r0,#84h		;low address byte	
	call 	ascii2num
	mov	dpl,a
	
	mov	r0,#82h
	call	ascii2num	;high address byte
	mov	dph,a

	jz	deva_addr_l
deva_addr_h1:			;high byte
	mov	b,#0
deva_addr_h2:	
	setb	pin_xtal1
	clr	pin_xtal1
	djnz	b,deva_addr_h2
	djnz	acc,deva_addr_h1	

deva_addr_l:		
	mov	a,dpl		;low byte
	jz	deva_addr_l2
deva_addr_l1:
	setb	pin_xtal1
	clr	pin_xtal1
	djnz	acc,deva_addr_l1;
deva_addr_l2:

	call	sendplus	
	jmp	deva_loop

deva_bad1:
	jmp	deva_bad

;--- program reading :rNN
;- read NN bytes from device
deva_read:
	cjne @r0,#'r',deva_sig

	mov	port_data,#0ffh

	setb	pin_p35
	setb	pin_p37
	clr	pin_p33
	clr	pin_p34

	mov 	r0,#82h		;count
	call	ascii2num
	mov	r1,a


deva_read_sig:	

	setb	pin_p5_12v	;5V
	clr	pin_p0v
	setb	pin_prog
	

deva_read_l:			;reading loop
	mov	a,port_data
	setb	pin_xtal1
	clr	pin_xtal1
	call	sendhex
	djnz r1,deva_read_l

	call	sendplus
	jmp	deva_loop

;-- read signature bytes (:s)
deva_sig:
	cjne @r0,#'s',deva_erase
	mov	dpl,#0
	mov	dph,#0
				;reset counter...
	setb	pin_p5_12v	;rising edge 0 -> 5V 
	setb	pin_p0v
	clr	pin_p0v

	mov	r1,#3
	mov	port_data,#0ffh
	clr	pin_p35
	clr	pin_p37
	clr	pin_p33
	clr	pin_p34
	jmp	deva_read_sig
;	jmp	deva_loop

;--- chip erase (:e)
deva_erase:
	cjne @r0,#'e',deva_write
	setb	pin_prog
	setb	pin_p33
	clr	pin_p34
	clr	pin_p35
	clr	pin_p37

	clr	pin_p5_12v	;12V
	clr	pin_p0v

	clr	pin_prog

	mov	a,#250		;10ms
deva_erase1:
	mov	b,#40
	djnz	b,$	
	djnz	acc,deva_erase1	

	setb	pin_prog

	setb	pin_p5_12v

	call	sendplus
	jmp	deva_loop

;--- write NN data bytes from current address (:wNNDDDDDDDDD)
deva_write:
	cjne	@r0,#'w',deva_ver
	setb	pin_prog
	clr	pin_p33
	setb	pin_p34
	setb	pin_p35
	setb	pin_p37	

	mov	r0,#82h		;in b is number of repeat (NN)
	call	ascii2num
	mov	b,a

	clr	pin_p5_12v	;12V
	clr	pin_p0v

	mov	r0,#84h		;first data byte

deva_write_loop:
	call	ascii2num
	mov	port_data,a
	
	clr	pin_prog
	setb	pin_prog

	inc	r0		;increment "data pointer"
	inc	r0

	mov	a,#250		;wait 2ms for bsy high else write failed	
deva_write_2ms:
	nop
	nop
	nop
	jb	pin_bsy,deva_write_ok
	djnz	acc,deva_write_2ms
	jmp	deva_bad
	
deva_write_ok:	

	setb	pin_xtal1	;increment counter
	clr	pin_xtal1

	djnz	b,deva_write_loop

	setb	pin_p5_12v

	call	sendplus
	jmp	deva_loop

;--- print version
deva_ver:
	cjne	@r0,#'v',deva_quit
	mov 	DPTR,#devaban
	call 	sendtext
	jmp	deva_loop

;--- return... (:q)
deva_quit:
	cjne	@r0,#'q',deva_bad
	 call	sendplus
	 ret




deva_bad:
	call	sendminus
	jmp	deva_loop
ret


devaban:	db "at89c2051 interface v 0.1+",0



;----------------------------------------------------------------------
;--- device 'j' -- I2C 24cxx memory
;--- tested with at24c16 and at24c256
base24cr	equ	10100001b	;read i2c address
base24cw	equ	10100000b	;write i2c address

bseg	
j_large:	dbit	1	;set for large memorry (two bytes address)
cseg
		
;init
device_j:			
	call	i2c_init
	inc	r0
	cjne	@r0,#'l', device_j1	;large mem
	 setb	j_large
	 call	sendplus
	 jmp	device_jloop
		
device_j1:
	cjne	@r0,#'s',device_j2	;small mem
	 clr	j_large
	 call	sendplus
	 jmp	device_jloop

device_j2:				;bad device
	call	sendminus
	ret

device_jloop:
	call	recvblock
	mov	r0,#80h
	cjne	@r0,#':',devj_b0	;bad command
	inc	r0			;r0=81h (second char)

;--- return... (:q)
devj_quit:
	cjne	@r0,#'q',device_jread
	 call	sendplus
	 ret

;--- read N bytes from address A (:rNNAAAA) 
device_jread:
	cjne	@r0,#'r',device_jwrite
	 call	i2c_start

	 jb	j_large,devj_readl	 
	 mov	r0,#84h		;SMALL high address byte
	 call	ascii2num
	 rl	a		;in acc address high byte 
	 orl	a,#10100000b	
	 push	acc		;save write addres
	 call	i2c_send
	 jc	devj_b1		;if no ack
	 jmp	devj_reads

devj_readl:
	 mov	a,#10100000b	;LARGE
	 push	acc
	 call	i2c_send
	 jc	devj_b1
	 mov	r0,#84h		;high address byte
	 call	ascii2num
 	 call	i2c_send
 	 jc	devj_b1
devj_reads:
	 
	 mov	r0,#86h 	;low address byte
	 call	ascii2num
	 call	i2c_send
	 jc	devj_b1		;if no ack

	 call	i2c_start	;read bytes
	 pop	acc
	 setb	acc.0		;in acc is read i2c address
	 call	i2c_send
devj_b0: jc	devj_b1		;if no ack

	 mov	r0,#82h		;repeat number byte
	 call	ascii2num
	 mov	r7,a

device_jrloop:         		;reading loop
	  call	i2c_recv
	  call	sendhex
	  cjne	r7,#1,device_jrlend
	   call	i2c_nak
	   jmp	device_jrend
device_jrlend:
	  call	i2c_ack
	 djnz	r7,device_jrloop

device_jrend:
	 call	i2c_stop
	 call	sendplus
	 jmp	device_jloop


;--- write N bytes D from address A (:wNNAAAADDDDDD...)
device_jwrite:		  
	cjne	@r0,#'w',device_jver
	 call	i2c_start
	 
	 jb	j_large, devj_wl
	 mov	r0,#84h		;SMALL high address byte
	 call	ascii2num
	 rl	a		;in acc address high byte 
	 orl	a,#10100000b	
	 call	i2c_send
devj_b1: jc	device_jbad	;if no ack
	 jmp	devj_ws
devj_wl:
	 mov	a,#10100000b	
	 call	i2c_send
	 jc	device_jbad	;if no ack
	 mov	r0,#84h		;SMALL high address byte
	 call	ascii2num
	 call	i2c_send	;in acc address high byte 
	 jc	device_jbad
	 jmp	devj_ws
 
devj_ws:
	 mov	r0,#86h 	;low address byte
	 call	ascii2num
	 call	i2c_send
	 jc	device_jbad	;if no ack

	 mov	r0,#82h		;repeat number byte
	 call	ascii2num
	 mov	r7,a

	 mov	r0,#88h	
device_jwloop:         		;reading loop
	call	ascii2num
	call	i2c_send
	  
	jc	device_jbad	;if no ack

	inc	r0
	inc	r0
	djnz	r7,device_jwloop

	call	i2c_stop

	call	sendplus
	jmp	device_jloop

device_jver:
	cjne	@r0,#'v',device_jbad
	mov 	DPTR,#devjban
	call 	sendtext
	jmp	device_jloop

device_jbad:
	call	sendminus
;	call	sendhex
;	mov	a,b
;	call	senda
	jmp	device_jloop
	
devjban:	db "24Cxx interface v 0.1+",0


;----------------------------------------------------------------------
;--- device 'i' -- I2C 
device_i:
;init
	call	sendplus
	call	i2c_init
device_iloop:
	call	recvblock
	 mov	r0,#80h
	 cjne	@r0,#':',device_ibad	;bad command
	 inc	r0	;r0=81h (second char)

;--- return (:q)
devi_quit:
	cjne	@r0,#'q',device_iread
	 call	sendplus
	 ret
	 	
;--- read byte (:r) (return +XX (x is hexa number) / -)
device_iread:	
	cjne	@r0,#'r',device_iwrite
	call	sendplus
	call	i2c_recv
	call	sendhex	
	jmp	device_iloop
	;/// read byte

;--- write byte (:wXX) return +a if ack,  +n if nak / -
device_iwrite:	
	cjne	@r0,#'w',device_iack
	inc	r0

	call	ascii2num
	call	i2c_send	;in carry is ack
	
	jnc	device_iwrite1
	mov	a,#'n'
	jmp	device_iwrite2
device_iwrite1:
	mov	a,#'a'
device_iwrite2:
	call	senda
	call	sendplus
	jmp 	device_iloop
	;/// write byte

;--- write ack (:a+) or nak (:a-)
device_iack:
	cjne	@r0,#'a',device_istop

	inc	r0

	cjne	@r0,#'-',device_iack1	;write nak
	call	i2c_nak
	call	sendplus
	jmp	device_iloop

device_iack1:
	cjne	@r0,#'+',device_ibad	;write ack
	call	i2c_ack
	call	sendplus
	jmp	device_iloop
	;/// write ack or nak

;--- generate stop (:t)
device_istop:
	cjne	@r0,#'t',device_istart
	call	i2c_stop
	call	sendplus
	jmp	device_iloop
	;///generate stop

;--- generate start (:s)
device_istart:
	cjne	@r0,#'s',device_iver
	call	i2c_start
	call	sendplus
	jmp	device_iloop
	;///generate start
	
device_iver:
	cjne	@r0,#'v',device_i6
	mov 	DPTR,#deviban
	call 	sendtext
	jmp	device_jloop


device_i6:


;bad command
device_ibad:
	call	sendminus
	jmp	device_iloop
ret

deviban:	db "I2C interface 0.1+",0

;----------------------------------------------------------------------
;--- COMMANDS
;command Get programmer version :v
comver:
	mov DPTR,#banner
	call sendtext
ret


;--- command Select programmed device :dxx
comdev:
	inc 	r0
;general i2c device (rs232 <-> i2c comvertor)
	cjne 	@r0,#'i',comdev1
	call	device_i
	ret

;i2c eeproms (24c01)
comdev1:
	cjne 	@r0,#'j',comdev2
	call	device_j
	ret

;MCU 51 (at89c51)
comdev2:
	cjne @r0,#'a',comdev3
	call dev_a
	ret

comdev3:
;bad command
	call	sendminus

comdevend:

ret





;----------------------------------------------------------------------
;--- Initialization
init: 
	clr EA
	mov     th1,#0fdh       ;9600 Bd pro XTAL=11,0592 Mhz
	setb	tr1
	mov     scon,#52h       ;8 bit. UART
	mov	tmod,#20h	;timer 1 mod 2
ret

;----------------------------------------------------------------------
;--- Utilites


;--- receive block of data, place it to high half memory (80h -- ffh)
;--- end of data is indicated #0 (like C string) 
recvblock:

recvblock1:
	mov r1,#080h-1

recvnext:
	jnb ri,$
	clr ri
	mov a,sbuf
	inc r1

;	cjne a,#0ah,recv1	;end (acc == \n)
	cjne a,#' ',recv1	;end (acc == \n)	

	mov a,#0
	mov @r1,a
	ret
recv1:	
	mov @r1,a
	jmp recvnext

ret	

;--- send text form address in DPTR to first \0 (like C string)
sendtext:
	push	dph
	push	dpl
sendloop:
	clr a
	movc a,@a+DPTR
	jz  sendend
	jnb TI,$
	clr TI
	mov sbuf,a
	inc dptr
	jmp sendloop
sendend:
	pop	dpl
	pop	dph
ret



;--- send acc
senda:
	jnb 	TI,$
	clr 	TI
	mov 	sbuf,a
ret

;--- send '-'  
sendminus:
	jnb 	TI,$
	clr 	TI
	mov 	sbuf,#'-'
ret

;--- send '+'  
sendplus:
	jnb 	TI,$
	clr 	TI
	mov 	sbuf,#'+'
ret

;--- send acc as a hex number (high byte first)
;ok
sendhex:
	mov	b,#16
	div	ab

	add	a,#'0'
	cjne	a,#9+'0'+1,sendhex1	;if (a<#9) {c=1}
sendhex1:
	jc 	sendhex10
	add	a,#'A' - '0' - 10
sendhex10:
	jnb 	TI,$
	clr 	TI
	mov 	sbuf,a
	
	mov	a,b
	add	a,#'0'
	cjne	a,#9+'0'+1,sendhex2	;if (a<#9) {c=1}
sendhex2:
	jc	sendhex20
	add	a,#'A'-'0' - 10
sendhex20:
	jnb 	TI,$
	clr 	TI
	mov 	sbuf,a

ret

;--- convert ascii hex number in @r0 (H) and @r0+1 (L) into byte in acc
;ok
ascii2num:
	push	b
	mov 	a,@r0		;high 
	cjne 	a,#'0'+10,a2n0	;if (a <= #'9'){c=1}
a2n0:	jc	a2n1		;
	subb 	a,#'A'-'0'-10 	;is a letter
a2n1:
	clr	c	
	subb 	a,#'0'
	mov 	b,#16
	mul 	ab
	mov 	b,a

	inc	r0
	mov 	a,@r0		;low
	cjne 	a,#'0'+10,a2n2	;if (a <= #'9'){c=1}
a2n2:	jc	a2n3		;
	subb 	a,#'A'-'0'-10 	;is a letter
a2n3:
	clr	c	
	subb 	a,#'0'
	add 	a,b
	pop	b
	dec	r0
ret

$include (i2c.a51)

banner:	db "LamaProg v 0.2+",0
copyleft: db "LamaProg, Jan Parkman, parkmaj@users.sourceforge.net, 2003"
end
