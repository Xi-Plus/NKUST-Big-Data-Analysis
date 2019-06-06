outputsuf = .out.txt

all: 1

1: main
	./main data/baseball.csv data/baseball-out.txt

2: main
	./main data/breast-cancer-training.csv data/breast-cancer-training-out.txt

3: main
	./main data/breast-cancer-wisconsin.csv data/breast-cancer-wisconsin-out.txt

4: main
	./main data/fish.csv data/fish-out.txt

5: main
	./main data/nursery.csv data/nursery-out.txt

main: main.cpp id3.cpp
	g++ -o main main.cpp id3.cpp -std=c++11 -O3

main.o: main.cpp
	g++ -c main.cpp -std=c++11 -O3

id3.o: id3.cpp
	g++ -c id3.cpp -std=c++11 -O3

clean:
	del *.exe
