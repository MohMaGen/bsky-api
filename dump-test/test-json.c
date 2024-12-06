#define BSKY_API_IMPLEMENTATION
#include "stdio.h"
#include "../bsky-api.h"


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
    printf("%s", bsky_tmp_str_of_json(json).start);

    return 0;
}
