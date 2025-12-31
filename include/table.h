#ifndef TABLE_H
#define TABLE_H

#include "row.h"
#include <stdint.h>

#define TABLE_MAX_PAGES 100

typedef struct {
    uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
} Table;

extern const uint32_t PAGE_SIZE;

uint32_t get_rows_per_page();
uint32_t get_table_max_rows();

Table *new_table();
void free_table(Table *table);
void *row_slot(Table *table, uint32_t row_num);

#endif