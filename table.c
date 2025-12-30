#include "table.h"
#include <stdio.h>
#include <stdlib.h>

const uint32_t PAGE_SIZE = 4096; // 4 kilobytes

uint32_t get_rows_per_page() {
    return PAGE_SIZE / ROW_SIZE;
}

uint32_t get_table_max_rows() {
    return get_rows_per_page() * TABLE_MAX_PAGES;
}

Table *new_table() {
    Table *table = (Table *)malloc(sizeof(Table));
    table->num_rows = 0;
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        table->pages[i] = NULL;
    }
    return table;
}

void free_table(Table *table) {
    for (int i = 0; table->pages[i]; i++) {
        free(table->pages[i]);
    }
    free(table);
}

void *row_slot(Table *table, uint32_t row_num) {
    uint32_t rows_per_page = get_rows_per_page();
    uint32_t page_num = row_num / rows_per_page;
    void *page = table->pages[page_num];
    if (page == NULL) {
        // allocate memory only when we try to access page
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }
    uint32_t row_offset = row_num % rows_per_page;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}