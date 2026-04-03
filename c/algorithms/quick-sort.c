#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int* genArray(int* n) {
	*n = rand()%10+3;
	int* array = (int*)malloc(*n*sizeof(int));
	for(int i = 0; i < *n; i++)
		array[i] = rand() % 100;
	return array;
}
void printArray(int* arr, int n, const char* msg) {
	printf("%s\n", msg);
	for(int i = 0; i < n; i++)
		printf("%d ", arr[i]);
	printf("\n");
}
void swap(int* arr, int i, int j) {
	int t = arr[i];
	arr[i] = arr[j];
	arr[j] = t;
}
int partition(int* arr, int p, int r) {
	int i = p-1;
	for(int j = p; j < r; j++) {
		if(arr[j] <= arr[r])
			swap(arr, ++i, j);
	}
	swap(arr, ++i, r);
	return i;
}
void quick_sort(int* arr, int p, int r) {
	if(p >= r)
		return;
	int q = partition(arr, p, r);
	quick_sort(arr, p, q-1);
	quick_sort(arr, q+1, r);
}

int main() {
	int n = 0;
	srand(time(NULL));
	int* array = genArray(&n);
	printArray(array, n, "unsorted:");
	quick_sort(array, 0, n-1);
	printArray(array, n, "sorted:");
	free(array);
	return 0;
}
