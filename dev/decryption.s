section .text:
	global _start
	global _decrypt

_start:
	call _decrypt
	mov rax, 0x4444444444444444
	jmp rax

_decrypt:
	push rbp
	mov rbp, rsp
	sub rsp, 0x12c
	mov rax, 0xa
	mov rdi, 0x11111111
	;lea rdi, [rel zone]
	mov rsi, QWORD [0x22222222] 
	mov rdx, 0x3
	syscall
	xor rcx, rcx

_init_table:
	cmp rcx, 0x100
	jge _init_sorting
	mov BYTE [rsp + rcx], cl
	inc rcx
	jmp _init_table

_init_sorting:
	xor rcx, rcx
	mov r10, QWORD [rsp + 260]
	xor QWORD [rsp + 260], r10

_sorting:
	cmp rcx, 0x100
	jge _init_decrypt_loop
	xor r10, r10
	mov r10b, [rsp + rcx]
	add QWORD [rsp + 260], r10
	xor r10, r10
	mov r10b, [0x33333333 + rcx]
	add QWORD [rsp + 260], r10
	and QWORD [rsp + 260], 255
	xor r10, r10
	mov r10b, BYTE [rsp + 260]
	lea rdi, [rel rsp + rcx]
	lea rsi, [rel rsp + r10]
	inc rcx
	push QWORD _sorting

_swap:
	xor r10, r10
	mov r10b, BYTE [rdi]
	xor r11, r11
	mov r11b, BYTE [rsi]
	mov BYTE [rdi], r11b
	mov BYTE [rsi], r10b
	pop rax
	jmp rax

_init_decrypt_loop:
	xor r10, r10
	mov r10, QWORD [rsp + 260]
	xor QWORD [rsp + 260], r10
	xor r10, r10
	mov r10, QWORD [rsp + 268]
	xor QWORD [rsp + 268], r10
	xor rcx, rcx

_decrypt_loop:
	cmp rcx, QWORD [0x22222222]
	jge _end
	add QWORD [rsp + 260], 1
	and QWORD [rsp + 260], 255
	xor r10, r10
	mov r10, QWORD [rsp + 260]
	lea rdi, [rel rsp + r10]
	xor r10, r10
	mov r10b, BYTE [rdi]
	add QWORD [rsp + 268], r10
	and QWORD [rsp + 268], 255
	xor r10, r10
	mov r10, QWORD [rsp + 268]
	lea rsi, [rel rsp + r10]
	push QWORD _continue
	jmp _swap
	_continue:
	mov r10, QWORD [rsp + 268]
	xor QWORD [rsp + 268], r10
	xor r10, r10
	mov r10b, BYTE [rdi]
	xor r11, r11
	mov r11b, BYTE [rsi]
	add QWORD [rsp + 268], r10
	add QWORD [rsp + 268], r11
	and QWORD [rsp + 268], 255
	xor r11, r11
	mov r11, QWORD [rsp + 268]
	xor r10, r10
	mov r10b, BYTE [rsp + r11]
	xor BYTE [0x11111111 + rcx], r10b
	inc rcx
	jmp _decrypt_loop

_end:
	mov rax, 0xa
	mov rdi, 0x11111111
	mov rsi, QWORD [0x22222222] 
	mov rdx, 0x3
	syscall
	leave
	ret
