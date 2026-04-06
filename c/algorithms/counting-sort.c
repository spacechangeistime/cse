#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int* genArray(int* n, int* k) {
	*n = rand()%10+3;
	int* a = (int*)malloc(*n*sizeof(int));
	int max = 0;
	for(int i = 0; i < *n; i++) {
		a[i] = rand() % 100;
		if(a[i] > max)
			max = a[i];
	}
	*k = max;
	return a;
}
void printArray(int* arr, int n, const char* msg) {
	printf("%s\n", msg);
	for(int i = 0; i < n; i++)
		printf("%d ", arr[i]);
	printf("\n");
}
int* counting_sort(int* a, int n, int k) {
	int* b = (int*)malloc(n*sizeof(int));
	int* c = (int*)malloc((k+1)*sizeof(int));
	for(int i = 0; i <= k; i++)
		c[i] = 0;
	for(int i = 0; i < n; i++)
		c[a[i]] += 1;
	for(int i = 1; i <= k; i++)
		c[i] += c[i-1]; // running total elements count upto including a[i] in final sorted array
	for(int i = n - 1; i >= 0; i--) {
		// array index start from 0, so decrease 1 from total count c[a[i]] of elements to get index of a[i] in final sorted array b
		c[a[i]] -= 1;
		b[c[a[i]]] = a[i];
	}
	free(c);
	return b;
}

int main() {
	srand(time(NULL));
	int n = 0, k = 0;
	int* arr = genArray(&n, &k);
	printArray(arr, n, "unsorted:");
	printf("k: %d\n", k);
	int* sorted = counting_sort(arr, n, k);
	printArray(sorted, n, "sorted:");
	free(sorted);
	free(arr);
	return 0;
}
