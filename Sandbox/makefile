LIBS = -lRakNetDLL
INCLUDES= -I/usr/include/raknet 
COMPILER= g++ --std=c++14 

all: sandbox


sandbox: main.o
	$(COMPILER) main.o -o sandbox $(LIBS) $(INCLUDES)

main.o: main.cpp
	$(COMPILER) -c main.cpp -o main.o $(LIBS) $(INCLUDES)


