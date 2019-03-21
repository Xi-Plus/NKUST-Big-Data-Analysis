outputsuf = .out.txt
SANDBOX = /var/local/lib/isolate/0

all: 1

main.exe: apriori.o main.o
	g++ -o main apriori.o main.o -lstdc++ -std=c++11 -O3

apriori.o: apriori.cpp apriori.h
	g++ -c apriori.cpp -std=c++11 -O3

main.o: main.cpp
	g++ -c main.cpp -std=c++11 -O3

main: apriori.o main.o
	g++ -o main apriori.o main.o -lstdc++ -std=c++11 -O3

0: main.exe
	main.exe data/small.data data/small-out.txt 2

1: main.exe
	main.exe data/T15I7N0.5KD1K.data data/T15I7N0.5KD1K${outputsuf} 5

2: main.exe
	main.exe data/T15I7N0.5KD10K.data data/T15I7N0.5KD10K${outputsuf} 50

3: main.exe
	main.exe data/T15I7N0.5KD100K.data data/T15I7N0.5KD100K${outputsuf} 500

4: main.exe
	main.exe data/T15I7N0.5KD1000K.data data/T15I7N0.5KD1000K${outputsuf} 30000

5: main.exe
	main.exe data/T10I10N0.1KD1K.data data/T10I10N0.1KD1K${outputsuf} 4

6: main.exe
	main.exe data/T20I15N1KD10K.data data/T20I15N1KD10K${outputsuf} 9

7: main.exe
	main.exe data/T12I30N0.5KD100K.data data/T12I30N0.5KD100K${outputsuf} 300

8: main.exe
	main.exe data/T18I20N2KD5000K.data data/T18I20N2KD5000K${outputsuf} 65000


i1: main
	isolate --cg --clean || echo ""
	isolate --cg --init
	cp main ${SANDBOX}/box
	cp data/T15I7N0.5KD1K.data ${SANDBOX}/box
	isolate --cg --meta=/tmp/log.txt --run main T15I7N0.5KD1K.data 1K-out.txt 5 
	cat /tmp/log.txt
	isolate --clean

i2: main
	isolate --cg --clean || echo ""
	isolate --cg --init
	cp main ${SANDBOX}/box
	cp data/T15I7N0.5KD10K.data ${SANDBOX}/box
	isolate --cg --meta=/tmp/log.txt --run main T15I7N0.5KD10K.data 10K-out.txt 50 
	cat /tmp/log.txt
	isolate --clean

i3: main
	isolate --cg --clean || echo ""
	isolate --cg --init
	cp main ${SANDBOX}/box
	cp data/T15I7N0.5KD100K.data ${SANDBOX}/box
	isolate --cg --meta=/tmp/log.txt --run main T15I7N0.5KD100K.data 100K-out.txt 500 
	cat /tmp/log.txt
	isolate --clean

i4: main
	isolate --cg --clean || echo ""
	isolate --cg --init
	cp main ${SANDBOX}/box
	cp data/T15I7N0.5KD1000K.data ${SANDBOX}/box
	isolate --cg --meta=/tmp/log.txt --run main T15I7N0.5KD1000K.data 1000K-out.txt 30000 
	cat /tmp/log.txt
	isolate --clean

i5: main
	isolate --cg --clean || echo ""
	isolate --cg --init
	cp main ${SANDBOX}/box
	cp data/T10I10N0.1KD1K.data ${SANDBOX}/box
	isolate --cg --meta=/tmp/log.txt --run main T10I10N0.1KD1K.data 1K-out.txt 5 
	cat /tmp/log.txt
	isolate --clean

i6: main
	isolate --cg --clean || echo ""
	isolate --cg --init
	cp main ${SANDBOX}/box
	cp data/T20I15N1KD10K.data ${SANDBOX}/box
	isolate --cg --meta=/tmp/log.txt --run main T20I15N1KD10K.data 10K-out.txt 50 
	cat /tmp/log.txt
	isolate --clean

i7: main
	isolate --cg --clean || echo ""
	isolate --cg --init
	cp main ${SANDBOX}/box
	cp data/T12I30N0.5KD100K.data ${SANDBOX}/box
	isolate --cg --meta=/tmp/log.txt --run main T12I30N0.5KD100K.data 100K-out.txt 500 
	cat /tmp/log.txt
	isolate --clean

i8: main
	isolate --cg --clean || echo ""
	isolate --cg --init
	cp main ${SANDBOX}/box
	cp data/T18I20N2KD5000K.data ${SANDBOX}/box
	isolate --cg --meta=/tmp/log.txt --run main T18I20N2KD5000K.data 1000K-out.txt 30000 
	cat /tmp/log.txt
	isolate --clean
