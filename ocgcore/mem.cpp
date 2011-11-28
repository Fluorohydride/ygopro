/*
 * mem.cpp
 *
 *  Created on: 2011-9-8
 *      Author: argon
 */
//#define USE_TCMALLOC

#ifdef USE_TCMALLOC

#define PERFTOOLS_DLL_DECL
#include <tcmalloc.h>

void* operator new(size_t size) {
	return tc_malloc(size);
}
void* operator new[](size_t size) {
	return tc_malloc(size);
}
void* operator new(size_t size, const std::nothrow_t& nt) {
	return tc_malloc(size);
}
void* operator new[](size_t size, const std::nothrow_t& nt) {
	return tc_malloc(size);
}
void operator delete(void* p) {
	return tc_free(p);
}
void operator delete[](void* p) {
	return tc_free(p);
}
void operator delete(void* p, const std::nothrow_t& nt) {
	return tc_free(p);
}
void operator delete[](void* p, const std::nothrow_t& nt) {
	return tc_free(p);
}

#endif //USE_TCMALLOC
