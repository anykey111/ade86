use32

sub al,10h
sub ax,1fffh
sub eax,1fffffffh

imul sp,bp,-1H
imul eax,ecx,01fffh

test al,-1
test eax,01111h

mov al,1
mov eax,-1

rol eax,020H
shl esp,1

enter 1,2

use16

imul cx,dx,-0111H
imul esi,edi,01eeeeeh

enter 1fffh,-10h

