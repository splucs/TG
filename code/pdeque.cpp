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
 * Persistent linked list deque with fat node method
 */

#include <map>

struct FNode {
	std::map<int, int> data;
	std::map<int, FNode*> nxt, prv;
	FNode(FNode *_nxt, FNode *_prv, int _data, int i) {
		nxt[i] = _nxt;
		prv[i] = _prv;
		data[i] = _data;
	}
	FNode* GetNxt(int ver) {
		return (--nxt.upper_bound(ver))->second;
	}
	FNode* GetPrv(int ver) {
		return (--prv.upper_bound(ver))->second;
	}
	int GetData(int ver) {
		return (--data.upper_bound(ver))->second;
	}
};

class FatDeque {
private:
	std::vector<FNode*> front, back, nodes;
	std::vector<int> size;
public:
	FatDeque() {
		front.push_back(NULL);
		back.push_back(NULL);
		size.push_back(0);
	}
	~FatDeque() { Clear(); }
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
	int Back(int ver) {
		return back[ver]->GetData(ver);
	}
	void PopFront() {
		int ver = LatestVersion();
		front.push_back(front[ver]->GetNxt(ver));
		if (front.back() == NULL) back.push_back(NULL);
		else {
			front.back()->prv[ver+1] = NULL;
			back.push_back(back[ver]);
		}
		size.push_back(size[ver]-1);
	}
	void PopBack() {
		int ver = LatestVersion();
		back.push_back(back[ver]->GetPrv(ver));
		if (back.back() == NULL) front.push_back(NULL);
		else {
			back.back()->nxt[ver+1] = NULL;
			front.push_back(front[ver]);
		}
		size.push_back(size[ver]-1);
	}
	void PushBack(int data) {
		int ver = LatestVersion();
		if (back[ver] != NULL) {
			FNode *newNode = new FNode(NULL, back[ver], data, ver+1);
			nodes.push_back(newNode);
			back[ver]->nxt[ver+1] = newNode;
			back.push_back(newNode);
			front.push_back(front[ver]);
		}
		else {
			FNode *newNode = new FNode(NULL, NULL, data, ver+1);
			back.push_back(newNode);
			front.push_back(newNode);
			nodes.push_back(newNode);
		}
		size.push_back(size[ver]+1);
	}
	void PushFront(int data) {
		int ver = LatestVersion();
		if (front[ver] != NULL){
			FNode *newNode = new FNode(front[ver], NULL, data, ver+1);
			nodes.push_back(newNode);
			front[ver]->prv[ver+1] = newNode;
			front.push_back(newNode);
			back.push_back(back[ver]);
		}
		else {
			FNode *newNode = new FNode(NULL, NULL, data, ver+1);
			back.push_back(newNode);
			front.push_back(newNode);
			nodes.push_back(newNode);
		}
		size.push_back(size[ver]+1);
	}
	int Size(int ver) {
		return size[ver];
	}
	bool Empty(int ver) {
		return size[ver] == 0;
	}
	int LatestVersion() {
		return int(size.size()) - 1;
	}
};

/*
 * Persistent linked list deque with node copy
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
	CNode *nxt, *prv;
	CNode(CNode *_nxt, CNode *_prv, int _data, int i, bool newLeader = true) :
		nxt(_nxt), prv(_prv), data(_data), timestamp(i) {
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
		CNode *copy = new CNode(nxt, prv, data, i, false);
		copy->leader = leader;
		leader->recent = copy;
		return copy;
	}
};

class CopyDeque {
private:
	std::vector<CNode*> front, back, nodes;
	std::vector<int> size;
public:
	CopyDeque() {
		front.push_back(NULL);
		back.push_back(NULL);
		size.push_back(0);
	}
	~CopyDeque() { Clear(); }
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
	int Back(int ver) {
		return back[ver]->data;
	}
	void PopFront() {
		int i = LatestVersion();
		CNode *newFront = front[i]->GetRecent()->nxt;
		if (newFront != NULL) {
			newFront = newFront->Copy(i+1);
			newFront->prv = NULL;
		}
		front.push_back(newFront);
		if (newFront == NULL) back.push_back(NULL);
		else back.push_back(back[i]->GetRecent());
		size.push_back(size[i]-1);
	}
	void PopBack() {
		int i = LatestVersion();
		CNode *newBack = back[i]->GetRecent()->prv;
		if (newBack != NULL) {
			newBack = newBack->Copy(i+1);
			newBack->nxt = NULL;
		}
		back.push_back(newBack);
		if (newBack == NULL) front.push_back(NULL);
		else front.push_back(front[i]->GetRecent());
		size.push_back(size[i]-1);
	}
	void PushBack(int data) {
		int i = LatestVersion();
		if (back[i] == NULL) {
			CNode *newNode = new CNode(NULL, NULL, data, i+1);
			nodes.push_back(newNode);
			front.push_back(newNode);
			back.push_back(newNode);
		}
		else {
			CNode *copyBack = back[i]->Copy(i+1);
			CNode *newNode = new CNode(NULL, copyBack, data, i+1);
			nodes.push_back(newNode);
			copyBack->nxt = newNode;
			front.push_back(front[i]->GetRecent());
			back.push_back(newNode);
		}
		size.push_back(size[i]+1);
	}
	void PushFront(int data) {
		int i = LatestVersion();
		if (front[i] == NULL) {
			CNode *newNode = new CNode(NULL, NULL, data, i+1);
			nodes.push_back(newNode);
			front.push_back(newNode);
			back.push_back(newNode);
		}
		else {
			CNode *copyFront = front[i]->Copy(i+1);
			CNode *newNode = new CNode(copyFront, NULL, data, i+1);
			nodes.push_back(newNode);
			copyFront->prv = newNode;
			back.push_back(back[i]->GetRecent());
			front.push_back(newNode);
		}
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
	bool Empty(int ver) {
		return Size(ver) == 0;
	}
	int LatestVersion() {
		return int(back.size())-1;
	}
};

/*
 * Unit tests
 */

