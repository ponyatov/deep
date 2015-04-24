exec: genimage.exe
	genimage.exe
	pause

%.exe: %.c
	gcc -std=c99 -o $@ $<

%.exe: %.cpp
	g++ -Wall -o $@ $<

