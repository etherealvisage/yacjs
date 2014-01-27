add_definitions(-W -Wall -std=c99)
add_definitions(-g)

add_executable(yacjs_test yacjs.c yacjs_dict.c yacjs_u8s.c main.c)
