section .text
	global	_start

_start:
	mov		rax, 1
	mov		rdi, 1
	lea		rsi, [rel msg]
	mov		rdx, 14
	syscall

	xor		rdx, rdx
	mov		rax, 0x1111111111111111
	jmp		rax

msg:
	db 'Hello World !',10,0
