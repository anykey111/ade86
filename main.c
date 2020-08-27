#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "ade86.h"

/* Disasssembler variables */
Long pos;			// Position in file
Long maxpos;			// Total file size
Byte usbuf[0x80];		// Buffer for command block
char imbuf[0x80];		// Buffer for instruction mnemonic

/* Assembler wariables */
Long org;			// Current origin
Long bits;			// Bits mode;
Long tsize;			// Total code size
Long nline;			// Current line number
Byte asmem[0x100];		// Memory for assembler
char lnmem[0x100];		// Source code buffer (current line)
int asmode;			// Assemble mode


ASM86* as = (ASM86*)asmem;	// Result structure
DIS86* da = (DIS86*)asmem;

FILE* fin; 		// Input file
FILE* fout;		// Output file


int
die(const char* msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}


int
print_prompt()
{
	// Print command line prompt
	if(asmode==1 && fin==stdin)
		fprintf(stderr,"%08X: ",org);
	// Print file offset
	if(asmode==0)
		fprintf(fout,"%08X> ",pos);
	return 1;
}


void
write_result()
{
	unsigned int n,p;
	if (fin==stdin) {
		if(fout==stdout) {
			fprintf(fout,"%10c",' ');
			for(n=0; n<as->bin_size; n++)
				fprintf(fout,"%02X",as->bin_output[n]);
			fputc('\n',fout);
		} else {
			fprintf(stderr,"%10c",' ');
			for(n=0; n<as->bin_size; n++)
				fprintf(stderr,"%02X",as->bin_output[n]);
			fputc('\n',stderr);
		}
	}
	if (fout!=stdout) {
		n = fwrite(as->bin_output,1,as->bin_size,fout);
		if(n != as->bin_size && ferror(fout))
			die("write_output()");
	}
}

int
process_directive()
{
	char *error=0,*pp=0;
	int p=0,n=0;

	// Pointer to first character
	while(isspace(lnmem[n]) && lnmem[n]!=0) n++;
	// Word length
	if(isalpha(lnmem[n])) {
		while(isalnum(lnmem[n+p])) p++;
	}

	if(p==5 && strnicmp(&lnmem[n],"USE16",p)==0) {
		bits = 16; return 1;
	}

	if(p==5 && strnicmp(&lnmem[n],"USE32",p)==0) {
		bits = 32; return 1;
	}

	if(p==3 && strnicmp(&lnmem[n],"ORG",p)==0) {
		if(strtoul(&lnmem[4],&pp,16)==0 && *pp!=0) {
			error = "Invalid origin value";
			goto return_error;
		}
		org = strtoul(&lnmem[4],&pp,16);
		return 1;
	}

	if(asmode==0 && p==4 && strnicmp(&lnmem[n],"GOTO",p)==0) {
		if(strtoul(&lnmem[4],&pp,16)==0 && *pp!=0) {
			error = "Invalid offset value";
			goto return_error;
		}
		if(maxpos<strtoul(&lnmem[4],&pp,16)) {
			error = "Offset too lage";
			goto return_error;
		}
		pos = strtoul(&lnmem[4],&pp,16);
		return 1;
	}

	if(asmode==0 && p==4 && strnicmp(&lnmem[n],"QUIT",p)==0) {
		fclose(fin);
		exit(EXIT_SUCCESS);
	}
	// No errors
	return 0;

return_error:

	// Continue in interactive mode
	if(fin==stdin || asmode==0) {
		fprintf(stdout,"\t  %s\n",error);
		return 1;
	}

	fprintf(stderr,">%s\n line %d: %s\n",lnmem,nline,error);
	return -1;
}


