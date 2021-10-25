.phony all:
all: asc


asc: ASC.c
	gcc -Wall ACS.c -lreadline -o ACS -g


.PHONY clean:
clean:
	-rm -rf *.o *.exe