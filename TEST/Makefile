ade = ../ade86.exe
test:	$(ade)
	@echo IMM instructions
	@$(ade) imm.out imm.asm
	@fc /b  imm.bin imm.out

	@echo MODRM 16bit addressig
	@$(ade) m16.out m16.asm 
	@fc /b  m16.bin m16.out

	@echo MODRM 32bit addressig
	@$(ade) m32.out m32.asm
	@fc /b  m32.bin m32.out

	@echo SIB addressig
	@$(ade) sib.out sib.asm
	@fc /b  sib.bin sib.out

	@echo Call/Jmp/Jcc instructions
	@$(ade) jmp.out jmp.asm
	@fc /b  jmp.bin jmp.out

	@echo All other instructions
	@$(ade) all.out all.asm
	@fc /b  all.bin all.out	
		
	@echo Test complete!!!