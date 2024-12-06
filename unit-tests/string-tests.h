#ifndef string_tests_h_INCLUDED
#define string_tests_h_INCLUDED


void run_string_tests(void);


#ifdef IMPLEMENT_TESTS

    #include "../bsky-api.h"

    static void string_builder(void)
    {
        struct bsky_str_builder sb = { 0 };

        bsky_sb_push(&sb, 'A');
        TEST_ASSERT_EQUAL_STRING(sb.data, "A");

        bsky_sb_push(&sb, 'B');
        TEST_ASSERT_EQUAL_STRING(sb.data, "AB");

        bsky_sb_push(&sb, 'C');
        TEST_ASSERT_EQUAL_STRING(sb.data, "ABC");

        bsky_sb_push_fmt(&sb, "[ERROR]");
        TEST_ASSERT_EQUAL_STRING(sb.data, "ABC[ERROR]");

        bsky_sb_push_fmt(&sb, "value=%d, value=%s, value=%d", 10, "A", 10);
        TEST_ASSERT_EQUAL_STRING(sb.data,
                                 "ABC[ERROR]value=10, value=A, value=10");

        struct bsky_str res = bsky_sb_build_tmp(&sb);
        TEST_ASSERT_EQUAL_STRING(res.start,
                                 "ABC[ERROR]value=10, value=A, value=10");
    	TEST_ASSERT(sb.data == 0 && sb.len == 0 && sb.cap == 0);
    }

    static void string_trim(void)
    {
        struct bsky_str_builder sb = { 0 };
        struct bsky_str fst;

        bsky_sb_push_fmt(&sb, "  \t\n \n aaa_BBBB");
        fst = bsky_trim_left(bsky_sb_build_tmp(&sb));
        TEST_ASSERT_EQUAL_STRING("aaa_BBBB", fst.start);

        bsky_sb_push_fmt(&sb, "      ");
        fst = bsky_trim_left(bsky_sb_build_tmp(&sb));
        TEST_ASSERT_EQUAL_STRING("", fst.start);

        bsky_sb_push_fmt(&sb, "");
        fst = bsky_trim_left(bsky_sb_build_tmp(&sb));
        TEST_ASSERT_EQUAL_STRING("", fst.start);
    }

    static void string_cmp(void)
    {
        struct bsky_str_builder sb = { 0 };
        struct bsky_str fst, snd;


        bsky_sb_push_fmt(&sb, "aaaaaaa");
        fst = bsky_sb_build_tmp(&sb);
        bsky_sb_push_fmt(&sb, "aaaaaaa");
        snd = bsky_sb_build_tmp(&sb);

        TEST_ASSERT(bsky_str_starts_with(fst, snd));
        TEST_ASSERT(bsky_str_ends_with(fst, snd));
        TEST_ASSERT(bsky_str_eq(fst, snd));
        TEST_ASSERT(bsky_str_cmp(fst, snd) == 0);

        bsky_sb_push_fmt(&sb, "aaaaaaa");
        fst = bsky_sb_build_tmp(&sb);
        bsky_sb_push_fmt(&sb, "aabb");
        snd = bsky_sb_build_tmp(&sb);
        TEST_ASSERT(!bsky_str_starts_with(fst, snd));
        TEST_ASSERT(!bsky_str_ends_with(fst, snd));
        TEST_ASSERT(!bsky_str_eq(fst, snd));
        TEST_ASSERT(bsky_str_cmp(fst, snd) == -3);

        bsky_sb_push_fmt(&sb, "aaaaaaa");
        fst = bsky_sb_build_tmp(&sb);
        bsky_sb_push_fmt(&sb, "aa");
        snd = bsky_sb_build_tmp(&sb);
        TEST_ASSERT(bsky_str_starts_with(fst, snd));
        TEST_ASSERT(bsky_str_ends_with(fst, snd));
        TEST_ASSERT(!bsky_str_eq(fst, snd));
        TEST_ASSERT(bsky_str_cmp(fst, snd) == 3);

        bsky_sb_push_fmt(&sb, "aa");
        fst = bsky_sb_build_tmp(&sb);
        bsky_sb_push_fmt(&sb, "aaaaaa");
        snd = bsky_sb_build_tmp(&sb);
        TEST_ASSERT(!bsky_str_starts_with(fst, snd));
        TEST_ASSERT(!bsky_str_ends_with(fst, snd));
        TEST_ASSERT(!bsky_str_eq(fst, snd));
        TEST_ASSERT(bsky_str_cmp(fst, snd) == -3);
    }


    void run_string_tests(void)
    {
        RUN_TEST(string_builder);
        RUN_TEST(string_trim);
        RUN_TEST(string_cmp);
    }

#endif


#endif // string-tests_h_INCLUDED
