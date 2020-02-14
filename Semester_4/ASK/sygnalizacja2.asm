$mod842

org 0
jmp start
org 03h
jmp najpierw

start:
setb ex0
setb ea
mov R7, #01
mov P2 #0Ah
jmp $

najpierw:
clr ea
jmp zmien

potem:
setb ea
reti

zmien:
etyk1:
cjne R7, #05, etyk2
mov P2, #0Ah ; czerwony czerwony
mov R7, #01
jmp potem
etyk2:
cjne R7, #01, etyk3
mov P2, #82h ; zielony czerwony
mov R7, #02
jmp potem
etyk3:
cjne R7, #02, etyk4
mov R0, #50
miganie:
mov P2, #82h ; zielony czerwony
shortDelay
mov P2, #02h ; - czerwony
shortDelay
djnz R0, miganie
mov P2, #0Ah ; czerwony czerwony
mov R7, #03
jmp potem
etyk4:
cjne R7, #03, etyk5
mov P2, #0Eh ; czerwony żółty+czerwony
call delay
mov P2, #28h ; czerwony zielony
mov R7, #04
jmp potem
etyk5:
cjne R7, #04, etyk1
mov P2, #0Ch ; czerwony żółty
call delay
mov P2, #0Ah ; czerwony czerwony
mov R7, #05
jmp potem
jmp zmien


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