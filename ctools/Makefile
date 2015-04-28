gen: gen.exe
	$<

%.exe: %.c
	gcc -std=c99 -o $@ $<

MODULES =
H =
MODULES += ../../emCpp/hardware/win32/UART.cpp
H	+= ../../emCpp/UART.h
MODULES += ../../emCpp/CRC.cpp
H	+= ../../emCpp/CRC.h

%.exe: %.cpp $(MODULES) $(H)
	g++ -Wall -I../.. -o $@ $< $(MODULES)

