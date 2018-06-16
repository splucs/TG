#include <cstdlib>
#include <vector>

struct Node {
	Node *parent;
	int rank;
	Node(Node *_parent, int _rank) :
		parent(parent), rank(_rank) { }
	Node() :
		parent(this), rank(0) { }
}

class UnionFind {
private:
	std::vector<Node*> nodes;
	Node *Find(Node *node) {
		if (node == node->parent) return node;
		else return Find(node->parent);
	}
public:
	UnionFind(int N = 0) { Clear(N); }
	~UnionFind() { Clear(0); }
	void Clear(int N) {
		nodes.resize(N);
		for (int i = 0; i < N; i++) nodes[i] = new Node();
	}
	bool IsSameSet(Node *u, Node *v) {
		return Find(u) == Find(v);
	}
	void unionSet (int i, int j) {
		Node *u = &nodes[i];
		Node *v = &nodes[j];
		if (IsSameSet(u, v)) return;
		int x = find(i), y = find(j);
		if (rank[x] > rank[y]) parent[y] = x;
		else {
			parent[x] = y;
			if (rank[x] == rank[y]) rank[y]++;
		}
	}
};