/*
 * common.h
 *
 *  Created on: 2009-1-7
 *      Author: Argon.Sun
 */

#ifndef COMMON_H_
#define COMMON_H_

#ifdef _MSC_VER
#pragma warning(disable: 4244)
#endif

typedef unsigned long uptr;
typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef unsigned char byte;
typedef long ptr;
typedef long long int64;
typedef int int32;
typedef short int16;
typedef char int8;
typedef int BOOL;

#define MATCH_ALL(x,y) (((x)&(y))==(y))
#define MATCH_ANY(x,y) ((x)&(y))
#define ADD_BIT(x,y) ((x)|=(y))
#define REMOVE_BIT(x,y) ((x)&=~(y))

#define OPERATION_SUCCESS 1
#define OPERATION_FAIL 0
#define OPERATION_CANCELED -1
#define TRUE 1
#define FALSE 0
#ifndef NULL
#define NULL 0
#endif
struct card_sort {
	bool operator()(void* const & c1, void* const & c2) const;
};

#endif /* COMMON_H_ */
