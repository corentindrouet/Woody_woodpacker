section .text
	global	_start

_start:
	mov		rax, 0x04
	mov		rbx, 0x01
	lea		rcx, [rel msg]
	mov		rdx, 0x14
	int		0x80

	xor		rdx, rdx
	mov		rax, 0x1111111111111111
	jmp		rax

msg:
	db 'Hello World !',0x0a,0x00
