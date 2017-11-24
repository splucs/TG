#include <cstdlib>
#include <vector>

/*
 * Normal linked list deque
 */

struct Node {
	const int data;
	Node *nxt, *prv;
	Node(Node *_nxt, Node *_prv, int _data) :
		nxt(_nxt), prv(_prv), data(_data) { }
};

class Deque {
private:
	Node *front, *back;
	int size;
public:
	Deque() :
		front(NULL), back(NULL), size(0) {}
	~Deque() { Clear(); }
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
	int Back() {
		return back->data;
	}
	void PopFront() {
		Node *aux = front;
		front = front->nxt;
		delete aux;
		if (front == NULL) back = NULL;
		else front->prv = NULL;
		size--;
	}
	void PopBack() {
		Node *aux = back;
		back = back->prv;
		delete aux;
		if (back == NULL) front = NULL;
		else back->nxt = NULL;
		size--;
	}
	void PushBack(int data) {
		if (back != NULL) back = back->nxt = new Node(NULL, back, data);
		else front = back = new Node(NULL, NULL, data);
		size++;
	}
	void PushFront(int data) {
		if (front != NULL) front = front->prv = new Node(front, NULL, data);
		else front = back = new Node(NULL, NULL, data);
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
 * Persistent linked list deque with LA and LCA
 */

struct PNode {
	const int data, depth;
	std::vector<PNode*> par;
	PNode(int _data, int _depth) :
		data(_data), depth(_depth) { }
};
	
class PersistentDeque {
private:
	std::vector<PNode*> front, back;
	std::vector<PNode*> nodes;
	PNode *Newnode(PNode *parent, int data) {
		PNode *node;
		if (parent != NULL) {
			node = new PNode(data, parent->depth + 1);
			node->par.push_back(parent);
			for(int i = 1; i-1 < (int)node->par[i-1]->par.size(); i++) {
				node->par.push_back(node->par[i-1]->par[i-1]);
			}
		}
		else node = new PNode(data, 1);
		nodes.push_back(node);
		return node;
	}
	PNode *LowestCommonAncestor(PNode *u, PNode *v) {
		if (u == NULL || v == NULL) return NULL;
		u = LevelAncestor(u, u->depth - v->depth);
		v = LevelAncestor(v, v->depth - u->depth);
		if (u == v) return u;
		for(int i = int(u->par.size()) - 1; i>= 0; i--) {
			while(i < (int)u->par.size() && u->par[i] != v->par[i]) {
				u = u->par[i];
				v = v->par[i];
			}
		}
		return u->par[0];
	}
	PNode *LevelAncestor(PNode *node, int jump) {
		if (node == NULL || jump <= 0) return node;
		for(int i = 0; i < (int)node->par.size(); i++) {
			if (jump & (1<<i)) {
				node = node->par[i];
			}
		}
		return node;
	}
public:
	PersistentDeque() { Clear(); }
	~PersistentDeque() { Clear(); }
	void Clear() {
		for(int i = 0; i < (int)nodes.size(); i++) {
			delete nodes[i];
		}
		nodes.clear();
		back.clear(); back.push_back(NULL);
		front.clear(); front.push_back(NULL);
	}
	int Front(int ver) {
		return front[ver]->data;
	}
	int Back(int ver) {
		return back[ver]->data;
	}
	int PopFront(int ver) {
		if (front[ver] == back[ver]) {
			back.push_back(NULL);
			front.push_back(NULL);
			return int(front.size())-1;
		}
		PNode *lca = LowestCommonAncestor(front[ver], back[ver]);
		back.push_back(back[ver]);
		if (front[ver] == lca) {
			PNode *la = LevelAncestor(back[ver], back[ver]->depth - lca->depth - 1);
			front.push_back(la);
		}
		else front.push_back(front[ver]->par[0]);
		return int(front.size())-1;
	}
	int PopBack(int ver) {
		if (front[ver] == back[ver]) {
			back.push_back(NULL);
			front.push_back(NULL);
			return int(back.size())-1;
		}
		PNode *lca = LowestCommonAncestor(front[ver], back[ver]);
		front.push_back(front[ver]);
		if (back[ver] == lca) {
			PNode *la = LevelAncestor(front[ver], front[ver]->depth - lca->depth - 1);
			back.push_back(la);
		}
		else back.push_back(back[ver]->par[0]);
		return int(back.size())-1;
	}
	int PushFront(int ver, int data) {
		PNode *node = Newnode(front[ver], data);
		front.push_back(node);
		if (back[ver] == NULL) back.push_back(node);
		else back.push_back(back[ver]);
		return int(front.size()) - 1;
	}
	int PushBack(int ver, int data) {
		PNode *node = Newnode(back[ver], data);
		back.push_back(node);
		if (front[ver] == NULL) front.push_back(node);
		else front.push_back(front[ver]);
		return int(back.size()) - 1;
	}
	int At(int ver, int i) {
		PNode *lca = LowestCommonAncestor(front[ver], back[ver]);
		int frontDist = front[ver]->depth - lca->depth;
		int backDist = back[ver]->depth - lca->depth;
		PNode *node;
		if (i <= frontDist) node = LevelAncestor(front[ver], i);
		else node = LevelAncestor(back[ver], frontDist + backDist - i);
		return node->data;
	}
	int Size(int ver) {
		PNode *lca = LowestCommonAncestor(front[ver], back[ver]);
		if (lca == NULL) return 0;
		return front[ver]->depth + back[ver]->depth - 2*lca->depth + 1;
	}
	int LatestVersion() {
		return int(back.size())-1;
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
	Deque deq;
	for(int test = 1; test <= NTESTS || !control.empty(); test++) {
		int add = rand()%2;
		int front = rand()%2;
		if (add && test <= NTESTS) {
			int data = rand()%RANGE;
			if (front) {
				control.push_front(data);
				deq.PushFront(data);
			}
			else {
				control.push_back(data);
				deq.PushBack(data);
			}
		}
		else {
			if (control.empty()) {
				test--;
				continue;
			}
			if (front) {
				control.pop_front();
				deq.PopFront();
			}
			else {
				control.pop_back();
				deq.PopBack();
			}
		}
		if (control.size() != deq.Size()) {
			printf("failed on size test %d\n", test);
			return false;
		}
		if (!control.empty()) {
			int data1 = control.front();
			int data2 = deq.Front();
			if (data1 != data2) {
				printf("failed on front test %d: %d-%d\n", test, data1, data2);
				return false;
			}
			data1 = control.back();
			data2 = deq.Back();
			if (data1 != data2) {
				printf("failed on back test %d: %d-%d\n", test, data1, data2);
				return false;
			}
		}
	}
	printf("All normal deque tests passed\n");
	return true;
}

bool TestPersistentDeque() {
	srand(time(NULL));
	std::deque<int> control[NTESTS];
	PersistentDeque deq;
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
	printf("All persistent deque tests passed\n");
	return true;
}

int main() {
	TestNormalDeque();
	TestPersistentDeque();
	return 0;
}