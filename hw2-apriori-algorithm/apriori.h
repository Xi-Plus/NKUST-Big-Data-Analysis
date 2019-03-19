#ifndef APRIORI_H
#define APRIORI_H

#include <algorithm>
#include <cstdio>
#include <cstring>
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
	FILE *fin, *fout;
	// Data
	std::unordered_map<int, int> C1;
	std::unordered_map<Node *, int> Csup;
	std::vector<std::vector<unsigned int>> Cits, Ctemp;
	std::set<std::vector<unsigned int>> Lset;
	Node *root;
	unsigned int grouplen = 1;
	unsigned int Llen;
	// Setting
	bool isshowCtemp = false, isshowCits = false;
	// Temporary variables
	int tempa, tempb;
	unsigned int tempn, cnt;
	std::vector<unsigned int> tempv;

   public:
	Apriori(char *_inputpath, char *_outputpath, unsigned int _support);
	unsigned int run();
	void showCtemp(bool setting);
	void showCits(bool setting);

   private:
	inline unsigned int readint();

	void generateC1();
	void generateL1();
	void dfsOutputFile(Node *&now, std::vector<unsigned int> item);
	void outputFile();
	void dfsGenerateCtemp(Node *&now, std::vector<unsigned int> item);
	void generateCtemp();
	void generateC();
	void generateCsup();
	void dfs(Node *&now);
	void generateL();
};

#endif
