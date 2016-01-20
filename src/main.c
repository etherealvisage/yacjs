#include <stdio.h>

#include "yacjs.h"

int main() {
    struct yacjs_node *root = yacjs_parse("{\"foo\": \"bar\", \"baz\": [\"a\", \"b\", \"c\"]}");

    yacjs_destroy(root);
    root = NULL;

    FILE *fp = fopen("test/test1.json", "r");
    char buffer[32768];
    int ret = fread(buffer, 1, 32768, fp);
    buffer[ret] = 0;
    printf("bytes read: %i\n", ret);
    fclose(fp);

    root = yacjs_parse(buffer);
    printf("root: %p\n", root);

    printf("error: %i\n", yacjs_last_error());

    /*
    struct yacjs_node *node = yacjs_node_array_elem(root, 0);
    node = yacjs_node_dict_get(node, "friends");
    printf("%i friends\n", yacjs_node_array_size(node));

    node = yacjs_node_array_elem(node, 1);
    printf("second friend ID and name: %li, %s\n",
        yacjs_node_num(yacjs_node_dict_get(node, "id")),
        yacjs_node_str(yacjs_node_dict_get(node, "name")));
*/
    yacjs_destroy(root);

    return 0;
}
