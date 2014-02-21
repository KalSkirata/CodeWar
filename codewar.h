struct cpu{
	uint16_t registres[8]; /*8 regsitres de 16 bits*/
	uint8_t RAM[256]; /*RAM de 256 octets*/
	int Z,N,C; /*Registres d'états*/
};



void move_general(struct cpu* c,uint16_t mot,uint16_t mot4);
void add(struct cpu* c,uint16_t mot);
void execute(struct cpu* c,uint16_t mot1,uint16_t mot2);
void afficher_cpu(struct cpu* c);
void init_cpu(struct cpu *c);
void move_simple(struct cpu *c,int typeSource,int typeDes,uint16_t valeurR,uint16_t mot1,uint16_t mot2);
void move_l(struct cpu *c,int typeSource,int typeDes,uint16_t valeurR,uint16_t mot1,uint16_t mot2);
void move_h(struct cpu *c,int typeSource,int typeDes,uint16_t valeurR,uint16_t mot1,uint16_t mot2);
void sub(struct cpu* c,uint16_t mot);
void lsl(struct cpu* c,uint16_t mot);
void lsr(struct cpu* c,uint16_t mot);
void and(struct cpu* c,uint16_t mot);
void or(struct cpu* c,uint16_t mot);
void xor(struct cpu* c,uint16_t mot);
void not(struct cpu* c,uint16_t mot);
void cmp(struct cpu* c,uint16_t mot);
void push(struct cpu* c,uint16_t mot);
void pop(struct cpu* c,uint16_t mot);
