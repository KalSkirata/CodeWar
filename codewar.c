#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include "codewar.h"

#define MOVE 0
#define PUSH 1

void move_general(struct cpu *c,uint16_t mot1,uint16_t mot2){
	/*c->registres[0]=0;
	c->N=0;		
	c->C=0;
	c->Z=1;*/
	int typeMove=0;
	int typeSource=0;
	int typeDes=0;
	uint16_t ins=mot1 & 0x600;	
	int T1 = (mot1 >> 6) & 7;
	int T2 = (mot1 >> 3) & 7;	
	uint16_t valeurR=mot1 & 0x7;

	ins=ins>>9;
	
	if(ins==1){
		printf(" C'est un move.l \n" );
		typeMove=1;
	}else if(ins==2){
		printf(" C'est un move.h \n");
		typeMove=2;
	}else {
		printf(" C'est un move tout simple \n" );
		typeMove=3;
	}
	
	switch(T1){
		case 0: printf("Registre \n"); typeSource=0; break;
		case 1: printf("Registre pré-decrementé \n"); typeSource=1;break;
		case 2: printf("Adressage indirect \n");typeSource=2; break;
		case 3: printf("Registre post-incremente \n"); typeSource=3;break;
		case 4: printf("valeur immediate \n"); typeSource=4;break;
		case 5: printf("Adresse \n"); typeSource=5; break;
	}
	switch(T2){
		case 0: printf("Registre \n"); typeDes=0; break;
		case 1: printf("Registre pré-decrementé \n"); typeDes=1; break;
		case 2: printf("Adressage indirect \n"); typeDes=2;break;
		case 3: printf("Registre post-incremente \n");typeDes=3; break;
		case 4: printf("valeur immediate \n"); typeDes=4;break;
		case 5: printf("Adresse \n"); typeDes=5;break;
	}

	if((typeSource==4 || typeSource==5) && (typeDes==0 || typeDes==1 || typeDes==2 ||typeDes==3  )){
		printf(" Registre de destination %d \n", valeurR);

		/* Move.l */
		if(typeMove==1){
		move_l(c,typeSource,typeDes,valeurR,mot1,mot2);

		c->C=0;
		c->N=((mot1&0xFF)&0x40)>>6;
		if(mot1==0){
			c->Z=1;
		}else{
			c->Z=0;
		}

		/* Move.h */
		}else if(typeMove==2){
			move_h(c,typeSource,typeDes,valeurR,mot1,mot2);

			c->C=0;
			c->N=(((mot1&0xFF00)>>8)&0x40)>>6; /* bits de poids forts et on prend le 7ème */
			if(mot1==0){
				c->Z=1;
			}else{
				c->Z=0;
			}

		/* Move simple */
		}else if(typeMove==3){
			move_simple(c,typeSource,typeDes,valeurR,mot1,mot2);

			c->C=0;
			c->N=(mot1&0x4000)>>14;
			if(mot1==0){
				c->Z=1;
			}else{
				c->Z=0;
			}
		}	
	}else{
		printf(" Registre de source  %d \n", valeurR);
			
		/* Move simple */
		if(typeMove==3){

			/* Move simple / S:Registre / D:Registre */	
			move_simple(c,typeSource,typeDes,valeurR,mot1,mot2);

			c->C=0;
			c->N=(mot1&0x4000)>>14;
			if(mot1==0){
				c->Z=1;
			}else{
				c->Z=0;
			}

		/* Move.h */	  
		}else if(typeMove==2){
			move_h(c,typeSource,typeDes,valeurR,mot1,mot2);		

			c->C=0;
			c->N=(((mot1&0xFF00)>>8)&0x40)>>6; /* bits de poids forts et on prend le 7ème */
			if(mot1==0){
				c->Z=1;
			}else{
				c->Z=0;
			}

		/* Move.l */
		}else if(typeMove==1){
			move_l(c,typeSource,typeDes,valeurR,mot1,mot2);

			c->C=0;
			c->N=((mot1&0xFF)&0x40)>>6;
			if(mot1==0){
				c->Z=1;
			}else{
				c->Z=0;
			}
		}
	}
}

