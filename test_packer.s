section .text
global	_start

_start:
push	rbp
mov		rbp, rsp

mov		rax, 1
mov		rdi, 1
lea		rsi, [rel msg]
mov		rdx, 14
syscall

mov		rax, 10
mov		rdi, 0x2222222222222222
mov		rsi, 0x3333333333333333
mov		rdx, 7

cmp		rax, -1
je		_printerror
jmp		_printsuccess

mov		rdx, 0x4444444444444444
mov		rcx, 0x5555555555555555
xor		rsi, rsi

_xorloop:
cmp		rsi, rcx
je		_jumpend
xor		[rdx+rsi], byte 120
inc		rsi
jmp		_xorloop

_printerror:
mov		rax, 1
mov		rdi, 1
lea		rsi, [rel err]
mov		rdx, 16
syscall
jmp		_jumpend

_printsuccess:
mov		rax, 1
mov		rdi, 1
lea		rsi, [rel suc]
mov		rdx, 17
syscall
jmp		_jumpend

_jumpend:
xor		rdx, rdx
mov		rax, 0x1111111111111111
pop		rbp
jmp		rax

msg:
db 'Hello World !',10,0

err:
db 'Mprotect failed',10,0

suc:
db 'Mprotect success',10,0
