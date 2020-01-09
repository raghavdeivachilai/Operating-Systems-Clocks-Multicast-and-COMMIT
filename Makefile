compile: socket

socket: client.o front.o server.o
	g++ -pthread client.o -o client 
	g++ -pthread front.o -o front
	g++ -pthread server.o -o server        

Berkeley.o: Client.cpp
	g++ -c Client.cpp -pthread

causalordering.o: frontendserver.cpp
	g++ -c frontendserver.cpp -pthread

noncausal.o: server.cpp
	g++ -c server.cpp -pthread

clean:
	rm -rf *o client server
 
