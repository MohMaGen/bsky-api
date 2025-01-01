#define BSKY_API_IMPLEMENTATION
#include "../bsky-api.h"

int main(void)
{
    enum bsky_error_code ec;
    struct bsky_http_response res = { 0 };

    struct bsky_str req_str = bsky_mk_str(
                              "{ \"name\": \"aboba\", \"pswd\": \"1234\" }");
    struct bsky_json req = bsky_parse_json(&req_str, &ec);
    if (ec != bsky_ec_Ok) { bsky_log_error(ec); return 1; }

    struct bsky_future future = bsky_http_request_async(
            bsky_mk_str("https://echo.free.beeceptor.com/sample-request"),
            (struct bsky_view) { NULL, NULL }, req, &ec);

    while (bsky_fufure_poll(future, NULL, &res, &ec) == 0) {
        bsky_log(bsky_log_Info, "%s", "getting response...");
    }

    if (ec == bsky_ec_Ok)
        bsky_log(bsky_log_Info, "Ok: `%s'", 
                 bsky_tmp_str_of_json(res.json).start);
    else
        bsky_log_error(ec);

    return 0;
}
