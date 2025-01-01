#define BSKY_API_IMPLEMENTATION
#include "../bsky-api.h"


int main(void)
{
    enum bsky_error_code ec;

    struct bsky_str req_str = bsky_mk_str(
                              "{ \"name\": \"aboba\", \"pswd\": \"1234\" }");

    struct bsky_json req = bsky_parse_json(&req_str, &ec);

    if (ec != bsky_ec_Ok) { bsky_log_error(ec); return 1; }

    struct bsky_http_response res = bsky_http_request(
            bsky_mk_str("https://echo.free.beeceptor.com/sample-request"),
            (struct bsky_view) { NULL, NULL }, req, &ec );

    if (ec != bsky_ec_Ok) { bsky_log_error(ec); return 1; }

    bsky_log(bsky_log_Info, "result: `%s`.", 
             bsky_tmp_str_of_json(res.json).start);

    return 0;
}
