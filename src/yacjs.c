#include <stdio.h> // for debugging
#include <stdlib.h>

#include "yacjs.h"
#include "yacjs_dict.h"
#include "yacjs_u8s.h"

struct yacjs_node {
    enum yacjs_node_type type;
    union {
        const char *string;
        int64_t number;
        double fp;
        struct {
            struct yacjs_node *entries;
            int entries_count;
            int entries_size;
        } array;
        struct yacjs_dict *dict;
    } data;
};

enum token_type {
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_OPENDICT,
    TOKEN_CLOSEDICT,
    TOKEN_OPENARRAY,
    TOKEN_CLOSEARRAY,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_NONE,
    TOKEN_ERROR,
    TOKEN_TYPES
};

static enum yacjs_error last_error = YACJS_ERROR_NONE;

static void destroy_helper(struct yacjs_node *node);
static void skip_whitespace(const char ** const ptr);
static const char *next_token(const char ** const ptr, int *length,
    enum token_type *type);
static const char *peek_token(const char ** const ptr, int *length,
    enum token_type *type);
static struct yacjs_node *parse_any(const char **string);
static struct yacjs_node *parse_dict_contents(const char **string);
static struct yacjs_node *parse_array_contents(const char **string);

enum yacjs_error yacjs_last_error() {
    enum yacjs_error err = last_error;
    last_error = YACJS_ERROR_NONE;
    return err;
}

struct yacjs_node *yacjs_parse(const char *string) {
    struct yacjs_node *ret = parse_any(&string);
    // ensure there's nothing afterwards.
    skip_whitespace(&string);
    if(string[0] != 0) {
        yacjs_destroy(ret);
        last_error = YACJS_ERROR_PARSE;
        return NULL;
    }
    return ret;
}

void yacjs_destroy(struct yacjs_node *node) {
    if(!node) return;
    destroy_helper(node);
    free(node);
}

static void destroy_helper(struct yacjs_node *node) {
    if(node->type == YACJS_NODE_ARRAY) {
        for(int i = 0; i < node->data.array.entries_count; i ++) {
            yacjs_destroy(node->data.array.entries + i);
        }
        free(node->data.array.entries);
    }
    else if(node->type == YACJS_NODE_DICT) {
        yacjs_dict_destroy(node->data.dict,
            (yacjs_dict_visitor)destroy_helper);
    }
}

enum yacjs_node_type yacjs_node_type(struct yacjs_node *node) {
    return node->type;
}

const char *yacjs_node_str(struct yacjs_node *node) {
    if(node->type != YACJS_NODE_STRING) {
        last_error = YACJS_ERROR_TYPE;
        return NULL;
    }
    return node->data.string;
}

int64_t yacjs_node_num(struct yacjs_node *node) {
    if(node->type != YACJS_NODE_NUMBER) {
        last_error = YACJS_ERROR_TYPE;
        return -1;
    }
    return node->data.number;
}

double yacjs_node_float(struct yacjs_node *node) {
    if(node->type != YACJS_NODE_FLOAT) {
        last_error = YACJS_ERROR_TYPE;
        return 1/0.0; // NaN
    }
    return node->data.fp;
}

int yacjs_node_array_size(struct yacjs_node *node) {
    if(node->type != YACJS_NODE_ARRAY) {
        last_error = YACJS_ERROR_TYPE;
        return -1;
    }
    return node->data.array.entries_count;
}

struct yacjs_node *yacjs_node_array_elem(struct yacjs_node *node, int index) {
    if(node->type != YACJS_NODE_ARRAY) {
        last_error = YACJS_ERROR_TYPE;
        return NULL;
    }
    if(index >= node->data.array.entries_count || index < 0) {
        last_error = YACJS_ERROR_BOUNDS;
        return NULL;
        
    }
    return node->data.array.entries + index;
}

struct yacjs_node *yacjs_node_dict_get(struct yacjs_node *node,
    const char *key) {

    if(node->type != YACJS_NODE_DICT) {
        last_error = YACJS_ERROR_TYPE;
        return NULL;
    }

    return yacjs_dict_get(node->data.dict, key);
}

static void skip_whitespace(const char ** const ptr) {
    while((**ptr == ' ' || **ptr == '\t') && **ptr != 0) {
        *ptr = yacjs_u8s_next(*ptr);
    }
}

