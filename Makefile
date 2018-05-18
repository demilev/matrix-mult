FLAGS = -std=c++11 -pthread -lboost_program_options
CC = g++
compile :
	$(CC) $(FLAGS) -o matrix-mult.o matrix-mult.cpp

