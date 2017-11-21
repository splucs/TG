#include <cstdlib>
#include <vector>

/*
 * Normal linked list queue
 */

struct Node {
	int data;
	Node *nxt;
	Node(Node *_nxt, int _data) :
		nxt(_nxt), data(_data) { }
};

class Queue {
private:
	Node *front, *back;
	int size;
public:
	Queue() :
		front(NULL), back(NULL), size(0) {}
	~Queue() { Clear(); }
	void Clear() {
		Node *aux;
		while(front != NULL) {
			aux = front;
			front = front->nxt;
			delete aux;
		}
		back = NULL;
		size = 0;
	}
	int Front() {
		return front->data;
	}
	void Pop() {
		Node *prv = front;
		front = front->nxt;
		delete prv;
		size--;
		if (front == NULL) back = NULL;
	}
	void Push(int data) {
		if (front != NULL) {
			back->nxt = new Node(NULL, data);
			back = back->nxt;
		}
		else {
			front = back = new Node(NULL, data);
		}
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
 * Persistent linked list queue with LA
 */

struct PNode {
	int data;
	std::vector<PNode*> par;
	PNode(int _data) :
		data(_data) { }
};
	
class PersistentQueue {
private:
	std::vector<PNode*> back;
	std::vector<int> size;
	std::vector<PNode*> nodes;
public:
	PersistentQueue() {
		back.push_back(NULL);
		size.push_back(0);
	}
	~PersistentQueue() { Clear(); }
	void Clear() {
		for(int i = 0; i < (int)nodes.size(); i++) {
			delete nodes[i];
		}
		back.clear(); back.push_back(NULL);
		size.clear(); size.push_back(0);
		nodes.clear();
	}
	int Front(int ver) {
		PNode *front = back[ver];
		unsigned long long jump = size[ver]-1;
		for(int i = 0; i < (int)front->par.size(); i++) {
			if (jump & (1ULL<<i)) {
				front = front->par[i];
			}
		}
		return front->data;
	}
	int Pop(int ver) {
		back.push_back(back[ver]);
		size.push_back(size[ver] - 1);
		return int(back.size())-1;
	}
	int Push(int ver, int data) {
		PNode *aux = new PNode(data);
		if (back[ver] != NULL) {
			aux->par.push_back(back[ver]);
			for(int i = 1; i-1 < (int)aux->par[i-1]->par.size(); i++) {
				aux->par.push_back(aux->par[i-1]->par[i-1]);
			}
		}
		back.push_back(aux);
		size.push_back(size[ver] + 1);
		nodes.push_back(aux);
		return int(back.size()) - 1;
	}
	int Size(int ver) {
		return size[ver];
	}
	int Empty(int ver) {
		return size[ver] == 0;
	}
	int LatestVersion() {
		return int(back.size())-1;
	}
};

/*
 * Unit tests
 */

#include <queue>
#include <ctime>
#include <cstdio>
#define NTESTS 10000

bool TestNormalQueue() {
	srand(time(NULL));
	std::queue<int> control;
	Queue que;
	for(int test = 1; test <= NTESTS; test++) {
		int op = rand()%2;
		if (op == 0 && !control.empty()) {
			control.pop();
			que.Pop();
		}
		else {
			int data = rand();
			que.Push(data);
			control.push(data);
		}
		if (control.size() != que.Size()) {
			printf("failed on test %d\n", test);
			return false;
		}
		if (!control.empty()) {
			int data1 = control.front();
			int data2 = que.Front();
			if (data1 != data2) {
				printf("failed on test %d\n", test);
				return false;
			}
		}
	}
	while(!control.empty()) {
		int data1 = control.front();
		int data2 = que.Front();
		if (data1 != data2) {
			printf("failed on closing loop %d\n");
			return false;
		}
		control.pop();
		que.Pop();
	}
	printf("All normal queue tests passed\n");
	return true;
}

bool TestPersistentQueue() {
	srand(time(NULL));
	std::queue<int> control[NTESTS];
	PersistentQueue que;
	for(int test = 1; test < NTESTS; test++) {
		int op = rand()%2;
		int ver = rand()%test;
		control[test] = control[ver];
		if (op == 0 && !control[test].empty()) {
			control[test].pop();
			que.Pop(ver);
		}
		else {
			int data = rand();
			que.Push(ver, data);
			control[test].push(data);
		}
	}
	for(int test = 0; test < NTESTS; test++) {
		if (control[test].size() != que.Size(test)) {
			printf("size failed on test %d: %u-%d\n", test, control[test].size(), que.Size(test));
			return false;
		}
		if (!control[test].empty()) {
			int data1 = control[test].front();
			int data2 = que.Front(test);
			if (data1 != data2) {
				printf("front failed on test %d: %d-%d\n", test, data1, data2);
				return false;
			}
		}
	}
	printf("All persistent queue tests passed\n");
	return true;
}

int main() {
	TestNormalQueue();
	TestPersistentQueue();
	return 0;
}