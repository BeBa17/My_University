$mod842

org 0


start:
mov R1, #50
obroty:
mov P2, #01h
call delay
mov P2, #02h
call delay
mov P2, #04h
call delay
mov P2, #08h
djnz R1, obroty

delay:
mov R2, #010h
d1:
mov R3, #0ffh
d2:
mov R4, #0ffh
djnz R4, $
djnz R3, d2
djnz R2, d1
ret

end