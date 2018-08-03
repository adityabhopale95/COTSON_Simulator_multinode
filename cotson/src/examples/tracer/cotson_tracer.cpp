// This is a simple example of an application (to run in the guest)
// that invokes simulation hooks (handler defined in tracer_example.cpp).
//
// The call to COTSON_INTERNAL() [defined in cotson_tracer.h] uses
// a backdoor to invoke the simulator with some simulated-world parameters.
// The simulator can then use these parameters to triggers different 
// behavior (for example sampling), examine simulated memory or data
// structures, and so on.
//

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "cotson_tracer.h"

int main(int argc, char *argv[])
{
    char* sbuf = (char *)malloc(20);
	uint64_t magic_id=0;
	uint64_t a=0;
	uint64_t b=0;
	uint64_t cp_withb=0;

	magic_id=atoll(argv[1]);
	a=atoll(argv[2]);
	b=atoll(argv[3]);
    sprintf(sbuf,"hello world %ld",b); cp_withb=(uint64_t)sbuf;

// Pass two argc parameters by value and the third as a pointer
// to some simulated memory (a string buf in this example)
    printf("%s called with %ld %ld 0x%lx\n", argv[0],magic_id,a,cp_withb);
	COTSON_INTERNAL(magic_id,a,cp_withb);

	free(sbuf);
	return 0;
}
