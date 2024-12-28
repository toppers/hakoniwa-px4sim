#ifndef _HAKO_OSDEP_H_
#define _HAKO_OSDEP_H_

#include "drone_physics_osdep.h"

#ifdef WIN32
#include "windows.h"
static inline void usleep(long microseconds) {
    Sleep(microseconds / 1000);
}
#else
#include <unistd.h>
#endif

#endif /* _HAKO_OSDEP_H_ */