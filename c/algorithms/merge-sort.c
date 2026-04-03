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
void merge(int* arr, int p, int q, int r) {
	int* tmp = (int*)malloc((r-p)*sizeof(int));
	int i = p;
	int j = q;
	int k = 0;
	// consume smaller element of both subarray arr[p,...,q-1] and arr[q,...,r-1]
	while(i != q && j != r) {
		if(arr[i] <= arr[j])
			tmp[k++] = arr[i++];
		else
			tmp[k++] = arr[j++];
	}
	// if subarray arr[q,...,r-1] is already consumed, shift remaining elements of subarray arr[p,...,q-1]
	// i.e. arr[i,...,q-1] which are largest, to end of final merged subarray arr[p,...,r-1]
	while(i != q) {
		arr[r-q+i] = arr[i];
		i++;
	}
	// copy sorted elements from array tmp[0,...,k-1] to begining of final sorted subarray arr[p,...,r-1]
	while(k > 0) {
		k--;
		arr[p+k] = tmp[k];
	}
	free(tmp);
}
void merge_sort(int* arr, int p, int r) {
	if(p >= r-1)
		return;
	int q = (p + r)/2;
	merge_sort(arr, p, q);
	merge_sort(arr, q, r);
	merge(arr, p, q, r);
}

int main() {
	int n = 0;
	srand(time(NULL));
	int* array = genArray(&n);
	printArray(array, n, "unsorted:");
	merge_sort(array, 0, n);
	printArray(array, n, "sorted:");
	free(array);
	return 0;
}
