#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum color { RED, BLACK } Color;
typedef struct node {
	int d;
	Color c;
	struct node* parent;
	struct node* left;
	struct node* right;
} Node;

Node* newNode(Node* parent) {
	Node* p = (Node*)malloc(sizeof(Node));
	p->d = -1;
	p->c = RED;
	p->parent = parent;
	p->left = NULL;
	p->right = NULL;
	return p;
}
void rotate_left(Node** proot, Node* p) {
	Node* x = p;
	if(x == NULL || x->right == NULL)
		return;
	Node* y = x->right;
	x->right = y->left;
	if(y->left != NULL)
		y->left->parent = x;
	y->left = x;
	if(x->parent == NULL) {
		*proot = y;
	}
	else if(x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;
	y->parent = x->parent;
	x->parent = y;
}
void rotate_right(Node** proot, Node* p) {
	Node* y = p;
	if(y == NULL || y->left == NULL)
		return;
	Node* x = y->left;
	y->left = x->right;
	if(x->right != NULL)
		x->right->parent = y;
	x->right = y;
	if(y->parent == NULL) {
		*proot = x;
	}
	else if(y == y->parent->left)
		y->parent->left = x;
	else
		y->parent->right = x;
	x->parent = y->parent;
	y->parent = x;
}
void RB_Insert_Fixup(Node** proot, Node* z) {
	while(z->parent != NULL && z->parent->c == RED && z->parent->parent != NULL) {
		if(z->parent == z->parent->parent->left) {
			Node* y = z->parent->parent->right;
			if(y != NULL && y->c == RED) {
				y->c = BLACK;
				z->parent->c = BLACK;
				z->parent->parent->c = RED;
				z = z->parent->parent;
			}
			else {
				if(z == z->parent->right) {
					z = z->parent;
					rotate_left(proot, z);
				}
				z->parent->c = BLACK;
				z->parent->parent->c = RED;
				rotate_right(proot, z->parent->parent);
			}
		}
		else {
			Node* y = z->parent->parent->left;
			if(y != NULL && y->c == RED) {
				y->c = BLACK;
				z->parent->c = BLACK;
				z->parent->parent->c = RED;
				z = z->parent->parent;
			}
			else {
				if(z == z->parent->left) {
					z = z->parent;
					rotate_right(proot, z);
				}
				z->parent->c = BLACK;
				z->parent->parent->c = RED;
				rotate_left(proot, z->parent->parent);
			}
		}
	}
	(*proot)->c = BLACK;
}
void insert(Node** proot, Node* z) {
	Node* y = NULL;
	Node* x = *proot;
	while(x != NULL) {
		y = x;
		if(z->d <= x->d)
			x = x->left;
		else
			x = x->right;
	}
	z->parent = y;
	if(y == NULL) {
		z->c = BLACK;
		*proot = z;
		return;
	}
	else if(z->d <= y->d)
		y->left = z;
	else
		y->right = z;
	z->c = RED;
	RB_Insert_Fixup(proot, z);
}
Node* tree_min(Node* x) {
	Node* y = x;
	while(x != NULL) {
		y = x;
		x = x->left;
	}
	return y;
}
void RB_Delete_Fixup(Node** proot, Node* x) {
	while(x != *proot && x->c == BLACK) {
		Node* w = NULL;
		if(x == x->parent->left) {
			w = x->parent->right;
			if(w->c == RED) {
				w->c = BLACK;
				x->parent->c = RED;
				rotate_left(proot, x->parent);
				w = x->parent->right;
			}
			if(w->left->c == BLACK && w->right->c == BLACK) {
				w->c = RED;
				x = x->parent;
			}
			else {
				if(w->right->c == BLACK) {
					w->left->c = BLACK;
					w->c = RED;
					rotate_right(proot, w);
					w = x->parent->right;
				}
				w->c = x->parent->c;
				x->parent->c = BLACK;
				w->right->c = BLACK;
				rotate_left(proot, x->parent);
				x = *proot;
			}
		}
		else {
			w = x->parent->left;
			if(w->c == RED) {
				w->c = BLACK;
				x->parent->c = RED;
				rotate_right(proot, x->parent);
				w = x->parent->left;
			}
			if(w->left->c == BLACK && w->right->c == BLACK) {
				w->c = RED;
				x = x->parent;
			}
			else {
				if(w->left->c == BLACK) {
					w->right->c = BLACK;
					w->c = RED;
					rotate_left(proot, w);
					w = x->parent->left;
				}
				w->c = x->parent->c;
				x->parent->c = BLACK;
				w->right->c = BLACK;
				rotate_right(proot, x->parent);
				x = *proot;
			}
		}
	}
	x->c = BLACK;
}
void RB_Transplant(Node** proot, Node* u, Node* v) {
	if(u->parent == NULL)
		*proot = v;
	else if(u == u->parent->left)
		u->parent->left = v;
	else
		u->parent->right = v;
	if(v != NULL)
		v->parent = u->parent;
}
void delete(Node** proot, Node* z) {
	Node* y = z;
	Color yc = y->c;
	Node* x = NULL;
	if(z->left == NULL) {
		x = z->right;
		RB_Transplant(proot, z, z->right);
	}
	else if(z->right == NULL) {
		x = z->left;
		RB_Transplant(proot, z, z->left);
	}
	else {
		y = tree_min(z->right);
		yc = y->c;
		x = y->right;
		if(y->parent != z) {
			RB_Transplant(proot, y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}
		RB_Transplant(proot, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->c = z->c;
	}
	free(z);
	if(yc == BLACK && x != NULL)
		RB_Delete_Fixup(proot, x);
}
Node* newRBTree() {
	Node* root = NULL;
	int count = rand()%17 + 7;
	for(int i = 0; i < count; i++) {
		Node* z = newNode(NULL);
		z->d = rand()%101;
		insert(&root, z);
	}
	return root;
}

void printNode(Node* p) {
	if(p != NULL) {
		printf("(%d: %s, ", p->d, p->c==RED? "RED": "BLACK");
		if(p->parent != NULL)
			printf("parent: %d, ", p->parent->d);
		else
			printf("parent: NULL, ");
		if(p->left != NULL)
			printf("left: %d, ", p->left->d);
		else
			printf("left: NULL, ");
		if(p->right != NULL)
			printf("right: %d)\n", p->right->d);
		else
			printf("right: NULL)\n");
	}
}
void printTree(Node* root) {
	if(root == NULL)
		return;
	printTree(root->left);
	printNode(root);
	printTree(root->right);
}

int main() {
	srand(time(NULL));
	Node* root = newRBTree();
	printTree(root);
	printf("\n\n\n");
	delete(&root, root);
	printTree(root);
	return 0;
}
