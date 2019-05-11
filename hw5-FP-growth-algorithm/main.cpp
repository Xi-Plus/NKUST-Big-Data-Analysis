#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>
#include "fpgrowth.cpp"

using namespace std;

int main(int argc, char *argv[]) {
	char inputpath[100], outputpath[100];
	unsigned int support;
	if (argc < 4) {
		printf("Missing parameter\n");
		string temps;
		printf("file input:");
		getline(cin, temps);
		strcpy(inputpath, temps.c_str());
		printf("file output: ");
		getline(cin, temps);
		strcpy(outputpath, temps.c_str());
		printf("support: ");
		cin >> support;
	} else {
		strcpy(inputpath, argv[1]);
		strcpy(outputpath, argv[2]);
		support = atoi(argv[3]);
	}
	printf("file input: %s\n", inputpath);
	printf("file output: %s\n", outputpath);
	printf("support: %d\n", support);

	clock_t start_time = clock();

	FPGrowth fpgrowth(inputpath, outputpath, support);
	fpgrowth.showDetailedTime(true);
	unsigned int Llensum = fpgrowth.run();

	printf("Lall: %d\n", Llensum);
	printf("It tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);

	return 0;
}