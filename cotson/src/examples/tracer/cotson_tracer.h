// (C) Copyright 2006 Hewlett-Packard Development Company, L.P.
// $Id: cotson_tracer.h 745 2016-11-25 18:35:57Z robgiorgi $
#ifndef __COTSON_TRACING__
#define __COTSON_TRACING__

#ifdef __LP64__
#define COTSON_INTERNAL(k,v,w) \
{ \
        asm volatile (   \
				"push %%rax   ;\n\t" \
				"push %%rbx   ;\n\t" \
				"push %%rcx   ;\n\t" \
				"push %%rdx   ;\n\t" \
				"push %%rsi   ;\n\t" \
				"push %%rdi   ;\n\t" \
                : \
        );\
        asm volatile (   \
				"mov $0x80000009, %%rax   ;\n\t" \
				"cpuid                    ;\n\t" \
				: \
				: "D" (k), "S" (v), "b" (w) \
        );\
        asm volatile (   \
				"pop %%rdi   ;\n\t" \
				"pop %%rsi   ;\n\t" \
				"pop %%rdx   ;\n\t" \
				"pop %%rcx   ;\n\t" \
				"pop %%rbx   ;\n\t" \
				"pop %%rax   ;\n\t" \
				: \
        );\
}
#else
#define COTSON_INTERNAL(k,v,w) \
{ \
        asm volatile (   \
				"push %%eax   ;\n\t" \
				"push %%ebx   ;\n\t" \
				"push %%ecx   ;\n\t" \
				"push %%edx   ;\n\t" \
				"push %%esi   ;\n\t" \
				"push %%edi   ;\n\t" \
                : \
        );\
        asm volatile (   \
				"mov $0x80000009, %%eax   ;\n\t" \
				"cpuid                    ;\n\t" \
				: \
				: "D" (k), "S" (v), "b" (w) \
        );\
        asm volatile (   \
				"pop %%edi   ;\n\t" \
				"pop %%esi   ;\n\t" \
				"pop %%edx   ;\n\t" \
				"pop %%ecx   ;\n\t" \
				"pop %%ebx   ;\n\t" \
				"pop %%eax   ;\n\t" \
				: \
        );\
}
#endif

#define COTSON_ZONE_START(x) COTSON_INTERNAL(10,x,0)
#define COTSON_ZONE_END(x)   COTSON_INTERNAL(10,x,1)

#endif
