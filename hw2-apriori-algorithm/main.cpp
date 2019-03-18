#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>
using namespace std;

struct Node {
	unsigned int val, level;
	unordered_map<int, Node *> child;
};

char inputpath[100];
char outputpath[100];
FILE *fin, *fout;
unordered_map<int, int> C1;
vector<int> tempv, Lsup;
unordered_map<Node *, int> Csup;
vector<vector<int>> Cits, Ctemp, Lits;
set<vector<int>> Lset;
int tempa, tempb;
unsigned int tempn, cnt;
Node *root;

unsigned int grouplen = 1, support;

void generateC1();
void generateL1();
void outputFile();
void generateCtemp();
void generateC();
void generateCsup();
void generateL();

int main(int argc, char *argv[]) {
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

	fin = fopen(inputpath, "rb");
	if (fin == NULL) {
		cerr << "Fail to open input file";
		return 1;
	}
	fout = fopen(outputpath, "w");
	if (fout == NULL) {
		cerr << "Fail to open output file";
		return 1;
	}

	generateC1();
	generateL1();

	int Llensum = 0;
	while (Lits.size()) {
		outputFile();

		printf("L%d: %d\n", grouplen, Lits.size());
		Llensum += Lits.size();

		generateCtemp();
		printf("C%dtemp: %d\n", grouplen + 1, Ctemp.size());

		grouplen++;

		generateC();
		printf("C%dits: %d\n", grouplen, Cits.size());

		generateCsup();

		generateL();
	}

	fclose(fout);

	printf("Lall: %d\n", Llensum);
	printf("It tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);
}

inline unsigned int readint(FILE *&file) {
	fread(&tempa, 1, 4, file);
	return tempa;
}

void dfs(Node *&now, vector<int> item) {
	if (now->level == grouplen) {
		if (Csup[now] >= support) {
			Lits.push_back(item);
			Lsup.push_back(Csup[now]);
		}
	} else {
		for (auto &next : now->child) {
			vector<int> nextitem = item;
			nextitem.push_back(next.first);
			dfs(next.second, nextitem);
		}
	}
}

void generateC1() {
	while (true) {
		tempn = readint(fin);
		if (feof(fin)) break;
		readint(fin);
		cnt = readint(fin);
		while (cnt--) {
			tempn = readint(fin);
			C1[tempn]++;
		}
	}
	fclose(fin);
}

void generateL1() {
	Lsup.clear();
	for (auto &v : C1) {
		if (v.second >= support) {
			Lits.push_back(vector<int>(1, v.first));
			Lsup.push_back(v.second);
		}
	}
}

void outputFile() {
	for (int i = 0; i < Lits.size(); i++) {
		fprintf(fout, "%d", Lits[i][0]);
		for (int j = 1; j < grouplen; j++) {
			fprintf(fout, ",%d", Lits[i][j]);
		}
		fprintf(fout, ":%d\n", Lsup[i]);
	}
}

void generateCtemp() {
	Ctemp.clear();

	// cross join
	for (int i = 0; i < Lits.size(); i++) {
		for (int j = i + 1; j < Lits.size(); j++) {
			int k = 0;
			while (k < grouplen - 1 && Lits[i][k] == Lits[j][k]) k++;
			if (k == grouplen - 1) {
				Ctemp.push_back(Lits[i]);
				tempa = Lits[i][grouplen - 1];
				tempb = Lits[j][grouplen - 1];
				if (tempa > tempb) swap(tempa, tempb);
				Ctemp[Ctemp.size() - 1][grouplen - 1] = tempa;
				Ctemp[Ctemp.size() - 1].push_back(tempb);
			}
		}
	}
}

void generateC() {
	Lset.clear();
	for (auto &itemset : Lits) {
		Lset.insert(itemset);
	}
	Cits.clear();
	for (auto &itemset : Ctemp) {
		int i = 0;
		while (i < grouplen) {
			tempv = itemset;
			tempv.erase(tempv.begin() + i);
			if (Lset.find(tempv) == Lset.end()) {  // https://en.cppreference.com/w/cpp/container/set/find
				break;
			}
			i++;
		}
		if (i == grouplen) {
			Cits.push_back(itemset);
		}
	}
}

void generateCsup() {
	Csup.clear();

	Node *now;
	root = new Node();
	root->val = -1;
	root->level = 0;
	for (auto &item : Cits) {
		now = root;
		for (int j = 0; j < grouplen; j++) {
			if (now->child.find(item[j]) == now->child.end()) {
				now->child[item[j]] = new Node();
				now->child[item[j]]->val = item[j];
				now->child[item[j]]->level = j + 1;
			}
			now = now->child[item[j]];
		}
		Csup[now] = 0;
	}

	// cnt sup
	fin = fopen(inputpath, "rb");
	if (fin == NULL) {
		fprintf(stderr, "Fail to open input file");
		exit(1);
	}
	unsigned int cnt;
	vector<Node *> nownode;
	while (true) {
		tempn = readint(fin);
		if (feof(fin)) break;
		readint(fin);
		cnt = readint(fin);
		nownode.clear();
		nownode.push_back(root);
		while (cnt--) {
			tempn = readint(fin);
			for (int i = nownode.size() - 1; i >= 0; i--) {
				if (nownode[i]->child.find(tempn) != nownode[i]->child.end()) {
					if (nownode[i]->child[tempn]->level == grouplen) {
						Csup[nownode[i]->child[tempn]]++;
					} else {
						nownode.push_back(nownode[i]->child[tempn]);
					}
				}
			}
		}
	}
	fclose(fin);
}

void generateL() {
	// filter support
	Lits.clear();
	Lsup.clear();
	dfs(root, vector<int>());
}
