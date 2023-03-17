[bits 64]
jmp $

%macro push_all 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro pop_all 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

[extern interrupt_handler]

%macro int_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push_all
	mov rdi, %1
	call interrupt_handler
    pop_all
	iretq
%endmacro

%macro eint_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
	pop rsi
    push_all
	mov rdi, %1
	call interrupt_handler
	pop_all
    iretq
%endmacro
[extern clock_handler]
global interrupt_handler_32
interrupt_handler_32:
	push_all
	mov rdi, rsp
	call clock_handler
	mov rsp, rax
    mov al, 0x20
    out 0x20, al
	pop_all
	iretq

int_handler 0
int_handler 4
int_handler 6
eint_handler 8
eint_handler 14
int_handler 33
int_handler 44

[extern syscall_handler]
global interrupt_handler_128
interrupt_handler_128:
    push_all
    call syscall_handler
    pop_all
    iretq