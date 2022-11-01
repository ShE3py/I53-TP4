#include "util/stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/misc.h"

stack stack_init() {
	stack s;
	s.buf = NULL;
	s.len = 0;
	s.capacity = 0;
	
	return s;
}


stack stack_init_one(int val) {
	stack s;
	s.buf = checked_malloc(1 * sizeof(int));
	s.buf[0] = val;
	s.len = 1;
	s.capacity = 1;
	
	return s;
}

stack stack_init_from(const int *values, size_t n) {
	if(n == 0) {
		return stack_init();
	}
	
	stack s;
	s.buf = checked_malloc(n * sizeof(int));
	memcpy(s.buf, values, n * sizeof(int));
	s.len = n;
	s.capacity = n;
	
	return s;
}

void stack_push(stack *s, int val) {
	if(s->len == s->capacity) {
		stack_reserve(s, 5);
	}
	
	s->buf[s->len] = val;
	++(s->len);
}

void stack_push_all(stack *s, const int *values, size_t n) {
	if((s->len + n) > s->capacity) {
		stack_reserve(s, (s->len + n) - s->capacity);
	}
	
	memcpy(s->buf + s->len, values, n * sizeof(int));
	s->len += n;
}

void stack_reserve(stack *s, size_t n) {
	if(n > 0) {
		s->capacity += n;
		
		if(s->buf == NULL) {
			s->buf = checked_malloc(s->capacity * sizeof(int));
		}
		else {
			s->buf = checked_realloc(s->buf, s->capacity * sizeof(int));
		}
	}
}

int stack_pop(stack *s) {
	int val = stack_peek(*s);
	--(s->len);
	
	return val;
}

int stack_try_pop(stack *s, int *out) {
	if(stack_try_peek(*s, out)) {
		--(s->len);
		return 1;
	}
	
	return 0;
}

void stack_pop_all(stack *s) {
	s->len = 0;
}

int stack_peek(stack s) {
	if(stack_is_empty(s)) {
		fprintf(stderr, "peek() on empty stack");
		exit(1);
	}
	
	return s.buf[s.len - 1];
}

int stack_try_peek(stack s, int *out) {
	if(stack_is_empty(s)) {
		return 0;
	}
	
	*out = s.buf[s.len - 1];
	return 1;
}

int stack_is_empty(stack s) {
	return s.len == 0;
}

void stack_print(stack s) {
	if(s.len == 0) {
		printf("[]\n");
	}
	else {
		printf("[");
		
		for(size_t i = 0; i < (s.len - 1); ++i) {
			printf("%i, ", s.buf[i]);
		}
		
		printf("%i]\n", s.buf[s.len - 1]);
	}
}

void stack_free(stack *s) {
	free(s->buf);
	s->buf = NULL;
	s->len = 0;
	s->capacity = 0;
}
