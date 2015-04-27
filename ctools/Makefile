compoll: compoll.exe
	compoll.exe

gen: gen.exe
	gen.exe
	pause

genimage: genimage.exe
	genimage.exe
	pause

%.exe: %.c
	gcc -std=c99 -o $@ $<

MODULES =
MODULES += ../../emCpp/hardware/win32/UART.cpp
%.exe: %.cpp $(MODULES)
	g++ -Wall -I../.. -o $@ $< $(MODULES)

