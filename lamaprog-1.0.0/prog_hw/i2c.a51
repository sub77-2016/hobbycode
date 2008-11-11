;LAMAPROG
;I2C subroutines v 0.1
;Jan Parkman (parkmaj@users.sourceforge.net)
;this software is under GNU license (www.gpl.org)

;---- i2c defines
i2c_sda 	equ	p1.0
i2c_scl		equ	p1.1


;-----------------------------------------------------------------
;--- I2C bus routines


;--- initializate bus (set SDA and SCL to high)
i2c_init:
	setb i2c_scl
	setb i2c_sda
ret



;--- send stop
i2c_stop:
	clr	i2c_sda
	clr	i2c_scl
	nop
	nop
	setb	i2c_scl
	nop
	nop
	nop
	nop
	setb 	i2c_sda
ret	


;--- send start
i2c_start:
	setb	i2c_scl
	setb	i2c_sda	
	clr	i2c_sda
	nop
	nop
	nop
	nop
	clr	i2c_scl
ret	
	
	
;--- send byte (ACK is in c) 
i2c_send:
	push	ar0
	mov	r0,#8
i2c_send_loop:
	rlc	a
	mov	i2c_sda,c
	nop
	setb	i2c_scl
	nop
	nop
	nop
	nop
	clr	i2c_scl
	djnz	r0,i2c_send_loop

	setb	i2c_sda
	nop
	nop
	setb	i2c_scl
	nop
	nop
	nop
	mov	c,i2c_sda
	clr	i2c_scl

	pop	ar0
ret


;--- recv byte
i2c_recv:
	setb	i2c_sda
	push	ar0
	mov	r0,#8
i2c_recv_loop:
	nop
	nop
	nop
	setb	i2c_scl
	nop
	nop
	mov	c,i2c_sda
	rlc	a
	clr	i2c_scl
	djnz	r0,i2c_recv_loop

	pop	ar0
ret


;--- send ack
i2c_ack:
	clr	i2c_sda
	nop
	nop
i2c_ack1:
	setb	i2c_scl
	nop
	nop
	nop
	clr	i2c_scl
ret


;--- send nak
i2c_nak:
	setb	i2c_sda
	jmp	i2c_ack1
; ----------------------------------------------------------------


