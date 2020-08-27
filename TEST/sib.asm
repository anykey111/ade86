use32

sub eax,[esp]
sub al,[ecx*2]
sub ax,[ebx*4]
sub eax,[ebp*8]
sub eax,[ebp]

sub ch,[esp+ebp]
sub dx,[ebp+ebp]
sub esi,[eax+ecx]

sub [eax+ebx*1],dh
sub [esi+edi*2],dx
sub [eax+ecx*4],cx
sub [ebp+ecx*8],ecx

sub ax,[esi+edi*2+1Fh]
sub ecx,[esi+edi*8-1Fh]
sub [esp+ebp*4+1FFFFFFh],bh
sub [esp+ebp*4-1FFFFFFh],eax

