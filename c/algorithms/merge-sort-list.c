#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// This program demonstrate merge sort on linked list
// compile using gcc command
// $ gcc merge-sort-list.c
// run the program
// $ ./a.out
//
// Node represents one link or node of a list
typedef struct node {
	int d;
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
		printf("->%d", p->d);
	printf("\n");
}

Node* split_middle(Node* head) {
	Node t;
	t.next = head;
	Node* prev = &t;
	for(Node* fast = head; fast != NULL && fast->next != NULL;
		       prev = prev->next, fast = fast->next->next)
		if(fast == NULL || fast->next == NULL)
			break;
	t.next = prev->next;
	prev->next = NULL;
	return t.next;
}

Node* mergedList(Node* k, Node* l) {
	Node merged;
	Node* tail = &merged;
	while(k != NULL && l != NULL) {
		Node* n;
		if(k->d < l->d) {
			n = k;
			k = k->next;
		}
		else {
			n = l;
			l = l->next;
		}
		tail->next = n;
		tail = tail->next;
	}
	// list k or l has remaining elements
	tail->next = (k != NULL? k: l);
	return merged.next;
}

// merge sort
Node* mergeSortList(Node* head) {
	if(head == NULL || head->next == NULL)
		return head;
	Node* mid = split_middle(head);
	Node* first = mergeSortList(head);
	Node* second = mergeSortList(mid);
	return mergedList(first, second);
}

Node* getList(const char* msg) {
	Node list;
	Node* tail = &list;
	printf("Enter the integer elemets of %s list(q to stop):\n", msg);
	int n = rand()%10 + 7;
	for(int i = 0; i < n; i++) {
		Node* p = newNode();
		p->d = rand()%101;
		p->next = NULL;
		tail->next = p;
		tail = tail->next;
	}
	return list.next;
}

int main() {
	srand(time(NULL));
	Node* head = getList("");
	printList("list:\n", head);
	Node* sorted = mergeSortList(head);
	printList("merge sorted list:\n", sorted);

	return 0;
}
