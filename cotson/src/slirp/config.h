#define COTSON_CONFIG 1
#define HOST_X86_64 1
#define HOST_LONG_BITS 64
#define HAVE_BYTESWAP_H 1
#define CONFIG_SLIRP 1

#ifndef container_of
#define container_of(ptr, type, member) ({                      \
     const typeof(((type *) 0)->member) *__mptr = (ptr);     \
     (type *) ((char *) __mptr - offsetof(type, member));})
#endif

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *) 0)->MEMBER)
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

