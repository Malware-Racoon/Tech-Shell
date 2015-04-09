all: tech_shell

tech_shell: tech_shell.o parse.o
	gcc tech_shell.o parse.o -o tech_shell

tech_shell.o: tech_shell.c
	gcc -c tech_shell.c

parse.o: parse.c
	gcc -c parse.c

clean:
	rm -f *o tech_shell	
