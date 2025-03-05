//
// A symbol table.
//

#include "always.h"
#include "st.h"

//
// A symbol.
//

typedef struct
{
    std::uint32_t hash;
    std::int32_t value;
    std::int32_t flag;
    std::uint16_t string; // Offset into the buffer
    std::uint16_t next;

} ST_Symbol;

//
// A symbol table.
//

typedef struct
{
    //
    // Where we store strings.
    //

    char *buffer;
    std::int32_t buffer_upto;
    std::int32_t buffer_max;

    //
    // Where we store symbols.
    //

    ST_Symbol *symbol;
    std::int32_t symbol_upto;
    std::int32_t symbol_max;

    //
    // A hash table. Each entry is the start of a linked list.
    //

#define ST_HASH_SIZE 512

    std::uint16_t hash[ST_HASH_SIZE];

} ST_Table;

ST_Table ST_table[ST_TABLE_NUMBER];

//
// Computes the hash value of a string.
//

std::uint32_t ST_hash_value(char *string) {
    std::uint32_t ans = 0;
    std::int32_t rot = 0;

    char *ch;

    for (ch = string; *ch; ch++) {
        ans ^= _lrotl(*ch, rot);

        rot += 7;
    }

    ans ^= ans >> 16; // So more letters count...

    return ans;
}

void ST_init() {
    std::int32_t i;

    ST_Table *st;

    //
    // Clear all data.
    //

    memset(ST_table, 0, sizeof(ST_table));

    //
    // Initialise each table.
    //

    for (i = 0; i < ST_TABLE_NUMBER; i++) {
        st = &ST_table[i];

        st->buffer_max = 8192;
        st->buffer_upto = 0;
        st->buffer = (char *) malloc(sizeof(char) * st->buffer_max);

        st->symbol_max = 1024;
        st->symbol_upto = 1; // The 0th element is the nullptr index...
        st->symbol = (ST_Symbol *) malloc(sizeof(ST_Symbol) * st->symbol_max);
    }
}

void ST_add(std::int32_t table, char *string, std::int32_t value, std::int32_t flag) {
    ST_Table *st;
    ST_Symbol *ss;

    std::int32_t symbol;

    ASSERT(WITHIN(table, 0, ST_TABLE_NUMBER - 1));

    st = &ST_table[table];

    //
    // Compute the hash value.
    //

    std::uint32_t hash = ST_hash_value(string);

#ifdef _DEBUG

    //
    // Do we already have this symbol?
    //

#endif

    //
    // Get a new symbol. Is there enough room?
    //

    if (st->symbol_upto >= st->symbol_max) {
        st->symbol_max *= 2;
        st->symbol = (ST_Symbol *) realloc(st->symbol, sizeof(ST_Symbol) * st->symbol_max);
    }

    //
    // The index of our new symbol.
    //

    symbol = st->symbol_upto++;

    //
    // Initialise the symbol.
    //

    ss = &st->symbol[symbol];

    ss->hash = hash;
    ss->value = value;
    ss->flag = flag;
    ss->string = (std::uint16_t) st->buffer_upto;
    ss->next = nullptr;

    //
    // Enough room to add the string?
    //

    std::int32_t length = strlen(string) + 1; // + 1 to include the terminating nullptr.

    if (st->buffer_upto + length > st->buffer_max) {
        //
        // Must allocate a bigger string buffer.
        //

        st->buffer_max *= 2;
        st->buffer = (char *) realloc(st->buffer, sizeof(char) * st->buffer_max);
    }

    //
    // Add the string to the buffer.
    //

    strcpy(st->buffer + st->buffer_upto, string);

    st->buffer_upto += length;

    //
    // Insert the symbol into the hash table.
    //

    ss->next = st->hash[ss->hash & (ST_HASH_SIZE - 1)];
    st->hash[ss->hash & (ST_HASH_SIZE - 1)] = (std::uint16_t) symbol;
}

