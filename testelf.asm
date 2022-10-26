[bits 64]
main:
	mov rdi, 0
	mov rsi, 4
	mov rdx, 'T' 
	int 0x80
	jmp $
	