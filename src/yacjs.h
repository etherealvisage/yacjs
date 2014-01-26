#ifndef YACJS_H
#define YACJS_H

#include <stdint.h>

struct yacjs_node;

enum yacjs_node_type {
    YACJS_NODE_STRING,
    YACJS_NODE_NUMBER,
    YACJS_NODE_FLOAT,
    YACJS_NODE_ARRAY,
    YACJS_NODE_DICT,
    YACJS_NODE_TYPES
};

enum yacjs_error {
    YACJS_ERROR_NONE,
    YACJS_ERROR_PARSE,
    YACJS_ERROR_MEMORY,
    YACJS_ERROR_TYPE,
    YACJS_ERROR_BOUNDS,
    YACJS_ERROR_TYPES
};

enum yacjs_error yacjs_last_error();

struct yacjs_node *yacjs_parse(const char *string);

enum yacjs_node_type yacjs_node_type(struct yacjs_node *node);
const char *yacjs_node_str(struct yacjs_node *node);
int64_t yacjs_node_num(struct yacjs_node *node);
double yacjs_node_float(struct yacjs_node *node);
int yacjs_node_array_size(struct yacjs_node *node);
struct yacjs_node *yacjs_node_array_elem(struct yacjs_node *node, int index);
struct yacjs_node *yacjs_node_dict_get(struct yacjs_node *node,
    const char *key);

#endif
