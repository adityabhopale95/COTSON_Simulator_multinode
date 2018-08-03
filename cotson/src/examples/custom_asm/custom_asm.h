#ifndef CUSTOM_ASM_H
#define CUSTOM_ASM_H

#define _XS(s) #s

#define _TSU_BASE 0x2DAF0000
#define _TSUOP(x) (_TSU_BASE+(x))
#define _TSUOPI(x,i) (_TSU_BASE+((i)<<8)+(x))
#define _ASMOPX(x) ((x)-_TSU_BASE)
#define _ASMOP(x) (_ASMOPX(x) + 5000)

#define _TSU_TINIT _TSUOP(0x01)
#define _TSU_TREAD  _TSUOP(0x02)
#define _TSU_TWRITE _TSUOP(0x03)

#define _ASMVOL  __asm__ volatile 
#define _ASMNV   __asm__ 
#define _INL  __always_inline 

#define _REG_ARG1  "R"     /* argument 1 */
#define _REGQ_ARG1 "Q"     /* argument 1 (only ABCD)*/
#define _REG_ARG1d "0"     /* argument 1 = dest*/
#define _REG_ARG2  "R"     /* argument 2 */
#define _REGQ_ARG2 "Q"     /* argument 2 (only ABCD)*/
#define _REG_RES   "=R"    /* result */

#define _REG_CLOB  /*example: #define _REG_CLOB :"%rax" // don't forget the colon! */

#define _ASM_SEQ1(_of)  _XS(prefetchnta _of(%q0,%q0,1))
#define _ASM_SEQ1a(_of) _XS(prefetchnta _of(%q0,%%rax,1))
#define _ASM_SEQ2(_of)  _XS(prefetchnta _of(%q0,%q1,1))
#define _ASM_SEQ3(_of)  _XS(prefetchnta _of(%q0,%q2,1))

#define TSU_TINIT(_x) \
    _ASMVOL (_ASM_SEQ1(_TSU_TINIT)::_REG_ARG1(_x) _REG_CLOB)

#define TSU_READ(_off,_res) \
    _ASMVOL  (_ASM_SEQ1(_TSU_TREAD):_REG_RES(_res):_REG_ARG1d(_off) _REG_CLOB)

#define TSU_WRITE(_tloc,_val) \
    _ASMVOL  (_ASM_SEQ2(_TSU_TWRITE)::_REG_ARG1(_tloc),_REG_ARG2(_val) _REG_CLOB)


#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
_INL static void custom_init(uint64_t x) { TSU_TINIT(x); }
_INL static uint64_t custom_read(uint64_t loc) { uint64_t val; TSU_READ(loc,val); return val; }
_INL static void custom_write(uint64_t loc, uint64_t val) { TSU_WRITE(loc,val); }

#ifdef __cplusplus
}
#endif
#endif
