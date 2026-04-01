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
void max_heapify(int* arr, int i, int n) {
	int left = i*2 + 1;
	int right = i*2 + 2;
	int largest = i;
	if(left < n && arr[left] > arr[i])
		largest = left;
	if(right < n && arr[right] > arr[largest])
		largest = right;
	if(largest != i) {
		swap(arr, i, largest);
		max_heapify(arr, largest, n);
	}
}
void heap_sort(int* arr, int n) {
	for(int i = n/2 - 1; i >= 0; i--)
		max_heapify(arr, i, n);
	for(int i = n - 1; i > 0; i--) {
		swap(arr, 0, i);
		max_heapify(arr, 0, i);
	}
}

int main() {
	int n = 0;
	srand(time(NULL));
	int* array = genArray(&n);
	printArray(array, n, "unsorted:");
	heap_sort(array, n);
	printArray(array, n, "sorted:");
	free(array);
	return 0;
}