void move_h(struct cpu *c,int typeSource,int typeDes,uint16_t valeurR,uint16_t mot1,uint16_t mot2){
	if(typeSource==4 && typeDes==0){
		uint16_t tmp = mot2 & 0xFF00;
		printf(" affichage tmp \n");
		c->registres[1]=0xde24;
		c->registres[valeurR]=  (c->registres[valeurR] & 0xFF00) + (tmp>>8); 

	/*  Move.h / S:valeur immediate / D:Registre pré-décrementé */
	}else if(typeSource==4 && typeDes==1){
		uint16_t tmp = mot2 & 0xFF00;
		c->registres[valeurR]--;
		printf(" affichage tmp \n");
		c->registres[1]=0xde24;
		c->registres[valeurR]=  (c->registres[valeurR] & 0xFF00) + (tmp>>8);
		c->RAM[c->registres[valeurR]]=mot2;	

	/*  Move.h / S:valeur immediate / D:Adressage indirect */
	}else if(typeSource==4 && typeDes==2){
		uint16_t tmp = mot2 & 0xFF00;
		printf(" affichage tmp \n");
		c->registres[1]=0xde24;
		c->registres[valeurR]=  (c->registres[valeurR] & 0xFF00) + (tmp>>8);
		c->RAM[c->registres[valeurR]]=mot2;	
		
	/*  Move.h / S:valeur immediate / D:Registre post-incrementé */
	}else if(typeSource==4 && typeDes==3){
		uint16_t tmp = mot2 & 0xFF00;
		printf(" affichage tmp \n");
		c->registres[1]=0xde24;
		c->registres[valeurR]=  (c->registres[valeurR] & 0xFF00) + (tmp>>8);
		c->RAM[c->registres[valeurR]]=mot2;	
		c->registres[valeurR]++;

	/*  Move.h / S:Adresse / D:Registre */
	}else if(typeSource==5 && typeDes==0){
		c->registres[valeurR]= c->RAM[mot2 +1] | (c->registres[valeurR] & 0xFF00);

	/*  Move.h / S:Adresse / D:Registre pré-décrémenté*/
	}else if(typeSource==5 && typeDes==1){
		c->registres[valeurR]--;
		c->registres[valeurR]= c->RAM[mot2 +1] | (c->registres[valeurR] & 0xFF00);

	/*  Move.h / S:Adresse / D:Adressage indirect*/
	}else if(typeSource==5 && typeDes==2){
		c->RAM[c->registres[valeurR]]= c->RAM[mot2 +1] | (c->registres[valeurR] & 0xFF00);

	/*  Move.h / S:Adresse / D:Registre post-incrémenté*/
	}else if(typeSource==5 && typeDes==3){
		c->registres[valeurR]= c->RAM[mot2 +1] | (c->registres[valeurR] & 0xFF00);
		c->registres[valeurR]++;
	}

	/* Move.h / S:Registre / D:Registre */
	if((typeSource==0) && (typeDes==0)){
		c->registres[valeurR]=0x2e56;
		c->registres[mot2]=(c->registres[valeurR]>>8) & 0xFF;

	/* Move.h / S:Registre / D:Registre pré-décrémenté */
	}else if((typeSource==0) && (typeDes==1)){
		c->registres[mot2]--;
		c->RAM[c->registres[mot2]]=(c->registres[valeurR]>>8) & 0xFF;
				
	/* Move.h / S:Registre / D:Adressage indirect */
	}else if((typeSource==0) && (typeDes==2)){
		c->RAM[c->registres[mot2]]=(c->registres[valeurR]>>8) & 0xFF;

	/* Move.h / S:Registre / D:Registre post-incrémenté */
	}else if((typeSource==0) && (typeDes==3)){
		c->RAM[c->registres[mot2]]=(c->registres[valeurR]>>8) & 0xFF;
		c->registres[mot2]++;
				
	/* Move.h / S:Registre / D:Adresse */
	}else if((typeSource==0) && (typeDes==5)){
		 *((uint16_t*) & (c->RAM[mot2]))=(c->registres[valeurR]>>8) & 0xFF;
	
	/* Move.h / S:Registre pré-décrémenté / D:Registre */
	}else if((typeSource==1) && (typeDes==0)){
		c->registres[valeurR]-=1;
		c->registres[mot2]=(c->RAM[c->registres[valeurR]]>>8) & 0xFF;
				
	/* Move.h / S:Registre pré-décrémenté / D:Registre pré-décrémenté */
	}else if((typeSource==1) && (typeDes==1)){
		c->registres[valeurR]-=1;
		c->registres[mot2]-=1;
		c->RAM[c->registres[mot2]]=(c->RAM[c->registres[valeurR]]>>8) & 0xFF;

	/* Move.h / S:Registre pré-décrémenté / D:Adressage indirect */
	}else if((typeSource==1) && (typeDes==2)){
		c->registres[valeurR]-=1;
		c->RAM[c->registres[mot2]]=(c->RAM[c->registres[valeurR]]>>8) & 0xFF;

	/* Move.h / S:Registre pré-décrémenté / D:Resgitre post-incrémenté */
	}else if((typeSource==1) && (typeDes==3)){
		c->registres[valeurR]-=1;
		c->RAM[c->registres[mot2]]=(c->RAM[c->registres[valeurR]]>>8) & 0xFF;
		c->registres[mot2]+=1;

	/* Move.h / S:Registre pré-décrémenté / D:Adresse */
	}else if((typeSource==1) && (typeDes==5)){
		 c->registres[valeurR]-=1;
		 *((uint16_t*) & (c->RAM[mot2]))=(c->registres[valeurR]>>8) & 0xFF;

	/* Move.h / S:Adressage indirect / D:Registre */
	}else if((typeSource==2) && (typeDes==0)){		
		c->registres[valeurR]=(c->RAM[c->registres[mot2]]>>8) & 0xFF;

	/* Move.h / S:Registre pré-décrémenté / D:Registre pré-décrémenté */
	}else if((typeSource==2) && (typeDes==1)){
		c->RAM[c->registres[valeurR]]=(c->RAM[c->registres[mot2]]>>8) & 0xFF;

	/* Move.h / S:Registre pré-décrémenté / D:Adressage indirect */
	}else if((typeSource==2) && (typeDes==2)){
		c->RAM[c->registres[valeurR]]=(c->RAM[c->registres[mot2]]>>8) & 0xFF;	

	/* Move.h / S:Registre pré-décrémenté / D:Resgistre post-incrémenté */	
	}else if((typeSource==2) && (typeDes==3)){
		c->RAM[c->registres[valeurR]]=(c->RAM[c->registres[mot2]]>>8) & 0xFF; 	

	/* Move.h / S:Registre pré-décrémenté / D:Adresse */
	}else if((typeSource==2 )&& (typeDes==5)){
		 *((uint16_t*) & (c->RAM[mot2]))=(c->RAM[c->registres[valeurR]]>>8) & 0xFF;
		 
	/* Move.h / S:Registre post-incrémenté / D:Registre */
	}else if((typeSource==3) && (typeDes==0)){
		c->registres[mot2]=(c->RAM[c->registres[valeurR]]>>8) & 0xFF;
		c->registres[valeurR]+=1;

	/* Move.h / S:Registre post-incrémenté / D:Registre pré-décrémenté */
	}else if((typeSource==3) && (typeDes==1)){
		c->registres[mot2]-=1;
		c->RAM[c->registres[mot2]]=(c->RAM[c->registres[valeurR]]>>8) & 0xFF;
		c->registres[valeurR]+=1;
	
	/* Move.h/ S:Registre post-incrémenté / D:Adressage indirect */
	}else if((typeSource==3) && (typeDes==2)){
		c->RAM[c->registres[mot2]]=(c->RAM[c->registres[valeurR]]>>8) & 0xFF;
		c->registres[valeurR]+=1;

	/* Move.h / S:Registre post-incrémenté / D:Registre post-incrémenté */
	}else if((typeSource==3) && (typeDes==3)){	
		c->RAM[c->registres[mot2]]=(c->RAM[c->registres[valeurR]]>>8) & 0xFF;
		c->registres[mot2]+=1;
		c->registres[valeurR]+=1;

	/* Move.h / S:Registre post-incrémenté / D:Adresse */
	}else if((typeSource==3) && (typeDes==5)){
		 *((uint16_t*) & (c->RAM[mot2]))=(c->registres[valeurR]>>8) & 0xFF;	 
		  c->registres[valeurR]+=1;
	}

	/* Gestion des flags */
	c->C=0;
	c->N=(((mot1&0xFF00)>>8)&0x40)>>6; /* bits de poids forts et on prend le 7ème */
	if(mot1==0){
		c->Z=1;
	}else{
		c->Z=0;
	}
}

void move_l(struct cpu *c,int typeSource,int typeDes,uint16_t valeurR,uint16_t mot1,uint16_t mot2){

	/*  Move.l / S:valeur immediate / D:registre */
	if(typeSource==4 && typeDes==0){
		uint16_t tmp = mot2 & 0xFF;
		printf(" affichage tmp \n");
		c->registres[1]=0xde24;
		c->registres[valeurR]=  (c->registres[valeurR] & 0xFF) + (tmp<<8);

	/*  Move.l / S:valeur immediate / D:registre pré-décrémenté */
	}else if(typeSource==4 && typeDes==1){
		uint16_t tmp = mot2 & 0xFF;
		c->registres[valeurR]--;
		printf(" affichage tmp \n");
		c->registres[1]=0xde24;
		c->registres[valeurR]=  (c->registres[valeurR] & 0xFF) + (tmp<<8);
		c->RAM[c->registres[valeurR]]=mot2;	

	/*  Move.l / S:valeur immediate / D:adressage indirect */
	}else if(typeSource==4 && typeDes==2){
		uint16_t tmp = mot2 & 0xFF;
		printf(" affichage tmp \n");
		c->registres[1]=0xde24;
		c->registres[valeurR]=  (c->registres[valeurR] & 0xFF) + (tmp<<8);
		c->RAM[c->registres[valeurR]]=mot2;	

	/*  Move.l / S:valeur immediate / D:Registre post-incremente */
	}else if(typeSource==4 && typeDes==3){
		uint16_t tmp = mot2 & 0xFF;
		printf(" affichage tmp \n");
		c->registres[1]=0xde24;
		c->registres[valeurR]=  (c->registres[valeurR] & 0xFF) + (tmp<<8);
		c->RAM[c->registres[valeurR]]=mot2;	
		c->registres[valeurR]++;

	/*  Move.l / S:Adresse / D:Registre */
	}else if(typeSource==5 && typeDes==0){
		c->registres[valeurR]= c->RAM[mot2] | (c->registres[valeurR] & 0xFF00);

	/*  Move.l / S:Adresse / D:Registre pré-décremente */
	}else if(typeSource==5 && typeDes==1){
		c->registres[valeurR]--;
		c->registres[valeurR]= c->RAM[mot2] | (c->registres[valeurR] & 0xFF00);

	/*  Move.l / S:Adresse / D:Adresse indirect */
	}else if(typeSource==5 && typeDes==2){
		c->RAM[c->registres[valeurR]]= c->RAM[mot2] | (c->registres[valeurR] & 0xFF00);

	/*  Move.l / S:Adresse / D:Registre post-incremente */
	}else if(typeSource==5 && typeDes==3){
		c->registres[valeurR]= c->RAM[mot2] | (c->registres[valeurR] & 0xFF00);
		c->registres[valeurR]++;
	}

	/* Move.l / S:Registre / D:Registre */
	if((typeSource==0) && (typeDes==0)){
		c->registres[mot2]=(c->registres[valeurR]) & 0xFF;
		
	/* Move.l / S:Registre / D:Registre pré-décrémenté */
	}else if((typeSource==0) && (typeDes==1)){
		c->registres[mot2]--;
		c->RAM[c->registres[mot2]]=(c->registres[valeurR]) & 0xFF;
				
	/* Move.l / S:Registre / D:Adressage indirect */
	}else if((typeSource==0) && (typeDes==2)){
		c->registres[valeurR]=0x2e56;
		c->registres[mot2]=56;
		c->RAM[c->registres[mot2]]=(c->registres[valeurR]) & 0xFF;
		afficher_cpu(c);

	/* Move.l / S:Registre / D:Registre post-incrémenté */
	}else if((typeSource==0) && (typeDes==3)){
		c->RAM[c->registres[mot2]]=(c->registres[valeurR]) & 0xFF;
		c->registres[mot2]+=1;

	/* Move.l / S:Registre / D:Adresse */
	}else if((typeSource==0) && (typeDes==5)){
		 *((uint16_t*) & (c->RAM[mot2]))=(c->registres[valeurR]) & 0xFF;

	/* Move.l / S:Registre pré-décrémenté / D:Registre */
	}else if((typeSource==1) && (typeDes==0)){
		c->registres[valeurR]-=1;
		c->registres[mot2]=(c->RAM[c->registres[valeurR]]) & 0xFF;
				
	/* Move.l / S:Registre pré-décrémenté / D:Registre pré-décrémenté */
	}else if((typeSource==1) && (typeDes==1)){
		c->registres[valeurR]-=1;
		c->registres[mot2]-=1;
		c->RAM[c->registres[mot2]]=(c->RAM[c->registres[valeurR]]) & 0xFF;

	/* Move.l / S:Registre pré-décrémenté / D:Adressage indirect */
	}else if((typeSource==1) && (typeDes==2)){
		c->registres[valeurR]-=1;
		c->RAM[c->registres[mot2]]=(c->RAM[c->registres[valeurR]]) & 0xFF;
			
	/* Move.l / S:Registre pré-décrémenté / D:Registre post-incrémenté */
	}else if((typeSource==1) && (typeDes==3)){
		c->registres[valeurR]-=1;
		c->RAM[c->registres[mot2]]=(c->RAM[c->registres[valeurR]]) & 0xFF;
		c->registres[mot2]+=1;

	/* Move.l / S:Registre pré-décrémenté / D:Adresse */
	}else if((typeSource==1) && (typeDes==5)){
		 c->registres[valeurR]-=1;
		 *((uint16_t*) & (c->RAM[mot2]))=(c->registres[valeurR]) & 0xFF;

	/* Move.l / S:Adressage indirect / D:Registre */
	}else if((typeSource==2) && (typeDes==0)){
	c->registres[valeurR]=(c->RAM[c->registres[mot2]]) & 0xFF;

	/* Move.l / S:Adressage indirect / D:Registre pré-décrémenté */
	}else if((typeSource==2) && (typeDes==1)){
		c->registres[mot2]-=1;
		c->registres[valeurR]=(c->RAM[c->registres[mot2]]) & 0xFF;

	/* Move.l / S:Adressage indirect / D:Adressage indirect */
	}else if((typeSource==2) && (typeDes==2)){
		c->RAM[c->registres[valeurR]]=(c->RAM[c->registres[mot2]]) & 0xFF;	

	/* Move.l / S:Adressage indirect / D:Registre post-incrémenté */	
	}else if((typeSource==2) && (typeDes==3)){
		c->registres[valeurR]=(c->RAM[c->registres[mot2]]) & 0xFF; 	
		c->registres[mot2]+=1;

	/* Move.l / S:Adressage indirect / D:Adresse */
	}else if((typeSource==2 )&& (typeDes==5)){
		 *((uint16_t*) & (c->RAM[mot2]))=(c->RAM[c->registres[valeurR]]) & 0xFF;

	/* Move.l / S:Registre post-incrémenté / D:Registre */
	}else if((typeSource==3) && (typeDes==0)){
		c->registres[mot2]=(c->RAM[c->registres[valeurR]]) & 0xFF;
		c->registres[valeurR]+=1;

	/* Move.l / S:Registre post-incrémenté / D:Registre pré-décrémenté */
	}else if((typeSource==3) && (typeDes==1)){
		c->registres[mot2]-=1;
		c->RAM[c->registres[mot2]]=(c->RAM[c->registres[valeurR]]) & 0xFF;
		c->registres[valeurR]+=1;

	/* Move.l / S:Registre post-incrémenté / D:Adressage indirect */
	}else if((typeSource==3) && (typeDes==2)){
		c->RAM[c->registres[mot2]]=(c->RAM[c->registres[valeurR]]) & 0xFF;
		c->registres[valeurR]+=1;

	/* Move.l / S:Registre post-incrémenté / D:Registre post-incrémenté */
	}else if((typeSource==3) && (typeDes==3)){	
		c->RAM[c->registres[mot2]]=(c->RAM[c->registres[valeurR]]) & 0xFF;
		c->registres[mot2]+=1;
		c->registres[valeurR]+=1;


	/* Move.l / S:Registre post-incrémenté / D:Adresse */
	}else if((typeSource==3) && (typeDes==5)){
		 *((uint16_t*) & (c->RAM[mot2]))=(c->registres[valeurR]) & 0xFF;	 
		  c->registres[valeurR]+=1;
	}

	/* Gestion des flags */
	c->C=0;
	c->N=((mot1&0xFF)&0x40)>>6;
	if(mot1==0){
		c->Z=1;
	}else{
		c->Z=0;
	}
}

void move_simple(struct cpu *c,int typeSource,int typeDes,uint16_t valeurR,uint16_t mot1,uint16_t mot2){

	/*  Move simple avec valeur immediate comme source et registre comme destination */
	if(typeSource==4 && typeDes==0){
		c->registres[valeurR]=mot2;
	
	/*  Move simple / S:valeur immediate / D:registre pré-decrementé */
	} else if(typeSource==4 && typeDes==1){
		c->registres[valeurR]-=2;
		c->RAM[c->registres[valeurR]]=mot2;

	/*  Move simple / S:valeur immediate / D:adressage indirect */
	}else if(typeSource==4 && typeDes==2){
		c->RAM[c->registres[valeurR]]=mot2;

	/*  Move simple / S:valeur immediate / D:registre post-décrementé */
	}else if(typeSource==4 && typeDes==3){
		c->RAM[c->registres[valeurR]]=mot2;
		c->registres[valeurR]+=2;

	/*  Move simple / S:Adresse / D:Registre*/
	}else if(typeSource==5 && typeDes==0){
		c->registres[valeurR]= *((uint16_t*) & (c->RAM[mot2]));

	/*  Move simple / S:Adresse / D:Registre pré-décrémenté*/
	}else if(typeSource==5 && typeDes==1){
		c->registres[valeurR]-=2;
		c->registres[valeurR]= *((uint16_t*) & (c->RAM[mot2]));

	/*  Move simple / S:Adresse / D:Adressage indirect*/
	}else if(typeSource==5 && typeDes==2){
	c->RAM[c->registres[valeurR]]= *((uint16_t*) & (c->RAM[mot2]));

	/*  Move simple / S:Adresse / D:Registre post-incrémenté*/
	}else if(typeSource==5 && typeDes==3){
		c->registres[valeurR]= *((uint16_t*) & (c->RAM[mot2]));
		c->registres[valeurR]+=2;
	}	

	/* Move simple / S:Registre / D:Registre */
	if((typeSource==0) && (typeDes==0)){
		c->registres[mot2]=c->registres[valeurR];
			
	/* Move simple / S:Registre / D:Regsitre pré-décrémenté */
	}else if((typeSource==0) && (typeDes==1)){
		c->registres[mot2]--;
		c->RAM[c->registres[mot2]]=c->registres[valeurR];

	/* Move simple / S:Registre / D:Adressage indirect */
	}else if((typeSource==0) && (typeDes==2)){
		c->RAM[c->registres[mot2]]=c->registres[valeurR];

	/* Move simple / S:Registre / D:Regsitre post-décrémenté */
	}else if((typeSource==0) && (typeDes==3)){
		c->RAM[c->registres[mot2]]=c->registres[valeurR];
		c->registres[mot2]+=2;

	/* Move simple / S:Registre / D:Adresse */
	}else if((typeSource==0) && (typeDes==5)){
		 *((uint16_t*) & (c->RAM[mot2]))=c->registres[valeurR];


	/* Move simple / S:Registre pré-décrémenté / D:Registre */
	}else if((typeSource==1) && (typeDes==0)){
		c->registres[valeurR]-=2;
		c->registres[mot2]=c->RAM[c->registres[valeurR]];
				
	/* Move simple / S:Registre pré-décrémenté / D:Registre pré-décrémenté */
	}else if((typeSource==1) && (typeDes==1)){
		c->registres[valeurR]-=2;
		c->registres[mot2]-=2;
		c->RAM[c->registres[mot2]]=c->RAM[c->registres[valeurR]];

	/* Move simple / S:Registre pré-décrémenté / D:Adressage indirect */
	}else if((typeSource==1) && (typeDes==2)){
		c->registres[valeurR]-=2;
		c->RAM[c->registres[mot2]]=c->RAM[c->registres[valeurR]];

	/* Move simple / S:Registre pré-décrémenté / D:Registre post-incrémenté */
	}else if((typeSource==1) && (typeDes==3)){
		c->registres[valeurR]-=2;
		c->RAM[c->registres[mot2]]=c->RAM[c->registres[valeurR]];
		c->registres[mot2]+=2;

	/* Move simple / S:Registre pré-décrémenté / D:Adresse */
	}else if((typeSource==1) && (typeDes==5)){
		 c->registres[valeurR]-=2;
		 *((uint16_t*) & (c->RAM[mot2]))=c->registres[valeurR];

	/* Move simple / S:Adressage indirect / D:Registre */
	}else if((typeSource==2) && (typeDes==0)){
		c->registres[valeurR]=c->RAM[c->registres[mot2]];

	/* Move simple / S:Adressage indirect / D:Registre pré-décrémenté */
	}else if((typeSource==2) && (typeDes==1)){
		c->registres[mot2]-=2;
		c->RAM[c->registres[mot2]] = c->RAM[c->registres[valeurR]];


	/* Move simple / S:Adressage indirect / D:Adressage indirect */
	}else if((typeSource==2) && (typeDes==2)){
		c->RAM[c->registres[mot2]] = c->RAM[c->registres[valeurR]];


	/* Move simple / S:Adressage indirect / D:Registre post-incrémenté */	
	}else if((typeSource==2) && (typeDes==3)){
		c->RAM[c->registres[mot2]] = c->RAM[c->registres[valeurR]];
		c->registres[mot2]+=2;

	/* Move simple / S:Adressage indirect / D:Adresse */
	}else if((typeSource==2 )&& (typeDes==5)){
		 *((uint16_t*) & (c->RAM[mot2]))=c->RAM[c->registres[valeurR]];

	/* Move simple / S:Registre post-incrémenté / D:Registre */
	}else if((typeSource==3) && (typeDes==0)){
		c->registres[mot2]=c->RAM[c->registres[valeurR]];
		c->registres[valeurR]+=2;

	/* Move simple / S:Registre post-incrémenté / D:Registre pré-décrémenté */
	}else if((typeSource==3) && (typeDes==1)){
		c->registres[mot2]-=2;
		c->RAM[c->registres[mot2]] = c->RAM[c->registres[valeurR]];
		c->registres[valeurR]+=2;

	/* Move simple / S:Registre post-incrémenté / D:Adressage indirect */
	}else if((typeSource==3) && (typeDes==2)){
		c->RAM[c->registres[mot2]] = c->RAM[c->registres[valeurR]];
		c->registres[valeurR]+=2;

	/* Move simple / S:Registre post-incrémenté / D:Registre post-incrémenté */
	}else if((typeSource==3) && (typeDes==3)){	
		c->RAM[c->registres[mot2]]=c->RAM[c->registres[valeurR]];
		c->registres[mot2]+=2;
		c->registres[valeurR]+=2;

	/* Move simple / S:Registre post-incrémenté / D:Adresse */
	}else if((typeSource==3) && (typeDes==5)){
		 *((uint16_t*) & (c->RAM[mot2]))=c->registres[valeurR];		 
		  c->registres[valeurR]+=2;
	}

	/* Gestion des flags */
	c->C=0;
	c->N=(mot1&0x4000)>>14;
	if(mot1==0){
		c->Z=1;
	}else{
		c->Z=0;
	}
}

