
global _start

;; --- entry --- 
_start:
	mov [args_ptr], rsp
	mov rax, ret_stack_end
	mov [ret_stack_rsp], rax
;; --- literal_i64 --- 
addr_6:
	mov rax, 14
	push rax
;; --- call --- 
	push str_1
;; --- literal_i64 --- 
addr_8:
	mov rax, 1
	push rax
;; --- literal_i64 --- 
addr_9:
	mov rax, 1
	push rax
;; --- sys3 --- 
addr_10:
	pop rax
	pop rdi
	pop rsi
	pop rdx
	syscall
	push rax
;; --- drop --- 
addr_11:
	pop rax
;; --- literal_i64 --- 
addr_12:
	mov rax, 0
	push rax
;; --- literal_i64 --- 
addr_13:
	mov rax, 60
	push rax
;; --- sys1 --- 
addr_14:
	pop rax
	pop rdi
	syscall
	push rax
;; --- drop --- 
addr_15:
	pop rax
;; --- entry-end --- 
	mov rax, 60
	mov rdi, 0
	syscall

section .bss
	args_ptr: resq 1
	ret_stack_rsp: resq 1
	ret_stack: resb 4096

section .bss
	ret_stack_end:
