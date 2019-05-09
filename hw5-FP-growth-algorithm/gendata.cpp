#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>
#include "fpgrowth.cpp"

using namespace std;

int main(int argc, char *argv[]) {
	string data[] = {
		"acdfgimp",
		"abcflmo",
		"bfhjow",
		"bckps",
		"aceflmnp",
	};

	fstream file;
	file.open("small.data", std::fstream::out | std::fstream::binary);
	unsigned int c;
	for (size_t i = 0; i < 5; i++) {
		c = (i + 1);
		file.write((char *)&c, 4);
		file.write((char *)&c, 4);
		c = data[i].size();
		file.write((char *)&c, 4);
		for (size_t j = 0; j < data[i].size(); j++) {
			c = data[i][j] - 'a';
			file.write((char *)&c, 4);
		}
	}

	file.close();

	return 0;
}