void add(struct cpu* c,uint16_t mot){

	int typeSource=0;
	int T1 = (mot >> 8) & 7;
	int T2 = (mot >> 5) & 7;
	uint16_t valeurR=mot & 0x1F;
	printf("%d\n", T1);

	switch(T2){
		case 0: printf("Registre \n"); typeSource=0; break;
		case 1: printf("Registre pré-decrementé \n"); typeSource=1;break;
		case 2: printf("Adressage indirect \n");typeSource=2; break;
		case 3: printf("Registre post-incremente \n"); typeSource=3;break;
		case 4: printf("valeur immediate \n"); typeSource=4;break;
		case 5: printf("Adresse \n"); typeSource=5; break;
	}
	

	if(typeSource==4){
		c->registres[T1]=0x36;
		c->registres[T1]=c->registres[T1] + valeurR ;
	}else if (typeSource==0){
		c->registres[T1]=c->registres[T1] + c->registres[valeurR];
	}else if(typeSource==1){
		c->registres[valeurR]-=2;
		c->registres[T1]=c->registres[T1] + c->RAM[c->registres[valeurR]];
	}else if(typeSource==2){
		c->registres[T1]=c->registres[T1] + c->RAM[c->registres[valeurR]];
	}else if(typeSource==3){
		c->registres[T1]=c->registres[T1] + c->RAM[c->registres[valeurR]];
		c->registres[valeurR]+=2;	
	}

	/* Gestion des flags */
	c->N=((c->registres[T1])&0x2)>>1;
	if(c->registres[T1]==0){
		c->Z=1;
	}else{
		c->Z=0;
	}
	if(c->registres[T1]>0xFFFF){
		c->C=1;
	}else{
		c->C=0;
	}
}

void cmp(struct cpu* c,uint16_t mot){

	int typeSource=0;
	uint16_t RES=0;
	int T1 = (mot >> 8) & 7;
	int T2 = (mot >> 5) & 7;	
	uint16_t valeurR=mot & 0x1F;

	switch(T2){
		case 0: printf("Registre \n"); typeSource=0; break;
		case 1: printf("Registre pré-decrementé \n"); typeSource=1;break;
		case 2: printf("Adressage indirect \n");typeSource=2; break;
		case 3: printf("Registre post-incremente \n"); typeSource=3;break;
		case 4: printf("valeur immediate \n"); typeSource=4;break;
		case 5: printf("Adresse \n"); typeSource=5; break;
	}

	if(typeSource==4){
		c->registres[T1]=0x36;
		RES=c->registres[T1] - valeurR ;
		printf("%d\n",RES );
	}else if (typeSource==0){
		RES=c->registres[T1] - c->registres[valeurR];
		printf("%d\n",RES );
	}else if(typeSource==1){
		c->registres[valeurR]-=2;
		RES=c->registres[T1] - c->RAM[c->registres[valeurR]];
		printf("%d\n",RES );
	}else if(typeSource==2){
		RES=c->registres[T1] - c->RAM[c->registres[valeurR]];
		printf("%d\n",RES );
	}else if(typeSource==3){
		RES=c->registres[T1] - c->RAM[c->registres[valeurR]];
		c->registres[valeurR]+=2;	
		printf("%d\n",RES );
	}

	/* Gestion des flags */
	c->N=((c->registres[T1])&0x2)>>1;
	if(c->registres[T1]==0){
		c->Z=1;
	}else{
		c->Z=0;
	}
	if(c->registres[T1]<0){
		c->C=1;
	}else{
		c->C=0;
	}
}

void sub(struct cpu* c,uint16_t mot){

	int typeSource=0;
	int T1 = (mot >> 8) & 7;
	int T2 = (mot >> 5) & 7;	
	uint16_t valeurR=mot & 0x1F;

	switch(T2){
		case 0: printf("Registre \n"); typeSource=0; break;
		case 1: printf("Registre pré-decrementé \n"); typeSource=1;break;
		case 2: printf("Adressage indirect \n");typeSource=2; break;
		case 3: printf("Registre post-incremente \n"); typeSource=3;break;
		case 4: printf("valeur immediate \n"); typeSource=4;break;
		case 5: printf("Adresse \n"); typeSource=5; break;
	}

	if(typeSource==4){
		c->registres[T1]=0x36;
		c->registres[T1]=c->registres[T1] - valeurR ;
	}else if (typeSource==0){
		c->registres[T1]=c->registres[T1] - c->registres[valeurR];
	}else if(typeSource==1){
		c->registres[valeurR]-=2;
		c->registres[T1]=c->registres[T1] - c->RAM[c->registres[valeurR]];
	}else if(typeSource==2){
		c->registres[T1]=c->registres[T1] - c->RAM[c->registres[valeurR]];
	}else if(typeSource==3){
		c->registres[T1]=c->registres[T1] - c->RAM[c->registres[valeurR]];
		c->registres[valeurR]+=2;	
	}

	/* Gestion des flags */
	c->N=((c->registres[T1])&0x2)>>1;
	if(c->registres[T1]==0){
		c->Z=1;
	}else{
		c->Z=0;
	}
	if(c->registres[T1]<0){
		c->C=1;
	}else{
		c->C=0;
	}
}

