#include <cstdlib>
#include <vector>

/*
 * Normal linked list stack
 */

struct Node {
	int data;
	Node *nxt;
	Node(Node *_nxt, int _data) :
		nxt(_nxt), data(_data) { }
};

class Stack {
private:
	Node* back;
	int size;
public:
	Stack() :
		back(NULL), size(0) { }
	~Stack() { Clear(); }
	void Clear() {
		Node *aux;
		while(back != NULL) {
			aux = back;
			back = back->nxt;
			delete aux;
		}
		size = 0;
	}
	int Top() {
		return back->data;
	}
	void Pop() {
		Node *prv = back;
		back = back->nxt;
		delete prv;
		size--;
	}
	void Push(int data) {
		back = new Node(back, data);
		size++;
	}
	int Size() {
		return size;
	}
	bool Empty() {
		return size == 0;
	}
};

/*
 * Persistent linked list stack
 */

struct PNode {
	int data, size;
	PNode *nxt;
	PNode(PNode *_nxt, int _data, int _size) :
		nxt(_nxt), data(_data), size(_size) { }
};

class PersistentStack {
private:
	std::vector<PNode*> back;
	std::vector<PNode*> nodes;
public:
	PersistentStack() {
		back.push_back(NULL);
	}
	~PersistentStack() { Clear(); }
	void Clear() {
		for(int i = 0; i < (int)nodes.size(); i++) {
			delete nodes[i];
		}
		back.clear(); back.push_back(NULL);
		nodes.clear();
	}
	int Top(int ver) {
		return back[ver]->data;
	}
	int Pop(int ver) {
		back.push_back(back[ver]->nxt);
		return int(back.size())-1;
	}
	int Push(int ver, int data) {
		back.push_back(new PNode(back[ver], data, Size(ver) + 1));
		nodes.push_back(back.back());
		return int(back.size())-1;
	}
	int Size(int ver) {
		if (back[ver] == NULL) return 0;
		return back[ver]->size;
	}
	int Empty(int ver) {
		return Size(ver) == 0;
	}
	int LatestVersion() {
		return int(back.size())-1;
	}
};

/*
 * Unit tests
 */

#include <stack>
#include <ctime>
#include <cstdio>
#define NTESTS 10000

bool TestNormalStack() {
	srand(time(NULL));
	std::stack<int> control;
	Stack stck;
	for(int test = 1; test <= NTESTS; test++) {
		int op = rand()%2;
		if (op == 0 && !control.empty()) {
			control.pop();
			stck.Pop();
		}
		else {
			int data = rand();
			stck.Push(data);
			control.push(data);
		}
		if (control.size() != stck.Size()) {
			printf("failed on test %d\n", test);
			return false;
		}
		if (!control.empty()) {
			int data1 = control.top();
			int data2 = stck.Top();
			if (data1 != data2) {
				printf("failed on test %d\n", test);
				return false;
			}
		}
	}
	printf("All normal stack tests passed\n");
	return true;
}

bool TestPersistentStack() {
	srand(time(NULL));
	std::stack<int> control[NTESTS];
	PersistentStack stck;
	for(int test = 1; test < NTESTS; test++) {
		int op = rand()%2;
		int ver = rand()%test;
		control[test] = control[ver];
		if (op == 0 && !control[test].empty()) {
			control[test].pop();
			stck.Pop(ver);
		}
		else {
			int data = rand();
			stck.Push(ver, data);
			control[test].push(data);
		}
	}
	for(int test = 0; test < NTESTS; test++) {
		if (control[test].size() != stck.Size(test)) {
			printf("failed on test %d\n", test);
			return false;
		}
		if (!control[test].empty()) {
			int data1 = control[test].top();
			int data2 = stck.Top(test);
			if (data1 != data2) {
				printf("failed on test %d\n", test);
				return false;
			}
		}
	}
	printf("All persistent stack tests passed\n");
	return true;
}

int main() {
	TestNormalStack();
	TestPersistentStack();
	return 0;
}