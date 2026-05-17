#include "include/statement.h"
#include "include/btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PrepareResult prepare_statement(InputBuffer *input_buffer,
                                Statement *statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        return prepare_insert(input_buffer, statement);
    }
    if (strcmp(input_buffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    if (strncmp(input_buffer->buffer, "select where id >=", 18) == 0) {
        statement->type = STATEMENT_RANGE_SCAN;
        sscanf(input_buffer->buffer, "select where id >= %d and id <= %d",
               &statement->key, &statement->key_end);
        return PREPARE_SUCCESS;
    }
    if (strncmp(input_buffer->buffer, "select where", 12) == 0) {
        statement->type = STATEMENT_SELECT_WHERE;
        sscanf(input_buffer->buffer, "select where id = %d", &statement->key);
        return PREPARE_SUCCESS;
    }
    if (strncmp(input_buffer->buffer, "update", 6) == 0) {
        return prepare_update(input_buffer, statement);
    }
    if (strncmp(input_buffer->buffer, "delete", 6) == 0) {
        statement->type = STATEMENT_DELETE;
        sscanf(input_buffer->buffer, "delete %d", &statement->key);
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

PrepareResult prepare_insert(InputBuffer *input_buffer, Statement *statement) {
    statement->type = STATEMENT_INSERT;

    char *keyword = strtok(input_buffer->buffer, " ");
    (void)keyword; // stfu compiler
    char *id_string = strtok(NULL, " ");
    char *username = strtok(NULL, " ");
    char *email = strtok(NULL, " ");

    if (id_string == NULL || username == NULL || email == NULL) {
        return PREPARE_SYNTAX_ERROR;
    }

    int id = atoi(id_string);
    if (id < 0) {
        return PREPARE_NEGATIVE_ID;
    }

    if (strlen(username) > COLUMN_USERNAME_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    if (strlen(email) > COLUMN_EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);
    return PREPARE_SUCCESS;
}

PrepareResult prepare_update(InputBuffer *input_buffer, Statement *statement) {
    statement->type = STATEMENT_UPDATE;

    char *keyword = strtok(input_buffer->buffer, " ");
    (void)keyword; // stfu compiler
    char *id_string = strtok(NULL, " ");
    char *username = strtok(NULL, " ");
    char *email = strtok(NULL, " ");

    if (id_string == NULL || username == NULL || email == NULL) {
        return PREPARE_SYNTAX_ERROR;
    }

    int id = atoi(id_string);
    if (id < 0) {
        return PREPARE_NEGATIVE_ID;
    }

    if (strlen(username) > COLUMN_USERNAME_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    if (strlen(email) > COLUMN_EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);
    return PREPARE_SUCCESS;
}

static ExecuteResult execute_insert(Statement *statement, Table *table) {
    void *node = get_page(table->pager, table->root_page_num);
    uint32_t num_cells = (*leaf_node_num_cells(node));

    Row *row_to_insert = &(statement->row_to_insert);
    uint32_t key_to_insert = row_to_insert->id;
    Cursor *cursor = table_find(table, key_to_insert);

    if (cursor->cell_num < num_cells) {
        uint32_t key_at_index = *leaf_node_key(node, cursor->cell_num);
        if (key_at_index == key_to_insert) {
            free(cursor);
            return EXECUTE_DUPLICATE_KEY;
        }
    }

    leaf_node_insert(cursor, key_to_insert, row_to_insert);
    table->num_rows++;

    free(cursor);
    return EXECUTE_SUCCESS;
}

static ExecuteResult
execute_select(__attribute__((unused)) Statement *statement, Table *table) {
    Cursor *cursor = table_start(table);
    Row row;
    while (!cursor->end_of_table) {
        deserialize_row(cursor_value(cursor), &row);
        print_row(&row);
        cursor_advance(cursor);
    }

    free(cursor);
    return EXECUTE_SUCCESS;
}

static ExecuteResult execute_select_where(Statement *statement, Table *table) {
    Cursor *cursor =
        leaf_node_find(table, table->root_page_num, statement->key);
    Row row;
    deserialize_row(cursor_value(cursor), &row);
    if (statement->key != row.id) {
        free(cursor);
        return EXECUTE_NOT_FOUND;
    }
    print_row(&row);
    free(cursor);
    return EXECUTE_SUCCESS;
}

static ExecuteResult execute_update(Statement *statement, Table *table) {
    Cursor *cursor = leaf_node_find(table, table->root_page_num,
                                    statement->row_to_insert.id);
    Row row;
    deserialize_row(cursor_value(cursor), &row);

    if (statement->row_to_insert.id != row.id) {
        free(cursor);
        return EXECUTE_NOT_FOUND;
    }

    serialize_row(&statement->row_to_insert, cursor_value(cursor));
    free(cursor);
    return EXECUTE_SUCCESS;
}

static ExecuteResult execute_range_scan(Statement *statement, Table *table) {
    Cursor *cursor = table_find(table, statement->key);
    Row row;
    while (!cursor->end_of_table) {
        deserialize_row(cursor_value(cursor), &row);
        if (row.id > statement->key_end) {
            break;
        }
        print_row(&row);
        cursor_advance(cursor);
    }
    free(cursor);
    return EXECUTE_SUCCESS;
}

static ExecuteResult execute_delete(Statement *statement, Table *table) {
    Cursor *cursor =
        leaf_node_find(table, table->root_page_num, statement->key);
    Row row;
    deserialize_row(cursor_value(cursor), &row);
    if (statement->key != row.id) {
        free(cursor);
        return EXECUTE_NOT_FOUND;
    }

    leaf_node_delete(cursor);
    table->num_rows--;

    free(cursor);
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement *statement, Table *table) {
    switch (statement->type) {
    case (STATEMENT_INSERT):
        return execute_insert(statement, table);
    case (STATEMENT_SELECT):
        return execute_select(statement, table);
    case (STATEMENT_SELECT_WHERE):
        return execute_select_where(statement, table);
    case (STATEMENT_RANGE_SCAN):
        return execute_range_scan(statement, table);
    case (STATEMENT_UPDATE):
        return execute_update(statement, table);
    case (STATEMENT_DELETE):
        return execute_delete(statement, table);
    default:
        return EXECUTE_FAILURE;
    }
}