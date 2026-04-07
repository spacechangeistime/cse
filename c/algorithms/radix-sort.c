#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// compile
// $ gcc radix-sort.c -lm
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
int* counting_sort(int* a, int n, int d) {
	int k = 9;
	int div = pow(10, d); // d'th digit
	int mod = 10;
	int* b = (int*)malloc(n*sizeof(int));
	int* c = (int*)malloc((k+1)*sizeof(int));
	for(int i = 0; i <= k; i++)
		c[i] = 0;
	for(int i = 0; i < n; i++)
		c[(a[i]/div)%mod] += 1;
	for(int i = 1; i <= k; i++)
		c[i] += c[i-1]; // running total elements count upto including a[i] in final sorted array
	for(int i = n - 1; i >= 0; i--) {
		int val = (a[i]/div)%mod; // i'th digit value
		// array index start from 0, so decrease 1 from total count c[val] of elements to get index of a[i] in final sorted array b
		c[val] -= 1;
		b[c[val]] = a[i];
	}
	free(c);
	return b;
}
int* radix_sort(int* a, int n, int k) {
	double dd = log10(k) + 1;
	int max_digits = (int)dd;
	int* b = a;
	for(int i = 0; i < max_digits; i++) {
	       	// sort based on i'th digit
		b = counting_sort(b, n, i);
	}
	return b;
}

int main() {
	srand(time(NULL));
	int n = 0, k = 0;
	int* arr = genArray(&n, &k);
	printArray(arr, n, "unsorted:");
	printf("k: %d\n", k);
	int* sorted = radix_sort(arr, n, k);
	printArray(sorted, n, "sorted:");
	free(sorted);
	free(arr);
	return 0;
}
