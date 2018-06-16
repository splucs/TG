#include <cstdlib>
#include <vector>

/*
 * Normal linked list queue
 */

struct Node {
	const int data;
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
		while(front != NULL) {
			Node *aux = front;
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
		if (front != NULL) back = back->nxt = new Node(NULL, data);
		else front = back = new Node(NULL, data);
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
 * Persistent linked list queue with fat node method
 */

#include <map>

struct FNode {
	std::map<int, int> data;
	std::map<int, FNode*> nxt;
	FNode(FNode *_nxt, int _data, int i) {
		data[i] = _data;
		nxt[i] = _nxt;
	}
	FNode* GetNxt(int ver) {
		return (--nxt.upper_bound(ver))->second;
	}
	int GetData(int ver) {
		return (--data.upper_bound(ver))->second;
	}
};

class FatQueue {
private:
	std::vector<FNode*> front, back, nodes;
	std::vector<int> size;
public:
	FatQueue() {
		front.push_back(NULL);
		back.push_back(NULL);
		size.push_back(0);
	}
	~FatQueue() { Clear(); }
	void Clear() {
		for(int i = 0; i < (int)nodes.size(); i++) {
			delete nodes[i];
		}
		back.clear(); back.push_back(NULL);
		front.clear(); front.push_back(NULL);
		size.clear(); size.push_back(0);
		nodes.clear();
	}
	int Front(int ver) {
		return front[ver]->GetData(ver);
	}
	void Pop() {
		int i = LatestVersion();
		front.push_back(front[i]->GetNxt(i));
		if (front.back() == NULL) back.push_back(NULL);
		else back.push_back(back[i]);
		size.push_back(size[i]-1);
	}
	void Push(int data) {
		int i = LatestVersion();
		FNode *newNode = new FNode(NULL, data, i+1);
		nodes.push_back(newNode);
		if (front[i] != NULL) {
			back[i]->nxt[i+1] = newNode;
			front.push_back(front[i]);
		}
		else front.push_back(newNode);
		back.push_back(newNode);
		size.push_back(size[i]+1);
	}
	int Size(int ver) {
		return size[ver];
	}
	bool Empty(int ver) {
		return size[ver] == 0;
	}
	int LatestVersion() {
		return int(back.size())-1;
	}
};

/*
 * Persistent linked list queue with node copy
 */
struct CNode;

struct Leader {
	CNode *recent;
	Leader(CNode *_recent) : recent(_recent) { }
};

struct CNode {
	Leader *leader;
	int timestamp;
	const int data;
	CNode *nxt;
	CNode(CNode *_nxt, int _data, int i, bool newLeader = true) :
		nxt(_nxt), data(_data), timestamp(i) {
		if (newLeader) leader = new Leader(this);
		else leader = NULL;
	}
	~CNode() {
		if (leader->recent == this) delete leader;
	}
	CNode *GetRecent() {
		return leader->recent;
	}
	CNode *Copy(int i) {
		if (this != leader->recent)
			return leader->recent->Copy(i);
		CNode *copy = new CNode(nxt, data, i, false);
		copy->leader = leader;
		leader->recent = copy;
		return copy;
	}
};

class CopyQueue {
private:
	std::vector<CNode*> front, back, nodes;
	std::vector<int> size;
public:
	CopyQueue() {
		front.push_back(NULL);
		back.push_back(NULL);
		size.push_back(0);
	}
	~CopyQueue() { Clear(); }
	void Clear() {
		for(int i = 0; i < (int)nodes.size(); i++) {
			delete nodes[i];
		}
		back.clear(); back.push_back(NULL);
		front.clear(); front.push_back(NULL);
		size.clear(); size.push_back(0);
		nodes.clear();
	}
	int Front(int ver) {
		return front[ver]->data;
	}
	void Pop() {
		int i = LatestVersion();
		CNode *newFront = front[i]->GetRecent()->nxt;
		if (newFront != NULL) newFront = newFront->GetRecent();
		front.push_back(newFront);
		if (front.back() == NULL) back.push_back(NULL);
		else back.push_back(back[i]->GetRecent());
		size.push_back(size[i]-1);
	}
	void Push(int data) {
		int i = LatestVersion();
		CNode *newNode = new CNode(NULL, data, i+1);
		nodes.push_back(newNode);
		if (front[i] != NULL) {
			CNode *copyBack = back[i]->Copy(i+1);
			nodes.push_back(copyBack);
			copyBack->nxt = newNode;
			front.push_back(front[i]->GetRecent());
		}
		else front.push_back(newNode);
		back.push_back(newNode);
		size.push_back(size[i]+1);
	}
	int Size(int ver) {
		return size[ver];
	}
	bool Empty(int ver) {
		return size[ver] == 0;
	}
	int LatestVersion() {
		return int(back.size())-1;
	}
};

/*
 * Persistent linked list queue with dynamic array
 */

class AQueue {
	std::vector<Node*> front, back, nodes;
	std::vector<int> size;
public:
	AQueue() {
		front.push_back(NULL);
		back.push_back(NULL);
		size.push_back(0);
	}
	~AQueue() { Clear(); }
	void Clear() {
		for(int i = 0; i < (int)nodes.size(); i++) {
			delete nodes[i];
		}
		nodes.clear();
		back.clear(); back.push_back(NULL);
		front.clear(); front.push_back(NULL);
		size.clear(); size.push_back(0);
	}
	int Front(int ver) {
		return front[ver]->data;
	}
	void Pop() {
		front.push_back(front.back()->nxt);
		if (front.back() == NULL) back.push_back(NULL);
		else back.push_back(back.back());
		size.push_back(size.back()-1);
	}
	void Push(int data) {
		Node *newNode = new Node(NULL, data);
		nodes.push_back(newNode);
		if (front.back() != NULL) {
			back.back()->nxt = newNode;
			back.push_back(newNode);
			front.push_back(front.back());
		}
		else {
			back.push_back(newNode);
			front.push_back(newNode);
		}
		size.push_back(size.back()+1);
	}
	int Size(int ver) {
		return size[ver];
	}
	bool Empty(int ver) {
		return size[ver] == 0;
	}
	int LatestVersion() {
		return int(back.size())-1;
	}
};

/*
 * Persistent linked list queue with LA
 */

struct PNode {
	const int data;
	std::vector<PNode*> par;
	PNode(int _data) :
		data(_data) { }
};
	
class PersistentQueue {
private:
	std::vector<PNode*> back;
	std::vector<int> size;
	std::vector<PNode*> nodes;
	PNode *Newnode(PNode *parent, int data) {
		PNode *node = new PNode(data);
		if (parent != NULL) {
			node->par.push_back(parent);
			for(int i = 1; i-1 < (int)node->par[i-1]->par.size(); i++) {
				node->par.push_back(node->par[i-1]->par[i-1]);
			}
		}
		nodes.push_back(node);
		return node;
	}
	PNode *LevelAncestor(PNode *node, int jump) {
		for(int i = 0; i < (int)node->par.size(); i++) {
			if (jump & (1<<i)) {
				node = node->par[i];
			}
		}
		return node;
	}
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
		PNode *front = LevelAncestor(back[ver], size[ver]-1);
		return front->data;
	}
	int Pop(int ver) {
		back.push_back(back[ver]);
		size.push_back(size[ver] - 1);
		return int(back.size())-1;
	}
	int Push(int ver, int data) {
		back.push_back(Newnode(back[ver], data));
		size.push_back(size[ver] + 1);
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

bool TestNormalQueue(bool push = false) {
	srand(time(NULL));
	std::queue<int> control;
	Queue que;
	for(int test = 1; test <= NTESTS; test++) {
		int op = push ? 1 : rand()%2;
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
			printf("failed on closing loop %d-%d\n", data1, data2);
			return false;
		}
		control.pop();
		que.Pop();
	}
	printf("All normal queue tests passed, %s\n",
		push ? "push only" : "push and pop"
	);
	return true;
}

bool TestPersistentQueueCorrectness(int suits, bool total, bool push = false) {
	srand(time(NULL));
	for(int suit = 0; suit < suits; suit++) {
		std::queue<int> control[NTESTS];
		PersistentQueue que;
		FatQueue fque;
		CopyQueue cque;
		AQueue aque;
		for(int test = 1; test < NTESTS; test++) {
			int op = push ? 1 : rand()%2;
			int ver = total ? rand()%test : test-1;
			control[test] = control[ver];
			if (op == 0 && !control[test].empty()) {
				control[test].pop();
				que.Pop(ver);
				if (!total) {
					fque.Pop();
					cque.Pop();
					aque.Pop();
				}
			}
			else {
				int data = rand();
				que.Push(ver, data);
				control[test].push(data);
				if (!total) {
					fque.Push(data);
					cque.Push(data);
					aque.Push(data);
				}
			}
		}
		for(int test = 0; test < NTESTS; test++) {
			if (control[test].size() != que.Size(test)) {
				printf("LA queue size failed on test %d: %u-%d\n", test, control[test].size(), que.Size(test));
				return false;
			}
			if (!total && control[test].size() != fque.Size(test)) {
				printf("Fat queue size failed on test %d: %u-%d\n", test, control[test].size(), fque.Size(test));
				return false;
			}
			if (!total && control[test].size() != cque.Size(test)) {
				printf("Copy queue size failed on test %d: %u-%d\n", test, control[test].size(), cque.Size(test));
				return false;
			}
			if (!total && control[test].size() != aque.Size(test)) {
				printf("Array queue size failed on test %d: %u-%d\n", test, control[test].size(), aque.Size(test));
				return false;
			}
			if (!control[test].empty()) {
				int data1 = control[test].front(), data2;
				data2 = que.Front(test);
				if (data1 != data2) {
					printf("LA front failed on test %d: %d-%d\n", test, data1, data2);
					return false;
				}
			}
			if (!total && !control[test].empty()) {
				int data1 = control[test].front(), data2;
				data2 = fque.Front(test);
				if (data1 != data2) {
					printf("Fat front failed on test %d: %d-%d\n", test, data1, data2);
					return false;
				}
				data2 = cque.Front(test);
				if (data1 != data2) {
					printf("Copy front failed on test %d: %d-%d\n", test, data1, data2);
					return false;
				}
				data2 = aque.Front(test);
				if (data1 != data2) {
					printf("Array front failed on test %d: %d-%d\n", test, data1, data2);
					return false;
				}
			}
		}
	}
	printf("All %s persistent queue tests passed, %s\n",
		total ? "total" : "partial",
		push ? "push only" : "push and pop"
	);
	return true;
}

bool TestPersistentQueueTime(int suits, bool total, bool push = false) {
	long long seed = time(NULL);
	srand(seed);
	clock_t t;
	t = clock(); //control
	for(int suit = 0; suit < suits; suit++) {
		std::queue<int> control[NTESTS];
		for(int test = 1; test < NTESTS; test++) {
			int op = push ? 1 : rand()%2;
			int ver = total ? rand()%test : test-1;
			control[test] = control[ver];
			if (op == 0 && !control[test].empty()) {
				control[test].pop();
			}
			else {
				int data = rand();
				control[test].push(data);
			}
		}
		for(int test = 0; test < NTESTS; test++) {
			control[test].size();
			if(!control[test].empty())
				control[test].front();
		}
	}
	t = clock() - t;
	printf("%s persistence control time, %s: %.0f us\n",
		total ? "Total" : "Partial",
		push ? "push only" : "push and pop",
		t*1000000.0/CLOCKS_PER_SEC/suits
	);
	t = clock(); //LA
	for(int suit = 0; suit < suits; suit++) {
		PersistentQueue que;
		for(int test = 1; test < NTESTS; test++) {
			int op = push ? 1 : rand()%2;
			int ver = total ? rand()%test : test-1;
			if (op == 0 && !que.Empty(ver)) {
				que.Pop(ver);
			}
			else {
				int data = rand();
				que.Push(ver, data);
			}
		}
		for(int test = 0; test < NTESTS; test++) {
			que.Size(test);
			if (!que.Empty(test)) que.Front(test);
		}
	}
	t = clock() - t;
	printf("%s persistence LA time, %s: %.0f us\n",
		total ? "Total" : "Partial",
		push ? "push only" : "push and pop",
		t*1000000.0/CLOCKS_PER_SEC/suits
	);
	if (total) return true;
	t = clock(); //fat
	for(int suit = 0; suit < suits; suit++) {
		FatQueue que;
		for(int test = 1; test < NTESTS; test++) {
			int op = push ? 1 : rand()%2;
			int ver = test-1;
			if (op == 0 && !que.Empty(ver)) {
				que.Pop();
			}
			else {
				int data = rand();
				que.Push(data);
			}
		}
		for(int test = 0; test < NTESTS; test++) {
			que.Size(test);
			if (!que.Empty(test)) que.Front(test);
		}
	}
	t = clock() - t;
	printf("%s persistence fat time, %s: %.0f us\n",
		total ? "Total" : "Partial",
		push ? "push only" : "push and pop",
		t*1000000.0/CLOCKS_PER_SEC/suits
	);
	t = clock(); //copy
	for(int suit = 0; suit < suits; suit++) {
		CopyQueue que;
		for(int test = 1; test < NTESTS; test++) {
			int op = push ? 1 : rand()%2;
			int ver = test-1;
			if (op == 0 && !que.Empty(ver)) {
				que.Pop();
			}
			else {
				int data = rand();
				que.Push(data);
			}
		}
		for(int test = 0; test < NTESTS; test++) {
			que.Size(test);
			if (!que.Empty(test)) que.Front(test);
		}
	}
	t = clock() - t;
	printf("%s persistence copy time, %s: %.0f us\n",
		total ? "Total" : "Partial",
		push ? "push only" : "push and pop",
		t*1000000.0/CLOCKS_PER_SEC/suits
	);
	t = clock(); //array
	for(int suit = 0; suit < suits; suit++) {
		AQueue que;
		for(int test = 1; test < NTESTS; test++) {
			int op = push ? 1 : rand()%2;
			int ver = test-1;
			if (op == 0 && !que.Empty(ver)) {
				que.Pop();
			}
			else {
				int data = rand();
				que.Push(data);
			}
		}
		for(int test = 0; test < NTESTS; test++) {
			que.Size(test);
			if (!que.Empty(test)) que.Front(test);
		}
	}
	t = clock() - t;
	printf("%s persistence array time, %s: %.0f us\n",
		total ? "Total" : "Partial",
		push ? "push only" : "push and pop",
		t*1000000.0/CLOCKS_PER_SEC/suits
	);
	return true;
}

int main() {
	TestNormalQueue(false);
	TestPersistentQueueCorrectness(100, false, false);
	TestPersistentQueueCorrectness(100, true, false);
	TestPersistentQueueTime(100, false, false);
	TestPersistentQueueTime(100, true, false);
	TestNormalQueue(true);
	TestPersistentQueueCorrectness(100, false, true);
	TestPersistentQueueCorrectness(100, true, true);
	TestPersistentQueueTime(100, false, true);
	TestPersistentQueueTime(100, true, true);
	return 0;
}