void lsl(struct cpu* c,uint16_t mot){

	int typeSource=0;
	int T1 = (mot >> 8) & 7;
	int T2 = (mot >> 5) & 7;	
	uint16_t valeurR=mot & 0x1F;
	int dernierbitdecale = (c->registres[T1])&0x1;

	switch(T2){
		case 0: printf("Registre \n"); typeSource=0; break;
		case 1: printf("Registre pré-decrementé \n"); typeSource=1;break;
		case 2: printf("Adressage indirect \n");typeSource=2; break;
		case 3: printf("Registre post-incremente \n"); typeSource=3;break;
		case 4: printf("valeur immediate \n"); typeSource=4;break;
		case 5: printf("Adresse \n"); typeSource=5; break;
	}

	if(typeSource==4){
		c->registres[T1]=0x4;
		c->registres[T1]=c->registres[T1]<<(valeurR);
	}else if (typeSource==0){
		c->registres[T1]=c->registres[T1]<< (c->registres[valeurR]);
	}else if(typeSource==1){
		c->registres[valeurR]-=2;
		c->registres[T1]=c->registres[T1] << (c->RAM[c->registres[valeurR]]);
	}else if(typeSource==2){
		c->registres[T1]=c->registres[T1] << (c->RAM[c->registres[valeurR]]);
	}else if(typeSource==3){
		c->registres[T1]=c->registres[T1] << (c->RAM[c->registres[valeurR]]);
		c->registres[valeurR]+=2;	
	}

	/* Gestion des flags */
	c->N=((c->registres[T1])&0x2)>>1;
	if(c->registres[T1]==0){
		c->Z=1;
	}else{
		c->Z=0;
	}
	c->C=dernierbitdecale;
}

void lsr(struct cpu* c,uint16_t mot){

	int typeSource=0;
	int T1 = (mot >> 8) & 7;
	int T2 = (mot >> 5) & 7;	
	uint16_t valeurR=mot & 0x1F;
	int dernierbitdecale = ((c->registres[T1])&0x10000>>15);

	switch(T2){
		case 0: printf("Registre \n"); typeSource=0; break;
		case 1: printf("Registre pré-decrementé \n"); typeSource=1;break;
		case 2: printf("Adressage indirect \n");typeSource=2; break;
		case 3: printf("Registre post-incremente \n"); typeSource=3;break;
		case 4: printf("valeur immediate \n"); typeSource=4;break;
		case 5: printf("Adresse \n"); typeSource=5; break;
	}

	if(typeSource==4){
		c->registres[T1]=0x400;
		c->registres[T1]=c->registres[T1]>>(valeurR );
	}else if (typeSource==0){
		c->registres[T1]=c->registres[T1]>> (c->registres[valeurR]);
	}else if(typeSource==1){
		c->registres[valeurR]-=2;
		c->registres[T1]=c->registres[T1] >> (c->RAM[c->registres[valeurR]]);
	}else if(typeSource==2){
		c->registres[T1]=c->registres[T1] >> (c->RAM[c->registres[valeurR]]);
	}else if(typeSource==3){
		c->registres[T1]=c->registres[T1] >> (c->RAM[c->registres[valeurR]]);
		c->registres[valeurR]+=2;	
	}
	
	/* Gestion des flags */
	c->N=((c->registres[T1])&0x2)>>1;
	if(c->registres[T1]==0){
		c->Z=1;
	}else{
		c->Z=0;
	}
	c->C=dernierbitdecale;
}

void and(struct cpu* c,uint16_t mot){

	int typeSource=0;
	int T1 = (mot >> 8) & 7;
	int T2 = (mot >> 5) & 7;	
	uint16_t valeurR=mot & 0x1F;

	switch(T2){
		case 0: printf("Registre \n"); typeSource=0; break;
		case 1: printf("Registre pré-decrementé \n"); typeSource=1;break;
		case 2: printf("Adressage indirect \n");typeSource=2; break;
		case 3: printf("Registre post-incremente \n"); typeSource=3;break;
		case 4: printf("valeur immediate \n"); typeSource=4;break;
		case 5: printf("Adresse \n"); typeSource=5; break;
	}

	if(typeSource==4){
		c->registres[T1]=0xc;
		c->registres[T1]=c->registres[T1] & (valeurR );
		afficher_cpu(c);
	}else if (typeSource==0){
		c->registres[T1]=c->registres[T1]& (c->registres[valeurR]);
	}else if(typeSource==1){
		c->registres[valeurR]-=2;
		c->registres[T1]=c->registres[T1] & (c->RAM[c->registres[valeurR]]);
	}else if(typeSource==2){
		c->registres[T1]=c->registres[T1] & (c->RAM[c->registres[valeurR]]);
	}else if(typeSource==3){
		c->registres[T1]=c->registres[T1] & (c->RAM[c->registres[valeurR]]);
		c->registres[valeurR]+=2;	
	}

	/* Gestion des flags */
	c->N=((c->registres[T1])&0x2)>>1;
	if(c->registres[T1]==0){
		c->Z=1;
	}else{
		c->Z=0;
	}
	c->C=0;
}

void or(struct cpu* c,uint16_t mot){

	int typeSource=0;
	int T1 = (mot >> 8) & 7;
	int T2 = (mot >> 5) & 7;	
	uint16_t valeurR=mot & 0x1F;

	switch(T2){
		case 0: printf("Registre \n"); typeSource=0; break;
		case 1: printf("Registre pré-decrementé \n"); typeSource=1;break;
		case 2: printf("Adressage indirect \n");typeSource=2; break;
		case 3: printf("Registre post-incremente \n"); typeSource=3;break;
		case 4: printf("valeur immediate \n"); typeSource=4;break;
		case 5: printf("Adresse \n"); typeSource=5; break;
	}

	if(typeSource==4){
		c->registres[T1]=0xc;
		c->registres[T1]=c->registres[T1] | (valeurR );
		afficher_cpu(c);
	}else if (typeSource==0){
		c->registres[T1]=c->registres[T1]| (c->registres[valeurR]);
	}else if(typeSource==1){
		c->registres[valeurR]-=2;
		c->registres[T1]=c->registres[T1] |(c->RAM[c->registres[valeurR]]);
	}else if(typeSource==2){
		c->registres[T1]=c->registres[T1] | (c->RAM[c->registres[valeurR]]);
	}else if(typeSource==3){
		c->registres[T1]=c->registres[T1] | (c->RAM[c->registres[valeurR]]);
		c->registres[valeurR]+=2;	
	}

	/* Gestion des flags */
	c->N=((c->registres[T1])&0x2)>>1;
	if(c->registres[T1]==0){
		c->Z=1;
	}else{
		c->Z=0;
	}
	c->C=0;
}

