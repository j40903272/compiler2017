#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#include"header.h"

#define TABLE_SIZE	256

symtab * hash_table[TABLE_SIZE];
char reserved_words[9][16] = {"return", "typedef", "if", "else", "int", "float", "for", "void", "while"};
extern int linenumber, identifier_num;

int HASH(char * str){
	int idx=0;
	while(*str){
		idx = idx << 1;
		idx+=*str;
		str++;
	}	
	return (idx & (TABLE_SIZE-1));
}

/*returns the symbol table entry if found else NULL*/

symtab * lookup(char *name){
	int hash_key;
	symtab* symptr;
	if(!name)
		return NULL;
	hash_key=HASH(name);
	symptr=hash_table[hash_key];

	while(symptr){
		if(!(strcmp(name,symptr->lexeme)))
			return symptr;
		symptr=symptr->front;
	}
	return NULL;
}


void insertID(char *name){
	identifier_num++;
	int hash_key;
	symtab* ptr;
	symtab* symptr=(symtab*)malloc(sizeof(symtab));	
	
	hash_key=HASH(name);
	ptr=hash_table[hash_key];
	
	if(ptr==NULL){
		/*first entry for this hash_key*/
		hash_table[hash_key]=symptr;
		symptr->front=NULL;
		symptr->back=symptr;
	}
	else{
		symptr->front=ptr;
		ptr->back=symptr;
		symptr->back=symptr;
		hash_table[hash_key]=symptr;	
	}
	
	strcpy(symptr->lexeme,name);
	symptr->line=linenumber;
	symptr->counter=1;
}

void printSym(symtab* ptr) 
{
	    printf(" Name = %s \n", ptr->lexeme);
	    printf(" References = %d \n", ptr->counter);
}

int cmp(const void *p1, const void *p2){
	return strcmp(((symtab*)p1)->lexeme, ((symtab*)p2)->lexeme);
}

void printSymTab()
{
    int i;
    printf("----- Symbol Table ---------\n");
    for (i=0; i<TABLE_SIZE; i++)
    {
        symtab* symptr;
		symptr = hash_table[i];
		while (symptr != NULL)
		{
	        printf("\n====>  index = %d \n", i);
		    printSym(symptr);
		    symptr=symptr->front;
		}
    }
}

void printIdentifiers(){
	int i;
	symtab* arr = (symtab*)malloc(sizeof(symtab)*identifier_num);
    for (i=0; i<TABLE_SIZE; i++){
        symtab* symptr;
		symptr = hash_table[i];
		while (symptr != NULL){
	        *arr++ = *symptr;
		    symptr=symptr->front;
		}
    }
    arr -= identifier_num;
    qsort(arr, identifier_num, sizeof(symtab), cmp);
    printf("Frequency of identifiers:\n");
    for(i = 0 ; i < identifier_num ; i++)
    	printf("%s\t%d\n", arr[i].lexeme, arr[i].counter);
}

int is_reserved(char* str){
	int i;
	for(i = 0 ; i < 9 ; i++){
		if(!strcmp(str, reserved_words[i]))
			return 1;
	}
	return 0;
}
