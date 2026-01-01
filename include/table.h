#ifndef TABLE_H
#define TABLE_H

#include "row.h"
#include <stdint.h>

#define TABLE_MAX_PAGES 100

typedef struct {
    int file_descriptor;
    uint32_t file_length;
    void *pages[TABLE_MAX_PAGES];
} Pager;

typedef struct {
    uint32_t num_rows;
    Pager *pager;
} Table;

extern const uint32_t PAGE_SIZE;

uint32_t get_rows_per_page();
uint32_t get_table_max_rows();

Table *db_open(const char *filename);
void db_close(Table *table);
Pager *pager_open(const char *filename);
void *row_slot(Table *table, uint32_t row_num);
void *get_page(Pager *pager, uint32_t page_num);
void pager_flush(Pager *pager, uint32_t page_num, uint32_t size);

#endif