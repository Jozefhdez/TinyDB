#ifndef INPUT_H
#define INPUT_H

#include "table.h"
#include <stddef.h>

typedef struct {
    char *buffer;
    size_t buffer_length;
    size_t input_length;
} InputBuffer;

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND,
    META_COMMAND_SCHEMA
} MetaCommandResult;

typedef enum {
    COMMAND_EXIT,
    COMMAND_TABLES,
    COMMAND_SCHEMA,
    COMMAND_HELP,
    COMMAND_CONSTANTS,
    COMMAND_BTREE,
    COMMAND_UNKNOWN
} MetaCommandType;

InputBuffer *new_input_buffer();
void read_input(InputBuffer *input_buffer);
void close_input_buffer(InputBuffer *input_buffer);
void table_information(Table *table);
void table_schema();
MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *table);
void print_constants();

#endif