OBJS = *.o

syntax: syntax.tab.c
	g++ -c main.cpp syntax.c syntax.tab.c 

syntax.tab.c: lex.yy.c
	bison -d syntax.y

lex.yy.c:
	flex --header-file=lex.yy.h lexical.l

clean:
	rm -f syntax.tab.c syntax.tab.h lex.yy.c lex.yy.h $(OBJS)



	