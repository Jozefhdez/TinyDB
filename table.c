#include "include/table.h"
#include "include/btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

Table *db_open(const char *filename) {
    Pager *pager = pager_open(filename);

    Table *table = (Table *)malloc(sizeof(Table));
    table->pager = pager;

    table->root_page_num = 0;
    if (pager->num_pages == 0) {
        // new database file, initialize page 0 as leaf node
        void *root_node = get_page(pager, 0);
        initialize_leaf_node(root_node);
        set_node_root(root_node, true);
    }

    return table;
}

void db_close(Table *table) {
    Pager *pager = table->pager;

    for (uint32_t i = 0; i < pager->num_pages; i++) {
        if (pager->pages[i] == NULL) {
            continue;
        }
        pager_flush(pager, i); // write to disk
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    int result = close(pager->file_descriptor);
    if (result == -1) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
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
        return internal_node_find(table, root_page_num, key);
    }
}