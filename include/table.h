#ifndef TABLE_H
#define TABLE_H

#include "cursor.h"
#include "pager.h"
#include "row.h"
#include <stdbool.h>
#include <stdint.h>

extern const uint32_t PAGE_SIZE;
#define ROWS_PER_PAGE (PAGE_SIZE / ROW_SIZE)

typedef struct Table {
    uint32_t num_rows;
    Pager *pager;
    uint32_t root_page_num;
} Table;

Table *db_open(const char *filename);
void db_close(Table *table);
Cursor *table_find(Table *table, uint32_t key);

#endif