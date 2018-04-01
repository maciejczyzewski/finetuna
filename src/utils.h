#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

//== MACROS
#define FOR(i, a, b) for(int i=a;i<=b;i++)

#define max(a,b)                \
	({ __typeof__ (a) _a = (a); \
	   __typeof__ (b) _b = (b); \
	   _a > _b ? _a : _b; })
#define min(a,b)                \
	({ __typeof__ (a) _a = (a); \
	   __typeof__ (b) _b = (b); \
	   _a < _b ? _a : _b; })

#define copy2d(a, b) FOR(i, 0, 10-1) \
	FOR(j, 0, 10-1) a[i][j] = b[i][j];

//== DEBUG
#define AA(S, ...) fprintf(stderr, "" S "\n", ##__VA_ARGS__);
#define EE(S, ...) fprintf(stderr,                                         \
	"\n\x1b[1m(%s:%d, %s)\x1b[0m\n  \x1b[1m\x1b[31merror:\x1b[0m " S "\n", \
	__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);

//== TRICKS
const int oo = 0x44444444;
