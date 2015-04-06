CXX = avr-g++

CXXFLAGS = -I$(ARDUINOCOREDIR) -mcpu=$(AVRTARGETMCU) 

all: core.a

core.a:
	$(CXX) $(CXXFLAGS) -c -o $@ $(ARDUINOCOREDIR)\main.cpp
