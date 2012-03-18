#include <stdlib.h>
#include <stdio.h>

#define DEBUG 1

#if DEBUG >= 1
#include <signal.h>
#endif

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

int    n;        /* Number of vertices                                 */
int    m;        /* Number of edges                                    */
int    init;     /* Initial vertex                                     */
list **edges;    /* Array of lists of edges (one list per vertex)      */
list  *cycles;   /* List of lists of vertices (one per Eulerian cycle) */
list  *c_0;      /* Main Eulerian sub-cycle (see `collect_cycles')     */

/* }}} */

/* Prototypes {{{ */

list *lst_new();
void  lst_insert_ordered(list *l, int value);
void  lst_remove(list *l, int value);
int   lst_empty(list *l);
int   lst_size(list *l);
void  lst_push(list *l, int value);
int   lst_pop(list *l);
int   lst_first(list *l);
void  lst_reverse(list *l);
int   lst_subst(list *l, int old, list *new);
void  lst_dump(list *);
void  lst_test();

void  setup();
int   check_solvable();

int   initial_vertex(list *cycle);
void  collect_cycles();
void  merge_cycles();

void  output();

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

	/* Item is at the head */
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

int lst_size(list *l) {
	int   siz = 1;
	node *n   = l->head;

	if (!n) return 0;

	while ((n = n->next)) siz++;

	return siz;
}

void lst_push(list *l, int value) {
	node *new  = malloc(sizeof(node));
	new->value = value;
	new->next  = l->head;
	l->head    = new;
}

int lst_pop(list *l) {
	int   res;
	node *n   = l->head;

	if (!n) return -1;

	res = n->value;
	l->head = n->next;
	free(n);
	return res;
}

int lst_first(list *l) {
	if (!l->head) return -1;
	return l->head->value;
}

void lst_reverse(list *l) {
	list *new  = lst_new();
	node *n    = l->head;
	node *next;

	if (!n) return;

	do {
		next = n->next;
		lst_push(new, lst_pop(l));
	} while ((n = next));

#if DEBUG >= 1
	if (!lst_empty(l)) printf("L IS NOT EMPTY\n");
#endif

	l->head = new->head;
	free(new);


}

int lst_subst(list *l, int old, list *new) {
	node *pre, *cur, *new_head, *new_tail;

	if (!l->head || !new->head) return 0;

	/* Fetch pointers for new_head and new_tail */
	pre = cur = new_head = new->head;
	while ((cur = cur->next)) pre = cur;
	new_tail = pre;

	pre = cur = l->head;
	while ((cur = cur->next)) {
		if (cur->value == old) {
			pre->next = new_head;
			new_tail->next = cur->next;
			free(cur);
			return 1;
		}
		pre = cur;
	}

	return 0;
}

void lst_dump(list *l) {
	node *n;

	for (n = l->head; n; n = n->next)
		printf("%d%s", n->value, n->next ? " " : "");
	printf("\n");
}

#if 0
void lst_test() {
	list *l;
	/* int   i; */

	printf("Creating new list\n");
	l = lst_new();
	printf("Size: %d\n", lst_size(l));
	printf("Inserting elements: 5, 3, 1, 7, 6\n");
	lst_insert_ordered(l, 5);
	lst_insert_ordered(l, 3);
	lst_insert_ordered(l, 1);
	lst_insert_ordered(l, 7);
	lst_insert_ordered(l, 6);
	printf("Size: %d\n", lst_size(l));
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
	printf("Size: %d\n", lst_size(l));
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
#endif

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
	int i;

	for (i = 0; i < n; i++) {
		int size = lst_size(edges[i]);
		/* Graph must be connected and every vertex must be of even degree */
		if (size == 0 || size % 2 == 1)
			return 0;
	}

	return 1;
}

/* }}} */

/* Path finding {{{ */

int initial_vertex(list *cycle) {
	node *n;

	for (n = cycle->head; n; n = n->next) {
		int v = n->value;
		if (!lst_empty(edges[v-1]))
			return v;
	}

	return 0;
}

void collect_cycles() {
	int   cur;
	list *cycle;

	cycles = lst_new();

	do {
#if DEBUG >= 1
		printf("NEW CYCLE, init=%d\n", init);
#endif
		cur = init;
		cycle = lst_new();
		lst_push(cycle, cur);

		/*
		 * Will only be called once. c_0 points to the first Eulerian
		 * sub-cycle we find, which will be our 'main' cycle for the merger
		 * later on.
		 */
		if (lst_empty(cycles)) c_0 = cycle;

		while (!lst_empty(edges[cur-1])) {
			int next = lst_pop(edges[cur-1]);
			lst_remove(edges[next-1], cur);
			lst_push(cycle, next);
			cur = next;
		}

#if DEBUG >= 1
		lst_dump(cycle);
#endif
		lst_push(cycles, (int) cycle);

	} while ((init = initial_vertex(cycle)));
}

void merge_cycles() {

	int i, n_cycles = lst_size(cycles);
	list **cycles_arr = malloc(sizeof(list *) * n_cycles);

	for (i = n_cycles - 1; i >= 0; i--)
		cycles_arr[i] = (list *) lst_pop(cycles);

#if DEBUG >= 1
	printf("SIZE: %d\n", n_cycles);
#endif

#if DEBUG >= 2
	for (i = n_cycles - 1; i >= 0; i--) {
		printf("--- %d\n", (int) cycles_arr[i]);
		printf("+++ ");
		lst_dump((list *) cycles_arr[i]);
	}
#endif

	for (i = n_cycles - 1; i > 0; i--) {

		list *c_i = cycles_arr[i];
		int   end = lst_first(c_i);
#if DEBUG >= 1
		printf("<-- MERGING AT %d\n", end);
		lst_dump(cycles_arr[i-1]);
		if (!lst_subst(cycles_arr[i-1], end, c_i)) {
			printf("-+-+-+- OOPS\n");
			raise(SIGUSR1);
		}
		lst_dump(cycles_arr[i-1]);
		printf("--> MERGED INTO %d\n", i-1);
#else
		lst_subst(cycles_arr[i-1], end, c_i);
#endif
	}
	free(cycles_arr);
}

/* }}} */

/* Output {{{ */

void output() {
	lst_reverse(c_0);
	lst_dump(c_0);
}

/* }}} */

/* Main */

int main() {

	setup();

	if (!check_solvable()) {
		printf("-1\n");
		return 0;
	}

	collect_cycles();
	merge_cycles();
	output();

	return 0;
}

/* vim: set ts=4 tw=78 fdm=marker fdl=0 fen :*/
