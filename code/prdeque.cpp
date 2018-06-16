#include <cstdlib>
#include <iostream>
#include <vector>

struct Container {
	int data;
	Container *left, *right;
	Container(int _data) :
		data(_data), left(NULL), right(NULL) { }
	Container(Container *_left, Container *_right) :
		data(0), left(_left), right(_right) { }
};
	
struct Node {
	Container *prefix, *suffix;
	Node *center;
	int size;
	Node(Container *_prefix, Node *_center, Container *_suffix, int _size) :
		prefix(_prefix), center(_center), suffix(_suffix), size(_size) { }
};

class PersistentRecursiveDeque {
private:
	std::vector<Node*> nodes;
	std::vector<Container*> containers;
	std::vector<Node*> root;
	Container *Newcontainer(int data) {
		Container *container = new Container(data);
		containers.push_back(container);
		return container;
	}
	Container *Newcontainer(Container *left, Container *right) {
		Container *container = new Container(left, right);
		containers.push_back(container);
		return container;
	}
	Node *Newnode(Container *prefix, Node *center, Container *suffix, int size) {
		Node *node = new Node(prefix, center, suffix, size);
		nodes.push_back(node);
		return node;
	}
	Container *Front(Node *node) {
		if (node->prefix != NULL) return node->prefix;
		else if (node->center == NULL) return node->suffix;
		else return Front(node->center)->left;
	}
	Container *Back(Node *node) {
		if (node->suffix != NULL) return node->suffix;
		else if (node->center == NULL) return node->prefix;
		else return Back(node->center)->right;
	}
	Node *PushFront(Node *node, Container *data) {
		if (node == NULL) {
			return Newnode(data, NULL, NULL, 1);
		}
		else if (node->prefix == NULL) {
			return Newnode(data, node->center, node->suffix, node->size + 1);
		}
		else {
			Container *newdata = Newcontainer(data, node->prefix);
			return Newnode(NULL, PushFront(node->center, newdata), node->suffix, node->size + 1);
		}
	}
	Node *PushBack(Node *node, Container *data) {
		if (node == NULL) {
			return Newnode(NULL, NULL, data, 1);
		}
		else if (node->suffix == NULL) {
			return Newnode(node->prefix, node->center, data, node->size + 1);
		}
		else {
			Container *newdata = Newcontainer(node->suffix, data);
			return Newnode(node->prefix, PushBack(node->center, newdata), NULL, node->size + 1);
		}
	}
	void PopFront(Node *node, Container *&head, Node *&tail) {
		if (node == NULL) {
			head = NULL; tail = NULL;
		}
		else if (node->prefix != NULL && node->center == NULL && node->suffix == NULL) {
			head = node->prefix; tail = NULL;
		}
		else if (node->prefix != NULL) {
			head = node->prefix;
			tail = Newnode(NULL, node->center, node->suffix, node->size - 1);
		}
		else if (node->center == NULL) {
			head = node->suffix; tail = NULL;
		}
		else {
			Container *data;
			Node *deq;
			PopFront(node->center, data, deq);
			head = data->left;
			tail = Newnode(data->right, deq, node->suffix, node->size - 1);
		}
	}
	void PopBack(Node *node, Node *&head, Container *&tail) {
		if (node == NULL) {
			head = NULL; tail = NULL;
		}
		else if (node->suffix != NULL && node->center == NULL && node->prefix == NULL) {
			head = NULL; tail = node->suffix;
		}
		else if (node->suffix != NULL) {
			head = Newnode(node->prefix, node->center, NULL, node->size - 1);
			tail = node->suffix;
		}
		else if (node->center == NULL) {
			head = NULL; tail = node->prefix;
		}
		else {
			Container *data;
			Node *deq;
			PopBack(node->center, deq, data);
			head = Newnode(node->prefix, deq, data->left, node->size - 1);
			tail = data->right;
		}
	}
	Container *At(Node *node, int i) {
		if (node == NULL) return NULL;
		if (i == 0 && node->prefix != NULL) return node->prefix;
		if (i == node->size-1 && node->suffix != NULL) return node->suffix;
		if (node->prefix != NULL) i--;
		Container *data = At(node->center, i >> 1);
		if (i & 1) return data->right;
		else return data->left;
	}
public:
	PersistentRecursiveDeque() { Clear(); }
	~PersistentRecursiveDeque() { Clear(); }
	void Clear() {
		for(int i = 0; i < (int)nodes.size(); i++) {
			delete nodes[i];
		}
		nodes.clear();
		for(int i = 0; i < (int)containers.size(); i++) {
			delete containers[i];
		}
		containers.clear();
		root.clear();
		root.push_back(NULL);
	}
	int Front(int ver) {
		return Front(root[ver])->data;
	}
	int Back(int ver) {
		return Back(root[ver])->data;
	}
	int PushFront(int ver, int data) {
		Container *dataContainer = Newcontainer(data);
		Node *node = PushFront(root[ver], dataContainer);
		root.push_back(node);
		return int(root.size()) - 1;
	}
	int PushBack(int ver, int data) {
		Container *dataContainer = Newcontainer(data);
		Node *node = PushBack(root[ver], dataContainer);
		root.push_back(node);
		return int(root.size()) - 1;
	}
	int PopFront(int ver) {
		Node *node;
		Container *data;
		PopFront(root[ver], data, node);
		root.push_back(node);
		return int(root.size()) - 1;
	}
	int PopBack(int ver) {
		Node *node;
		Container *data;
		PopBack(root[ver], node, data);
		root.push_back(node);
		return int(root.size()) - 1;
	}
	int At(int ver, int i) {
		return At(root[ver], i)->data;
	}
	int Size(int ver) {
		if (root[ver] == NULL) return 0;
		return root[ver]->size;
	}
	bool Empty(int ver) {
		return Size(ver) == 0;
	}
	int LatestVersion() {
		return int(root.size())-1;
	}
};