void xor(struct cpu* c,uint16_t mot){

	int typeSource=0;
	int T1 = (mot >> 8) & 7;
	int T2 = (mot >> 5) & 7;	
	uint16_t valeurR=mot & 0x1F;

	switch(T2){
		case 0: printf("Registre \n"); typeSource=0; break;
		case 1: printf("Registre pré-decrementé \n"); typeSource=1;break;
		case 2: printf("Adressage indirect \n");typeSource=2; break;
		case 3: printf("Registre post-incremente \n"); typeSource=3;break;
		case 4: printf("valeur immediate \n"); typeSource=4;break;
		case 5: printf("Adresse \n"); typeSource=5; break;
	}

	if(typeSource==4){
		c->registres[T1]=0xc;
		c->registres[T1]=c->registres[T1] ^ (valeurR );
	}else if (typeSource==0){
		c->registres[T1]=c->registres[T1]^ (c->registres[valeurR]);
	}else if(typeSource==1){
		c->registres[valeurR]-=2;
		c->registres[T1]=c->registres[T1] ^(c->RAM[c->registres[valeurR]]);
	}else if(typeSource==2){
		c->registres[T1]=c->registres[T1] ^ (c->RAM[c->registres[valeurR]]);
	}else if(typeSource==3){
		c->RAM[c->registres[valeurR]]=0x4;
		c->registres[T1]=0xc;
		c->registres[T1]=c->registres[T1] ^(c->RAM[c->registres[valeurR]]);
		c->registres[valeurR]+=2;	
		afficher_cpu(c);
	}

	/* Gestion des flags */
	c->N=((c->registres[T1])&0x2)>>1;
	if(c->registres[T1]==0){
		c->Z=1;
	}else{
		c->Z=0;
	}
	c->C=0;
}

void not(struct cpu* c,uint16_t mot){

	int typeSource=0;
	int T2 = (mot >> 5) & 7;	
	uint16_t valeurR=mot & 0x1F;

	switch(T2){
		case 0: printf("Registre \n"); typeSource=0; break;
		case 1: printf("Registre pré-decrementé \n"); typeSource=1;break;
		case 2: printf("Adressage indirect \n");typeSource=2; break;
		case 3: printf("Registre post-incremente \n"); typeSource=3;break;
		case 4: printf("valeur immediate \n"); typeSource=4;break;
		case 5: printf("Adresse \n"); typeSource=5; break;
	}

	if (typeSource==0){
		c->registres[valeurR]=0xc;
		c->registres[valeurR]= ~(c->registres[valeurR]);
		afficher_cpu(c);
	}else if(typeSource==1){
		c->registres[valeurR]-=2;
		c->RAM[c->registres[valeurR]]= ~(c->RAM[c->registres[valeurR]]);
	}else if(typeSource==2){
		c->RAM[c->registres[valeurR]]=~ (c->RAM[c->registres[valeurR]]);
	}else if(typeSource==3){
		c->RAM[c->registres[valeurR]]= ~(c->RAM[c->registres[valeurR]]);
		c->registres[valeurR]+=2;
	}

	/* Gestion des flags */
	c->N=((c->registres[T2])&0x2)>>1;
	if(c->registres[T2]==0){
		c->Z=1;
	}else{
		c->Z=0;
	}
	c->C=0;
}

void push(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;
		case 5: printf("Adresse \n");break;
	}

	/* Move simple / S:Registre */
	if(typeOperande == 0){
		c->RAM[c->registres[7]]=c->registres[valeurOperande];
	
	/* Move simple / S:Registre pré-décrémenté */
	}else if(typeOperande == 1){
		c->registres[valeurOperande]--;
		c->RAM[c->registres[7]]=c->registres[valeurOperande];

	/* Move simple / S:Adressage indirect */
	}else if(typeOperande == 2){
		c->RAM[c->registres[7]]=c->RAM[c->registres[valeurOperande]];
	
	/* Move simple / S:Registre post-incrémenté */
	}else if(typeOperande == 3){
		c->RAM[c->registres[7]]=c->registres[valeurOperande];
		c->registres[valeurOperande]++;
	}

	/* Gestion des flags */
	c->C=0;
	c->N=(mot1&0x4000)>>14;
	if(mot1==0){
		c->Z=1;
	}else{
		c->Z=0;
	}
}

void pop(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;
		case 5: printf("Adresse \n");break;
	}

	/* Move simple / S:Registre */
	if(typeOperande == 0){
		c->registres[valeurOperande]=c->RAM[c->registres[7]];
	

	/* Move simple / S:Registre pré-décrémenté */
	}else if(typeOperande == 1){
		c->registres[valeurOperande]--;
		c->registres[valeurOperande]=c->RAM[c->registres[7]];
	

	/* Move simple / S:Adressage indirect */
	}else if(typeOperande == 2){
		c->RAM[c->registres[valeurOperande]]=c->RAM[c->registres[7]];
	
	
	/* Move simple / S:Registre post-incrémenté */
	}else if(typeOperande == 3){	
		c->registres[valeurOperande]=c->RAM[c->registres[7]];
		c->registres[valeurOperande]+=2;

	}

	/* Gestion des flags */
	c->C=0;
	c->N=(mot1&0x4000)>>14;
	if(mot1==0){
		c->Z=1;
	}else{
		c->Z=0;
	}
}

void bcc(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;
	}

	if(c->C==0){
		if(typeOperande==0){
			c->registres[6]=c->registres[6]+c->registres[valeurOperande];
		}else if(typeOperande==1){
			c->registres[valeurOperande]-=2;
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==2){
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==3){
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
			c->registres[valeurOperande]+=2;
		}else {
			c->registres[6]=c->registres[6]+valeurOperande;
		}
	}
}

void bcs(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;
	}

	if(c->C==1){
		if(typeOperande==0){
			c->registres[6]=c->registres[6]+c->registres[valeurOperande];
		}else if(typeOperande==1){
			c->registres[valeurOperande]-=2;
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==2){
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==3){
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
			c->registres[valeurOperande]+=2;
		}else {
			c->registres[6]=c->registres[6]+valeurOperande;
		}
	}
}

void beq(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;
	}

	if(c->Z==1){
		if(typeOperande==0){
			c->registres[6]=c->registres[6]+c->registres[valeurOperande];
		}else if(typeOperande==1){
			c->registres[valeurOperande]-=2;
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==2){
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==3){
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
			c->registres[valeurOperande]+=2;
		}else {
			c->registres[6]=c->registres[6]+valeurOperande;
		}
	}
}

void bne(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;
	}

	if(c->Z==0){
		if(typeOperande==0){
			c->registres[6]=c->registres[6]+c->registres[valeurOperande];
		}else if(typeOperande==1){
			c->registres[valeurOperande]-=2;
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==2){
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==3){
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
			c->registres[valeurOperande]+=2;
		}else {
			c->registres[6]=c->registres[6]+valeurOperande;
		}
	}
}

void ble(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;
	}

	if((c->C==1) | (c->Z==1)){
		if(typeOperande==0){
			c->registres[6]=c->registres[6]+c->registres[valeurOperande];
		}else if(typeOperande==1){
			c->registres[valeurOperande]-=2;
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==2){
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==3){
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
			c->registres[valeurOperande]+=2;
		}else {
			c->registres[6]=c->registres[6]+valeurOperande;
		}
	}
}

