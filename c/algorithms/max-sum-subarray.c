#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

typedef struct {
	int i;
	int j;
	int sum;
} Subarray;

int* genArray(int* n) {
	*n = rand()%10+3;
	int* array = (int*)malloc(*n*sizeof(int));
	for(int i = 0; i < *n; i++)
		array[i] = rand() % 100 - 50;
	return array;
}
void printArray(int* arr, int n, const char* msg) {
	printf("%s\n", msg);
	for(int i = 0; i < n; i++)
		printf("%d ", arr[i]);
	printf("\n");
}
Subarray cross_max_sum_subarray(int* arr, int p, int q, int r) {
	int left = q, leftsum = INT_MIN;
	int right = q+1, rightsum = INT_MIN;
	int lsum = 0;
	int rsum = 0;
	for(int i = q; i >= p; i--) {
		lsum += arr[i];
		if(lsum > leftsum) {
			left = i;
			leftsum = lsum;
		}
	}
	for(int i = q+1; i <= r; i++) {
		rsum += arr[i];
		if(rsum > rightsum) {
			right = i;
			rightsum = rsum;
		}
	}
	Subarray c = {left, right, leftsum + rightsum};
	return c;
}
Subarray max_sum_subarray(int* arr, int p, int r) {
	if(p >= r) {
		Subarray s = {p, r, arr[p]};
		return s;
	}
	int q = (p+r)/2;
	Subarray t = max_sum_subarray(arr, p, q);
	Subarray u = max_sum_subarray(arr, q+1, r);
	Subarray v = cross_max_sum_subarray(arr, p, q, r);
	if(t.sum >= u.sum && t.sum >= v.sum)
		return t;
	if(u.sum >= t.sum && u.sum >= v.sum)
		return u;
	if(v.sum >= t.sum && v.sum >= u.sum)
		return v;
}

int main() {
	int n = 0;
	srand(time(NULL));
	int* array = genArray(&n);
	printArray(array, n, "array:");
	Subarray sa = max_sum_subarray(array, 0, n-1);
	printf("subarray:");
	for(int i = sa.i; i <= sa.j; i++)
		printf(" %d", array[i]);
	printf("\nsubarray sum: %d\n", sa.sum);
	free(array);
	return 0;
}
