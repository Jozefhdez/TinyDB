#ifndef STATEMENT_H
#define STATEMENT_H

#include "input.h"
#include "row.h"
#include "table.h"
#include <stdbool.h>

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_NEGATIVE_ID,
    PREPARE_STRING_TOO_LONG,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR
} PrepareResult;

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL,
    EXECUTE_DUPLICATE_KEY,
    EXECUTE_FAILURE
} ExecuteResult;

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;

typedef struct {
    StatementType type;
    Row row_to_insert;
} Statement;

PrepareResult prepare_statement(InputBuffer *input_buffer,
                                Statement *statement);
PrepareResult prepare_insert(InputBuffer *input_buffer, Statement *statement);
ExecuteResult execute_statement(Statement *statement, Table *table);

#endif