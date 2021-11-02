.phony all:
all: acs


acs: ACS.c
	gcc -Wall -pthread ACS.c -lreadline -o ACS -g


.PHONY clean:
clean:
	-rm -rf *.o *.exe