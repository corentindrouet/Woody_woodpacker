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

	mov		rax, 60
	mov		rdi, 0
	syscall

msg:
	db 'Hello World !',0x0a,0x00
