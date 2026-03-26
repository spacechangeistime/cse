#include <stdio.h>
#include <stdlib.h>

// This program demonstrate construction and sorting and merging of the sorted linked list of integers
// compile using gcc command
// $ gcc merge-lists.c
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

Node* newNode() {
	Node* n = (Node*)malloc(sizeof(Node));
	n->d = -1;
	n->next = NULL;
	return n;
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
	while(k != NULL && l != NULL) {
		Node* n = newNode();
		if(k->d < l->d) {
			n->d = k->d;
			k = k->next;
		}
		else {
			n->d = l->d;
			l = l->next;
		}
		appendToTail(&merged, &tail, n);
	}
	// list k has remaining elements
	while(k != NULL) {
		Node* n = newNode();
		n->d = k->d;
		n->next = NULL;
		k = k->next;
		appendToTail(&merged, &tail, n);
	}
	// list l has remaining elements
	while(l != NULL) {
		Node* n = newNode();
		n->d = l->d;
		n->next = NULL;
		l = l->next;
		appendToTail(&merged, &tail, n);
	}
	return merged;
}

Node* getList(const char* msg) {
	Node* head = NULL;
	Node* tail = head;
	printf("Enter the integer elemets of %s list(q to stop):\n", msg);
	int n;
	while(scanf("%d", &n) == 1) {
		Node* p = newNode();
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
	char q[10];
	scanf("%s", &q); // discard "q"
	return head;
}

int main() {
	Node* first = getList("first");
	Node* second = getList("second");
	printList("first list:\n", first);
	printList("second list:\n", second);
	sortList(&first);
	printList("first list(sorted):\n", first);
	sortList(&second);
	printList("second list(sorted):\n", second);
	Node* merged = mergedList(first, second);
	printList("merged list(sorted):\n", merged);

	return 0;
}
