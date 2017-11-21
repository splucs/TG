#include <cstdlib>
#include <vector>
#define INF (1<<30)

/*
 * Normal pointer heap
 */

struct Node {
	int data, size;
	Node *left, *right;
	Node(int _data) :
		data(_data), left(NULL), right(NULL), size(1) { }
};
	
class Heap {
private:
	Node *root;
	void Delete(Node *node) {
		if (node == NULL) return;
		Delete(node->left);
		Delete(node->right);
		delete node;
	}
	void Refresh(Node *node) {
		node->size = 1 + Size(node->left) + Size(node->right);
	}
	int Size(Node *node) {
		if (node == NULL) return 0;
		return node->size;
	}
	int Value(Node *node) {
		if (node == NULL) return -INF;
		return node->data;
	}
	Node *Push(Node *node, int data) {
		if (node == NULL) return new Node(data);
		if (Size(node->right) < Size(node->left)) {
			node->right = Push(node->right, data);
			if (node->right->data > node->data)
				std::swap(node->right->data, node->data);
		}
		else {
			node->left = Push(node->left, data);
			if (node->left->data > node->data)
				std::swap(node->left->data, node->data);
		}
		Refresh(node);
		return node;
	}
	Node *Pop(Node *node) {
		if (node == NULL) return NULL;
		if (node->left == NULL && node->right == NULL) {
			delete node;
			return NULL;
		}
		if (Value(node->left) > Value(node->right)) {
			node->data = node->left->data;
			node->left = Pop(node->left);
		}
		else  {
			node->data = node->right->data;
			node->right = Pop(node->right);
		}
		Refresh(node);
		return node;
	}
public:
	Heap() : root(NULL) { }
	~Heap() { Clear(); }
	void Clear() {
		Delete(root);
		root = NULL;
	}
	int Top() {
		return root->data;
	}
	void Push(int data) {
		root = Push(root, data);
	}
	void Pop() {
		root = Pop(root);
	}
	int Size() {
		return Size(root);
	}
	bool Empty() {
		return Size(root) == 0;
	}
};

/*
 * Persistent pointer heap
 */

class PersistentHeap {
private:
	std::vector<Node*> root;
	std::vector<Node*> nodes;
	Node* Newnode(int data = 0) {
		nodes.push_back(new Node(data));
		return nodes.back();
	}
	void Refresh(Node *node) {
		node->size = 1 + Size(node->left) + Size(node->right);
	}
	int Size(Node *node) {
		if (node == NULL) return 0;
		return node->size;
	}
	int Value(Node *node) {
		if (node == NULL) return -INF;
		return node->data;
	}
	Node *Push(Node *node, int data) {
		if (node == NULL) return Newnode(data);
		Node *copy = Newnode(node->data);
		if (Size(node->right) < Size(node->left)) {
			copy->right = Push(node->right, data);
			copy->left = node->left;
			if (copy->right->data > copy->data)
				std::swap(copy->right->data, copy->data);
		}
		else {
			copy->left = Push(node->left, data);
			copy->right = node->right;
			if (copy->left->data > copy->data)
				std::swap(copy->left->data, copy->data);
		}
		Refresh(copy);
		return copy;
	}
	Node *Pop(Node *node) {
		if (node == NULL) return NULL;
		if (node->left == NULL && node->right == NULL) {
			return NULL;
		}
		Node *copy = Newnode();
		if (Value(node->left) > Value(node->right)) {
			copy->data = node->left->data;
			copy->right = node->right;
			copy->left = Pop(node->left);
		}
		else {
			copy->data = node->right->data;
			copy->left = node->left;
			copy->right = Pop(node->right);
		}
		Refresh(copy);
		return copy;
	}
public:
	PersistentHeap() { Clear(); }
	~PersistentHeap() { Clear(); }
	void Clear() {
		for(int i = 0; i < (int)nodes.size(); i++) {
			delete nodes[i];
		}
		nodes.clear();
		root.clear();
		root.push_back(NULL);
	}
	int Top(int ver) {
		return root[ver]->data;
	}
	void Push(int ver, int data) {
		root.push_back(Push(root[ver], data));
	}
	void Pop(int ver) {
		root.push_back(Pop(root[ver]));
	}
	int Size(int ver) {
		return Size(root[ver]);
	}
	bool Empty(int ver) {
		return Size(root[ver]) == 0;
	}
};

/*
 * Unit tests
 */

#include <queue>
#include <ctime>
#include <cstdio>
#define NTESTS 100000

bool TestNormalHeap() {
	srand(time(NULL));
	std::priority_queue<int> control;
	Heap heap;
	for(int test = 1; test <= NTESTS; test++) {
		int op = rand()%2;
		if (op == 0 && !control.empty()) {
			control.pop();
			heap.Pop();
		}
		else {
			int data = rand()%1000;
			heap.Push(data);
			control.push(data);
		}
		if (control.size() != heap.Size()) {
			printf("failed on test %d\n", test);
			return false;
		}
		if (!control.empty()) {
			int data1 = control.top();
			int data2 = heap.Top();
			if (data1 != data2) {
				printf("failed on test %d\n", test);
				return false;
			}
		}
	}
	while(!control.empty()) {
		int data1 = control.top();
		int data2 = heap.Top();
		if (data1 != data2) {
			printf("failed on closing loop %d\n");
			return false;
		}
		control.pop();
		heap.Pop();
	}
	printf("All normal heap tests passed\n");
	return true;
}

bool TestPersistentHeap() {
	srand(time(NULL));
	std::priority_queue<int> control[NTESTS];
	PersistentHeap heap;
	for(int test = 1; test < NTESTS; test++) {
		int op = rand()%2;
		int ver = rand()%test;
		control[test] = control[ver];
		if (op == 0 && !control[test].empty()) {
			control[test].pop();
			heap.Pop(ver);
		}
		else {
			int data = rand()%1000;
			heap.Push(ver, data);
			control[test].push(data);
		}
	}
	for(int test = 0; test < NTESTS; test++) {
		if (control[test].size() != heap.Size(test)) {
			printf("size failed on test %d: %u-%d\n", test, control[test].size(), heap.Size(test));
			return false;
		}
		if (!control[test].empty()) {
			int data1 = control[test].top();
			int data2 = heap.Top(test);
			if (data1 != data2) {
				printf("front failed on test %d: %d-%d\n", test, data1, data2);
				return false;
			}
		}
	}
	printf("All persistent heap tests passed\n");
	return true;
}

int main() {
	TestNormalHeap();
	TestPersistentHeap();
	return 0;
}