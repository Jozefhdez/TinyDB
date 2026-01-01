#define _POSIX_C_SOURCE 200809L
#include "include/input.h"
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
    uint32_t ROWS_PER_PAGE = get_rows_per_page();
    uint32_t total_pages =
        (table->num_rows + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
    off_t current_file_length =
        lseek(table->pager->file_descriptor, 0, SEEK_END);
    printf("Table contains %" PRIu32 " rows\n", table->num_rows);
    printf("Table uses %" PRIu32 " pages\n", total_pages);
    printf("File size: %" PRId64 " bytes\n", (int64_t)current_file_length);
    printf("Note: Recent inserts are in memory; file size updates on exit.\n");
}

MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *table) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) {
        db_close(table);
        exit(EXIT_SUCCESS);
    }
    if (strcmp(input_buffer->buffer, ".tables") == 0) {
        table_information(table);
        return (META_COMMAND_SUCCESS);
    } else if (strcmp(input_buffer->buffer, ".help") == 0) {
        printf("Commands:\n");
        printf("  .tables - Show basic table information\n");
        printf("  .exit   - Exit the program\n");
        printf("  .help   - Show this help message lmao\n");
        return META_COMMAND_SUCCESS;
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}