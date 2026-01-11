#ifndef BTREE_H
#define BTREE_H

#include "row.h"
#include "table.h"
#include <stdint.h>

typedef enum { NODE_INTERNAL, NODE_LEAF } NodeType;

/*
    Common Node Header Layout
*/
extern const uint32_t NODE_TYPE_SIZE;
extern const uint32_t NODE_TYPE_OFFSET;
extern const uint32_t IS_ROOT_SIZE;
extern const uint32_t IS_ROOT_OFFSET;
extern const uint32_t PARENT_POINTER_SIZE;
extern const uint32_t PARENT_POINTER_OFFSET;
extern const uint8_t COMMON_NODE_HEADER_SIZE;

/*
    Leaf Node Header Layout
*/
extern const uint32_t LEAF_NODE_NUM_CELLS_SIZE;
extern const uint32_t LEAF_NODE_NUM_CELLS_OFFSET;
extern const uint32_t LEAF_NODE_NEXT_LEAF_SIZE;
extern const uint32_t LEAF_NODE_NEXT_LEAF_OFFSET;
extern const uint32_t LEAF_NODE_HEADER_SIZE;

/*
     Leaf Node Body Layout
*/
extern const uint32_t LEAF_NODE_KEY_SIZE;
extern const uint32_t LEAF_NODE_KEY_OFFSET;
extern uint32_t LEAF_NODE_VALUE_SIZE;
extern uint32_t LEAF_NODE_VALUE_OFFSET;
extern uint32_t LEAF_NODE_CELL_SIZE;
extern uint32_t LEAF_NODE_SPACE_FOR_CELLS;
extern uint32_t LEAF_NODE_MAX_CELLS;

/*
    Internal Node Header Layout
*/
extern const uint32_t INTERNAL_NODE_NUM_KEYS_SIZE;
extern const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET;
extern const uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE;
extern const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET;
extern const uint32_t INTERNAL_NODE_HEADER_SIZE;

/*
    Internal Node Body Layout
*/
extern const uint32_t INTERNAL_NODE_KEY_SIZE;
extern const uint32_t INTERNAL_NODE_CHILD_SIZE;
extern const uint32_t INTERNAL_NODE_CELL_SIZE;

extern uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT;
extern uint32_t LEAF_NODE_LEFT_SPLIT_COUNT;

void btree_init_constants();

uint32_t *leaf_node_num_cells(void *node);
void *leaf_node_cell(void *node, uint32_t cell_num);
uint32_t *leaf_node_key(void *node, uint32_t cell_num);
void *leaf_node_value(void *node, uint32_t cell_num);
void initialize_leaf_node(void *node);
void initialize_internal_node(void *node);
uint32_t *internal_node_num_keys(void *node);
uint32_t *internal_node_right_child(void *node);
uint32_t *internal_node_cell(void *node, uint32_t cell_num);
uint32_t *internal_node_child(void *node, uint32_t child_num);
uint32_t *internal_node_key(void *node, uint32_t key_num);
void update_internal_node_key(void *node, uint32_t old_key, uint32_t new_key);
uint32_t get_node_max_key(Pager *pager, void *node);
void create_new_root(Table *table, uint32_t right_child_page_num);
void leaf_node_insert(Cursor *cursor, uint32_t key, Row *value);
bool is_node_root(void *node);
void set_node_root(void *node, bool is_root);
void leaf_node_split_and_insert(Cursor *cursor, uint32_t key, Row *value);
Cursor *leaf_node_find(Table *table, uint32_t page_num, uint32_t key);
uint32_t *node_parent(void *node);
NodeType get_node_type(void *node);
void set_node_type(void *node, NodeType type);
void indent(uint32_t level);
void print_tree(Pager *pager, uint32_t page_num, uint32_t indentation_level);
uint32_t internal_node_find_child(void *node, uint32_t key);
Cursor *internal_node_find(Table *table, uint32_t page_num, uint32_t key);
uint32_t *leaf_node_next_leaf(void *node);
void internal_node_insert(Table *table, uint32_t parent_page_num,
                          uint32_t child_page_num);
void internal_node_split_and_insert(Table *table, uint32_t parent_page_num,
                                    uint32_t child_page_num);

#endif