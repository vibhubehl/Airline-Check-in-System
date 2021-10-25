.phony all:
all: acs


acs: ACS.c
	gcc -Wall ACS.c -lreadline -o ACS -g


.PHONY clean:
clean:
	-rm -rf *.o *.exe