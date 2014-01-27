#include <stdio.h>

#include "yacjs.h"

int main() {
    struct yacjs_node *root = yacjs_parse("{\"foo\": \"bar\", \"baz\": [\"a\", \"b\", \"c\"]}");

    FILE *fp = fopen("test/test1.json", "r");
    char buffer[8192];
    int ret = fread(buffer, 1, 8192, fp);
    buffer[ret] = 0;
    fclose(fp);

    root = yacjs_parse(buffer);
    printf("root: %p\n", root);

    //printf("%f\n", yacjs_node_float(yacjs_node_array_elem(root, 2)));

    return 0;
}
