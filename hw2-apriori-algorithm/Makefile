all: 1

main.exe: main.cpp
	C:/mingw-w64/mingw64/bin/g++.exe main.cpp -o main.exe -std=c++17 -O3

main: main.cpp
	g++ main.cpp -o main.exe -std=c++17 -O3

0: main.exe
	main.exe small.data small-out.txt 2

1: main.exe
	main.exe T15I7N0.5KD1K.data 1K-out.txt 5

2: main.exe
	main.exe T15I7N0.5KD10K.data 10K-out.txt 50

3: main.exe
	main.exe T15I7N0.5KD100K.data 100K-out.txt 500

4: main.exe
	main.exe T15I7N0.5KD1000K.data 1000K-out.txt 30000

i1: main
	isolate --cg --clean || echo ""
	isolate --cg --init
	cp main /var/local/lib/isolate/0/box
	cp T15I7N0.5KD1K.data /var/local/lib/isolate/0/box
	isolate --cg --meta=/tmp/log.txt --run main T15I7N0.5KD1K.data 1K-out.txt 5 
	cat /tmp/log.txt
	isolate --clean

i2: main
	isolate --cg --clean || echo ""
	isolate --cg --init
	cp main /var/local/lib/isolate/0/box
	cp T15I7N0.5KD10K.data /var/local/lib/isolate/0/box
	isolate --cg --meta=/tmp/log.txt --run main T15I7N0.5KD10K.data 10K-out.txt 50 
	cat /tmp/log.txt
	isolate --clean

i3: main
	isolate --cg --clean || echo ""
	isolate --cg --init
	cp main /var/local/lib/isolate/0/box
	cp T15I7N0.5KD100K.data /var/local/lib/isolate/0/box
	isolate --cg --meta=/tmp/log.txt --run main T15I7N0.5KD100K.data 100K-out.txt 500 
	cat /tmp/log.txt
	isolate --clean

i4: main
	isolate --cg --clean || echo ""
	isolate --cg --init
	cp main /var/local/lib/isolate/0/box
	cp T15I7N0.5KD1000K.data /var/local/lib/isolate/0/box
	isolate --cg --meta=/tmp/log.txt --run main T15I7N0.5KD1000K.data 1000K-out.txt 30000 
	cat /tmp/log.txt
	isolate --clean
