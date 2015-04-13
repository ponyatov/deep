exec: crcgen.exe
	crcgen.exe
	pause

crcgen.exe: crcgen.c
	gcc -std=c99 -o $@ $<

