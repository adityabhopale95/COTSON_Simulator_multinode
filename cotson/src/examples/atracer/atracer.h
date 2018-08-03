/*
   Copyright (c) 2015-2018 Roberto Giorgi - University of Siena
   Copyright (c) 2015-2018 Hewlett-Packard Development Company, L.P.
  
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/
#ifndef _ATRACER_H_INCLUDED
#define _ATRACER_H_INCLUDED

/* COTSON Base for custom instruction definition */
/* IMPORTANT: keep in sync with AbAeterno: abaeterno/lib/cpuid_call.h */
#define _COTSON_CUSTOM_BASE	0x2DAF0000
#define _COTSON_CUSTOM_TRACER	0xFF

/* Auxiliary macros */
#define _CASM_OPCODE_I(_o,_i)	(_COTSON_CUSTOM_BASE+((_i)<<8)+(_o))
#define _XP(str)		#str
#define _AS0(_of)		_XP(prefetchnta _of(%%rax,%%rax))
#define _JHERE			";jmp 1f;1:"
#define _AVOL			__asm__ volatile 
#define _PACKED8(_sw,_zo)	((((_sw)&1)<<7)|((_zo)&0x7F))

/* Accurate Zone Start/Stop functions
   similar to COTSON_INTERNAL(10,zone,0) --> azonestart(zone)
   similar to COTSON_INTERNAL(10,zone,1) --> azonestop(zone)
   NOTE: zone numbers are limited from 1 to 127
*/
#define azonestart(_z)		_AVOL (_AS0(_CASM_OPCODE_I(_COTSON_CUSTOM_TRACER,_PACKED8(0,_z))) _JHERE::)
#define azonestop(_z)		_AVOL (_AS0(_CASM_OPCODE_I(_COTSON_CUSTOM_TRACER,_PACKED8(1,_z))) _JHERE::)

#endif /* ATRACER_H END*/
