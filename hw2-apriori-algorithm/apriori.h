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

	char inputpath[100], outputpath[100];
	unsigned int support;
	FILE *fin, *fout;
	std::unordered_map<int, int> C1;
	std::vector<unsigned int> tempv, Lsup;
	std::unordered_map<Node *, int> Csup;
	std::vector<std::vector<unsigned int>> Cits, Ctemp, Lits;
	std::set<std::vector<unsigned int>> Lset;
	int tempa, tempb;
	unsigned int tempn, cnt;
	Node *root;
	unsigned int grouplen = 1;
	int Llen, Llensum;
	bool isshowCtemp = false, isshowCits = false;

   public:
	Apriori(char *_inputpath, char *_outputpath, unsigned int _support);
	unsigned int run();
	void showCtemp(bool setting);
	void showCits(bool setting);

   private:
	inline unsigned int readint(FILE *&file);

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
