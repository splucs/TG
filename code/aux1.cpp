#include <cstdio>
#include <cstring>
#include <cassert>
#define MAXN 10

int pascal[MAXN][MAXN], N[MAXN][MAXN];

int main() {
	for(int n = 0; n < MAXN; n++) {
		for(int k = 0; k < MAXN; k++) {
			if (k > n) pascal[n][k] = 0;
			else if (k == n || k == 0) pascal[n][k] = 1;
			else pascal[n][k] = pascal[n-1][k-1] + pascal[n-1][k];
			if (pascal[n][k]) printf("%4d ", pascal[n][k]);
		}
		printf("\n");
	}
	memset(&N, 0, sizeof N);
	N[0][0] = 1;
	for(int t = 0; t+1 < MAXN; t++) {
		for(int i = 0; i <= t; i++) {
			if (i > 0) {
				N[t+1][i+1] += N[t][i];
				N[t+1][i-1] += N[t][i];
			}
			else N[t+1][i+1] += 2*N[t][i];
			assert((i+t)%2 == 0 || N[t][i] == 0);
			int p;
			if ((i+t)%2 != 0) p = 0;
			else {
				int n = t;
				int k = (i+t)/2;
				p = (i == 0 ? 1 : 2)*pascal[n][k];
			}
			printf("%4d,%4d ", N[t][i], p);
		}
		printf("\n");
	}
	return 0;
}