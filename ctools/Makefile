exec: mbr.exe
	mbr.exe
	pause

%.exe: %.c
	gcc -std=c99 -o $@ $<

%.exe: %.cpp
	g++ -o $@ $<

