#ifndef APRIORI_H
#define APRIORI_H

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>

class Apriori {
   private:
	struct Node {
		unsigned int level;
		std::unordered_map<int, Node *> child;
	};

	// Arguments
	char inputpath[100], outputpath[100];
	unsigned int support;
	std::fstream fin, fout;
	// Data
	std::unordered_map<int, int> C1;
	std::unordered_map<Node *, int> Csup;
	std::set<std::vector<unsigned int>> Lset;
	Node *root;
	int grouplen = 1;
	unsigned int Llen = 0;
	// Setting
	bool isShowDetailedTime = false;
	// Temporary variables
	int tempa, tempb;
	unsigned int tempn, cnt;
	std::vector<unsigned int> tempv;

   public:
	Apriori(char *_inputpath, char *_outputpath, unsigned int _support);
	unsigned int run();
	void showDetailedTime(bool setting);

   private:
	inline unsigned int readint();

	void generateC1();
	void generateL1();
	void dfsOutputFile(Node *&now, std::vector<unsigned int> item);
	void outputFile();
	void generateCsup();
	void dfsGenerateL(Node *&now, std::vector<unsigned int> item);
	void generateL();
	// Debug
	void dfsPrintTrie(Node *&now, std::vector<unsigned int> item);
	void printTrie();
};

#endif
