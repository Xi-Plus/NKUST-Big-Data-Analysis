#include "apriori.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
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
	fout.open(outputpath, std::fstream::out);
	if (!fout.is_open()) {
		std::cerr << "Fail to open output file";
		exit(1);
	}

	generateC1();
	generateL1();

	unsigned int Llensum = 0;
	clock_t start_time;
	while (root->child.size()) {
		Llen = 0;
		start_time = clock();
		outputFile();
		printf("  outputFile, gernerateLset, generateCtemp tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);

		Llensum += Llen;
		printf("L%d: %d (sum: %d)\n", grouplen, Llen, Llensum);

		if (isshowCtemp) {
			printf("C%dtemp: %d\n", grouplen + 1, Ctemp.size());
			printf("L%dset: %d\n", grouplen + 1, Lset.size());
		}
		grouplen++;

		start_time = clock();
		generateC();
		printf("  generateC tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);
		if (isshowCits) {
			printf("C%dits: %d\n", grouplen, Cits.size());
		}

		start_time = clock();
		generateCsup();
		printf("  generateCsup tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);

		start_time = clock();
		generateL();
		printf("  generateL tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);
	}

	fout.close();

	return Llensum;
}

inline unsigned int Apriori::readint() {
	fin.read((char *)&tempa, 4);
	return tempa;
}

void Apriori::generateC1() {
	fin.open(inputpath, std::fstream::in | std::fstream::binary);
	if (!fin.is_open()) {
		std::cerr << "Fail to open input file";
		exit(1);
	}

	while (true) {
		readint();
		if (fin.eof()) break;
		readint();
		cnt = readint();
		while (cnt--) {
			tempn = readint();
			C1[tempn]++;
		}
	}
	fin.close();
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
		fout << item[0];
		for (int j = 1; j < grouplen; j++) {
			fout << "," << item[j];
		}
		fout << ":" << Csup[now] << "\n";
		Lset.insert(item);
		Llen++;
		return;
	} else if (now->level == grouplen - 1) {
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
		dfsOutputFile(next.second, nextitem);
	}
}

void Apriori::outputFile() {
	Lset.clear();
	Ctemp.clear();

	dfsOutputFile(root, std::vector<unsigned int>());
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

	clock_t start_time = clock();
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
	printf("  createTrie tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);

	start_time = clock();
	// cnt sup
	fin.open(inputpath, std::fstream::in | std::fstream::binary);
	if (!fin.is_open()) {
		std::cerr << "Fail to open input file";
		exit(1);
	}
	unsigned int cnt;
	std::vector<Node *> nownode;
	while (true) {
		tempn = readint();
		if (fin.eof()) break;
		readint();
		cnt = readint();
		nownode.clear();
		nownode.push_back(root);
		while (cnt--) {
			tempn = readint();
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
	fin.close();
	printf("  cntSup tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);
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
