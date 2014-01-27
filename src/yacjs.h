#ifndef YACJS_H
#define YACJS_H

#include <stdbool.h>
#include <stdint.h>

#include "yacjs_name.h"

struct YACJS_NAME(node);

enum YACJS_NAME(node_type) {
    YACJS_NODE_NULL,
    YACJS_NODE_BOOLEAN,
    YACJS_NODE_STRING,
    YACJS_NODE_NUMBER,
    YACJS_NODE_FLOAT,
    YACJS_NODE_ARRAY,
    YACJS_NODE_DICT,
    YACJS_NODE_TYPES
};

enum YACJS_NAME(error) {
    YACJS_ERROR_NONE,
    YACJS_ERROR_PARSE,
    YACJS_ERROR_MEMORY,
    YACJS_ERROR_TYPE,
    YACJS_ERROR_BOUNDS,
    YACJS_ERROR_TYPES
};

enum YACJS_NAME(error) YACJS_NAME(last_error)();

struct YACJS_NAME(node) *YACJS_NAME(parse)(const char *string);
void YACJS_NAME(destroy)(struct YACJS_NAME(node) *node);

enum YACJS_NAME(node_type) YACJS_NAME(node_type)(struct YACJS_NAME(node) *node);
bool YACJS_NAME(node_bool)(struct YACJS_NAME(node) *node);
const char *YACJS_NAME(node_str)(struct YACJS_NAME(node) *node);
int64_t YACJS_NAME(node_num)(struct YACJS_NAME(node) *node);
double YACJS_NAME(node_float)(struct YACJS_NAME(node) *node);
int YACJS_NAME(node_array_size)(struct YACJS_NAME(node) *node);
struct YACJS_NAME(node) *YACJS_NAME(node_array_elem)(
    struct YACJS_NAME(node) *node, int index);
struct YACJS_NAME(node) *YACJS_NAME(node_dict_get)(
    struct YACJS_NAME(node) *node, const char *key);

#endif
