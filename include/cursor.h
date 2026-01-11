#ifndef CURSOR_H
#define CURSOR_H

#include <stdbool.h>
#include <stdint.h>

// forward declaration to avoid circular dependency
struct Table;

typedef struct {
    struct Table *table;
    uint32_t page_num;
    uint32_t cell_num;
    bool end_of_table; // indicates a position one past the last element
} Cursor;

void *cursor_value(Cursor *cursor);
void cursor_advance(Cursor *cursor);
Cursor *table_start(struct Table *table);

#endif