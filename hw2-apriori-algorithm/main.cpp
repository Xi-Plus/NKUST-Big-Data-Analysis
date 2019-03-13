#include <algorithm>
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

unsigned char temps[5];
vector<int> tempv, Lsup;
unordered_map<Node *, int> Csup;
vector<vector<int>> Cits, Ctemp, Lits;
set<vector<int>> Lset;
int tempa, tempb;

unsigned int grouplen = 1, support;
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

inline unsigned int readint(FILE *&file) {
	fread(temps, 1, 4, file);
	return temps[0] | temps[1] << 8 | temps[2] << 16 | temps[3] << 24;
}

int main(int argc, char *argv[]) {
	clock_t start_time = clock();
	if (argc < 4) {
		printf("Missing parameter\n");
		return 0;
	}
	support = atoi(argv[3]);
	printf("file input: %s\n", argv[1]);
	printf("file output: %s\n", argv[2]);
	printf("support: %d\n", support);

	FILE *fin, *fout;
	unsigned int tempn, cnt;

	fin = fopen(argv[1], "rb");
	if (fin == NULL) {
		printf("Fail to open input file");
		return 0;
	}
	fout = fopen(argv[2], "w");
	if (fout == NULL) {
		printf("Fail to open output file");
		return 0;
	}
	unordered_map<int, int> C1;
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

	Lsup.clear();
	for (auto &v : C1) {
		if (v.second >= support) {
			Lits.push_back(vector<int>(1, v.first));
			Lsup.push_back(v.second);
		}
	}

	int Llensum = 0;
	while (Lits.size()) {
		// output file
		for (int i = 0; i < Lits.size(); i++) {
			fprintf(fout, "%d", Lits[i][0]);
			for (int j = 1; j < grouplen; j++) {
				fprintf(fout, ",%d", Lits[i][j]);
			}
			fprintf(fout, ":%d\n", Lsup[i]);
		}
		printf("L%d: %d\n", grouplen, Lits.size());
		Llensum += Lits.size();

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

		grouplen++;

		// remove not in Lits
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

		Csup.clear();

		Node *root = new Node(), *now;
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
		fin = fopen(argv[1], "rb");
		if (fin == NULL) {
			printf("Fail to open input file");
			return 0;
		}
		unsigned int tempn, cnt;
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

		// filter support
		Lits.clear();
		Lsup.clear();
		dfs(root, vector<int>());
	}

	fclose(fout);

	printf("Lall: %d\n", Llensum);
	printf("It tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);
}
