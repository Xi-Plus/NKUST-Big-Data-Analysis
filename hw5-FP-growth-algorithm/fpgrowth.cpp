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
		TreeNode(unsigned int _item, unsigned int _count) {
			item = _item;
			count = _count;
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
			tree = new TreeNode(INT_MAX);
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
	vector<pair<unsigned int, unsigned int>> L1;
	Tree *forest;
	unsigned int Llensum = 0;
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

		Llensum = 0;

		generateC1();
		generateL1();
		cout << "L1: " << L1.size() << endl;
		// for (auto v : L1) {
		// 	cout << _format_char(v.first) << ":" << v.second << endl;
		// }

		buildTree();
		// return 0;

		for (auto it = forest->header_table.rbegin(); it != forest->header_table.rend(); it++) {
			// cout << it->first << endl;
			buildSubTree(forest, it->first);
		}

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
		if (a.second == b.second) {
			return a.first < b.first;
		}
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

		// allTreeCount++;

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
					now = addItemToTreeNode(forest, now, item.first);
				}
			}
			// cout << endl;
			// dumpTree(forest);
		}
		fin.close();
	}

	TreeNode *addItemToTreeNode(Tree *tree, TreeNode *treeNode, unsigned int item, unsigned int count = 1) {
		if (treeNode->child.find(item) == treeNode->child.end()) {
			treeNode->child[item] = new TreeNode(item, count);
			treeNode->child[item]->parent = treeNode;
			if (tree->header_table_pointer.find(item) == tree->header_table_pointer.end()) {
				tree->header_table_pointer[item] = new HeaderTableNode();
			}
			if (tree->header_table_pointer[item]->end == nullptr) {
				tree->header_table_pointer[item]->start = treeNode->child[item];
				tree->header_table_pointer[item]->end = treeNode->child[item];
			} else {
				tree->header_table_pointer[item]->end->next = treeNode->child[item];
				tree->header_table_pointer[item]->end = treeNode->child[item];
			}
		} else {
			treeNode->child[item]->count += count;
		}
		return treeNode->child[item];
	}

	void pruning(Tree *tree, unsigned int item) {
		if (tree->header_table_pointer.find(item) == tree->header_table_pointer.end()) {
			return;
		}
		TreeNode *node = tree->header_table_pointer[item]->start;
		while (node != nullptr) {
			node->child.clear();
			node->parent->child[node->item] = nullptr;
			auto toDel = node;
			node = node->next;
			free(toDel);
		}
		tree->header_table_pointer.erase(item);
	}

	void buildSubTree(Tree *fromTree, unsigned int leafItem) {
		// cout << "buildSubTree " << _format_char(leafItem);
		// cout << " Base on ";
		// for (auto v : fromTree->prefix) {
		// 	cout << _format_char(v) << " ";
		// }
		// cout << endl;

		TreeNode *leaf = fromTree->header_table_pointer[leafItem]->start;

		// if (fromTree->prefix.size() >= 7) {
		// cout << "buildSubTree " << _format_char(leafItem);
		// cout << " Base on ";
		// for (auto v : fromTree->prefix) {
		// 	cout << _format_char(v) << " ";
		// }
		// cout << endl;
		// dumpTree(fromTree);
		// }

		vector<unsigned int> prefix(fromTree->prefix);
		prefix.push_back(leafItem);
		Tree *subTree = new Tree(prefix);
		fromTree->child_tree[leafItem] = subTree;

		unordered_map<unsigned int, unsigned int> count;
		while (leaf != nullptr) {
			// cout << "leaf " << leaf << endl;
			count[leaf->item] += leaf->count;

			TreeNode *now = leaf->parent;
			vector<unsigned int> pathItems;
			while (now->parent != nullptr) {
				count[now->item] += leaf->count;
				pathItems.push_back(now->item);
				// cout << now << " Add " << _format_char(now->item) << " " << leaf->count << endl;
				now = now->parent;
			}

			reverse(pathItems.begin(), pathItems.end());
			now = subTree->tree;
			for (auto &item : pathItems) {
				now = addItemToTreeNode(subTree, now, item, leaf->count);
			}

			// Move next leaf
			leaf = leaf->next;
		}

		// Output file
		// cout << "Ouput " << count[leafItem] << endl;
		for (auto &v : fromTree->prefix) {
			fout << _format_char(v) << ",";
		}
		fout << _format_char(leafItem);
		fout << ":" << count[leafItem] << "\n";
		Llensum++;
		// cout << endl;

		// pruning
		for (auto it = count.begin(); it != count.end(); it++) {
			if (it->second < support) {
				// cout << "pruning " << _format_char(it->first) << "(" << it->second << ")" << endl;
				pruning(subTree, it->first);
				// cout << "Next " << _format_char(it->first) << " " << it->second << " ";
			}
		}

		for (auto it = subTree->header_table_pointer.begin(); it != subTree->header_table_pointer.end(); it++) {
			buildSubTree(subTree, it->first);
		}

		freeTree(fromTree, leafItem);
	}

	void freeTree(Tree *fromTree, unsigned int leafItem) {
		// allTreeCount--;
		// cout << "freeTree " << _format_char(leafItem);
		// cout << " Base on ";
		// for (auto v : fromTree->prefix) {
		// 	cout << _format_char(v) << " ";
		// }
		// cout << " " << allTreeCount << endl;

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
		// dumpHeaderTable(tree);
		cout << endl
			 << "----------"
			 << endl;
	}
	void dfsPrintTree(TreeNode *&now) {
		for (auto iter = now->child.begin(); iter != now->child.end(); ++iter) {
			cout << now << "-" << _format_char(now->item) << "-" << now->count << " "
				 << iter->second;
			if (iter->second == 0) {
				cout << 0 << "-" << 0 << endl;
			} else {
				cout << _format_char(iter->second->item) << "-" << iter->second->count << endl;
				dfsPrintTree(iter->second);
			}
		}
	}
	void dumpHeaderTable(Tree *tree) {
		for (auto iter = tree->header_table_pointer.begin(); iter != tree->header_table_pointer.end(); ++iter) {
			cout << iter->second << " "
				 << iter->second->start << "-" << _format_char(iter->second->start->item) << "-" << iter->second->start->count
				 << " start" << endl;
			cout << iter->second << " "
				 << iter->second->end << "-" << _format_char(iter->second->end->item) << "-" << iter->second->end->count
				 << " end" << endl;
			auto now = iter->second->start;
			while (now != nullptr && now->next != nullptr) {
				cout << now << "-" << _format_char(now->item) << "-" << now->count << " "
					 << now->next << "-" << _format_char(now->next->item) << "-" << now->next->count << " next"
					 << endl;

				now = now->next;
			}
		}
	}
};