void bge(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;
	}

	if((c->C==0) | (c->Z==1)){
		if(typeOperande==0){
			c->registres[6]=c->registres[6]+c->registres[valeurOperande];
		}else if(typeOperande==1){
			c->registres[valeurOperande]-=2;
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==2){
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==3){
			c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
			c->registres[valeurOperande]+=2;
		}else {
			c->registres[6]=c->registres[6]+valeurOperande;
		}
	}
}

void bra(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;
	}

	if(typeOperande==0){
		c->registres[6]=c->registres[6]+c->registres[valeurOperande];
	}else if(typeOperande==1){
		c->registres[valeurOperande]-=2;
		c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
	}else if(typeOperande==2){
		c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
	}else if(typeOperande==3){
		c->registres[6]=c->registres[6]+c->RAM[c->registres[valeurOperande]];
		c->registres[valeurOperande]+=2;
	}else {
		c->registres[6]=c->registres[6]+valeurOperande;
	}
}

void jcc(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;
	}

	if(c->C==0){
		if(typeOperande==0){
			c->registres[6]=c->registres[valeurOperande];
		}else if(typeOperande==1){
			c->registres[valeurOperande]-=2;
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==2){
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==3){
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
			c->registres[valeurOperande]+=2;
		}else{
			c->registres[6]=valeurOperande;
		}

	}


}
void jcs(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;	
	}

	if(c->C==1){
		if(typeOperande==0){

			c->registres[6]=c->registres[valeurOperande];
		}else if(typeOperande==1){
			c->registres[valeurOperande]-=2;
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==2){
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==3){
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
			c->registres[valeurOperande]+=2;
		}else {
			c->registres[6]=valeurOperande;
		}

	}


}
void jeq(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;
		
	}

	if(c->Z==1){
		if(typeOperande==0){
			c->registres[6]=c->registres[valeurOperande];
		}else if(typeOperande==1){
			c->registres[valeurOperande]-=2;
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==2){
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==3){
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
			c->registres[valeurOperande]+=2;
		}else {
			c->registres[6]=valeurOperande;
		}
	}
}

void jne(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;
		
	}

	if(c->Z==0){
		if(typeOperande==0){
			c->registres[6]=c->registres[valeurOperande];
		}else if(typeOperande==1){
			c->registres[valeurOperande]-=2;
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==2){
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==3){
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
			c->registres[valeurOperande]+=2;
		}else {
			c->registres[6]=valeurOperande;
		}
	}
}

void jle(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;
	}

	if((c->C==1) | (c->Z==1)){
		if(typeOperande==0){
			c->registres[6]=c->registres[valeurOperande];
		}else if(typeOperande==1){
			c->registres[valeurOperande]-=2;
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==2){
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==3){
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
			c->registres[valeurOperande]+=2;
		}else {
			c->registres[6]=valeurOperande;
		}
	}
}

void jge(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;
	}

	if((c->C==0) | (c->Z==1)){
		if(typeOperande==0){
			c->registres[6]=c->registres[valeurOperande];
		}else if(typeOperande==1){
			c->registres[valeurOperande]-=2;
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==2){
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
		}else if(typeOperande==3){
			c->registres[6]=c->RAM[c->registres[valeurOperande]];
			c->registres[valeurOperande]+=2;
		}else {
			c->registres[6]=valeurOperande;
		}
	}
}
void jmp(struct cpu* c,uint16_t mot){
	int typeOperande = (mot >> 8) & 7;
	int valeurOperande = mot & 77;
	switch(typeOperande){
		case 0: printf("Registre \n"); break;
		case 1: printf("Registre pré-décrementé \n");break;
		case 2: printf("Adressage indirect \n");break;
		case 3: printf("Registre post-incrémenté \n");break;
		case 4: printf("Valeur immediate \n");break;
	}

	if(typeOperande==0){
		c->registres[6]=c->registres[valeurOperande];
	}else if(typeOperande==1){
		c->registres[valeurOperande]-=2;
		c->registres[6]=c->RAM[c->registres[valeurOperande]];
	}else if(typeOperande==2){
		c->registres[6]=c->RAM[c->registres[valeurOperande]];
	}else if(typeOperande==3){
		c->registres[6]=c->RAM[c->registres[valeurOperande]];
		c->registres[valeurOperande]+=2;
	}else{
		c->registres[6]=valeurOperande;
	}
}

void init_cpu(struct cpu *c){
	int i;
	int j;
	for(i=0;i<256;i++){
		c->RAM[i]=i;
	}
	for(j=0;j<8;j++){
		c->registres[j]=0;
	}
	c->Z=0;
	c->N=1;
	c->C=0;
}	

void execute(struct cpu *c,uint16_t mot1,uint16_t mot2){
	
	 /*mot1ruction de 16 bits*/
	/*mot1 = c->RAM[c->registres[6]] | (c->RAM[c->registres[6]+1] << 8);*/
	switch(mot1>>11){
		case 0: move_general(c,mot1,mot2); break;
		case 3: add(c,mot1); break;
		case 4: cmp(c,mot1); break;
		case 5: sub(c,mot1);break;
		case 6:lsl(c,mot1);break;
		case 7:lsr(c,mot1);break;
		case 8:and(c,mot1);break;
		case 9:or(c,mot1);break;
		case 10:xor(c,mot1);break;
		case 11:not(c,mot1);break;
		case 02:pop(c,mot1);break;
		case 01:push(c,mot1);break;
		case 12:bcc(c,mot1);break;
		case 13:bcs(c,mot1);break;
		case 14:beq(c,mot1);break;
		case 15:bne(c,mot1);break;
		case 16:ble(c,mot1);break;
		case 17:bge(c,mot1);break;
		case 18:bra(c,mot1);break;
		case 20:jcc(c,mot1);break;
		case 21:jcs(c,mot1);break;
		case 22:jeq(c,mot1);break;
		case 23:jne(c,mot1);break;
		case 24:jle(c,mot1);break;
		case 25:jge(c,mot1);break;
		case 26:jmp(c,mot1);break;
		/*...*/
	}	
}

/*adressage direct
Move 0, R0 : c->registre[r0]=0
adressage indirect
Move CAFE, (R0) : c->RAM[c->registre[R0]]=0xCAFE*/

void afficher_cpu(struct cpu* c){
	int j;
	int i;
	for(i=0;i<256;i++){
		printf("R[%d] = ",i);
		printf("%x",c->RAM[i] );
		printf("\n");
	}
	printf("\n");
	for(j=0;j<8;j++){
		printf("R[%d] = ",j);
		printf("%x",c->registres[j]);
		printf("\n");
	}
	printf(" \n");
	printf("%d \n %d \n %d \n",c->Z,c->N,c->C);	
}
