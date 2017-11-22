#include <cstdlib>
#include <vector>

/*
 * Normal pointer treap
 */

struct Node {
	int x, y, size;
	Node *left, *right;
	Node(int _x) : 
		x(_x), y(rand()), size(1), left(NULL), right(NULL) { }	
};

class Treap {
private:
	Node *root;
	Node *Refresh(Node *node) {
		node->size = 1 + Size(node->left) + Size(node->right);
		return node;
	}
	int Size(Node *node) {
		if (node == NULL) return 0;
		return node->size;
	}
	void Split(Node *node, int x, Node* &a, Node* &b) {
		Node *aux;
		if (node == NULL) {
			a = b = NULL;
		}
		else if (node->x < x) {
			Split(node->right, x, aux, b);
			node->right = aux;
			a = Refresh(node);
		}
		else {
			Split(node->left, x, a, aux);
			node->left = aux;
			b = Refresh(node);
		}
	}
	Node* Merge(Node *a, Node *b) {
		if (a == NULL) return b;
		else if (b == NULL) return a;
		if (a->y < b->y) {
			a->right = Merge(a->right, b);
			return Refresh(a);
		}
		else {
			b->left = Merge(a, b->left);
			return Refresh(b);
		}
	}
	Node* Count(Node* node, int x) {
		if (node == NULL) return NULL;
		else if (x == node->x) return node;
		else if (x < node->x) return Count(node->left, x);
		else return Count(node->right, x);
	}
	void Delete(Node *node) {
		if (node == NULL) return;
		Delete(node->left);
		Delete(node->right);
		delete node;
	}
public:
	Treap() { root = NULL; }
	~Treap() { Clear(); }
	void Clear() {
		Delete(root);
		root = NULL;
	}
	bool Count(int x) {
		return Count(root, x) != NULL;
	}
	void Insert(int x) {
		if (Count(root, x) != NULL) return;
		Node *leftTree, *rightTree;
		Split(root, x, leftTree, rightTree);
		root = Merge(leftTree, Merge(new Node(x), rightTree));
	}
	void Erase(int x) {
		Node *leftTree, *midRightTree, *mid, *rightTree;
		Split(root, x, leftTree, midRightTree);
		Split(midRightTree, x + 1, mid, rightTree);
		root = Merge(leftTree, rightTree);
		if (mid != NULL) delete mid;
	}
	int Size() {
		return Size(root);
	}
	bool Empty() {
		return Size(root) == 0;
	}

};

/*
 * Persistent treap
 */

class PersistentTreap {
private:
	std::vector<Node*> root;
	std::vector<Node*> nodes;
	Node *Newnode(int x) {
		nodes.push_back(new Node(x));
		return nodes.back();
	}
	Node *Refresh(Node *node) {
		node->size = 1 + Size(node->left) + Size(node->right);
		return node;
	}
	int Size(Node *node) {
		if (node == NULL) return 0;
		return node->size;
	}
	void Split(Node *node, int x, Node* &a, Node* &b) {
		Node *aux;
		if (node == NULL) {
			a = b = NULL;
			return;
		}
		Node *copy = Newnode(node->x);
		copy->left = node->left;
		copy->right = node->right;
		if (copy->x < x) {
			Split(copy->right, x, aux, b);
			copy->right = aux;
			a = Refresh(copy);
		}
		else {
			Split(copy->left, x, a, aux);
			copy->left = aux;
			b = Refresh(copy);
		}
	}
	Node* Merge(Node* a, Node* b) {
		if (a == NULL) return b;
		else if (b == NULL) return a;
		if (a->y < b->y) {
			a->right = Merge(a->right, b);
			return Refresh(a);
		}
		else {
			b->left = Merge(a, b->left);
			return Refresh(b);
		}
	}
	Node* Count(Node* node, int x) {
		if (node == NULL) return NULL;
		else if (x == node->x) return node;
		else if (x < node->x) return Count(node->left, x);
		else return Count(node->right, x);
	}
public:
	PersistentTreap() { Clear(); }
	~PersistentTreap() { Clear(); }
	void Clear() {
		for(int i = 0; i < (int)nodes.size(); i++) {
			delete nodes[i];
		}
		nodes.clear();
		root.clear();
		root.push_back(NULL);
	}
	bool Count(int ver, int x) {
		return Count(root[ver], x) != NULL;
	}
	int Insert(int ver, int x) {
		if (Count(root[ver], x) != NULL) return -1;
		Node *leftTree, *rightTree;
		Split(root[ver], x, leftTree, rightTree);
		root.push_back(Merge(leftTree, Merge(Newnode(x), rightTree)));
		return int(root.size()) - 1;
	}
	int Erase(int ver, int x) {
		Node *leftTree, *midRightTree, *mid, *rightTree;
		Split(root[ver], x, leftTree, midRightTree);
		Split(midRightTree, x + 1, mid, rightTree);
		root.push_back(Merge(leftTree, rightTree));
		return int(root.size()) - 1;
	}
	int Size(int ver) {
		return Size(root[ver]);
	}
	bool Empty(int ver) {
		return Size(root[ver]) == 0;
	}
	int LatestVersion() {
		return int(root.size())-1;
	}
};

/*
 * Unit tests
 */

#include <set>
#include <algorithm>
#include <ctime>
#include <cstdio>
#define NTESTS 10000
#define RANGE 100

bool TestNormalTreap() {
	srand(time(NULL));
	std::set<int> control;
	Treap treap;
	for(int test = 1; test <= NTESTS; test++) {
		int x = rand()%RANGE;
		if(!control.count(x)){
			control.insert(x);
			treap.Insert(x);
		}
		else{
			control.erase(x);
			treap.Erase(x);
		}
		for(x = 0; x < RANGE; x++) {
			if (control.count(x) != treap.Count(x)){
				printf("failed test %d, s.count(%d) = %d, t.count(%d) = %d\n", test, x, control.count(x), x, treap.Count(x));
				return false;
			}
		}
	}
	printf("All normal treap tests passed\n");
	return true;
}

bool TestPersistentTreap() {
	srand(time(NULL));
	std::set<int> control[NTESTS];
	PersistentTreap treap;
	for(int test = 1; test < NTESTS; test++) {
		int x = rand()%RANGE;
		int ver = rand()%test;
		control[test] = control[ver];
		if (control[test].count(x)) {
			control[test].erase(x);
			treap.Erase(ver, x);
		}
		else {
			control[test].insert(x);
			treap.Insert(ver, x);
		}
	}
	for(int test = 0; test < NTESTS; test++) {
		if (control[test].size() != treap.Size(test)) {
			printf("size failed on test %d: %u-%d\n", test, control[test].size(), treap.Size(test));
			return false;
		}
		for(int x = 0; x < RANGE; x++) {
			if (control[test].count(x) != treap.Count(test, x)){
				printf("failed test %d, control.count(%d) = %d, treap.count(%d) = %d\n",
					test, x, control[test].count(x), x, treap.Count(test, x));
				return false;
			}
		}
	}
	printf("All persistent treap tests passed\n");
	return true;
}

int main(){
	TestNormalTreap();
	TestPersistentTreap();
	return 0;
}
