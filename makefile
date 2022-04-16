server:
	g++ -g Server.cpp -lpthread -ljsoncpp -lfmt -lpqxx -lpq
	./a.out
	rm ./a.out

