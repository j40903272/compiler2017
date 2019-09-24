TARGET = parser
OBJECT = parser.tab.h parser.tab.c parser.tab.o lex.yy.c alloc.o functions.o codeGeneration.o semanticAnalysis.o printSourceFile.o myRegister.o offsetInAR.o symbolTable.o myIntVector.o
CC = gcc -g
LEX = flex
YACC = bison -v
YACCFLAG = -d
LIBS = -lfl 

parser: parser.tab.o alloc.o functions.o symbolTable.o semanticAnalysis.o codeGeneration.o myRegister.o offsetInAR.o myIntVector.o printSourceFile.o
	$(CC) -o $(TARGET) parser.tab.o alloc.o functions.o symbolTable.o semanticAnalysis.o codeGeneration.o myRegister.o offsetInAR.o myIntVector.o printSourceFile.o $(LIBS)

parser.tab.o: parser.tab.c lex.yy.c alloc.o functions.c symbolTable.o semanticAnalysis.o codeGeneration.o
	$(CC) -c parser.tab.c

codeGeneration.o: codeGeneration.c
	$(CC) -c codeGeneration.c
    
semanticAnalysis.o: semanticAnalysis.c
	$(CC) -c semanticAnalysis.c
    
printSourceFile.o: printSourceFile.c
	$(CC) -c printSourceFile.c

offsetInAR.o: offsetInAR.c
	$(CC) -c offsetInAR.c
    
myRegister.o: myRegister.c
	$(CC) -c myRegister.c
    
myIntVector.o: myIntVector.c
	$(CC) -c myIntVector.c
    
symbolTable.o: symbolTable.c
	$(CC) -c symbolTable.c

lex.yy.c: lexer3.l
	$(LEX) lexer3.l

parser.tab.c: parser.y 
	$(YACC) $(YACCFLAG) parser.y

alloc.o: alloc.c
	$(CC) -c alloc.c
	
functions.o: functions.c
	$(CC) -c functions.c

clean:
	rm -f $(TARGET) $(OBJECT) parser.output

