#define BSKY_API_IMPLEMENTATION
#include "stdio.h"
#include "../bsky-api.h"


typedef struct a { int a, b, c; } b;

#define c a


void func_of_a(struct a a)
{
	return;
}


int main(void) 
{
    struct bsky_json json = {
        .var = bsky_json_Arr,
        .arr.data = (bsky_Json[]) {
            (bsky_Json) {
                .var = bsky_json_Num,
                .num = 10
            },
            (bsky_Json) {
                .var = bsky_json_Str,
                .str = "Hello, JSON!"
            },
            (bsky_Json) {
                .var = bsky_json_Dct, 
                .dct.data = (bsky_Json_Pair[]) {
                    (bsky_Json_Pair) {
                        .name  = "name",
                        .value = (bsky_Json) {
                            .var = bsky_json_Str,
                            .str = "Vlad",
                        }
                    },
                    (bsky_Json_Pair) {
                        .name  = "age",
                        .value = (bsky_Json) {
                            .var = bsky_json_Num,
                            .num = 10,
                        }
                    },
                },
                .dct.len = 2,
            },
        },
        .arr.len = 3,
    };
    bsky_log(bsky_log_Info, "json: `%s`", bsky_tmp_str_of_json(json).start);

    int a = 10;

    bsky_log(bsky_log_Error,   "a is %d", a);
    bsky_log(bsky_log_Warning, "a is %d", a);
    bsky_log(bsky_log_Info,    "a is %d", a);

    struct bsky_str_builder sb = { 0 };
    bsky_sb_push(&sb, 'A');
    bsky_log(bsky_log_Info, "sb is %s", sb.data);

    bsky_sb_push_fmt(&sb, "{ \"aboba\":%d }", a);
    bsky_log(bsky_log_Info, "sb is %s", sb.data);

    struct bsky_str tmp_str = bsky_sb_build_tmp(&sb);
    bsky_log(bsky_log_Info, "tmp str is %s", tmp_str.start);
    bsky_log(bsky_log_Info, "sb is %s", sb.data); // sb has been freed


    return 0;
}

