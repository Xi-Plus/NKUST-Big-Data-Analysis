#include "apriori.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>

Apriori::Apriori(char *_inputpath, char *_outputpath, unsigned int _support) {
	strcpy(inputpath, _inputpath);
	strcpy(outputpath, _outputpath);
	support = _support;
}

void Apriori::showCtemp(bool setting) {
	isshowCtemp = setting;
}

void Apriori::showCits(bool setting) {
	isshowCits = setting;
}

unsigned int Apriori::run() {
	fin = fopen(inputpath, "rb");
	if (fin == NULL) {
		std::cerr << "Fail to open input file";
		return 1;
	}
	fout = fopen(outputpath, "w");
	if (fout == NULL) {
		std::cerr << "Fail to open output file";
		return 1;
	}

	generateC1();
	generateL1();

	int Llensum = 0;
	while (root->child.size()) {
		Llen = 0;
		outputFile();

		Llensum += Llen;
		printf("L%d: %d (sum: %d)\n", grouplen, Llen, Llensum);

		generateCtemp();
		if (isshowCtemp) {
			printf("C%dtemp: %d\n", grouplen + 1, Ctemp.size());
			printf("L%dset: %d\n", grouplen + 1, Lset.size());
		}
		grouplen++;

		generateC();
		if (isshowCits) {
			printf("C%dits: %d\n", grouplen, Cits.size());
		}

		generateCsup();

		generateL();
	}

	fclose(fout);

	return Llensum;
}

inline unsigned int Apriori::readint(FILE *&file) {
	fread(&tempa, 1, 4, file);
	return tempa;
}

void Apriori::generateC1() {
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

void Apriori::generateL1() {
	Csup.clear();
	root = new Node();
	root->level = 0;
	for (auto &v : C1) {
		if (v.second >= support) {
			root->child[v.first] = new Node();
			root->child[v.first]->level = 1;
			Csup[root->child[v.first]] = v.second;
		}
	}
}

void Apriori::dfsOutputFile(Node *&now, std::vector<unsigned int> item) {
	if (now->level == grouplen) {
		fprintf(fout, "%d", item[1]);
		for (int j = 2; j <= grouplen; j++) {
			fprintf(fout, ",%d", item[j]);
		}
		fprintf(fout, ":%d\n", Csup[now]);
		Llen++;
	} else {
		for (auto &next : now->child) {
			std::vector<unsigned int> nextitem = item;
			nextitem.push_back(next.first);
			dfsOutputFile(next.second, nextitem);
		}
	}
}

void Apriori::outputFile() {
	dfsOutputFile(root, std::vector<unsigned int>());
}

void Apriori::dfsGenerateCtemp(Node *&now, std::vector<unsigned int> item) {
	if (now->level == grouplen) {
		Lset.insert(item);
		return;
	}
	if (now->level == grouplen - 1) {
		if (now->child.size() >= 2) {
			for (auto i = now->child.begin(); i != now->child.end(); i++) {
				for (auto j = std::next(i, 1); j != now->child.end(); j++) {
					Ctemp.push_back(item);
					tempa = i->first;
					tempb = j->first;
					if (tempa > tempb) std::swap(tempa, tempb);
					Ctemp[Ctemp.size() - 1].push_back(tempa);
					Ctemp[Ctemp.size() - 1].push_back(tempb);
				}
			}
		}
	}
	for (auto &next : now->child) {
		std::vector<unsigned int> nextitem = item;
		nextitem.push_back(next.first);
		dfsGenerateCtemp(next.second, nextitem);
	}
}

void Apriori::generateCtemp() {
	Ctemp.clear();
	Lset.clear();

	dfsGenerateCtemp(root, std::vector<unsigned int>());
}

void Apriori::generateC() {
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

void Apriori::generateCsup() {
	Csup.clear();

	Node *now;
	root = new Node();
	root->level = 0;
	for (auto &item : Cits) {
		now = root;
		for (int j = 0; j < grouplen; j++) {
			if (now->child.find(item[j]) == now->child.end()) {
				now->child[item[j]] = new Node();
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
	std::vector<Node *> nownode;
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

void Apriori::dfs(Node *&now) {
	if (now->level == grouplen - 1) {
		for (auto &next : now->child) {
			if (Csup[next.second] < support) {
				now->child.erase(next.first);
			}
		}
	} else if (now->level < grouplen - 1) {
		for (auto &next : now->child) {
			dfs(next.second);
		}
		for (auto &next : now->child) {
			if (next.second->child.size() == 0) {
				now->child.erase(next.first);
			}
		}
	}
}

void Apriori::generateL() {
	dfs(root);
}
