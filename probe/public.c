#include "../bsky-api.h"

int main(int argc, char **argv) 
{
    enum bsky_error_code ec = { 0 };

    struct bsky_crses_resp res = { 0 };
    res = bsky_create_session(bsky_mk_handle("mohmagen.bsky.social"),
                              bsky_str_from_file_tmp(argv[1]),
                              bsky_str_null(), &ec);



    return 0;
}
