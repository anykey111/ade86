use16

sub al,[bx+si]
sub cx,[bx+di]
sub edx,[bp+si]
sub ch,[bp+di]

sub [si],sp
sub [di],esp
sub [bp],dh
sub [bx],edi

sub ch,[bx+si+01Fh]
sub ax,[bx+di+01Fh]
sub edx,[bp+si-01Fh]
sub bx,[bp+di-01Fh]

sub [si+01234h],bl
mov [di+01234h],ebp
sub [bp-01234h],si
sub [bx-01234h],di

sub al,[+01234h]
sub ax,[-01234h]
sub eax,[0ffffh]

sub bp,ax

mov ax,[0100h]

les ax,[bp]

call near [100h]
call far [200h]
