#include <stdio.h>
#include <stdlib.h>

// This program demonstrate construction a linked list of integers
// compile using gcc command
// $ gcc linked-list.c
// run the program
// $ ./a.out
//
// Node represents one link or node of a list
typedef struct node {
	int d;
	struct node* next;
} Node;

// print elements of list
void printList(Node* head) {
	for(Node* p = head; p != NULL; p = p->next)
		printf("->%d", p->d);
	printf("\n");
}

int main() {
	Node* head = NULL;
	Node* tail = head;
	printf("Enter the integer elemets of list(q to stop):\n");
	int n;
	while(scanf("%d", &n) == 1) {
		Node* p = (Node*)malloc(sizeof(Node));
		p->d = n;
		p->next = NULL;
		if(tail == NULL) {
			tail = p;
			head = tail;
		}
		else {
			tail->next = p;
			tail = p;
		}
	}

	printList(head);

	return 0;
}
