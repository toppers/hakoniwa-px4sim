#ifndef _HAKO_UTILS_HPP_
#define _HAKO_UTILS_HPP_

#include <iostream>
#include <stdlib.h>

static inline void HAKO_ABORT(const char* errmsg)
{
    std::cerr << "ERROR: " << errmsg << std::endl;
    exit(1);
}
#define HAKO_ASSERT(expr)	\
do {	\
	if (!(expr))	{	\
		std::cerr <<"ASSERTION FAILED: " << __FILE__ << ", " <<  __FUNCTION__ << ", " << __LINE__ << " :" << #expr << std::endl;	\
		exit(1);	\
	}	\
} while (0)
#endif /* _HAKO_UTILS_HPP_*/