#include <cstdio>
#include <deque>
#include <ctime>
#define NTESTS 10000
#define RANGE 1000

bool TestNormalDeque(bool push) {
	srand(time(NULL));
	std::deque<int> control;
	Deque deq;
	for(int test = 1; test <= NTESTS || !control.empty(); test++) {
		int add = push ? 1 : rand()%2;
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

bool TestPersistentDequeCorrectness(int suits, bool total, bool push = false) {
	srand(time(NULL));
	for(int suit = 0; suit < suits; suit++) {
		std::deque<int> control[NTESTS];
		PersistentDeque deq;
		FatDeque fdeq;
		CopyDeque cdeq;
		for(int test = 1; test < NTESTS; test++) {
			int add = push ? 1 : rand()%2;
			int front = rand()%2;
			int ver = total ? rand()%test : test-1;
			control[test] = control[ver];
			if (add || control[test].empty()) {
				int data = rand()%RANGE;
				if (front) {
					control[test].push_front(data);
					deq.PushFront(ver, data);
					if (!total) {
						fdeq.PushFront(data);
						cdeq.PushFront(data);
					}
				}
				else {
					control[test].push_back(data);
					deq.PushBack(ver, data);
					if (!total) {
						fdeq.PushBack(data);
						cdeq.PushBack(data);
					}
				}
			}
			else {
				if (front) {
					control[test].pop_front();
					deq.PopFront(ver);
					if (!total) {
						fdeq.PopFront();
						cdeq.PopFront();
					}
				}
				else {
					control[test].pop_back();
					deq.PopBack(ver);
					if (!total) {
						fdeq.PopBack();
						cdeq.PopBack();
					}
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
				int data3 = total ? data1 : fdeq.Front(test);
				int data4 = total ? data1 : cdeq.Front(test);
				if (data1 != data2) {
					printf("lca deque failed on front test %d: %d-%d\n", test, data1, data2);
					return false;
				}
				if (data1 != data3) {
					printf("fat deque failed on front test %d: %d-%d\n", test, data1, data2);
					return false;
				}
				if (data1 != data4) {
					printf("copy deque failed on front test %d: %d-%d\n", test, data1, data2);
					return false;
				}
				data1 = control[test].back();
				data2 = deq.Back(test);
				data3 = total ? data1 : fdeq.Back(test);
				data4 = total ? data1 : cdeq.Back(test);
				if (data1 != data2) {
					printf("lca deque failed on back test %d: %d-%d\n", test, data1, data2);
					return false;
				}
				if (data1 != data3) {
					printf("dat deque failed on back test %d: %d-%d\n", test, data1, data2);
					return false;
				}
				if (data1 != data4) {
					printf("copy deque failed on back test %d: %d-%d\n", test, data1, data2);
					return false;
				}
			}
			for(int at = 0; !control[test].empty() && at < 10; at++) {
				int i = rand()%control[test].size();
				int data1 = control[test][i];
				int data2 = deq.At(test, i);
				if (data1 != data2) {
					printf("failed on at %d test %d: %d-%d\n", i, test, data1, data2);
					return false;
				}
			}
		}
	}
	printf("All %s persistent deque tests passed, %s\n",
		total ? "total" : "partial",
		push ? "push only" : "push and pop"
	);
	return true;
}


bool TestPersistentDequeTime(int suits, bool total, bool push = false) {
	long long seed = time(NULL);
	srand(seed);
	clock_t t = clock(); //control
	for(int suit = 0; suit < suits; suit++) {
		std::deque<int> control[NTESTS];
		for(int test = 1; test < NTESTS; test++) {
			int add = push ? 1 : rand()%2;
			int front = rand()%2;
			int ver = total ? rand()%test : test-1;
			control[test] = control[ver];
			if (add || control[test].empty()) {
				int data = rand()%RANGE;
				if (front) control[test].push_front(data);
				else control[test].push_back(data);
			}
			else {
				if (front) control[test].pop_front();
				else control[test].pop_back();
			}
		}
		for(int test = 0; test < NTESTS; test++) {
			control[test].size();
			if (!control[test].empty()) {
				control[test].front();
				control[test].back();
			}
		}
	}
	t = clock() - t;
	printf("%s persistent control deque time, %s: %.0f us\n",
		total ? "Total" : "Partial",
		push ? "push only" : "push and pop",
		t*1000000.0/CLOCKS_PER_SEC/suits
	);
	srand(seed);
	t = clock(); //lca
	for(int suit = 0; suit < suits; suit++) {
		PersistentDeque deq;
		for(int test = 1; test < NTESTS; test++) {
			int add = push ? 1 : rand()%2;
			int front = rand()%2;
			int ver = total ? rand()%test : test-1;
			if (add || deq.Empty(ver)) {
				int data = rand()%RANGE;
				if (front) deq.PushFront(ver, data);
				else deq.PushBack(ver, data);
			}
			else {
				if (front) deq.PopFront(ver);
				else deq.PopBack(ver);
			}
		}
		for(int test = 0; test < NTESTS; test++) {
			deq.Size(test);
			if (!deq.Empty(test)) {
				deq.Front(test);
				deq.Back(test);
			}
		}
	}
	t = clock() - t;
	printf("%s persistent LCA deque time, %s: %.0f us\n",
		total ? "Total" : "Partial",
		push ? "push only" : "push and pop",
		t*1000000.0/CLOCKS_PER_SEC/suits
	);
	if (total) return true;
	srand(seed);
	t = clock(); //fat
	for(int suit = 0; suit < suits; suit++) {
		FatDeque deq;
		for(int test = 1; test < NTESTS; test++) {
			int add = push ? 1 : rand()%2;
			int front = rand()%2;
			int ver = total ? rand()%test : test-1;
			if (add || deq.Empty(ver)) {
				int data = rand()%RANGE;
				if (front) deq.PushFront(data);
				else deq.PushBack(data);
			}
			else {
				if (front) deq.PopFront();
				else deq.PopBack();
			}
		}
		for(int test = 0; test < NTESTS; test++) {
			deq.Size(test);
			if (!deq.Empty(test)) {
				deq.Front(test);
				deq.Back(test);
			}
		}
	}
	t = clock() - t;
	printf("%s persistent fat deque time, %s: %.0f us\n",
		total ? "Total" : "Partial",
		push ? "push only" : "push and pop",
		t*1000000.0/CLOCKS_PER_SEC/suits
	);
	srand(seed);
	t = clock(); //copy
	for(int suit = 0; suit < suits; suit++) {
		CopyDeque deq;
		for(int test = 1; test < NTESTS; test++) {
			int add = push ? 1 : rand()%2;
			int front = rand()%2;
			int ver = total ? rand()%test : test-1;
			if (add || deq.Empty(ver)) {
				int data = rand()%RANGE;
				if (front) deq.PushFront(data);
				else deq.PushBack(data);
			}
			else {
				if (front) deq.PopFront();
				else deq.PopBack();
			}
		}
		for(int test = 0; test < NTESTS; test++) {
			deq.Size(test);
			if (!deq.Empty(test)) {
				deq.Front(test);
				deq.Back(test);
			}
		}
	}
	t = clock() - t;
	printf("%s persistent copy deque time, %s: %.0f us\n",
		total ? "Total" : "Partial",
		push ? "push only" : "push and pop",
		t*1000000.0/CLOCKS_PER_SEC/suits
	);
	return true;
}

int main() {
	TestNormalDeque(false);
	TestPersistentDequeCorrectness(100, false, false);
	TestPersistentDequeCorrectness(100, true, false);
	TestPersistentDequeTime(100, false, false);
	TestPersistentDequeTime(100, true, false);
	TestNormalDeque(true);
	TestPersistentDequeCorrectness(100, false, true);
	TestPersistentDequeCorrectness(100, true, true);
	TestPersistentDequeTime(100, false, true);
	TestPersistentDequeTime(100, true, true);
	return 0;
}