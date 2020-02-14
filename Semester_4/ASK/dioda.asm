$mod842
led equ P3.4
org 0
jmp start

org 3
clp led ;change diode 1->0 0->1
call delay
reti

start:
mov ie, #81h
jmp $

delay:
mov R1, #010
d1: mov R2, #0ffh
d2:	mov R3, #0ffh
djnz R3, $
djnz R2, d2
djnz R1, d1
ret

end