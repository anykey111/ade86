use16

call 0100h
call 1aaah:0ffffh
jmp  0100h
jmp  1aaah:0ffffh

jz 070h
jge 01fffh
jcxz 70h
jecxz 70h

use32

call 0bbbh
call 1eeeh:0ccccddddh
jmp  0bbbh
jmp  1eeeh:0ccccddddh

jc  070h
jbe 01eeecccch
JMP 1:1

loop 10H
jcxz 20h
jecxz 20H

org  401000h
jmp  400F82h

org  401000h
jmp  401081h

org  401000h
jcxz 401082h

org  401000h
jcxz 400F83h

