.model tiny
.code
org 100h

MAX_PASSWORD_SIZE	equ 80h
PASSWORD_SIZE		equ 08h
	
start:	jmp main

include data.asm

PASSWORD_BUFFER	db PASSWORD_SIZE dup ('$'),			0dh, 0ah, '$'
PASSWORD	db 01h, 02h, 04h, 08h, 10h, 20h, 40h, 0d5h,	0dh, 0ah, '$'

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

	mov cx, PASSWORD_SIZE

	mov ax, ds
	mov es, ax

	mov si, offset PASSWORD_BUFFER
	push si
	
	mov bl, 11111111b

	@@next:
		mov al, [si]
		inc si
		xor bl, al
		
	loop @@next

	cmp bl, 01010101b
	jne notEqual

	mov cx, PASSWORD_SIZE	

	pop si
	
	lea si, PASSWORD_BUFFER
	lea di, PASSWORD

	repe cmpsb
	jnz notEqual

	jmp equal


notEqual:
	mov ax, 0900h
	mov dx, offset ACCESS_DENIED
	int 21h
	jmp exit

equal:	mov ax, 0900h
	mov dx, offset ACCESS_GRANTED
	int 21h

	jmp exit


exit:	
	mov ax, 4c00h
	int 21h

	ret
	endp

end	start