section	.text
	global	_start

_start:
	mov		rax, 1
	mov		rdi, 1
	lea		rsi, [rel msg]
	mov		rdx, msg_end - msg
	syscall

	mov		rax, 60
	mov		rdi, 0
	syscall

;	xor		rdx, rdx
;	mov		rax, 0x22222222
;	jmp		rax

align 8
	msg		db 'This file has been infected',0x0a,0
	msg_end	db 0x0
