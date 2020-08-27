use16
	cbw
	cdq
	popa
	popaw
	popad
use32
	cbw
	cwde
	pusha
	pushaw
	pushad

	lldt [eax]
	bswap eax
	lss ax,[eax]
	les eax,[ecx]
	cmpxchg8b [bx]
	mov eax,tr0
	mov cr7,eax
	mov dr7,ecx

