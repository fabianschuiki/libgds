/* Copyright (c) 2016 Fabian Schuiki */
#pragma once
#include <stddef.h>
#include <stdbool.h>

typedef struct ptrset ptrset_t;

void be16_to_host(void *buffer, size_t length);
void be32_to_host(void *buffer, size_t length);
void be64_to_host(void *buffer, size_t length);


/**
 * @defgroup ptrset Pointer Set
 * @{
 */
struct ptrset {
	/// The number of pointers in the set.
	size_t size;
	/// The maximum number of pointers the set can hold before it needs to be
	/// reallocated.
	size_t capacity;
	/// The pointers in the set, in ascending order.
	void **items;
};

void ptrset_init(ptrset_t*);
void ptrset_dispose(ptrset_t*);
bool ptrset_add(ptrset_t*, void*);
bool ptrset_remove(ptrset_t*, void*);
bool ptrset_contains(ptrset_t*, void*);
/** @} */


/* String and memory duplication */
char *gds_dupstr(const char *src);
char *gds_dupstrn(const char *src, size_t len);
void *gds_dupmem(const void *src, size_t len);