//
// Looks for the string in the given table. Returns true if it
// finds one. Sets the ST_found_table and ST_found_value variables
// if it finds the string.
//

std::int32_t ST_found_table;
std::int32_t ST_found_value;
std::int32_t ST_found_flag;
char *ST_found_string;
ST_Symbol *ST_found_ss;

std::int32_t ST_find_in_table(std::int32_t table, char *string) {
    ST_Table *st;
    ST_Symbol *ss;

    std::int32_t symbol;

    ASSERT(WITHIN(table, 0, ST_TABLE_NUMBER - 1));

    st = &ST_table[table];

    //
    // Compute the hash value.
    //

    std::uint32_t hash = ST_hash_value(string);

    for (symbol = st->hash[hash & (ST_HASH_SIZE - 1)]; symbol; symbol = ss->next) {
        ASSERT(WITHIN(symbol, 1, st->symbol_upto - 1));

        ss = &st->symbol[symbol];

        if (ss->hash == hash) {
            //
            // This is probably our symbol. We have to check though.
            //

            if (strcmp(st->buffer + ss->string, string) == 0) {
                //
                // This is our symbol.
                //

                ST_found_table = table;
                ST_found_value = ss->value;
                ST_found_flag = ss->flag;
                ST_found_ss = ss;
                ST_found_string = string;

                return true;
            }
        }
    }

    return false;
}

std::int32_t ST_find(char *string) {
    std::int32_t table;

    //
    // Look in the tables.
    //

    for (table = ST_TABLE_NUMBER - 1; table >= 0; table--) {
        if (ST_find_in_table(
                table,
                string)) {
            return true;
        }
    }

    return false;
}

void ST_update_flag(char *string, std::int32_t new_flag) {
    std::int32_t table;

    //
    // Look in the tables.
    //

    for (table = ST_TABLE_NUMBER - 1; table >= 0; table--) {
        if (ST_find_in_table(
                table,
                string)) {
            ST_found_ss->flag = new_flag;

            return;
        }
    }

    //
    // Symbol not found!
    //

    ASSERT(0);

    return;
}

std::int32_t ST_find_all_table;
std::int32_t ST_find_all_symbol;

void ST_find_all_start() {
    ST_find_all_table = 0;
    ST_find_all_symbol = 1;
}

std::int32_t ST_find_all_next() {
    ST_Table *st;
    ST_Symbol *ss;

    while (1) {
        ASSERT(WITHIN(ST_find_all_table, 0, ST_TABLE_NUMBER - 1));

        st = &ST_table[ST_find_all_table];

        if (ST_find_all_symbol >= st->symbol_upto) {
            ST_find_all_table += 1;
            ST_find_all_symbol = 1;

            if (ST_find_all_table >= ST_TABLE_NUMBER) {
                return false;
            }
        } else {
            ASSERT(WITHIN(ST_find_all_symbol, 0, st->symbol_upto - 1));

            ss = &st->symbol[ST_find_all_symbol++];

            ASSERT(WITHIN(ss->string, 0, st->buffer_upto - 2));

            ST_found_string = &st->buffer[ss->string];
            ST_found_table = ST_find_all_table;
            ST_found_value = ss->value;
            ST_found_flag = ss->flag;

            return true;
        }
    }
}

void ST_clear(std::int32_t table) {
    ST_Table *st;

    ASSERT(WITHIN(table, 0, ST_TABLE_NUMBER - 1));

    st = &ST_table[table];

    //
    // Zero length the arrays and clear the hash table.
    //

    st->symbol_upto = 1; // 1 because the 0'th index is reserved for nullptr.
    st->buffer_upto = 0;

    memset(st->hash, 0, sizeof(st->hash));
}

void ST_clear_all() {
    std::int32_t i;

    for (i = 0; i < ST_TABLE_NUMBER; i++) {
        ST_clear(i);
    }
}
