section .text
	global	_start

_start:
	mov		rdi, 0x22222222
	mov		rsi, 0x0
	xor		rcx, rcx

loop:
	cmp		rcx, rsi
	je		exit
	xor		[rdi+rcx], byte 120
	inc		rcx
	jmp		loop

exit:
	xor		rdx, rdx
	mov		rax, 0x22222222
	jmp		rax
