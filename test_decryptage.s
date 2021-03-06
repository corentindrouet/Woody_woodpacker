;section .data
;	zone: ;0x11111111
;		.string db 0x62,0x97,0x15,0x6b,0xc1,0x3f,0x1e,0x00
;	zone_size: ;0x22222222
;		.long dq 7
;	key: ;0x33333333
;		.string db  0xae,0x61,0x64,0x9a,0xf9,0xd4,0x1f,0xb3,0x50,0xb1,0x88,0x84,0x1c,0x6d,0x1a,0x93, \
; 0x9d,0xcc,0x15,0xdd,0x3b,0xa8,0x3e,0x69,0x1d,0x72,0xa9,0x6b,0x6a,0xf5,0x31,0xec, \
; 0xb2,0xba,0x6d,0xc0,0x22,0x86,0xbc,0x91,0x83,0x57,0x2d,0x63,0x24,0x39,0x6b,0xb1, \
; 0x61,0x0f,0xfe,0x3f,0xb6,0x33,0x1a,0xa4,0x13,0xb6,0xe9,0x3e,0xfe,0x46,0x76,0x0c, \
; 0x71,0xf4,0xfd,0x09,0xc0,0x21,0x3e,0xf8,0x9e,0x55,0xfa,0x41,0xe3,0x63,0x2f,0x66, \
; 0x9f,0xf2,0x43,0x6e,0xc0,0x99,0x3c,0xbf,0x02,0x10,0x45,0x07,0xbd,0x55,0x45,0x55, \
; 0xf0,0x8a,0xbc,0x1e,0x22,0xaa,0x55,0xbf,0x37,0xa4,0x2a,0xaf,0xf1,0x90,0x5a,0xc7, \
; 0x7a,0x70,0x45,0xe0,0x1d,0x26,0x58,0xd4,0xf0,0x82,0xc7,0xf9,0x30,0xe4,0xfe,0x36, \
; 0x02,0x17,0x6c,0xb5,0x04,0xa3,0xd4,0xff,0xa0,0xba,0x9b,0x50,0x3b,0x21,0x54,0xd6, \
; 0x84,0x22,0xd9,0x43,0x16,0x2a,0xa0,0x08,0xcd,0x76,0xbc,0xf5,0xe5,0xe9,0x32,0x8d, \
; 0x7e,0x84,0xd4,0x59,0x08,0x97,0xe3,0x48,0x72,0xfd,0xb6,0x82,0x8a,0xe5,0x69,0x25, \
; 0xf7,0x28,0xf9,0xd1,0x49,0xc1,0x41,0x74,0x65,0xb0,0x38,0x9e,0x07,0x6e,0xed,0xfa, \
; 0x35,0x1a,0x2a,0x3e,0xad,0x5c,0x2e,0xb0,0x5d,0xc3,0x56,0x1b,0xd5,0x9e,0xe6,0x37, \
; 0xfd,0x63,0x28,0xc9,0xb1,0x1b,0x22,0x2a,0x76,0x32,0x03,0x5e,0x5b,0x72,0xcf,0xd4, \
; 0x3b,0x63,0x55,0x67,0x8f,0x20,0xce,0xcf,0x87,0xb8,0xc0,0x67,0xe8,0x9f,0x04,0x13, \
; 0xdf,0x15,0x21,0xf4,0xa1,0x4e,0xb5,0xca,0x4e,0xa1,0xcc,0xef,0x6c,0xc0,0x82,0x6c
;	old_entry_pt:
;        .long dq 0x0000000000400470

section .text
	global _start
	global _decrypt

_start:
    mov rax, 0xa
    mov rdi, 0x5555555555555555
    mov rsi, 0x6666666666666666
    mov rdx, 0x3
    syscall
	call _decrypt
    mov rax, 0xa
    mov rdi, 0x5555555555555555
    mov rsi, 0x6666666666666666
    mov rdx, 0x5
    syscall
    mov rax, 0x4444444444444444
	jmp rax

_decrypt:
    push rbp
    mov rbp, rsp
    sub rsp, 0x12c
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
    xor r11, r11
    mov r11, 0x3333333333333333
    mov r10b, [r11 + rcx]
    add QWORD [rsp + 260], r10
    and QWORD [rsp + 260], 255
    xor r10, r10
    mov r10b, BYTE [rsp + 260]
    lea rdi, [rel rsp + rcx]
    lea rsi, [rel rsp + r10]
    inc rcx

_swap:
	xor r10, r10
	mov r10b, BYTE [rdi]
	xor r11, r11
	mov r11b, BYTE [rsi]
	mov BYTE [rdi], r11b
	mov BYTE [rsi], r10b
;	pop rax
	jmp _sorting

_init_decrypt_loop:
	xor r10, r10
	mov r10, QWORD [rsp + 260]
	xor QWORD [rsp + 260], r10
	xor r10, r10
	mov r10, QWORD [rsp + 268]
	xor QWORD [rsp + 268], r10
	xor rcx, rcx

_decrypt_loop:
;    xor r10, r10
    mov r10, 0x2222222222222222
	cmp rcx, r10
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
;	push rel _continue
;	jmp _swap

_swap2:
	xor r10, r10
	mov r10b, BYTE [rdi]
	xor r11, r11
	mov r11b, BYTE [rsi]
	mov BYTE [rdi], r11b
	mov BYTE [rsi], r10b

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
    xor r11, r11
    mov r11, 0x1111111111111111
	xor BYTE [r11 + rcx], r10b
	inc rcx
	jmp _decrypt_loop

_end:
	leave
	ret
