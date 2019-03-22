#include "apriori.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

Apriori::Apriori(char *_inputpath, char *_outputpath, unsigned int _support) {
	strcpy(inputpath, _inputpath);
	strcpy(outputpath, _outputpath);
	support = _support;
}

void Apriori::showDetailedTime(bool setting) {
	isShowDetailedTime = setting;
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
		start_time = clock();
		outputFile();
		if (isShowDetailedTime)
			printf("  outputFile, generateCits tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);

		Llensum += Llen;
		printf("L%d: %d (sum: %d)\n", grouplen, Llen, Llensum);

		grouplen++;

		start_time = clock();
		generateC();
		printf("  generateC tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);
		if (isShowDetailedTime)
			printf("C%dits: %d\n", grouplen, Cits.size());

		start_time = clock();
		generateCsup();
		if (isShowDetailedTime)
			printf("  generateCsup tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);

		Llen = 0;
		start_time = clock();
		generateL();
		if (isShowDetailedTime)
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
	Lset.clear();
	root = new Node();
	root->level = 0;
	for (auto &v : C1) {
		if (v.second >= support) {
			root->child[v.first] = new Node();
			root->child[v.first]->level = 1;
			Csup[root->child[v.first]] = v.second;
			// generateLset
			Lset.insert(std::vector<unsigned int>(1, v.first));
			// count L size
			Llen++;
		}
	}
}

void Apriori::dfsOutputFile(Node *&now, std::vector<unsigned int> item) {
	if (now->level == grouplen) {
		// outputFile
		fout << item[0];
		for (int j = 1; j < grouplen; j++) {
			fout << "," << item[j];
		}
		fout << ":" << Csup[now] << "\n";
		return;
	} else if (now->level == grouplen - 1) {
		// generateCits
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
	if (now->level < grouplen) {
		for (auto &next : now->child) {
			std::vector<unsigned int> nextitem = item;
			nextitem.push_back(next.first);
			dfsOutputFile(next.second, nextitem);
		}
	}
}

void Apriori::outputFile() {
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
			if (Lset.find(tempv) == Lset.end()) {
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
	if (isShowDetailedTime)
		printf("  createTrie tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);

	start_time = clock();
	// cnt sup
	fin.open(inputpath, std::fstream::in | std::fstream::binary);
	if (!fin.is_open()) {
		std::cerr << "Fail to open input file";
		exit(1);
	}
	unsigned int cnt;
	std::unordered_map<unsigned int, std::vector<Node *>> nownode;
	while (true) {
		tempn = readint();
		if (fin.eof()) break;
		readint();
		cnt = readint();
		nownode.clear();
		for (auto &node : root->child) {
			nownode[node.first].push_back(node.second);
		}
		while (cnt--) {
			tempn = readint();
			for (int i = nownode[tempn].size() - 1; i >= 0; i--) {
				if (nownode[tempn][i]->level == grouplen) {
					Csup[nownode[tempn][i]]++;
				} else {
					for (auto &node : nownode[tempn][i]->child) {
						nownode[node.first].push_back(node.second);
					}
				}
			}
		}
	}
	fin.close();
	if (isShowDetailedTime)
		printf("  cntSup tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);
}

void Apriori::dfsGenerateL(Node *&now, std::vector<unsigned int> item) {
	if (now->level == grouplen - 1) {
		for (auto next = now->child.begin(); next != now->child.end();) {
			if (Csup[next->second] < support) {
				next = now->child.erase(next);
			} else {
				// generateLset
				std::vector<unsigned int> nextitem = item;
				nextitem.push_back(next->first);
				Lset.insert(nextitem);
				// count L size
				Llen++;
				next++;
			}
		}
	} else if (now->level < grouplen - 1) {
		for (auto &next : now->child) {
			std::vector<unsigned int> nextitem = item;
			nextitem.push_back(next.first);
			dfsGenerateL(next.second, nextitem);
		}
		for (auto it = now->child.begin(); it != now->child.end();) {
			if (it->second->child.size() == 0) {
				it = now->child.erase(it);
			} else {
				it++;
			}
		}
	}
}

void Apriori::generateL() {
	Lset.clear();

	dfsGenerateL(root, std::vector<unsigned int>());
}

// Debug
void Apriori::dfsPrintTrie(Node *&now, std::vector<unsigned int> item) {
	if (!item.empty()) {
		for (int i = 0; i < item.size(); i++) std::cout << "  ";
		std::cout << item.back();
		if (Csup.find(now) != Csup.end()) std::cout << " -- " << Csup[now];
		std::cout << "\n";
	}
	for (auto &next : now->child) {
		std::vector<unsigned int> nextitem = item;
		nextitem.push_back(next.first);
		dfsPrintTrie(next.second, nextitem);
	}
}

void Apriori::printTrie() {
	std::cout << "------Trie------\n";
	dfsPrintTrie(root, std::vector<unsigned int>());
	std::cout << "------Trie end----------\n";
}
