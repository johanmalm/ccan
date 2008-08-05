#include "talloc/talloc.c"
#include "tap/tap.h"
#include <assert.h>

static int ext_alloc_count, ext_free_count, ext_realloc_count;
static void *expected_parent;

static void *ext_realloc(const void *parent, void *ptr, size_t size)
{
	ok1(parent == expected_parent);
	if (ptr == NULL)
		ext_alloc_count++;
	if (size == 0)
		ext_free_count++;
	if (ptr && size)
		ext_realloc_count++;
	return realloc(ptr, size);
}

int main(void)
{
	char *p, *p2, *head;
	plan_tests(12);

	expected_parent = NULL;
	head = talloc_add_external(NULL, ext_realloc);
	assert(head);
	ok1(ext_alloc_count == 1);

	expected_parent = head;
	p = talloc_array(head, char, 1);
	ok1(ext_alloc_count == 2);
	assert(p);

	/* Child is also externally allocated */
	expected_parent = p;
	p2 = talloc(p, char);
	ok1(ext_alloc_count == 3);

	expected_parent = head;
	p = talloc_realloc(NULL, p, char, 1000);
	ok1(ext_realloc_count == 1);
	assert(p);

	expected_parent = p;
	talloc_free(p2);
	ok1(ext_free_count == 1);

	expected_parent = head;
	talloc_free(p);
	ok1(ext_free_count == 2);

	return exit_status();
}
