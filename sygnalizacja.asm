$mod842

org 0

start:
mov P2, #0Ah ; czerwony czerwony
call delay
mov P2, #82h ; zielony czerwony
call delay
mov R0, #50
miganie:
mov P2, #82h ; zielony czerwony
shortDelay
mov P2, #02h ; - czerwony
shortDelay
djnz R0, miganie
mov P2, #0Ah ; czerwony czerwony
call delay
mov P2, #0Eh ; czerwony żółty+czerwony
call delay
mov P2, #28h ; czerwony zielony
call delay
mov P2, #0Ch ; czerwony żółty
call delay
jmp start

delay:
mov R1, #010h
d1:
mov R2, #0ffh
d2:
mov R3, #0ffh
djnz R3, $
djnz R2, d2
djnz R1 d1
ret

shortDelay:
mov R4, #03h
dd1:
mov R5, #10h
dd2:
mov R6, #10h
djnz R6, $
djnz R5, dd2
djnz R4, dd1
ret


end
