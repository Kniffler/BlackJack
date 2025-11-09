blackjack: blackjoe.cpp PAG.o
	g++ -o blackjack blackjoe.cpp PAG.o
PAG.o: PAG.cpp
	g++ -c PAG.cpp -o PAG.o
