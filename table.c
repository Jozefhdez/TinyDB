#include "include/table.h"
#include "include/btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const uint32_t PAGE_SIZE = 4096; // 4 kilobytes

uint32_t get_table_max_rows() {
    return ROWS_PER_PAGE * TABLE_MAX_PAGES;
}

Table *db_open(const char *filename) {
    Pager *pager = pager_open(filename);

    Table *table = (Table *)malloc(sizeof(Table));
    table->pager = pager;

    table->root_page_num = 0;
    if (pager->num_pages == 0) {
        // new database file, initialize page 0 as leaf node
        void *root_node = get_page(pager, 0);
        initialize_leaf_node(root_node);
    }

    return table;
}

void db_close(Table *table) {
    Pager *pager = table->pager;

    for (uint32_t i = 0; i < pager->num_pages; i++) {
        if (pager->pages[i] == NULL) {
            continue;
        }
        pager_flush(pager, i);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    int result = close(pager->file_descriptor);
    if (result == -1) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void *page = pager->pages[i];
        if (page) {
            free(page);
            pager->pages[i] = NULL;
        }
    }

    free(pager);
    free(table);
}

Cursor *table_find(Table *table, uint32_t key) {
    uint32_t root_page_num = table->root_page_num;
    void *root_node = get_page(table->pager, root_page_num);

    if (get_node_type(root_node) == NODE_LEAF) {
        return leaf_node_find(table, root_page_num, key);

    } else {
        printf("Need to implement searching an internal node\n");
        exit(EXIT_FAILURE);
    }
}