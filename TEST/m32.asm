use32

sub eax,[eax]
sub ebp,[ebp]

sub [eax+01Fh],eax
sub [ecx+012345678h],ecx

sub [ebp-01Fh],esi
sub [ebp-012345678h],ebp

les esp,[+012345678h]
sub ebp,[-012345678h]

sub esp,ebp

mov eax,[01000h]

shl edi,-1

call near [10000h]
call far  [10000h]

cmovz eax,[ecx]