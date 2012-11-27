#ifndef __CSIMPLE_INTERNET_H
#define __CSIMPLE_INTERNET_H

//todo add usage comment for every function.
#include <stdint.h>

#include "simple_platform.h"
#include "network_header.h"
#include "network_error.h"

#ifdef	__cplusplus
extern "C"
{
#endif

void internet_gethostbyname();

void internet_gethostbyaddr();

void internet_ptoa();

void internet_atop();

void internet_ntop();

void internet_pton();

void internet_addr();

#ifdef	__cplusplus
}
#endif

#endif