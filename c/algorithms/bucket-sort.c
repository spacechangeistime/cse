#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// This program demonstrate merge sort on linked list
// compile using gcc command
// $ gcc bucket-sort.c
// run the program
// $ ./a.out
//
// Node represents one link or node of a list
typedef struct node {
	double d;
	struct node* next;
} Node;

Node* newNode() {
	Node* n = (Node*)malloc(sizeof(Node));
	n->d = -1;
	n->next = NULL;
	return n;
}

// print elements of list
void printList(const char* msg, Node* head) {
	printf(msg);
	for(Node* p = head; p != NULL; p = p->next)
		printf("->%f", p->d);
	printf("\n");
}

void appendToTail(Node** head, Node** tail, Node* n) {
	if(*tail == NULL) {
		*tail = n;
		*head = *tail;
	}
	else {
		(*tail)->next = n;
		*tail = (*tail)->next;
	}
}

Node* mergedList(Node* k, Node* l) {
	Node* merged = NULL;
	Node* tail = merged;
	Node* n;
	while(k != NULL && l != NULL) {
		if(isless(k->d, l->d)) {
			n = k;
			k = k->next;
		}
		else {
			n = l;
			l = l->next;
		}
		n->next = NULL;
		appendToTail(&merged, &tail, n);
	}
	// list k has remaining elements
	while(k != NULL) {
		n = k;
		k = k->next;
		n->next = NULL;
		appendToTail(&merged, &tail, n);
	}
	// list l has remaining elements
	while(l != NULL) {
		n = l;
		l = l->next;
		n->next = NULL;
		appendToTail(&merged, &tail, n);
	}
	return merged;
}

Node* middle(Node* head) {
	for(Node* fast = head; fast != NULL && fast->next != NULL;
		       head = head->next, fast = fast->next->next)
		;
	return head;
}

Node* splitList(Node* head, Node* tail) {
	Node* ptr = head;
	while(ptr != NULL && ptr->next != tail)
		ptr = ptr->next;
	if(ptr)
		ptr->next = NULL;
	return head;
}
void freeList(Node* head) {
	while(head != NULL) {
		Node* ptr = head;
		head = head->next;
		free(ptr);
	}
}

// merge sort
Node* mergeSortList(Node* head) {
	if(head == NULL || head->next == NULL)
		return head;
	Node* mid = middle(head);
	Node* first = splitList(head, mid);
	Node* second = mid; // from mid till end
	Node* fs = mergeSortList(first);
	Node* ss = mergeSortList(second);
	return mergedList(fs, ss);
}

double* genArray(int* n) {
	*n = rand()%10+3;
	double* arr = (double*)malloc(*n*sizeof(double));
	for(int i = 0; i < *n; i++)
		arr[i] = ((double)(rand() % 1000000))/1000000;
	return arr;
}

void printArray(double* arr, int n, const char* msg) {
	printf("%s\n", msg);
	for(int i = 0; i < n; i++)
		printf("%f ", arr[i]);
	printf("\n");
}
void bucket_sort(double* arr, int n) {
	Node** b = (Node**)malloc(n*sizeof(Node*));
	for(int i = 0; i < n; i++)
		b[i] = NULL;
	for(int i = 0; i < n; i++) {
		int idx = (int)(arr[i]*n);
		Node* node = newNode();
		node->d = arr[i];
		node->next = b[idx];
		b[idx] = node;
	}
	for(int i = 0; i < n; i++) {
		printList("list: ", b[i]);
		Node* sorted = mergeSortList(b[i]);
		b[i] = sorted;
	}
	for(int i = 0; i < n; i++) {
		Node* ptr = b[i];
		while(ptr != NULL) {
			*arr++ = ptr->d;
			ptr = ptr->next;
		}
	}
	for(int i = 0; i < n; i++) {
		printList("sorted list: ", b[i]);
		freeList(b[i]);
	}
	free(b);
}

int main() {
	srand(time(NULL));
	int n;
	double* arr = genArray(&n);
	printArray(arr, n, "unsorted:");
	bucket_sort(arr, n);
	printArray(arr, n, "sorted:");
	free(arr);
	return 0;
}
