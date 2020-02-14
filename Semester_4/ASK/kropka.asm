; kropka przesuwajaca sie z prawej strony na lewą i z lewej na prawą i tak w kółko
$mod842

org 0

start:
mov A, #4Fh ; F- oznacza że nic się nie wyświetla
w_lewo:
mov R1, #3
petlaL:
mov P2, A
call delay
inc A
djnz R1, petlaL
jmp w_prawo
w_prawo:
mov R1, #3
petlaP:
mov P2, A
call delay
dec A
djnz R1, petlaP
jmp w_lewo


delay:

mov R2, #0fh
d1:	mov R3, #0ffh
d2:	mov R4, #0ffh
djnz R4, $
djnz R3, d2
djnz R2, d1
ret

end