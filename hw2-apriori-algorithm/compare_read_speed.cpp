#include <bits/stdc++.h>
using namespace std;

fstream fin;
unsigned int tempa;
int tempb, cnt, rowcnt;
const int runtimes = 1;
string s;
char c;

inline unsigned int readint() {
	fin.read((char *)&tempa, 4);
	return tempa;
}

int main(int argc, char *argv[]) {
	clock_t start_time;

	start_time = clock();
	cout << argv[1] << endl;
	for (int q = 0; q < runtimes; q++) {
		fin.open(argv[1], std::fstream::in | std::fstream::binary);
		rowcnt = 0;
		while (true) {
			readint();
			if (fin.eof()) break;
			readint();
			cnt = readint();
			while (cnt--) {
				tempb = readint();
				// cout << tempb << " ";
			}
			// cout << endl;
			rowcnt++;
		}
		fin.close();
	}
	printf("Read binary tooks %d milliseconds (%d rows)\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC, rowcnt);

	start_time = clock();
	cout << argv[2] << endl;
	for (int q = 0; q < runtimes; q++) {
		fin.open(argv[2], std::fstream::in);
		rowcnt = 0;
		while (getline(fin, s)) {
			stringstream ss(s);
			while (ss >> tempb) {
				// cout << tempb << " ";
				ss >> c;
			}
			// cout << endl;
			rowcnt++;
		}
		fin.close();
	}
	printf("Read int by stringstream tooks %d milliseconds (%d rows)\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC, rowcnt);

	start_time = clock();
	cout << argv[2] << endl;
	for (int q = 0; q < runtimes; q++) {
		fin.open(argv[2], std::fstream::in);
		rowcnt = 0;
		while (getline(fin, s)) {
			s += ',';
			tempb = 0;
			for (int q = 0; q < s.size(); q++) {
				if (s[q] == ',') {
					// cout << tempb << " ";
					tempb = 0;
				} else if ('0' <= s[q] && s[q] <= '9') {
					tempb *= 10;
					tempb += s[q] - '0';
				}
			}
			// cout << endl;
			rowcnt++;
		}
		fin.close();
	}
	printf("Read int by string parser tooks %d milliseconds (%d rows)\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC, rowcnt);
}
