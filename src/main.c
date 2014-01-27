#include <stdio.h>

#include "yacjs.h"

int main() {
    struct yacjs_node *root = yacjs_parse("{\"foo\": \"bar\", \"baz\": [\"a\", \"b\", \"c\"]}");

    printf("Result: %p\n", root);
    printf("foo: %p\n", yacjs_node_dict_get(root, "foo"));
    printf("foo string: %s\n", yacjs_node_str(yacjs_node_dict_get(root, "foo")));

    struct yacjs_node *baz = yacjs_node_dict_get(root, "baz");
    printf("baz: %p\n", baz);
    printf("\tfirst element string: %s\n", yacjs_node_str(yacjs_node_array_elem(baz, 0)));
    return 0;
}
