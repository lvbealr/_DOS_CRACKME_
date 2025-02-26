.model tiny
.code
org 100h

MAX_PASSWORD_SIZE	equ 64d
TRUE_PASSWORD_SIZE	equ 08d
	
start:	jmp main

include data.asm

TRUE_PASSWORD	db "maxfimoz",		    	  0dh, 0ah, '$'
PASSWORD_BUFFER	db TRUE_PASSWORD_SIZE dup(' '),   0dh, 0ah, '$'

main	proc
	
	mov ax, 0900h
	mov dx, offset HELLO_MESSAGE
	int 21h

	mov dx, offset ENTER_MESSAGE
	int 21h

	mov ax, 3f00h			
	mov bx, 0000h			
	mov cx, MAX_PASSWORD_SIZE
	mov dx, offset PASSWORD_BUFFER	
	int 21h		

	mov cx, ax

	mov ax, ds
	mov es, ax

	lea si, PASSWORD_BUFFER
	lea di, TRUE_PASSWORD

	repe cmpsb
	jnz notEqual

	jmp equal


equal:	mov ax, 0900h
	mov dx, offset ACCESS_GRANTED
	int 21h

	jmp exit

	
notEqual:
	mov ax, 0900h
	mov dx, offset ACCESS_DENIED
	int 21h	

	jmp exit

exit:	
	mov ax, 4c00h
	int 21h

	ret
	endp

end	start