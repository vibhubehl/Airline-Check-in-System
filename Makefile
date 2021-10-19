.phony all:
all: asc


asc: ASC.c
	gcc -Wall ASC.c -lreadline -o ASC -g


.PHONY clean:
clean:
	-rm -rf *.o *.exe