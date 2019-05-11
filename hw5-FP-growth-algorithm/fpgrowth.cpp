#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

// #define COUNTING_CATEGORY

class FPGrowth {
   private:
	struct TreeNode {
		unsigned int item;
		unsigned int count;
		unordered_map<int, TreeNode *> child;
		TreeNode *next = nullptr;
		TreeNode *parent = nullptr;
		TreeNode(unsigned int _item) {
			item = _item;
			count = 0;
		}
	};
	struct HeaderTableNode {
		TreeNode *start = nullptr;
		TreeNode *end = nullptr;
	};
	struct Tree {
		vector<unsigned int> prefix;
		TreeNode *tree;
		vector<pair<unsigned int, HeaderTableNode *>> header_table;
		unordered_map<unsigned int, HeaderTableNode *> header_table_pointer;
		unordered_map<unsigned int, Tree *> child_tree;
		Tree(vector<unsigned int> _prefix) {
			prefix = _prefix;
			tree = new TreeNode(-1);
		}
	};

	// Arguments
	char inputpath[100], outputpath[100];
	unsigned int support;
	fstream fin, fout;
	// Data
	unordered_map<int, int> C1;
	vector<pair<int, int>> L1;
	Tree *forest;
	/*
	unordered_map<Node *, int> Csup;
	vector<vector<unsigned int>> Ctemp, Cits;
	unordered_set<vector<unsigned int>, vector_hash> Lset;
	Node *root;
	int grouplen = 1;
	unsigned int Llen = 0;*/
	// Setting

	bool isShowDetailedTime = false;
	unsigned int CtempMin = 3;
	unsigned int CtempMax = 3;
	// Temporary variables
	int tempa, tempb, tempc;
	unsigned int tempn, cnt;
	/*
	bool inTrie;
	Node *tempNode;
	vector<unsigned int> tempv;*/

   public:
	FPGrowth(char *_inputpath, char *_outputpath, unsigned int _support) {
		strcpy(inputpath, _inputpath);
		strcpy(outputpath, _outputpath);
		support = _support;
	}
	unsigned int run() {
		fout.open(outputpath, fstream::out);
		if (!fout.is_open()) {
			cerr << "Fail to open output file";
			exit(1);
		}

		forest = new Tree(vector<unsigned int>(0, 0));

		generateC1();
		generateL1();
		cout << L1.size() << endl;
		for (auto v : L1) {
			cout << (char)('a' + v.first) << " " << v.second << endl;
		}
		buildTree();

		unsigned int Llensum = 0;
		/*
		clock_t start_time;
		while (root->child.size()) {
			start_time = clock();
			outputFile();
			if (isShowDetailedTime)
				printf("  outputFile, generateCtemp tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);

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
*/
		fout.close();

		return Llensum;
	}
	void showDetailedTime(bool setting) {
		isShowDetailedTime = setting;
	}

   private:
	inline unsigned int readint() {
		fin.read((char *)&tempa, 4);
		return tempa;
	}

	void generateC1() {
		fin.open(inputpath, fstream::in | fstream::binary);
		if (!fin.is_open()) {
			cerr << "Fail to open input file";
			exit(1);
		}

		while (true) {
			readint();
			if (fin.eof()) break;
			readint();
			cnt = readint();
			while (cnt--) {
				tempn = readint();
				cout << (char)('a' + tempn) << " ";
				C1[tempn]++;
			}
			cout << endl;
		}
		fin.close();
	}

	static bool L1Cmp(const pair<int, int> &a, const pair<int, int> &b) {
		return a.second > b.second;
	}

	void generateL1() {
		for (auto &v : C1) {
			if (v.second >= support) {
				L1.push_back(make_pair(v.first, v.second));
			}
		}
		sort(L1.begin(), L1.end(), L1Cmp);
		L1.clear();
		L1.push_back(make_pair(5, 4));
		L1.push_back(make_pair(2, 4));
		L1.push_back(make_pair(0, 3));
		L1.push_back(make_pair(1, 3));
		L1.push_back(make_pair(12, 3));
		L1.push_back(make_pair(15, 3));
		for (auto &v : L1) {
			HeaderTableNode *htn = new HeaderTableNode();
			forest->header_table.push_back(make_pair(v.first, htn));
			forest->header_table_pointer[v.first] = htn;
		}
	}

