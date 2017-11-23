#include <cstdlib>
#include <vector>

/*
 * Normal pointer treap
 */

struct Node {
	const int data, y;
	int size, sum;
	Node *left, *right;
	Node(int _data) : 
		data(_data), sum(_data), y(rand()), size(1), left(NULL), right(NULL) { }	
};

class ImplicitTreap {
private:
	Node *root;
	Node *Refresh(Node *node) {
		node->size = 1 + Size(node->left) + Size(node->right);
		node->sum = node->data + Sum(node->left) + Sum(node->right);
		return node;
	}
	int Size(Node *node) {
		if (node == NULL) return 0;
		return node->size;
	}
	int Sum(Node *node) {
		if (node == NULL) return 0;
		return node->sum;
	}
	void Split(Node *node, int i, Node* &a, Node* &b) {
		Node *aux;
		if (node == NULL) {
			a = b = NULL;
		}
		else if (Size(node->left) < i) {
			Split(node->right, i - Size(node->left) - 1, aux, b);
			node->right = aux;
			a = Refresh(node);
		}
		else {
			Split(node->left, i, a, aux);
			node->left = aux;
			b = Refresh(node);
		}
	}
	Node* Merge(Node *a, Node *b) {
		if (a == NULL) return b;
		else if (b == NULL) return a;
		if(a->y < b->y) {
			a->right = Merge(a->right, b);
			return Refresh(a);
		}
		else {
			b->left = Merge(a, b->left);
			return Refresh(b);
		}
	}
	Node* At(Node* node, int i) {
		if (node == NULL) return NULL;
		Refresh(node);
		if (i < Size(node->left)) return At(node->left, i);
		else if (i == Size(node->left)) return node;
		else return At(node->right, i - Size(node->left) - 1);
	}
	void Delete(Node *node) {
		if (node == NULL) return;
		Delete(node->left);
		Delete(node->right);
		delete node;
	}
public:
	ImplicitTreap() { root = NULL; }
	~ImplicitTreap() { Clear(); }
	void Clear() {
		Delete(root);
		root = NULL;
	}
	int At(int i) {
		return At(root, i)->data;
	}
	void Insert(int i, int v) {
		Node *leftTree, *rightTree;
		Split(root, i, leftTree, rightTree);
		root = Merge(leftTree, Merge(new Node(v), rightTree));
	}
	void Erase(int i) {
		Node *leftTree, *midRightTree, *mid, *rightTree;
		Split(root, i, leftTree, midRightTree);
		Split(midRightTree, 1, mid, rightTree);
		root = Merge(leftTree, rightTree);
		if (mid != NULL) delete mid;
	}
	int Query(int i, int j) {
		if (i > j) std::swap(i, j);
		Node *leftTree, *midRightTree, *midTree, *rightTree;
		Split(root, i, leftTree, midRightTree);
		Split(midRightTree, j - i + 1, midTree, rightTree);
		int sum = Sum(midTree);
		root = Merge(leftTree, Merge(midTree, rightTree));
		return sum;
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

class PersistentImplicitTreap {
private:
	std::vector<Node*> root;
	std::vector<Node*> nodes;
	Node *Newnode(int x) {
		nodes.push_back(new Node(x));
		return nodes.back();
	}
	Node *Refresh(Node *node) {
		node->size = 1 + Size(node->left) + Size(node->right);
		node->sum = node->data + Sum(node->left) + Sum(node->right);
		return node;
	}
	int Size(Node *node) {
		if (node == NULL) return 0;
		return node->size;
	}
	int Sum(Node *node) {
		if (node == NULL) return 0;
		return node->sum;
	}
	void Split(Node *node, int i, Node* &a, Node* &b) {
		Node *aux;
		if (node == NULL) {
			a = b = NULL;
			return;
		}
		Node *copy = Newnode(node->data);
		copy->left = node->left;
		copy->right = node->right;
		if (Size(copy->left) < i) {
			Split(copy->right, i - Size(copy->left) - 1, aux, b);
			copy->right = aux;
			a = Refresh(copy);
		}
		else {
			Split(copy->left, i, a, aux);
			copy->left = aux;
			b = Refresh(copy);
		}
	}
	Node* Merge(Node* a, Node* b) {
		if (a == NULL) return b;
		else if (b == NULL) return a;
		if(a->y < b->y) {
			a->right = Merge(a->right, b);
			return Refresh(a);
		}
		else {
			b->left = Merge(a, b->left);
			return Refresh(b);
		}
	}
	Node* At(Node* node, int i) {
		if (node == NULL) return NULL;
		Refresh(node);
		if (i < Size(node->left)) return At(node->left, i);
		else if (i == Size(node->left)) return node;
		else return At(node->right, i - Size(node->left) - 1);
	}
public:
	PersistentImplicitTreap() { Clear(); }
	~PersistentImplicitTreap() { Clear(); }
	void Clear() {
		for(int i = 0; i < (int)nodes.size(); i++) {
			delete nodes[i];
		}
		nodes.clear();
		root.clear();
		root.push_back(NULL);
	}
	int At(int ver, int i) {
		return At(root[ver], i)->data;
	}
	int Insert(int ver, int i, int v) {
		Node *leftTree, *rightTree;
		Split(root[ver], i, leftTree, rightTree);
		root.push_back(Merge(leftTree, Merge(Newnode(v), rightTree)));
		return int(root.size()) - 1;
	}
	int Erase(int ver, int i) {
		Node *leftTree, *midRightTree, *mid, *rightTree;
		Split(root[ver], i, leftTree, midRightTree);
		Split(midRightTree, 1, mid, rightTree);
		root.push_back(Merge(leftTree, rightTree));
		return int(root.size()) - 1;
	}
	int Query(int ver, int i, int j) {
		if (i > j) std::swap(i, j);
		Node *leftTree, *midRightTree, *midTree, *rightTree;
		Split(root[ver], i, leftTree, midRightTree);
		Split(midRightTree, j - i + 1, midTree, rightTree);
		int sum = Sum(midTree);
		return sum;
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

#include <cstdio>
#include <algorithm>
#include <ctime>
#define NTESTS 10000
#define NQUERY 10
#define RANGE 100

void Insert(std::vector<int> & arr, int i, int v) {
	arr.insert(arr.begin()+i, v);
}

int Query(std::vector<int> & arr, int i, int j) {
	if (i > j) std::swap(i, j);
	int sum = 0;
	for(int it=i; it<=j && it<(int)arr.size(); it++) {
		sum += arr[it];
	}
	return sum;
}

void Erase(std::vector<int> & arr, int i) {
	arr.erase(arr.begin()+i);
}

bool TestNormalTreap() {
	srand(time(NULL));
	std::vector<int> control;
	ImplicitTreap treap;
	for(int test = 1; test <= NTESTS; test++) {
		int v = rand()%RANGE;
		int i = rand()%int(control.size()+1);
		Insert(control, i, v);
		treap.Insert(i, v);
		if (test%5 == 0) {
			i = rand()%int(control.size());
			Erase(control, i);
			treap.Erase(i);
		}
		if (control.size() != treap.Size()) {
			printf("failed on test %d, size: %u-%d\n", test, control.size(), treap.Size());
			return false;
		}
		for(int i = 0; i < (int)control.size(); i++) {
			if (control[i] != treap.At(i)) {
				printf("failed on test %d, id %d: %d-%d\n", test, i, control[i], treap.At(i));
				return false;
			}
		}
		for(int query = 0; query < NQUERY; query++) {
			int i = rand()%int(control.size()+1);
			int j = rand()%int(control.size()+1);
			int sum1 = Query(control, i, j);
			int sum2 = treap.Query(i, j);
			if (sum1 != sum2) {
				printf("failed on test %d, query %d: %d-%d\n", test, query, sum1, sum2);
				return false;
			}
		}
	}
	printf("All normal implicit treap tests passed\n");
	return true;
}

bool TestPersistentTreap() {
	srand(time(NULL));
	std::vector<int> control[NTESTS];
	PersistentImplicitTreap treap;
	for(int test = 1; test < NTESTS; test++) {
		int ver = rand()%test;
		control[test] = control[ver];
		if (test%5 != 0 || true) {
			int v = rand()%RANGE;
			int i = rand()%int(control[test].size()+1);
			Insert(control[test], i, v);
			treap.Insert(ver, i, v);
		}
		else {
			if (control[test].empty()) {
				test--;
				continue;
			}
			int i = rand()%int(control[test].size());
			Erase(control[test], i);
			treap.Erase(ver, i);
		}
	}
	for(int test = 0; test < NTESTS; test++) {
		if (control[test].size() != treap.Size(test)) {
			printf("failed on test %d, size: %u-%d\n", test, control[test].size(), treap.Size(test));
			return false;
		}
		for(int i = 0; i < (int)control[test].size(); i++) {
			if (control[test][i] != treap.At(test, i)) {
				printf("failed on test %d, id %d: %d-%d\n", test, i, control[test][i], treap.At(test, i));
				return false;
			}
		}
		for(int query = 0; query < NQUERY; query++) {
			int i = rand()%int(control[test].size()+1);
			int j = rand()%int(control[test].size()+1);
			int sum1 = Query(control[test], i, j);
			int sum2 = treap.Query(test, i, j);
			if (sum1 != sum2) {
				printf("failed on test %d, query %d: %d-%d\n", test, query, sum1, sum2);
				return false;
			}
		}
	}
	printf("All persistent implicit treap tests passed\n");
	return true;
}

int main(){
	TestNormalTreap();
	TestPersistentTreap();
	return 0;
}
