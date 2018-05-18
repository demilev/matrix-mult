compile : matrix-mult.o
	g++ -std=c++11 matrix-mult.cpp -pthread -lboost_program_options