	void buildTree() {
		fin.open(inputpath, fstream::in | fstream::binary);
		if (!fin.is_open()) {
			cerr << "Fail to open input file";
			exit(1);
		}

		while (true) {
			readint();
			if (fin.eof()) break;
			readint();
			cnt = readint();
			unordered_map<unsigned int, bool> items;
			while (cnt--) {
				tempn = readint();
				cout << (char)('a' + tempn) << " ";
				if (forest->header_table_pointer.find(tempn) != forest->header_table_pointer.end()) {
					items[tempn] = true;
					cout << "+ ";
				} else {
					cout << "- ";
				}
			}
			cout << endl;
			TreeNode *now = forest->tree;
			for (auto &item : L1) {
				if (items.find(item.first) != items.end()) {
					cout << (char)('a' + item.first) << endl;
					if (now->child.find(item.first) == now->child.end()) {
						now->child[item.first] = new TreeNode(item.first);
						now->child[item.first]->parent = now;
						if (forest->header_table_pointer[item.first]->end == nullptr) {
							forest->header_table_pointer[item.first]->start = now->child[item.first];
							forest->header_table_pointer[item.first]->end = now->child[item.first];
						} else {
							forest->header_table_pointer[item.first]->end->next = now->child[item.first];
							forest->header_table_pointer[item.first]->end = now->child[item.first];
						}
					}
					now = now->child[item.first];
					now->count++;
				}
			}
			cout << endl;
			dumpTree(forest);
		}
		fin.close();
	}
	/*
	void dfsOutputFile(Node *&now, vector<unsigned int> item) {
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
					for (auto j = next(i, 1); j != now->child.end(); j++) {
						Ctemp.push_back(item);
						tempa = i->first;
						tempb = j->first;
						if (tempa > tempb) swap(tempa, tempb);
						Ctemp[Ctemp.size() - 1].push_back(tempa);
						Ctemp[Ctemp.size() - 1].push_back(tempb);
					}
				}
			}
		}
		if (now->level < grouplen) {
			for (auto &next : now->child) {
				vector<unsigned int> nextitem = item;
				nextitem.push_back(next.first);
				dfsOutputFile(next.second, nextitem);
			}
		}
	}

	void outputFile() {
		Ctemp.clear();

		dfsOutputFile(root, vector<unsigned int>());
	}
	void generateC() {
		if (grouplen == 2 || grouplen < CtempMin || CtempMax < grouplen) {
			Cits = Ctemp;
			return;
		}
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

	void generateCsup() {
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
		fin.open(inputpath, fstream::in | fstream::binary);
		if (!fin.is_open()) {
			cerr << "Fail to open input file";
			exit(1);
		}
		unsigned int cnt;
#ifdef COUNTING_CATEGORY
		unordered_map<unsigned int, vector<Node *>> nownode;
#else
		vector<Node *> nownode;
#endif
		while (true) {
			tempn = readint();
			if (fin.eof()) break;
			readint();
			cnt = readint();
			nownode.clear();
#ifdef COUNTING_CATEGORY
			for (auto &node : root->child) {
				nownode[node.first].push_back(node.second);
			}
#else
			nownode.push_back(root);
#endif
			while (cnt--) {
				tempn = readint();
#ifdef COUNTING_CATEGORY
				for (int i = nownode[tempn].size() - 1; i >= 0; i--) {
					if (nownode[tempn][i]->level == grouplen) {
						Csup[nownode[tempn][i]]++;
					} else {
						for (auto &node : nownode[tempn][i]->child) {
							nownode[node.first].push_back(node.second);
						}
					}
				}
#else
				for (int i = nownode.size() - 1; i >= 0; i--) {
					if (nownode[i]->child.find(tempn) != nownode[i]->child.end()) {
						if (nownode[i]->child[tempn]->level == grouplen) {
							Csup[nownode[i]->child[tempn]]++;
						} else {
							nownode.push_back(nownode[i]->child[tempn]);
						}
					}
				}
#endif
			}
		}
		fin.close();
		if (isShowDetailedTime)
			printf("  cntSup tooks %d milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);
	}

	void dfsGenerateL(Node *&now, vector<unsigned int> item) {
		if (now->level == grouplen - 1) {
			for (auto next = now->child.begin(); next != now->child.end();) {
				if (Csup[next->second] < support) {
					next = now->child.erase(next);
				} else {
					// generateLset
					if (CtempMin <= grouplen + 1 && grouplen + 1 <= CtempMax) {
						vector<unsigned int> nextitem = item;
						nextitem.push_back(next->first);
						Lset.insert(nextitem);
					}
					// count L size
					Llen++;
					next++;
				}
			}
		} else if (now->level < grouplen - 1) {
			for (auto &next : now->child) {
				vector<unsigned int> nextitem = item;
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

	void generateL() {
		if (CtempMin <= grouplen + 1 && grouplen + 1 <= CtempMax) {
			Lset.clear();
		}

		dfsGenerateL(root, vector<unsigned int>());
	}
*/
	// Debug
	void dumpTree(Tree *tree) {
		dfsPrintTree(tree->tree);
		for (auto iter = tree->header_table_pointer.begin(); iter != tree->header_table_pointer.end(); ++iter) {
			auto now = iter->second->start;
			while (now != nullptr && now->next != nullptr) {
				cout << now << "-" << (char)('a' + now->item) << "-" << now->count << " "
					 << now->next << "-" << (char)('a' + now->next->item) << "-" << now->next->count << " *"
					 << endl;

				now = now->next;
			}
		}
		cout << endl
			 << "----------"
			 << endl;
	}
	void dfsPrintTree(TreeNode *&now) {
		for (auto iter = now->child.begin(); iter != now->child.end(); ++iter) {
			cout << now << "-" << (char)('a' + now->item) << "-" << now->count << " "
				 << iter->second << "-" << (char)('a' + iter->second->item) << "-" << iter->second->count << endl;
			dfsPrintTree(iter->second);
		}
	}
	/*
	void printTrie() {
		cout << "------Trie------\n";
		dfsPrintTrie(root, vector<unsigned int>());
		cout << "------Trie end----------\n";
	}
	*/
};
