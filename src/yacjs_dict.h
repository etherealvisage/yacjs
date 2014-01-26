#ifndef YACJS_DICT_H
#define YACJS_DICT_H

struct yacjs_dict;

typedef void (*yacjs_dict_visitor)(void *value);

struct yacjs_dict *yacjs_dict_make();
void yacjs_dict_destroy(struct yacjs_dict *dict, yacjs_dict_visitor visitor);

void yacjs_dict_set(struct yacjs_dict *dict, const char *key,
    void *value);
void *yacjs_dict_get(struct yacjs_dict *dict, const char *key);

#endif
