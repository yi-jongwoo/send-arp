all : send-arp

send-arp : main.o proto_structures.o
	g++ main.o proto_structures.o -lpcap -o send-arp

main.o : main.cpp proto_structures.h
	g++ -c main.cpp -o main.o

proto_structures.o : proto_structures.cpp proto_structures.h
	g++ -c proto_structures.cpp -o proto_structures.o

clean :
	rm -f *.o
	rm -f send-arp
