compile : matrix-mult.o
	g++ -std=c++11 matrix-mult.cpp -o matrix-mult.o -pthread -lboost_program_options
