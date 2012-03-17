#include <stdlib.h>
#include <stdio.h>

#define DEBUG 1

/* Type and struct definitions for an integer linked list {{{ */

typedef struct _node {
	int value;
	struct _node *next;
} node;

typedef struct {
	node *head;
} list;

/* }}} */

/* Globals {{{ */

int n;				/* Number of vertices */
int m;				/* Number of edges */
int init;			/* Initial vertex */
list **edges;		/* List of edges for each vertex */

/* }}} */

/* Prototypes {{{ */

list *lst_new();
void  lst_insert_ordered(list *l, int value);
void  lst_remove(list *l, int value);
int   lst_empty(list *l);
void  lst_push(list *l, int value);
int   lst_pop(list *l);
void  lst_dump(list *);
void  lst_test();

void  setup();
int   check_solvable();

int   main();

/* }}} */

/* Constructors and methods for linked list {{{ */

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
	while ((cur = cur->next)) {
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

void lst_remove(list *l, int value) {
	node *pre, *cur;
	pre = cur = l->head;

	/* Empty list */
	if (!cur) return;

	/* Item is the head */
	if (cur->value == value) {
		l->head = cur->next;
		free(cur);
		return;
	}

	while ((cur = cur->next)) {
		if (cur->value == value) {
			pre->next = cur->next;
			free(cur);
			return;
		}
		pre = cur;
	}

}

int lst_empty(list *l) {
	return (l->head == NULL);
}

void lst_push(list *l, int value) {
	node *new  = malloc(sizeof(node));
	new->value = value;
	new->next  = l->head;
	l->head    = new;
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
	list *l;
	/* int   i; */

	printf("Creating new list\n");
	l = lst_new();
	printf("Inserting elements: 5, 3, 1, 7, 6\n");
	lst_insert_ordered(l, 5);
	lst_insert_ordered(l, 3);
	lst_insert_ordered(l, 1);
	lst_insert_ordered(l, 7);
	lst_insert_ordered(l, 6);
	printf("List dump: ");
	lst_dump(l);

	/*
	printf("Popping items:\n");
	while ((i = lst_pop(l)) != -1)
		printf("Popped: %d\n", i);
	*/

	printf("Pushing items: 6, 66, 666, 42\n");
	lst_push(l, 6);
	lst_push(l, 66);
	lst_push(l, 666);
	lst_push(l, 42);
	lst_dump(l);

	printf("Removing items: 3, 6, 1, 4\n");
	lst_remove(l, 3);
	lst_dump(l);
	lst_remove(l, 6);
	lst_dump(l);
	lst_remove(l, 1);
	lst_dump(l);
	lst_remove(l, 4);
	lst_dump(l);
}

/* }}} */

/* Parsing and setup {{{ */

void setup() {
	int i, a, b;

	/* Read n, m and init */
	scanf("%d\n%d\n%d\n", &n, &m, &init);

	/* Spawn edge lists */
	edges = malloc(sizeof(list *) * n);
	for (i = 0; i < n; i++)
		edges[i] = lst_new();

	/* Populate edge lists */
	while (scanf("%d %d\n", &a, &b) != EOF) {
#if DEBUG >= 2
		printf("Read line: %d, %d\n", a, b);
#endif
		lst_insert_ordered(edges[a-1], b);
		lst_insert_ordered(edges[b-1], a);
	}

#if DEBUG >= 1
	printf("Edge lists:\n");
	for (i = 0; i < n; i++)
		lst_dump(edges[i]);
#endif

}

int check_solvable() {
	return 0;
}

/* }}} */

/* Main */

int main() {

	lst_test(); return 0;

	setup();

	if (!check_solvable())
		return 0;

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

/* vim: set ts=4 tw=78 fdm=marker fdl=0 fen :*/
