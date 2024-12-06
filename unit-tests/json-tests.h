#ifndef json_tests_h_INCLUDED
#define json_tests_h_INCLUDED

void run_json_tests(void);



#ifdef IMPLEMENT_TESTS
    #include "../bsky-api.h"
    #include <unity.h> 

    static void json_to_string_array_nums(void)
    {
        struct bsky_json json = {
            .var = bsky_json_Arr,
            .arr.len  = 6,
            .arr.data = (bsky_Json[]) {
                (bsky_Json) { .var = bsky_json_Num, .num = 10, },
                (bsky_Json) { .var = bsky_json_Num, .num = 20, },
                (bsky_Json) { .var = bsky_json_Num, .num = 30, },
                (bsky_Json) { .var = bsky_json_Num, .num = 40, },
                (bsky_Json) { .var = bsky_json_Num, .num = 50, },
                (bsky_Json) { .var = bsky_json_Num, .num = 60, },
            },
        };

        TEST_ASSERT_EQUAL_STRING(bsky_tmp_str_of_json(json).start,
                               "[10,20,30,40,50,60]");
    }

    static void json_to_string_array_strs(void)
    {
        struct bsky_json json = {
            .var = bsky_json_Arr,
            .arr.len  = 6,
            .arr.data = (bsky_Json[]) {
                (bsky_Json) { .var = bsky_json_Str, .str = "10", },
                (bsky_Json) { .var = bsky_json_Str, .str = "20", },
                (bsky_Json) { .var = bsky_json_Str, .str = "30", },
                (bsky_Json) { .var = bsky_json_Str, .str = "40", },
                (bsky_Json) { .var = bsky_json_Str, .str = "50", },
                (bsky_Json) { .var = bsky_json_Str, .str = "60", },
            },
        };

        TEST_ASSERT_EQUAL_STRING(bsky_tmp_str_of_json(json).start,
                               "[\"10\",\"20\",\"30\",\"40\",\"50\",\"60\"]");
    }



    static void json_dct_tmp(void)
    {
        struct bsky_json json = {
            .var      = bsky_json_Dct,
            .dct.data = malloc(sizeof (struct bsky_json_pair) * 100),
            .dct.len  = 100
        };

        struct bsky_str_builder sb = { 0 };


        for (size_t i = 0; i < 100; ++i) {
            bsky_sb_push_fmt(&sb, "%d", i);

            json.dct.data[i].name  = bsky_sb_build_tmp(&sb).start;
            json.dct.data[i].value = (struct bsky_json) {
                .var = bsky_json_Num,
                .num = i,
            };
        }

        bsky_sb_push_fmt(&sb, "{");
        for (size_t i = 0; i < 100; ++i) {
            bsky_sb_push_fmt(&sb, "\"%d\":%d%c", i, i, i < 99 ? ',': '}');
        }

        TEST_ASSERT_EQUAL_STRING(bsky_tmp_str_of_json(json).start,
                               bsky_sb_build_tmp(&sb).start);


 

        free(json.dct.data);
        bsky_default_tmp_reset();
    }

    void run_json_tests(void)
    {
        RUN_TEST(json_to_string_array_nums);
        RUN_TEST(json_to_string_array_strs);
        RUN_TEST(json_dct_tmp);
    }

#endif

#endif // json-tests_h_INCLUDED
