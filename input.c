#define _POSIX_C_SOURCE 200809L
#include "include/input.h"
#include "include/btree.h"
#include "include/table.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

InputBuffer *new_input_buffer() {
    InputBuffer *input_buffer = (InputBuffer *)malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;
    return input_buffer;
}

void read_input(InputBuffer *input_buffer) {
    size_t bytes_read =
        getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}

void close_input_buffer(InputBuffer *input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

void table_information(Table *table) {
    uint32_t total_pages =
        (table->num_rows + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
    off_t current_file_length =
        lseek(table->pager->file_descriptor, 0, SEEK_END);
    printf("Table contains %" PRIu32 " rows\n", table->num_rows);
    printf("Table uses %" PRIu32 " pages\n", total_pages);
    printf("File size: %" PRId64 " bytes\n", (int64_t)current_file_length);
    printf("Note: Recent inserts are in memory; file size updates on exit.\n");
}

void table_schema() {
    printf("\nSchema:\n");
    printf("  id: uint32_t (%d bytes)\n", (int)sizeof(uint32_t));
    printf("  username: char[%d] (%d bytes)\n", COLUMN_USERNAME_SIZE,
           COLUMN_USERNAME_SIZE + 1);
    printf("  email: char[%d] (%d bytes)\n\n", COLUMN_EMAIL_SIZE,
           COLUMN_EMAIL_SIZE + 1);
}

void print_constants() {
    printf("ROW_SIZE: %d\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

MetaCommandType get_meta_command_type(const char *buffer) {
    if (strcmp(buffer, ".exit") == 0)
        return COMMAND_EXIT;
    if (strcmp(buffer, ".tables") == 0)
        return COMMAND_TABLES;
    if (strcmp(buffer, ".schema") == 0)
        return COMMAND_SCHEMA;
    if (strcmp(buffer, ".constants") == 0)
        return COMMAND_CONSTANTS;
    if (strcmp(buffer, ".btree") == 0)
        return COMMAND_BTREE;
    if (strcmp(buffer, ".help") == 0)
        return COMMAND_HELP;
    return COMMAND_UNKNOWN;
}

MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *table) {
    MetaCommandType command = get_meta_command_type(input_buffer->buffer);
    switch (command) {
    case COMMAND_EXIT:
        db_close(table);
        exit(EXIT_SUCCESS);
    case COMMAND_TABLES:
        table_information(table);
        return META_COMMAND_SUCCESS;
    case COMMAND_SCHEMA:
        table_schema(table);
        return META_COMMAND_SCHEMA;
    case COMMAND_CONSTANTS:
        printf("Constants:\n");
        print_constants();
        return META_COMMAND_SUCCESS;
    case COMMAND_BTREE:
        printf("Tree:\n");
        print_leaf_node(get_page(table->pager, 0));
        return META_COMMAND_SUCCESS;
    case COMMAND_HELP:
        printf("Commands:\n");
        printf("  .tables.   - Show basic table information\n");
        printf("  .schema    - Show table schema\n");
        printf("  .constants - Show btree constants\n");
        printf("  .btree     - printins the structure of btree\n");
        printf("  .exit      - Exit the program\n");
        printf("  .help      - Show this help message lmao\n");
        return META_COMMAND_SUCCESS;
    case COMMAND_UNKNOWN:
    default:
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}