#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>
#include "apriori.h"

Apriori::Apriori(char *_inputpath, char *_outputpath, unsigned int _support) {
	strcpy(inputpath, _inputpath);
	strcpy(outputpath, _outputpath);
	support = _support;
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
	while (Lits.size()) {
		outputFile();

		printf("L%d: %d\n", grouplen, Lits.size());
		Llensum += Lits.size();

		generateCtemp();
		if (showCtemp) {
			printf("C%dtemp: %d\n", grouplen + 1, Ctemp.size());
		}
		grouplen++;

		generateC();
		if (showCits) {
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
	Lsup.clear();
	for (auto &v : C1) {
		if (v.second >= support) {
			Lits.push_back(std::vector<int>(1, v.first));
			Lsup.push_back(v.second);
		}
	}
}

void Apriori::outputFile() {
	for (int i = 0; i < Lits.size(); i++) {
		fprintf(fout, "%d", Lits[i][0]);
		for (int j = 1; j < grouplen; j++) {
			fprintf(fout, ",%d", Lits[i][j]);
		}
		fprintf(fout, ":%d\n", Lsup[i]);
	}
}

void Apriori::generateCtemp() {
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
				if (tempa > tempb) std::swap(tempa, tempb);
				Ctemp[Ctemp.size() - 1][grouplen - 1] = tempa;
				Ctemp[Ctemp.size() - 1].push_back(tempb);
			}
		}
	}
}

void Apriori::generateC() {
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

void Apriori::dfs(Node *&now, std::vector<int> item) {
	if (now->level == grouplen) {
		if (Csup[now] >= support) {
			Lits.push_back(item);
			Lsup.push_back(Csup[now]);
		}
	} else {
		for (auto &next : now->child) {
			std::vector<int> nextitem = item;
			nextitem.push_back(next.first);
			dfs(next.second, nextitem);
		}
	}
}

void Apriori::generateL() {
	// filter support
	this->Lits.clear();
	this->Lsup.clear();
	dfs(root, std::vector<int>());
}