/*
 * Unit tests
 */

#include <cstdio>
#include <list>
#include <deque>
#include <ctime>
#define NTESTS 10000
#define RANGE 1000

bool TestNormalDeque() {
	srand(time(NULL));
	std::list<int> control;
	PersistentRecursiveDeque deq;
	for(int test = 1; test <= NTESTS || !control.empty(); test++) {
		int add = rand()%2;
		int front = rand()%2;
		if (add && test <= NTESTS) {
			int data = rand()%RANGE;
			if (front) {
				control.push_front(data);
				deq.PushFront(deq.LatestVersion(), data);
			}
			else {
				control.push_back(data);
				deq.PushBack(deq.LatestVersion(), data);
			}
		}
		else {
			if (control.empty()) {
				test--;
				continue;
			}
			if (front) {
				control.pop_front();
				deq.PopFront(deq.LatestVersion());
			}
			else {
				control.pop_back();
				deq.PopBack(deq.LatestVersion());
			}
		}
		if (control.size() != deq.Size(deq.LatestVersion())) {
			printf("failed on size test %d\n", test);
			return false;
		}
		if (!control.empty()) {
			int data1 = control.front();
			int data2 = deq.Front(deq.LatestVersion());
			if (data1 != data2) {
				printf("failed on front test %d: %d-%d\n", test, data1, data2);
				return false;
			}
			data1 = control.back();
			data2 = deq.Back(deq.LatestVersion());
			if (data1 != data2) {
				printf("failed on back test %d: %d-%d\n", test, data1, data2);
				return false;
			}
		}
	}
	printf("All normal recursive deque tests passed\n");
	return true;
}

bool TestPersistentDeque() {
	srand(time(NULL));
	std::deque<int> control[NTESTS];
	PersistentRecursiveDeque deq;
	for(int test = 1; test < NTESTS; test++) {
		int add = rand()%2;
		int front = rand()%2;
		int ver = rand()%test;
		control[test] = control[ver];
		if (add) {
			int data = rand()%RANGE;
			if (front) {
				control[test].push_front(data);
				deq.PushFront(ver, data);
			}
			else {
				control[test].push_back(data);
				deq.PushBack(ver, data);
			}
		}
		else {
			if (control[test].empty()) {
				test--;
				continue;
			}
			if (front) {
				control[test].pop_front();
				deq.PopFront(ver);
			}
			else {
				control[test].pop_back();
				deq.PopBack(ver);
			}
		}
	}
	for(int test = 0; test < NTESTS; test++) {
		if (control[test].size() != deq.Size(test)) {
			printf("failed on size test %d\n", test);
			return false;
		}
		if (!control[test].empty()) {
			int data1 = control[test].front();
			int data2 = deq.Front(test);
			if (data1 != data2) {
				printf("failed on front test %d: %d-%d\n", test, data1, data2);
				return false;
			}
			data1 = control[test].back();
			data2 = deq.Back(test);
			if (data1 != data2) {
				printf("failed on back test %d: %d-%d\n", test, data1, data2);
				return false;
			}
		}
		for(int i = 0; i < (int)control[test].size(); i++) {
			int data1 = control[test][i];
			int data2 = deq.At(test, i);
			if (data1 != data2) {
				printf("failed on at %d test %d: %d-%d\n", i, test, data1, data2);
				return false;
			}
		}
	}
	printf("All persistent recursive deque tests passed\n");
	return true;
}

int main() {
	TestNormalDeque();
	TestPersistentDeque();
	return 0;
}

