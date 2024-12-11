#define BSKY_API_IMPLEMENTATION
#include "../bsky-api.h"

int main(void) {
    struct bsky_str str = bsky_mk_str("[\"aboba\",10, 20, null, true, { \"name\": 10, \"age\":\"Lolka\" }]");
    enum bsky_error_code ec;
    struct bsky_json json = bsky_parse_json(&str, &ec);
    printf("%s\n", bsky_tmp_str_of_json(json).start);
}
