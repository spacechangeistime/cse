#include <stdio.h>
#include <stdlib.h>

// This program demonstrate construction and sorting of the linked list of integers
// compile using gcc command
// $ gcc sort-list.c
// run the program
// $ ./a.out
//
// Node represents one link or node of a list
typedef struct node {
	int d;
	struct node* next;
} Node;

// set new value of Node data and return old value
int setValue(Node* p, int i) {
	int t = p->d;
	p->d = i;
	return t;
}

// print elements of list
void printList(const char* msg, Node* head) {
	printf(msg);
	for(Node* p = head; p != NULL; p = p->next)
		printf("->%d", p->d);
	printf("\n");
}

// sortList uses insertion sort
void sortList(Node** listPtr) {
	for(Node* p = *listPtr; p != NULL; p = p->next) { // p is current Node whose position needs to be determined in sorted list(from list head until Node p)
		for(Node* q = *listPtr; q != p; q = q->next) { // head of list to previous Node of p, its sorted list
			if(p->d < q->d) { // Node q data is greater than Node p data, so p should get inserted before q
				// data of Nodes from q until p will replace value of their next Node and final node p data will take place of q data
				int t = p->d;
				while(q != p) {
					t = setValue(q, t);
					q = q->next;
				}
				p->d = t;
				break;
			}
		}
	}
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

	printList("original list:\n", head);
	sortList(&head);
	printList("sorted list:\n", head);

	return 0;
}
