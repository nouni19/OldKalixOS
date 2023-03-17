#include <stdint.h>

typedef struct {
	uint64_t base;
	uint64_t limit;
	uint16_t accessbyte;
	uint8_t flags;
} GDTDesc;