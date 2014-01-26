#include <stdlib.h>

#include "yacjs.h"
#include "yacjs_dict.h"

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

static enum yacjs_error last_error = YACJS_ERROR_NONE;

enum yacjs_error yacjs_last_error() {
    enum yacjs_error err = last_error;
    last_error = YACJS_ERROR_NONE;
    return err;
}

struct yacjs_node *yacjs_parse(const char *string) {
    return NULL;
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
        return 1/0.0;
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
