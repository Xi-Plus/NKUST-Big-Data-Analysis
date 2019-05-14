outputsuf = .out.txt

all: 1

0: main
	./main data/small.data data/small-out.txt 3 18

1: main
	./main data/T15I7N0.5KD1K.data data/T15I7N0.5KD1K${outputsuf} 5 7029

2: main
	./main data/T15I7N0.5KD10K.data data/T15I7N0.5KD10K${outputsuf} 50 4353

3: main
	./main data/T15I7N0.5KD100K.data data/T15I7N0.5KD100K${outputsuf} 500 4117

4: main
	./main data/T15I7N0.5KD1000K.data data/T15I7N0.5KD1000K${outputsuf} 30000 184

5: main
	./main data/T10I10N0.1KD1K.data data/T10I10N0.1KD1K${outputsuf} 4 59800

6: main
	./main data/T20I15N1KD10K.data data/T20I15N1KD10K${outputsuf} 9 10702904

7: main
	./main data/T12I30N0.5KD100K.data data/T12I30N0.5KD100K${outputsuf} 300 27989

8: main
	./main data/T18I20N2KD5000K.data data/T18I20N2KD5000K${outputsuf} 65000 413

main: main.o fpgrowth.o
	g++ -o main main.o -std=c++11 -O3

main.o: main.cpp
	g++ -c main.cpp -std=c++11 -O3

fpgrowth.o: fpgrowth.cpp
	g++ -c fpgrowth.cpp -std=c++11 -O3

clean:
	del *.exe
