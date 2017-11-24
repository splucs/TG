#include <cstdlib>
#include <vector>

/*
 * Normal pointer segment tree
 */

struct Node {
	int data;
	const int low, high;
	Node *left, *right;
	Node(int _data, int _low, int _high) :
		data(_data), low(_low), high(_high), left(NULL), right(NULL) { }
};

class SegmentTree {
private:
	Node *root;
	int size;
	Node *Build(int low, int high, const int arr[]) {
		int mid = (low + high) / 2;
		if (low == high) return new Node(arr[mid], low, high);
		else {
			Node *node = new Node(0, low, high);
			node->left = Build(low, mid, arr);
			node->right = Build(mid + 1, high, arr);
			node->data = node->right->data + node->left->data;
			return node;
		}
	}
	void Update(Node *node, int i, int data) {
		int high = node->high;
		int low = node->low;
		int mid = (low + high) / 2;
		if (i > high || i < low) return;
		if (low == high) node->data = data;
		else {
			Update(node->left, i, data);
			Update(node->right, i, data);
			node->data = node->right->data + node->left->data;
		}
	}
	int Query(Node *node, int i, int j) {
		int high = node->high;
		int low = node->low;
		if (high < i || low > j) return 0;
		else if (low >= i && high <= j) return node->data;
		else return Query(node->left, i, j) + Query(node->right, i, j);
	}
	void Delete(Node *node) {
		if (node == NULL) return;
		Delete(node->left);
		Delete(node->right);
		delete node;
	}
public:
	SegmentTree(const int *begin, const int *end) {
		size = int(end-begin);
		root = Build(0, size - 1, begin);
	}
	~SegmentTree() { Delete(root); }
	int Query(int i, int j) {
		return Query(root, i, j);
	}
	void Update(int i, int data) {
		Update(root, i, data);
	}
};

/*
 * Persistent pointer segment tree
 */

class PersistentSegmentTree {
private:
	std::vector<Node*> root;
	std::vector<Node*> nodes;
	int size;
	Node *Newnode(int x, int low, int high) {
		nodes.push_back(new Node(x, low, high));
		return nodes.back();
	}
	Node *Build(int low, int high, const int arr[]) {
		int mid = (low + high) / 2;
		if (low == high) return Newnode(arr[mid], low, high);
		else {
			Node *node = Newnode(0, low, high);
			node->left = Build(low, mid, arr);
			node->right = Build(mid + 1, high, arr);
			node->data = node->right->data + node->left->data;
			return node;
		}
	}
	Node *Update(Node *node, int i, int data) {
		int high = node->high;
		int low = node->low;
		int mid = (low + high) / 2;
		if (i > high || i < low) return node;
		Node *copy = Newnode(node->data, low, high);
		if (low == high) copy->data = data;
		else {
			copy->left = Update(node->left, i, data);
			copy->right = Update(node->right, i, data);
			copy->data = copy->right->data + copy->left->data;
		}
		return copy;
	}
	int Query(Node *node, int i, int j) {
		int high = node->high;
		int low = node->low;
		if (high < i || low > j) return 0;
		else if (low >= i && high <= j) return node->data;
		else return Query(node->left, i, j) + Query(node->right, i, j);
	}
	void Delete(Node *node) {
		if (node == NULL) return;
		Delete(node->left);
		Delete(node->right);
		delete node;
	}
public:
	PersistentSegmentTree(const int *begin, const int *end) {
		size = int(end-begin);
		root.push_back(Build(0, size - 1, begin));
	}
	~PersistentSegmentTree() { 
		for(int i = 0; i < (int)nodes.size(); i++) {
			delete nodes[i];
		}
	}
	int Query(int ver, int i, int j) {
		return Query(root[ver], i, j);
	}
	int Update(int ver, int i, int data) {
		root.push_back(Update(root[ver], i, data));
		return int(root.size()) - 1;
	}
	int LatestVersion() {
		return int(root.size())-1;
	}
};

/*
 * Unit tests
 */

#include <cstdio>
#include <ctime>
#define NTESTS 10000
#define NQUERY 10
#define SIZE 1000
#define RANGE 100

int Sum(const std::vector<int> & arr, int i, int j) {
	int sum = 0;
	for(int it = i; it <= j; it++) {
		sum += arr[it];
	}
	return sum;
}

bool TestSegmentTree() {
	srand(time(NULL));
	std::vector<int> control(SIZE);
	for(int i = 0; i < SIZE; i++) {
		control[i] = rand()%RANGE;
	}
	SegmentTree segTree(&control[0], (&control[0])+SIZE);
	for(int test = 1; test <= NTESTS; test++) {
		int i = rand()%SIZE;
		int data = rand()%RANGE;
		control[i] = data;
		segTree.Update(i, data);
		for(int i = 0; i < SIZE; i++) {
			int value1 = control[i];
			int value2 = segTree.Query(i, i);
			if (value1 != value2) {
				printf("failed on test %d, id %d = %d-%d", test, i, value1, value2);
				return false;
			}
		}
		for(int query = 1; query <= NQUERY; query++) {
			int i = rand()%SIZE;
			int j = rand()%SIZE;
			if (i > j) std::swap(i, j);
			int sum1 = Sum(control, i, j);
			int sum2 = segTree.Query(i, j);
			if (sum1 != sum2) {
				printf("failed on test %d, query %d = %d-%d", test, query, sum1, sum2);
				return false;
			}
		}
	}
	printf("All normal segment tree tests passed.\n");
	return true;
}

bool TestPersistentSegmentTree() {
	srand(time(NULL));
	std::vector<int> control[NTESTS];
	control[0].resize(SIZE);
	for(int i = 0; i < SIZE; i++) {
		control[0][i] = rand()%RANGE;
	}
	PersistentSegmentTree segTree(&control[0][0], (&control[0][0])+SIZE);
	for(int test = 1; test <= NTESTS; test++) {
		int ver = rand()%test;
		int i = rand()%SIZE;
		int data = rand()%RANGE;
		control[test] = control[ver];
		control[test][i] = data;
		segTree.Update(ver, i, data);
	}
	for(int test = 0; test < NTESTS; test++) {
		for(int i = 0; i < SIZE; i++) {
			int value1 = control[test][i];
			int value2 = segTree.Query(test, i, i);
			if (value1 != value2) {
				printf("failed on test %d, id %d = %d-%d", test, i, value1, value2);
				return false;
			}
		}
		for(int query = 1; query <= NQUERY; query++) {
			int i = rand()%SIZE;
			int j = rand()%SIZE;
			if (i > j) std::swap(i, j);
			int sum1 = Sum(control[test], i, j);
			int sum2 = segTree.Query(test, i, j);
			if (sum1 != sum2) {
				printf("failed on test %d, query %d = %d-%d", test, query, sum1, sum2);
				return false;
			}
		}
	}
	printf("All persistent segment tree tests passed.\n");
	return true;
}

int main() {
	TestSegmentTree();
	TestPersistentSegmentTree();
	return 0;
}