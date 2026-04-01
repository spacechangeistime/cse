#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 100

int* getList(const char* msg, int* n) {
	int* array = (int*)malloc(MAX_SIZE*sizeof(int));
	printf("Enter the integer elemets of %s array(q to stop):\n", msg);
	int i = 0;
	while(i < MAX_SIZE && scanf("%d", &array[i]) == 1)
		i++;
	*n = i;
	return array;
}
void printList(int* arr, int n) {
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

int* heap_sort(int* arr, int n) {
	for(int i = n/2 - 1; i >= 0; i--)
		max_heapify(arr, i, n);
	for(int i = n - 1; i > 0; i--) {
		swap(arr, 0, i);
		max_heapify(arr, 0, i);
	}
	return arr;
}

int main() {
	int n = 0;
	int* array = getList("", &n);
	printList(array, n);
	int* sorted = heap_sort(array, n);
	printList(sorted, n);
	free(array);
	return 0;
}
