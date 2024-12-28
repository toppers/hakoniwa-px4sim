#ifndef _ICOMM_OSDEP_HPP_
#define _ICOMM_OSDEP_HPP_

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#ifdef _WIN32
typedef SOCKET ICOMM_SOCKET;
#else
typedef int ICOMM_SOCKET;
#endif


#endif /* _ICOMM_OSDEP_HPP_ */