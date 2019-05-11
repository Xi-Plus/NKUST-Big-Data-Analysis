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
		~TreeNode() {
			child.clear();
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
		~Tree() {
			header_table.clear();
			header_table_pointer.clear();
			child_tree.clear();
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
	int allTreeCount = 0;
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
		// cout << L1.size() << endl;
		// for (auto v : L1) {
		// 	cout << _format_char(v.first) << " " << v.second << endl;
		// }
		buildTree();

		for (auto it = forest->header_table.rbegin(); it != forest->header_table.rend(); it++) {
			buildSubTree(forest, it->first);
		}

		unsigned int Llensum = 0;
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
				// cout << _format_char(tempn) << " ";
				C1[tempn]++;
			}
			// cout << endl;
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
		// L1.clear();
		// L1.push_back(make_pair(5, 4));
		// L1.push_back(make_pair(2, 4));
		// L1.push_back(make_pair(0, 3));
		// L1.push_back(make_pair(1, 3));
		// L1.push_back(make_pair(12, 3));
		// L1.push_back(make_pair(15, 3));
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

		allTreeCount++;

		while (true) {
			readint();
			if (fin.eof()) break;
			readint();
			cnt = readint();
			unordered_map<unsigned int, bool> items;
			while (cnt--) {
				tempn = readint();
				// cout << _format_char(tempn) << " ";
				if (forest->header_table_pointer.find(tempn) != forest->header_table_pointer.end()) {
					items[tempn] = true;
					// cout << "+ ";
				} else {
					// cout << "- ";
				}
			}
			// cout << endl;
			TreeNode *now = forest->tree;
			for (auto &item : L1) {
				if (items.find(item.first) != items.end()) {
					// cout << _format_char(item.first) << endl;
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
			// cout << endl;
			// dumpTree(forest);
		}
		fin.close();
	}

	void buildSubTree(Tree *fromTree, unsigned int leafItem) {
		cout << "buildSubTree " << _format_char(leafItem);
		cout << " Base on ";
		for (auto v : fromTree->prefix) {
			cout << _format_char(v) << " ";
		}
		cout << " " << allTreeCount << endl;
		TreeNode *leaf = fromTree->header_table_pointer[leafItem]->start;

		dumpTree(fromTree);

		unordered_map<unsigned int, unsigned int> count;
		while (leaf != nullptr) {
			// cout << "leaf " << leaf << endl;
			TreeNode *now = leaf;
			while (now->parent != nullptr) {
				count[now->item] += leaf->count;
				// cout << now << " Add " << _format_char(now->item) << " " << leaf->count << endl;
				now = now->parent;
			}
			leaf = leaf->next;
		}

		// Output file
		// cout << "Ouput " << count[leafItem] << endl;
		for (auto &v : fromTree->prefix) {
			fout << _format_char(v) << ",";
		}
		fout << _format_char(leafItem);
		fout << ":" << count[leafItem] << "\n";
		// cout << endl;

		// Build next tree
		vector<pair<unsigned int, unsigned int>> L;
		for (auto it = count.begin(); it != count.end(); it++) {
			// cout << _format_char(it->first) << " " << it->second << endl;
			if (it->second >= support && it->first != leafItem) {
				L.push_back(make_pair(it->first, it->second));
				// cout << "Next " << _format_char(it->first) << " " << it->second << endl;
			}
		}
		if (L.size() == 0) {
			// cout << "No next tree" << endl;
			return;
		}
		sort(L.begin(), L.end(), L1Cmp);

		vector<unsigned int> prefix(fromTree->prefix);
		prefix.push_back(leafItem);
		fromTree->child_tree[leafItem] = new Tree(prefix);
		allTreeCount++;

		TreeNode *now = fromTree->child_tree[leafItem]->tree;
		// cout << "Build sub tree" << endl;
		for (auto &item : L) {
			// cout << _format_char(item.first) << endl;
			now->child[item.first] = new TreeNode(item.first);
			now->child[item.first]->parent = now;

			HeaderTableNode *htn = new HeaderTableNode();
			fromTree->child_tree[leafItem]->header_table.push_back(make_pair(item.first, htn));
			fromTree->child_tree[leafItem]->header_table_pointer[item.first] = htn;
			fromTree->child_tree[leafItem]->header_table_pointer[item.first]->start = now->child[item.first];
			fromTree->child_tree[leafItem]->header_table_pointer[item.first]->end = now->child[item.first];

			now = now->child[item.first];
			now->count = item.second;

			dumpTree(fromTree->child_tree[leafItem]);
		}
		// cout << endl;

		for (auto it = L.rbegin(); it != L.rend(); it++) {
			buildSubTree(fromTree->child_tree[leafItem], it->first);
		}

		freeTree(fromTree, leafItem);
	}

	void freeTree(Tree *fromTree, unsigned int leafItem) {
		allTreeCount--;
		cout << "freeTree " << _format_char(leafItem);
		cout << " Base on ";
		for (auto v : fromTree->prefix) {
			cout << _format_char(v) << " ";
		}
		cout << " " << allTreeCount << endl;
		free(fromTree->child_tree[leafItem]);
	}

	// char _format_char(unsigned int item) {
	// 	return 'a' + item;
	// }
	unsigned int _format_char(unsigned int item) {
		return item;
	}

	// Debug
	void dumpTree(Tree *tree) {
		dfsPrintTree(tree->tree);
		for (auto iter = tree->header_table_pointer.begin(); iter != tree->header_table_pointer.end(); ++iter) {
			auto now = iter->second->start;
			while (now != nullptr && now->next != nullptr) {
				cout << now << "-" << _format_char(now->item) << "-" << now->count << " "
					 << now->next << "-" << _format_char(now->next->item) << "-" << now->next->count << " *"
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
			cout << now << "-" << _format_char(now->item) << "-" << now->count << " "
				 << iter->second << "-" << _format_char(iter->second->item) << "-" << iter->second->count << endl;
			dfsPrintTree(iter->second);
		}
	}
};
