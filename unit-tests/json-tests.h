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

    static void json_parse_null(void)
    {
        enum bsky_error_code ec;
        struct bsky_json json = { 0 };
        struct bsky_str str   = { 0 };

        str  = bsky_mk_str("null");
        json = bsky_parse_json_null(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Ok);
        TEST_ASSERT(json.var = bsky_json_Null);
        TEST_ASSERT(bsky_str_len(str) == 0);

        str  = bsky_mk_str("    null");
        json = bsky_parse_json_null(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Ok);
        TEST_ASSERT(json.var = bsky_json_Null);
        TEST_ASSERT(bsky_str_len(str) == 0);

        str  = bsky_mk_str("    null  ");
        json = bsky_parse_json_null(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Ok);
        TEST_ASSERT(json.var = bsky_json_Null);
        TEST_ASSERT_EQUAL_STRING(str.start, "  ");

        str  = bsky_mk_str("    null,  ");
        json = bsky_parse_json_null(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Ok);
        TEST_ASSERT(json.var = bsky_json_Null);
        TEST_ASSERT_EQUAL_STRING(str.start, ",  ");

        str  = bsky_mk_str(";null");
        json = bsky_parse_json_null(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Json_expect_Null);
        TEST_ASSERT_EQUAL_STRING(str.start, ";null");

        str  = bsky_mk_str("  ;null");
        json = bsky_parse_json_null(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Json_expect_Null);
        TEST_ASSERT_EQUAL_STRING(str.start, ";null");
    }

    static void json_parse_bool(void)
    {
        enum bsky_error_code ec;
        struct bsky_json json = { 0 };
        struct bsky_str str   = { 0 };

        str  = bsky_mk_str("true");
        json = bsky_parse_json_bool(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Ok);
        TEST_ASSERT(json.var = bsky_json_Bool);
        TEST_ASSERT(json._bool);
        TEST_ASSERT(bsky_str_len(str) == 0);

        str  = bsky_mk_str("false");
        json = bsky_parse_json_bool(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Ok);
        TEST_ASSERT(json.var = bsky_json_Bool);
        TEST_ASSERT(!json._bool);
        TEST_ASSERT(bsky_str_len(str) == 0);

        str  = bsky_mk_str("    true");
        json = bsky_parse_json_bool(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Ok);
        TEST_ASSERT(json.var = bsky_json_Bool);
        TEST_ASSERT(json._bool);
        TEST_ASSERT(bsky_str_len(str) == 0);

        str  = bsky_mk_str("    false  ");
        json = bsky_parse_json_bool(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Ok);
        TEST_ASSERT(json.var = bsky_json_Bool);
        TEST_ASSERT(!json._bool);
        TEST_ASSERT_EQUAL_STRING(str.start, "  ");

        str  = bsky_mk_str("    false,  ");
        json = bsky_parse_json_bool(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Ok);
        TEST_ASSERT(json.var = bsky_json_Bool);
        TEST_ASSERT(!json._bool);
        TEST_ASSERT_EQUAL_STRING(str.start, ",  ");

        str  = bsky_mk_str(";true");
        json = bsky_parse_json_bool(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Json_expect_Bool);
        TEST_ASSERT_EQUAL_STRING(str.start, ";true");

        str  = bsky_mk_str("  ;false");
        json = bsky_parse_json_null(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Json_expect_Null);
        TEST_ASSERT_EQUAL_STRING(str.start, ";false");
    }

    static void json_parse_num(void)
    {
        enum bsky_error_code ec;
        struct bsky_json json = { 0 };
        struct bsky_str str   = { 0 };

        str  = bsky_mk_str("1020");
        json = bsky_parse_json_num(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Ok);
        TEST_ASSERT(json.var = bsky_json_Num);
        TEST_ASSERT(json.num == 1020);
        TEST_ASSERT(bsky_str_len(str) == 0);

        str  = bsky_mk_str("1020.5");
        json = bsky_parse_json_num(&str, &ec);
        TEST_ASSERT(json.var = bsky_json_Num);
        TEST_ASSERT(ec == bsky_ec_Ok);
        TEST_ASSERT(json.num == 1020.5);
        TEST_ASSERT(bsky_str_len(str) == 0);

        str  = bsky_mk_str("10201231231");
        json = bsky_parse_json_num(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Ok);
        TEST_ASSERT(json.var = bsky_json_Num);
        TEST_ASSERT(json.num == 10201231231);
        TEST_ASSERT(bsky_str_len(str) == 0);

        str  = bsky_mk_str("\n \t  1020");
        json = bsky_parse_json_num(&str, &ec);
        TEST_ASSERT(json.var = bsky_json_Num);
        TEST_ASSERT(ec == bsky_ec_Ok);
        TEST_ASSERT(json.num == 1020);
        TEST_ASSERT(bsky_str_len(str) == 0);

        str  = bsky_mk_str("\n \t  -1020,  ");
        json = bsky_parse_json_num(&str, &ec);
        TEST_ASSERT(json.var = bsky_json_Num);
        TEST_ASSERT(ec == bsky_ec_Ok);
        TEST_ASSERT(json.num == -1020);
        TEST_ASSERT_EQUAL_STRING(str.start, ",  ");

        str  = bsky_mk_str("aa1020");
        json = bsky_parse_json_num(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Json_expect_Number);
        TEST_ASSERT_EQUAL_STRING(str.start, "aa1020");

        str  = bsky_mk_str("  aa1020");
        json = bsky_parse_json_num(&str, &ec);
        TEST_ASSERT(ec == bsky_ec_Json_expect_Number);
        TEST_ASSERT_EQUAL_STRING(str.start, "aa1020");
    }

    static void json_parse_str(void)
    {
        enum bsky_error_code ec;
        struct bsky_json json = { 0 };
        struct bsky_str str   = { 0 };

        str  = bsky_mk_str("\"str\"");
        json = bsky_parse_json_str(&str, &ec);
        TEST_ASSERT_EQUAL(ec, bsky_ec_Ok);
        TEST_ASSERT(json.var == bsky_json_Str);
        TEST_ASSERT_EQUAL_STRING(json.str, "str");
        TEST_ASSERT(bsky_str_len(str) == 0);

        str  = bsky_mk_str("  \"str\", ");
        json = bsky_parse_json_str(&str, &ec);
        TEST_ASSERT_EQUAL(ec, bsky_ec_Ok);
        TEST_ASSERT(json.var == bsky_json_Str);
        TEST_ASSERT_EQUAL_STRING(json.str, "str");
        TEST_ASSERT_EQUAL_STRING(str.start, ", ");

        str  = bsky_mk_str("  \"str");
        json = bsky_parse_json_str(&str, &ec);
        TEST_ASSERT_EQUAL(bsky_ec_Json_expect_CQ, ec);
        TEST_ASSERT_EQUAL_STRING("", str.start);

        str  = bsky_mk_str("  str");
        json = bsky_parse_json_str(&str, &ec);
        TEST_ASSERT_EQUAL(bsky_ec_Json_expect_OQ, ec);
        TEST_ASSERT_EQUAL_STRING("str", str.start);

        str  = bsky_mk_str("  \"a\\\"str\\\"\"");
        json = bsky_parse_json_str(&str, &ec);
        TEST_ASSERT_EQUAL(ec, bsky_ec_Ok);
        TEST_ASSERT(json.var == bsky_json_Str);
        TEST_ASSERT_EQUAL_STRING("a\\\"str\\\"", json.str);
        TEST_ASSERT(bsky_str_len(str) == 0);

        str  = bsky_mk_str("  \"a\\");
        json = bsky_parse_json_str(&str, &ec);
        TEST_ASSERT_EQUAL(bsky_ec_Json_expect_CQ, ec);
        TEST_ASSERT_EQUAL_STRING("", str.start);
    }

    static void json_parse_arr(void)
    {
        enum bsky_error_code ec;
        struct bsky_json json = { 0 };
        struct bsky_str str   = { 0 };

        str  = bsky_mk_str("[12, \"str\", true]");
        json = bsky_parse_json_arr(&str, &ec);
        TEST_ASSERT_EQUAL(bsky_ec_Ok, ec);
        TEST_ASSERT_EQUAL(bsky_json_Arr, json.var);
        TEST_ASSERT_EQUAL(3, json.arr.len);

        TEST_ASSERT_EQUAL(bsky_json_Num, json.arr.data[0].var);
        TEST_ASSERT_EQUAL(12, json.arr.data[0].num);

        TEST_ASSERT_EQUAL(bsky_json_Str, json.arr.data[1].var);
        TEST_ASSERT_EQUAL_STRING("str", json.arr.data[1].str);

        TEST_ASSERT(json.arr.data[2]._bool);
        TEST_ASSERT(bsky_str_len(str) == 0);

        str  = bsky_mk_str("[12, [\"str\", true]]");
        json = bsky_parse_json_arr(&str, &ec);
        TEST_ASSERT_EQUAL(bsky_ec_Ok, ec);
        TEST_ASSERT_EQUAL(bsky_json_Arr, json.var);
        TEST_ASSERT_EQUAL(2, json.arr.len);

        TEST_ASSERT_EQUAL(bsky_json_Num, json.arr.data[0].var);
        TEST_ASSERT_EQUAL(12, json.arr.data[0].num);

        TEST_ASSERT_EQUAL(bsky_json_Arr, json.arr.data[1].var);
        TEST_ASSERT_EQUAL(2, json.arr.data[1].arr.len);

        TEST_ASSERT_EQUAL(bsky_json_Str, json.arr.data[1].arr.data[0].var);
        TEST_ASSERT_EQUAL_STRING("str", json.arr.data[1].arr.data[0].str);

        TEST_ASSERT_EQUAL(bsky_json_Bool, json.arr.data[1].arr.data[1].var);
        TEST_ASSERT(json.arr.data[1].arr.data[1]._bool);


        str  = bsky_mk_str("     [12, \"str\", true]");
        json = bsky_parse_json_arr(&str, &ec);
        TEST_ASSERT_EQUAL(bsky_ec_Ok, ec);
        TEST_ASSERT_EQUAL(bsky_json_Arr, json.var);
        TEST_ASSERT_EQUAL(3, json.arr.len);

        TEST_ASSERT_EQUAL(bsky_json_Num, json.arr.data[0].var);
        TEST_ASSERT_EQUAL(12, json.arr.data[0].num);

        TEST_ASSERT_EQUAL(bsky_json_Str, json.arr.data[1].var);
        TEST_ASSERT_EQUAL_STRING("str", json.arr.data[1].str);

        TEST_ASSERT(json.arr.data[2]._bool);
        TEST_ASSERT(bsky_str_len(str) == 0);


        str  = bsky_mk_str("     []");
        json = bsky_parse_json_arr(&str, &ec);
        TEST_ASSERT_EQUAL(bsky_ec_Ok, ec);
        TEST_ASSERT_EQUAL(bsky_json_Arr, json.var);
        TEST_ASSERT_EQUAL(0, json.arr.len);
        TEST_ASSERT(bsky_str_len(str) == 0);

        str  = bsky_mk_str("     [  \n ]");
        json = bsky_parse_json_arr(&str, &ec);
        TEST_ASSERT_EQUAL(bsky_ec_Ok, ec);
        TEST_ASSERT_EQUAL(bsky_json_Arr, json.var);
        TEST_ASSERT_EQUAL(0, json.arr.len);
        TEST_ASSERT(bsky_str_len(str) == 0);
    }

    static void json_parse_dct(void)
    {
        enum bsky_error_code ec;
        struct bsky_json json = { 0 };
        struct bsky_str str   = { 0 };

        str  = bsky_mk_str("{ \"arr\": [10, 23, 23], \"name\":\"Vlad\" }");
        json = bsky_parse_json_dct(&str, &ec);
        TEST_ASSERT_EQUAL(bsky_ec_Ok, ec);
        TEST_ASSERT_EQUAL(bsky_json_Dct, json.var);
        TEST_ASSERT_EQUAL(2, json.dct.len);

        TEST_ASSERT_EQUAL_STRING("arr", json.dct.data[0].name);
        TEST_ASSERT_EQUAL(bsky_json_Arr, json.dct.data[0].value.var);
        TEST_ASSERT_EQUAL(3, json.dct.data[0].value.arr.len);
        TEST_ASSERT_EQUAL(10, json.dct.data[0].value.arr.data[0].num);
        TEST_ASSERT_EQUAL(23, json.dct.data[0].value.arr.data[1].num);
        TEST_ASSERT_EQUAL(23, json.dct.data[0].value.arr.data[2].num);

        TEST_ASSERT_EQUAL_STRING("name", json.dct.data[1].name);
        TEST_ASSERT_EQUAL(bsky_json_Str, json.dct.data[1].value.var);
        TEST_ASSERT_EQUAL_STRING("Vlad", json.dct.data[1].value.str);
    }

    void run_json_tests(void)
    {
        RUN_TEST(json_to_string_array_nums);
        RUN_TEST(json_to_string_array_strs);
        RUN_TEST(json_dct_tmp);
        RUN_TEST(json_parse_null);
        RUN_TEST(json_parse_bool);
        RUN_TEST(json_parse_num);
        RUN_TEST(json_parse_str);
        RUN_TEST(json_parse_arr);
        RUN_TEST(json_parse_dct);
    }

#endif

#endif // json-tests_h_INCLUDED