static const char *next_token(const char ** const ptr, int *length,
    enum token_type *type) {

    *length = 0;
    // skip whitespace
    skip_whitespace(ptr);
    if(*ptr == 0) {
        *type = TOKEN_NONE;
        return NULL;
    }

    if(**ptr == '"') {
        // TODO: parse string PROPERLY
        const char *start = ++(*ptr);
        *length = 0;
        while(**ptr != '"' && **ptr != 0) {
            (*ptr) ++, (*length) ++;
        }
        if(**ptr == 0) return NULL;
        // skip closing quotes
        else (*ptr)++;

        *type = TOKEN_STRING;
        return start;
    }
    else if(**ptr == '{') {
        *type = TOKEN_OPENDICT;
        return (*ptr)++;
    }
    else if(**ptr == '}') {
        *type = TOKEN_CLOSEDICT;
        return (*ptr)++;
    }
    else if(**ptr == '[') {
        *type = TOKEN_OPENARRAY;
        return (*ptr)++;
    }
    else if(**ptr == ']') {
        *type = TOKEN_CLOSEARRAY;
        return (*ptr)++;
    }
    else if(**ptr == ',') {
        *type = TOKEN_COMMA;
        return (*ptr)++;
    }
    else if(**ptr == ':') {
        *type = TOKEN_COLON;
        return (*ptr)++;
    }

    *type = TOKEN_ERROR;

    return NULL;
}

static const char *peek_token(const char ** const ptr, int *length,
    enum token_type *type) {

    const char *s = *ptr;

    return next_token(&s, length, type);
}

static struct yacjs_node *parse_any(const char **string) {
    enum token_type type;
    const char *s;
    int len;

    s = next_token(string, &len, &type);
    if(type == TOKEN_OPENDICT) return parse_dict_contents(string);
    else if(type == TOKEN_OPENARRAY) return parse_array_contents(string);
    else if(type == TOKEN_STRING) {
        struct yacjs_node *build = malloc(sizeof(*build));
        build->type = YACJS_NODE_STRING;
        build->data.string = yacjs_u8s_strndup(s, len);
        return build;
    }
    else if(type == TOKEN_NUMBER) {
        struct yacjs_node *build = malloc(sizeof(*build));
        build->type = YACJS_NODE_NUMBER;
        build->data.number = strtoll(s, NULL, 0);
        return build;
    }

    printf("Unknown token type %i\n", type);

    return NULL;
}

static struct yacjs_node *parse_dict_contents(const char **string) {
    enum token_type type;
    const char *s;
    int len;

    struct yacjs_node *result = malloc(sizeof(*result));
    result->type = YACJS_NODE_DICT;
    result->data.dict = yacjs_dict_make();

    while((s = next_token(string, &len, &type))) {
        // closing dictionary token
        if(type == TOKEN_CLOSEDICT) break;
        // we expect a string here if it's not a closing dictionary
        else if(type != TOKEN_STRING) {
            return NULL;
        }

        char *ds = yacjs_u8s_strndup(s, len);

        // expect a colon after the name
        next_token(string, &len, &type);
        if(type != TOKEN_COLON) {
            return NULL;
        }

        struct yacjs_node *value = parse_any(string);

        if(value == NULL) {
            // TODO: leaked memory
            return NULL;
        }

        yacjs_dict_set(result->data.dict, ds, value);
        free(ds);

        peek_token(string, &len, &type);
        if(type == TOKEN_COMMA) {
            // NOTE: this means things like {"a":1,} are accepted
            // eat the comma
            next_token(string, &len, &type);
        }
    }
    if(!s) {
        // TODO: leaked memory
        return NULL;
    }

    return result;
}

static struct yacjs_node *parse_array_contents(const char **string) {
    enum token_type type;
    const char *s;
    int len;

    struct yacjs_node *result = malloc(sizeof(*result));
    result->type = YACJS_NODE_ARRAY;
    result->data.array.entries = NULL;
    result->data.array.entries_size = 0;
    result->data.array.entries_count = 0;

    while((s = peek_token(string, &len, &type))) {
        // closing dictionary token
        if(type == TOKEN_CLOSEARRAY) {
            // eat first
            next_token(string, &len, &type);
            break;
        }
        struct yacjs_node *next = parse_any(string);
        if(!next) {
            // TODO: leaked memory
            return NULL;
        }

        if(result->data.array.entries_size
            == result->data.array.entries_count) {

            void *nmem = realloc(result->data.array.entries,
                result->data.array.entries_size * 2 + 1);
            if(nmem == NULL) {
                last_error = YACJS_ERROR_MEMORY;
                return NULL;
            }

            result->data.array.entries_size *= 2;
            result->data.array.entries_size ++;
            result->data.array.entries = nmem;
        }
        result->data.array.entries[result->data.array.entries_count++] = *next;
        free(next);

        peek_token(string, &len, &type);
        if(type == TOKEN_COMMA) {
            // NOTE: this means things like [1,2,] are accepted
            // eat the comma
            next_token(string, &len, &type);
        }
    }
    if(!s) {
        // TODO: leaked memory
        return NULL;
    }

    return result;
}
