#ifndef SERIALIZE_H_00
#define SERIALIZE_H_00

#include "types.h"
#include "list.h"

void read_world(LineSet** lines, Storage* line_storage);

void write_world(LineSet* lines, uint32_t line_count);

#endif
