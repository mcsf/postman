#include <stdlib.h>
#include <stdio.h>

/* Type and struct definitions for an integer linked list */

typedef struct _node {
	int value;
	struct _node *next;
} node;

typedef struct {
	node *head;
} list;


/* Prototypes */

list *lst_new();
void lst_insert_ordered(list *, int);
int lst_empty(list *l);
int lst_pop(list *l);
void lst_dump(list *);
void lst_test();

int main();


/* Constructors and methods for linked list */

list *lst_new() {
	list *l = (list *) malloc(sizeof(list));
	l->head = NULL;
	return l;
}

void lst_insert_ordered(list *l, int value) {
	node *pre, *cur, *new;
	pre = cur = l->head;
	new = (node *) malloc(sizeof(node));
	new->value = value;
	new->next = NULL;

	/* Empty list */
	if (!cur) {
		l->head = new;
		return;
	}

	/* One-item list, insert before */
	if (value < cur->value) {
		new->next = cur;
		l->head = new;
		return;
	}

	/* List traversal */
	while (cur = cur->next) {
		if (value < cur->value) {
			pre->next = new;
			new->next = cur;
			return;
		}
		pre = cur;
	}

	/* Insert at end */
	pre->next = new;
}

int lst_empty(list *l) {
	return (l->head == NULL);
}

int lst_pop(list *l) {
	node *n = l->head;
	int res = n->value;

	if (!n) return -1;

	l->head = n->next;
	free(n);
	return res;
}

void lst_dump(list *l) {
	node *n;

	for (n = l->head; n; n = n->next)
		printf("%d ", n->value);
	printf("\n");
}

void lst_test() {
	printf("Creating new list\n");
	list *l = lst_new();
	printf("Inserting elements: 5, 3, 1, 7, 6\n");
	lst_insert_ordered(l, 5);
	lst_insert_ordered(l, 3);
	lst_insert_ordered(l, 1);
	lst_insert_ordered(l, 7);
	lst_insert_ordered(l, 6);
	printf("List dump: ");
	lst_dump(l);
	printf("Popping items:\n");
	int i;
	while ((i = lst_pop(l)) != -1)
		printf("Popped: %d\n", i);
}


/* Main */

int main() {

	lst_test();

	/* Rough sequence:
	 * 1. Parse input:
	 * - check for Eulerian graph
	 * - set n, m, first vertex
	 * - generate n edge lists
	 * 2. Collect cycles
	 * 3. Merge cycles
	 * 4. Generate output
	 */

	return 0;
}
