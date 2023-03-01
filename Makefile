compile:
	g++ player.cpp -g --std=c++14 -o ./player
	valgrind --leak-check=full ./player