all: scheduling.cpp
	g++ -Wall -o hw3 scheduling.cpp

clean:
	$(RM) hw3