int
assemble_loop()
{
	Long n;
	while(print_prompt() && fgets(lnmem,sizeof(lnmem),fin) != 0) {
		nline++;
		// Replace '\n' to '\0'
		if(strchr(lnmem,'\n') != NULL)
			*strchr(lnmem,'\n') = '\0';
		// Process directive
		n=process_directive();
		// bad directive usage
		if(n==-1) break;
		// directive parsed
		if(n==1) continue;

		// Stop if void line in interactive mode
		if(fin == stdin && *lnmem=='\0')
			break;

		assemble(asmem,asmem+sizeof(asmem),lnmem,org,bits);
		org += as->bin_size;
		tsize += as->bin_size;

		// error while assembling
		if(as->error != 0) {
			// Continue recv commands from "stdin"
			if(fin == stdin) {
				fprintf(stderr,"\t  %s\n",
					ade86_errors[as->error]);
				continue;
			}
			// Stop in file mode
			fprintf(stderr,">%s\n line %d: %s\n",
				lnmem,nline,ade86_errors[as->error]);
			break;
		}

		write_result();
	}
	
	fprintf(stderr,"Output: %d bytes\n",tsize);
	return as->error;
}


int
unassemble_block(Long block)
{
	Long p,n,i,j,t;
	for(p=n=0; n<8 && p<block; n++)
	{
		unassemble(&usbuf[p],usbuf+sizeof(usbuf),da,org,bits);
		memset(imbuf,0,sizeof(imbuf));
		if(da->error==E86_SUPERFLUOUS_PREFIX) {
			fprintf(fout,"%08X: %02X\n",org,usbuf[p]);
			p++;
			org++;
			pos++;
			continue;
		}
		if(da->error!=0) {
			fprintf(fout,"\t  %s\n",ade86_errors[da->error]);
			break;
		}
		for(t=i=0; i<da->size; i++)
			t+=sprintf(imbuf+t,"%02X",usbuf[p+i]);
		for(j=i; j<16; j++,t+=2)
			imbuf[t]=imbuf[t+1]=' ';
		print_dis86(imbuf+t,imbuf+sizeof(imbuf),da,0);
		fprintf(fout,"%08X: %s\n",org,imbuf);
		p += da->size;
		org += da->size;
		pos += da->size;
	}
	return 0;
}

int
unassemble_loop()
{
	Long n;
	while(print_prompt() && fgets(lnmem,sizeof(lnmem),stdin)!=0) {
		// Replace '\n' to '\0'
		if(strchr(lnmem,'\n') != NULL)
			*strchr(lnmem,'\n') = '\0';
		// Process directive
		n=process_directive();
		// directive parsed
		if(n==1 || n==-1) continue;
		// unassemble block if void line
		if(*lnmem!='\0') {
			fprintf(stderr,
				"\t  Press ENTER to unassemble\n"
				"\t  goto OFFSET  ; move to offset\n"
				"\t  use16        ; switch to 16 bits mode\n"
				"\t  use32        ; switch to 32 bits mode\n"
				"\t  org ADDRESS  ; set current origin\n"
				"\t  quit         ; quit program\n");
			continue;
		}

		fseek(fin,pos,SEEK_SET)==0 || die("fseek");
		// Get file block	
		n = sizeof(usbuf);
		if(pos+sizeof(usbuf) > maxpos)
			n = maxpos-pos;
		if(fread(usbuf,n,1,fin)!=(unsigned)n && ferror(fin))
			die("fread");

		unassemble_block(n);
	}
	return EXIT_SUCCESS;
}


int
main(int argc, char** argv)
{
	org = 0;
	pos = 0;
	bits = 32;
	nline = 0;
	fin = stdin;
	fout = stdout;

	if((--argc)) {
		// Execute disassembler
		if(stricmp(argv[1],"-U")==0) {
			(--argc) || die("No input file");
			fin = fopen(argv[2],"rb");
			fin != NULL || die(argv[2]);
			asmode = 0;
			// Get file size
			fseek(fin,0,SEEK_END);
			maxpos = ftell(fin);
			fseek(fin,pos,SEEK_SET);
			fprintf(stderr,";; Running interactive disassembler.\n");
			fprintf(stderr,";; Press ENTER to unassemble\n");
			return unassemble_loop();
		}
		// Output file
		fout = fopen(argv[1],"wb");
		fout != NULL || die(argv[1]);
		if((--argc)) {
			// Input file
			fin = fopen(argv[2],"r");
			fin != NULL || die(argv[2]);
		}
	}

	if (fin==stdin) {
		fprintf(stderr,";; Running interactive assembler.\n");
		fprintf(stderr,";; Press ENTER on empty line to stop.\n");
	}

	asmode = 1;
	return assemble_loop();
}
