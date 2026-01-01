#ifndef TABLE_H
#define TABLE_H

#include "row.h"
#include <stdbool.h>
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

typedef struct {
    Table *table;
    uint32_t row_num;
    bool end_of_table; // indicates a position one past the last element
} Cursor;

extern const uint32_t PAGE_SIZE;
#define ROWS_PER_PAGE (PAGE_SIZE / ROW_SIZE)
uint32_t get_table_max_rows();

Table *db_open(const char *filename);
void db_close(Table *table);
Pager *pager_open(const char *filename);
void *cursor_value(Cursor *cursor);
void *get_page(Pager *pager, uint32_t page_num);
void pager_flush(Pager *pager, uint32_t page_num, uint32_t size);
Cursor *table_start(Table *table);
Cursor *table_end(Table *table);
void cursor_advance(Cursor *cursor);

#endif