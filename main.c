#include <stdio.h>
#include <inttypes.h>
#include <stdio.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
#include <unistd.h>
#include "codewar.h"


int main(int agrc,char** argv){
	
	uint16_t mot=0x2;
	uint16_t mot1=0x5904;
	/*int op=0000000000011;*/
	struct cpu c;
	int PC;	
	int fd;	
	
	PC = c.registres[6];	
	fd = open(argv[1],O_RDONLY);
	if(fd==-1) {
		printf("ERREUR");
		init_cpu(&c);
		execute(&c,mot1,mot);
	}
	
	while(fd==1){
		read(fd,&(c.RAM[0x10]),240);
		c.registres[PC]=0x10;
	}
	return 1;
}
