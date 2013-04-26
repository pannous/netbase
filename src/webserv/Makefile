webserv: webserv.o helper.o servreq.o reqhead.o resphead.o resource.o
	gcc -o webserv webserv.o helper.o servreq.o reqhead.o resphead.o resource.o -Wall

webserv.o: webserv.c helper.h servreq.h
	gcc -o webserv.o webserv.c -c -ansi -pedantic -Wall

helper.o: helper.c helper.h
	gcc -o helper.o helper.c -c -ansi -pedantic -Wall

servreq.o: servreq.c servreq.h helper.h
	gcc -o servreq.o servreq.c -c -ansi -pedantic -Wall

reqhead.o: reqhead.c reqhead.h servreq.h helper.h
	gcc -o reqhead.o reqhead.c -c -ansi -pedantic -Wall

resphead.o: resphead.c resphead.h helper.h
	gcc -o resphead.o resphead.c -c -ansi -pedantic -Wall

resource.o: resource.c resource.h
	gcc -o resource.o resource.c -c -ansi -pedantic -Wall
