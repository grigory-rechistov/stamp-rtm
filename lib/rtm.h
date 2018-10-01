/*
 * rtm.h
 *
 * Created on: 03.07.2012
 *     Introduces intrinsics for Intel compiler as defines.
 */

#ifndef RTM_H_
#define RTM_H_

#include <stdint.h>
#include <assert.h>

/* Check CPUID Leaf 0x7 EBX[11] */
#define CPUID_RTM_CHECK {            \
        int rtm_present;             \
        uint32_t ebx, eax = 0x7;     \
        asm volatile("cpuid"         \
                     :"=b"(ebx)      \
                     :"a"(eax)       \
                     :"ecx","edx");  \
        rtm_present = ebx & (1u<<11); \
        assert(rtm_present && "RTM is not present"); \
        }

// These macros are never defined
#ifdef OLD_RTM_MACROSES
// Issue 'XBEGIN -6' to jump to the beginning of the Tx
#define _xbegin() {\
        __asm__ __volatile__ (".byte 0xc7, 0xf8, 0xfa, 0xff, 0xff, 0xff":::"eax") ;\
        }

#define _xend() {\
        __asm__ __volatile__ (".byte 0x0f, 0x01, 0xd5") ;\
        }

#define _xabort(byte) {\
        __asm__ __volatile__ (".byte 0xc6, 0xf8, " #byte :::"eax") ;\
        }
#endif // OLD_RTM_MACROSES

#define XBEGIN(label) asm volatile goto(".byte 0xc7,0xf8 ; .long %l0-1f\n1:" ::: "eax","memory" : label)
#define XEND() asm volatile(".byte 0x0f,0x01,0xd5" ::: "memory")
#define XFAIL(label) label: do { asm volatile(".byte 0x48, 0x87, 0xC9" ::: "memory"); asm volatile("" ::: "eax", "memory"); } while(0)
#define XFAIL_STATUS(label, status) label: asm volatile("" : "=a" (status) :: "memory")
#define XABORT(status) do { asm volatile (".byte 0xc6, 0xf8, " #status :::"eax"); } while(0)
#define XTEST() ({ char o = 0 ;                     \
           asm volatile(".byte 0x0f,0x01,0xd6 ; setnz %0" : "+r" (o)::"memory"); \
           o; })

/* Status bits */
#define XABORT_EXPLICIT_ABORT   (1 << 0)
#define XABORT_RETRY        (1 << 1)
#define XABORT_CONFLICT     (1 << 2)
#define XABORT_CAPACITY     (1 << 3)
#define XABORT_DEBUG        (1 << 4)
#define XABORT_STATUS(x)    (((x) >> 24) & 0xff)


#endif /* RTM_H_ */
