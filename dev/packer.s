section	.text
	global	_start

_start:
	mov		rax, 1
	mov		rdi, 1
	lea		rsi, [rel msg]
	mov		rdx, msg_end - msg
	syscall

	xor		rdx, rdx
	mov		rax, 0x580
	jmp		rax

msg:		db 'This file has been infected', 10, 0
msg_end:	db 0x0
