; wyswietlanie cyfr 0-9 na każdym wyświetlaczu po kolei
$mod842

org 0

start:
mov R1, #10
mov A, #00h
etyk:
mov P2, A
call delay
inc A
djnz R1, etyk
add A, #10h
anl A, #00110000b ; wyświetlacze: 00->01->10->11->00-> ...
jmp etyk


delay:

mov R2, #0fh
d1:
mov R3, #0ffh
d2:
mov R4, #0ffh
djnz R4, $
djnz R3, d2
djnz R2, d1
ret
end