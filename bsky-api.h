/*
 * ============================================================================
 *                                   bsky-ap.h
 *                  BlueSky client API single header c library.
 *
 *               _         _                            _   _     
 *              | |__  ___| | ___   _        __ _ _ __ (_) | |__  
 *              | '_ \/ __| |/ / | | |_____ / _` | '_ \| | | '_ \ 
 *              | |_) \__ \   <| |_| |_____| (_| | |_) | |_| | | |
 *              |_.__/|___/_|\_\\__, |      \__,_| .__/|_(_)_| |_|
 *                              |___/            |_|              
 *
 * 
 * This is classic single header c library.
 * To use just include this header and predefine `BSKY_API_IMPLEMENTATION'
 * macro. Like this:
 *     > #define  BSKY_API_IMPLEMENTATION
 *     > #include "bsky-api.h"
 *
 * 
 * This library use `bsky_' for all functions, macroses, types, a.t.c. To not
 * mess your namespace. But also this library allows you to strip (remove)
 * prefix from all this names. To do id predefine `BSKY_API_STRIP_PREFIX'
 * macro. Like this;
 *     > #define  BSKY_API_IMPLEMENTATION
 *     > #define  BSKY_API_STRIP_PREFIX
 *     > #include "bsky-api.h"
 * 
 *  
 *                                              by Mokhov Mark
 *                                              (bluesky: mohmagen.bsky.social)
 * ============================================================================
 */


#ifndef __BSKY_API_H_GUARD__
#define __BSKY_API_H_GUARD__
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>

#define BSKY_ARRAY_LEN(array) (sizeof (array) / sizeof (array)[0])


/*
 * module
 * ============================================================================
 *                                   LOG
 * ============================================================================
*/
    enum bsky_log_level {
        bsky_log_None = 0, // not output
        bsky_log_Error,
        bsky_log_Warning,
        bsky_log_Info,
    };

    #ifndef BSKY_LOG_LEVEL
    #define BSKY_LOG_LEVEL bsky_log_Info
    #endif

    /**
     * Logger function used by `bsky_log'. Logger must sutisfy following
     * usage:
     *          BSKY_SIMPLE_LOGGER(fmt, __VA_ARGS__) or
     *          BSKY_LOGGER(level, fmt, __VA_ARGS__)
     *
     * NOTE: Simple logger do not accept level of logging. The `bsky_log's
     *       fmg will be modified to show log level. If you wanna display
     *       log level in custom way --- use `BSKY_LOGGER'
     *       instead.
     */
    #ifndef BSKY_LOGGER
        #ifndef BSKY_SIMPLE_LOGGER
            #define BSKY_SIMPLE_LOGGER(fmt, ...)                 \
                                       printf(fmt, __VA_ARGS__)
        #endif

        #define BSKY_LOGGER(level, fmt, ...) do {                \
                    switch (level) {                                     \
                    case bsky_log_None: break;                           \
                    case bsky_log_Error:                                 \
                        BSKY_SIMPLE_LOGGER("\x1b[31m[ERR]: `" fmt "'\n" \
                                           "\x1b[0m", __VA_ARGS__);      \
                        break;                                           \
                    case bsky_log_Warning:                               \
                        BSKY_SIMPLE_LOGGER("\x1b[33m[WAR]: `" fmt "'\n" \
                                           "\x1b[0m", __VA_ARGS__);      \
                        break;                                           \
                    case bsky_log_Info:                                  \
                        BSKY_SIMPLE_LOGGER("[INF]: `" fmt "'\n",        \
                                          __VA_ARGS__);                  \
                        break;                                           \
                    }                                                    \
                } while(0)                                               \

    #endif


    /**
     * Log formatted message using BSKY_LOGGER, unless level lesser than
     * `BSKY_LOG_LEVELE'
     */
    #define bsky_log(level, fmt, ...) do { if (level <= BSKY_LOG_LEVEL)     \
            BSKY_LOGGER(level, fmt, __VA_ARGS__);                           \
        } while(0)                                                          \

/*
 * module
 * ============================================================================
 *                                ERROR CODES
 * ============================================================================
*/
    enum bsky_error_code {
        bsky_ec_Ok = 0, // no error
        bsky_ec_Tmp_overflow,


        bsky_ec_Json_expect_CSB,
        bsky_ec_Json_expect_OSB,
        bsky_ec_Json_expect_CCB,
        bsky_ec_Json_expect_OCB,
        bsky_ec_Json_expect_Bool,
        bsky_ec_Json_expect_Null,
        bsky_ec_Json_expect_Number,
        bsky_ec_Json_expect_OQ,
        bsky_ec_Json_expect_CQ,
        bsky_ec_Json_expect_Colon,
        bsky_ec_Json_invalid_variant,

        bsky_ec_Failed_to_init_curl,
        bsky_ec_Failed_to_perform_curl,
        bsky_ec_Failed_to_init_curlm,
        bsky_ec_Failed_to_perform_curlm,
        bsky_ec_Failed_to_poll_curlm,
        bsky_ec_Failed_to_get_response_code,
        bsky_ec_Invalid_id,

        bsky_ec_Invalid_request,
        bsky_ec_Expired_token,
        bsky_ec_Ivalid_token,
        bsky_ec_Account_takedown,
        bsky_ec_Auth_factor_token_required,
    };

    /**
     * Get error code information.
     */
	const char *bsky_str_of_error_code(enum bsky_error_code);

    #define bsky_log_error(ec) bsky_log(bsky_log_Error,        \
                    "%s. %s:%d", bsky_str_of_error_code(ec),   \
                    __FILE__, __LINE__);                       \

    #define bsky_return_error(ec) do {                         \
                            bsky_log_error(ec);                \
                            return (ec);                       \
                        } while(0)                             \

    #define bsky_return_error_v(ec_v, ret) do {                \
                        *ec = ec_v;                            \
                        return (ret)                           \
                    } while(0)                                 \

	#define bsky_defer_ec(ec_v) do {                       \
                        *ec = ec_v;                            \
	                    goto defer;                        \
                    } while(0)                                 \


/*
 * module:
 * ============================================================================
 *                               TEMPORARY ARENA
 * ============================================================================
 */
    /**
     * 
     * Default tmp arena allocation. To use your own tmp arena
     * define `bsky_tmp_alloc' before including library.
     *
     * Example:
     *      #define bsky_tmp_alloc my_tmp_alloc
     *      #define BSKY_API_IMPLEMENTATION
     *      #include <bsky-api.h>
     * 
     * You can change default tmp arena size by predefining
     * `BSKY_DEFAULT_TMP_ARENA_CAPACITY'.
     *
     * The default arena return NULL, when it's overflow, so
     * the user must reset arena properly. The library itself
     * donot done this.
     *
     * To reset default tmp arena call:
     * bsky_default_tmp_reset();
     *
     * NOTE: if `bsky_tmp_alloc' returns NULL, it will be interpreted as
     *       temporary arena overflow.
     */
    #ifndef bsky_tmp_alloc
        #define bsky_tmp_alloc __bsky_default_tmp_alloc
    #endif
    #ifndef BSKY_DEFAULT_TMP_ARENA_CAPACITY
        #define BSKY_DEFAULT_TMP_ARENA_CAPACITY (0x8 * 0x400 * 0x400)
    #endif


    /**
     * Default tmp arena allocator.
     */
    void *__bsky_default_tmp_alloc(size_t);

    /**
     * Reset default tmp arena.
     */
    void bsky_default_tmp_reset(void);


/*
 * module:
 * ============================================================================
 *                             DYNAMIC ARRAY
 * ============================================================================
*/

    /**
     * Generic interface for dynamic arrays.
     * All dynamic arrays must start with fiels:
     *     0. pointer to the data.       (type: pointer)
     *     1. the length of the array.   (type: size_t)
     *     2. the capacity of the array. (type: size_t)
     *
     * To use generic dynamic array with macroses it must have
     * fields names alike `struct bsky_dynamic_arr'.
     *
     * Functions will work even with different fields names.
     */
    struct bsky_dynamic_arr {
        void *data; size_t len, cap;
    };

    /**
     * Push copy of element to the end of the dynamic array.
     */
    enum bsky_error_code
    __bsky_da_push(void *self_gen, const void *elem, size_t elem_size);

    /**
     * Append array of elements to the dynamic array;
     */
    enum bsky_error_code
    __bsky_da_append(void *self_gen, const void *elems,
                     size_t elem_size, size_t len);


    void  bsky_clear_da(void *self_get);

    /**
     * Free dynamic array.
     */
    void   bsky_da_free(void *da);

    /**
     * Push copy of element to the end of the dynamic array.
     */
    #define bsky_da_push(da, elem) __bsky_da_push(da, &(elem), sizeof (elem))

    /**
     * Push copy of element to the end of the dynamic array.
     */
    #define bsky_da_append(da, elems, len) __bsky_da_push(da, elems,\
                                                    sizeof (*elems), len)



/*
 * module:
 * ============================================================================
 *                                   VIEW
 * ============================================================================
*/
    /**
     * Generic interface for views. (fat pointers with start and end);
     *     0. pointer to the start of the data.              (type: pointer)
     *     1. pointer to the end of the data [not included]. (type: pointer)
     * 
     * To use generic view with macroses it must have
     * fields names alike `struct bsky_view'.
     *
     * Functions will work even with different fields names.
     */
    struct bsky_view { void *start, *end; };

	/**
     * Create view from gneric dynamic array and element size.
     */
    struct bsky_view __bsky_view_of_da(void *da, size_t elem_size);

    /**
     * Copy view to tmp storage.
     */
    struct bsky_view   bsky_view_to_tmp(struct bsky_view);

	/**
     * Copy generic dynamic array to tmp areana, free it, and return view to
     * the data.
     */
    struct bsky_view __bsky_tmp_view_of_da(void *da, size_t elem_size); 

    /**
     * Construct view of dynamic array pointer.
     */
    #define bsky_view_of_da(da)\
          __bsky_view_of_da(da, sizeof da->data[0])

    /**
     * Copy dynamic array pointer to tmp view and free it.
     */
    #define bsky_tmp_view_of_da(da)\
          __bsky_tmp_view_of_da(da,\
              sizeof (da)->data[0])\




/*
 * module:
 * ============================================================================
 *                                   STRING
 * ===========================================================================
*/
    /**
     * Builder for the string.
     * 
     * NOTE: string builder ensures that string always null terminated.
     */
    struct bsky_str_builder { char *data; size_t len, cap; };

    /**
     * Null terminated string. The end pointer points to '\0' char, so the
     * length of the string (without null char) equal to end - start;
     */
    struct bsky_str         { char *start; char *end; };

    /**
     * Create BSKY string from valid c zero terminated string.
     */
    struct bsky_str bsky_mk_str(char *str);

    /**
     * Push char to `string builder'.
     */
    void bsky_sb_push(struct bsky_str_builder *sb, char c);

    /**
     * Push string to string builder.
     *
     * NOTE: even if string is not null terminated, string builder will
     * add null character at the end.
     */
    void bsky_sb_push_str(struct bsky_str_builder *, struct bsky_str);

    /**
     * Push formated string to string builder.
     */
    void bsky_sb_push_fmt(struct bsky_str_builder *, char *, ...);

    /**
     * Build string from string builder.
     *
     * NOTE: string owning data of builder.
     */
    struct bsky_str bsky_sb_build(struct bsky_str_builder *sb);

    /**
     * Build string in tmp arena and free builder.
     */
    struct bsky_str bsky_sb_build_tmp(struct bsky_str_builder *sb);

    /**
     * Construct view of string. String is view by itself, but
     * string not include last character (null character), however
     * returned view will contain null character.
     */
    struct bsky_view bsky_view_of_str(struct bsky_str);

    /**
     * Left trim string. This function donot copy
     * string it's just create subview of the original string.
     */
    struct bsky_str bsky_trim_left(struct bsky_str);

    /**
     * Check if string starts with other string.
     * Retrun 1 if string starts with prefix and 0 otherwise.
     */
    int bsky_str_starts_with(struct bsky_str, struct bsky_str);

    /**
     * Check if string ends with other string.
     * Retrun 1 if string ends with prefix and 0 otherwise.
     */
    int bsky_str_ends_with(struct bsky_str, struct bsky_str);

    /**
     * Check if two strings equal to each other.
     * Return 1 if strings are equal and 0 otherwise.
     */
    int bsky_str_eq(struct bsky_str, struct bsky_str);

    /**
     * Compare two strings.
     * 
     * Return 0 if strings are equal.
     * Return 1+idx if first string bigger than second.
     * Return -1-idx if secodn string bigger than first.
     *
     * idx -- idx of first different character of strings.
     */
    int bsky_str_cmp(struct bsky_str, struct bsky_str);

    /**
     * Return length of the string.
     */
    size_t bsky_str_len(struct bsky_str);

    /**
     * Shift string view.
     */

    struct bsky_str bsky_shift_str(struct bsky_str, size_t n);

    /**
     * FN                         BSKY STR FROM FILE                         FN
     *
     * Read string from file. Will return empty string in case when read
     * failed.
     */
	struct bsky_str bsky_str_from_file_tmp(const char *file_path);


    /**
     * FN                           BSKY STR NULL                            FN
     *
     * Create NULL string.
     */
	struct bsky_str bsky_str_null();

	/**
     * FN                           BSKY SB WRITE                            FN
     *
     * CULR write function for string builders.
     */
    size_t bsky_sb_write(char *ptr, size_t size, size_t nmemb,
                         struct bsky_str_builder *sb);



/*
 * ===========================================================================
 *                                    JSON
 * ===========================================================================
*/

	// Just forward declaration :)
    struct bsky_json_pair;

    /**
     * `bsky_json' is data structure to represent json format, parse, and
     * serialize it. It can be static created.
     *
     * By default `bsky_json' is not dynamic data structure, cause in primary
     * use cases it should just serve as help format to create JSON strings or
     * read data from JSON format.
     */
    struct bsky_json {
        enum {
            bsky_json_Arr,  // JSON Array
            bsky_json_Dct,  // JSON Dictionary

            bsky_json_Num,  // JSON Number
            bsky_json_Str,  // JSON String
            bsky_json_Bool, // JSON Boolean
            bsky_json_Null, // JSON NULL
        } var;

        union {
            struct { struct bsky_json      *data; size_t len; } arr;
            struct { struct bsky_json_pair *data; size_t len; } dct;
            long double num; 
            char *str;
            int _bool;
        };
    };

    // pair to implement dictionaries.
    struct bsky_json_pair {
        char *name; struct bsky_json value;
    };

    /**
     * Create temporary string of json. The resulting string would be in
     * compressed format.
     */
    struct bsky_str bsky_tmp_str_of_json(struct bsky_json);

    /**
     * Push JSON string to string builder.
     */
    void bsky_sb_push_json(struct bsky_str_builder *, struct bsky_json);


    /**
     * 
     */
    struct bsky_json bsky_parse_json(struct bsky_str*, enum bsky_error_code*); 

    struct bsky_json bsky_parse_json_arr(struct bsky_str*,
                                         enum bsky_error_code*);
    struct bsky_json bsky_parse_json_dct(struct bsky_str*,
                                         enum bsky_error_code*);
    struct bsky_json bsky_parse_json_str(struct bsky_str*,
                                         enum bsky_error_code*);
    struct bsky_json bsky_parse_json_num(struct bsky_str*,
                                         enum bsky_error_code*);
    struct bsky_json bsky_parse_json_null(struct bsky_str*,
                                          enum bsky_error_code*);
    struct bsky_json bsky_parse_json_bool(struct bsky_str*,
                                          enum bsky_error_code*);


    typedef struct bsky_json      bsky_Json;
    typedef struct bsky_json_pair bsky_Json_Pair;

    // helper functions.
    struct bsky_json_da      { struct bsky_json      *data; size_t len, cap; };
    struct bsky_json_pair_da { struct bsky_json_pair *data; size_t len, cap; };

/**
 * ============================================================================
 *                                  FUTURES
 * ============================================================================
 */
    struct bsky_future;
    typedef int (*bsky_poll_fn)(void * /*self data*/,
                                void * /*data*/, void * /*result*/,
                                enum bsky_error_code * /*ec*/);

    /**
     * STRUCT                     BSKY FUTURE                            STRUCT
     *
     * Interface representing furure handler for async programming. "Future" in
     * `rust' meaning.
     * 
     */
    struct bsky_future {
        void *self;
        bsky_poll_fn poll;
    };

    #define bksy_mk_future(self, poll_fn) \
                          (struct bsky_future) { self, poll_fn }


    /**
     * FN                       BSKY FUTURE POLL                             FN
     *
     * Wrapper to call `poll' of `struct bsky_future'.
     *
     * RETURNS:
     *    - 0:   the future is still running.
     *    - > 0: the future've finished successfully. (in that case `ec' will
     *           always equal to `bsky_ec_Ok').
     *    - < 0: the future've finished with error.
     * 
     * ARGS:
     *    1. self   -- the future.
     *    2. data   -- additional data to poll. (may be deltatime or result of
     *                previous chained future for example.)
     *    3. result -- pointer to resulting data.
     *    4. ec     -- information about error.
     * 
     * EXAMPLE:
     *
     *    enum bsky_error_code ec;
     *    struct bsky_http_response res = { 0 };
     *    struct bsky_future req = bsky_http_request_async(...some args);
     *
     *    while (bsky_future_poll(req, NULL, &res, &ec) == 0) {
     *        bsky_log(bsky_log_Info, "getting response...");
     *    }
     *
     *    if (ec == bsky_ec_Ok)
     *        bsky_log(bsky_log_Info, "Ok: `%s'", bsky_tmp_str_of_json(res));
     *    else
     *        bsky_log_error(ec);
     * 
     */
    int bsky_fufure_poll(struct bsky_future self, void *data, void *result,
                         enum bsky_error_code *ec);




/**
 * ============================================================================
 *                                    CURL
 * ============================================================================
 */
   /*
    * STRUCT                       BSKY HEADERS                          STRUCT
    *
    * Dynamic array of http headers.
    *
    */
    struct bsky_headers {
        struct bsky_str *data;
        size_t len, cap;
    };

    /**
     * STRUCT                      BSKY HEADERS VIEW                     STRUCT
     *
     * View of http headers.
     *
     */
    struct bsky_headers_view {
        struct bsky_str *start;
        struct bsky_str *end;
    };

	/**
     * STRUCT                      BSKY HTTP RESPONSE                    STRUCT
     *
     * The `struct bsky_http_response' contains resulting data of http request.
     * 
     */
    struct bsky_http_response {
        struct bsky_json json;
        long             code;
    };

    /**
     *
     * FN                         BSKY HTTP REQUST                           FN
     *
     * Do a http/https post requst to the server with provided headers and
     * json request data.
     *
     * This request have `Content-type: application/json' header additional to
     * the headers list.
     *
     * ARGS:
     *    1. server url. (bsky_str)
     *    2. headers. (bsky_headers)
     *    3. request. (bsky_json)
     *    5. error_code. (bsky_error_code) -- 
     */
    struct bsky_http_response bsky_http_request(struct bsky_str,
                                                struct bsky_view,
                                                struct bsky_json,
                                                enum bsky_error_code*);


    /**
     *
     * FN                       BSKY HTTP REQUST ASYNC                       FN
     *
     * Async version of `bsky_http_request'.
     *
     * NOTE:
     *    when poll resulting future, the data will be pointer to long or NULL.
     *    if `data' is NULL, then will not wait on every poll, otherwise will
     *    `data' milliseconds on every poll.
     */
    struct bsky_future bsky_http_request_async(struct bsky_str,
                                               struct bsky_view,
                                               struct bsky_json,
                                               enum bsky_error_code*);
/*
 * ============================================================================
 *                                   API TYPES
 * ============================================================================
 */

	/**
     * STRUCT                     BSKY IDENTIFIER                        STRUCT
     *
     * Tagged union representing different types of user identifiers.
     *
     * TODO:
     *    implement other types.
     */
    struct bsky_identifier {
        enum { bsky_id_Handle, bsky_id_Invalid } var;
        union { 
            struct { struct bsky_str username, server; } handle;
        };
    };

	/**
     * FN                       BKSY MK HANDLE TMP                           FN
     *
     * Construct bsky identifier from handle str. This function do not fully
     * validate handle. So handle must be valid.
     *
     * PARAMS:
     *    - handle -- Bsky handle. e.g. "username.server.exmpl".
     */
    struct bsky_identifier bsky_mk_handle_tmp(char *handle);


	/**
     * FN                     BKSY GET SERVER OF ID                          FN
     *
     * Return PDS server of actor by identifier.
     *
     * SEE ALSO:
     *    - BSKY SERVERS:
     *      `https://docs.bsky.app/docs/advanced-guides/api-directory'
     */
    struct bsky_str bsky_get_server_of_id(struct bsky_identifier);

	/**
     * FN                        BKSY ID IS VALID                            FN
     *
     * Check is id valid or not.
     */
    int bsky_id_is_valid(struct bsky_identifier);

/*
 * ============================================================================
 *                                    SESSION
 * ============================================================================
 */
    /**
     * ENUM                      BSKY SESSION STATUS                       ENUM
     *
     * 
     * This field represent connections's status. `bsky_session_status_Active'
     * represents that session is active; `bsky_session_status_Null' represents
     * that no information about session active and status.
     * all other statuses tells that
     * session is inactive.
     *
     * NOTE:
     *    This enumeration represents `active' and `status' fields of 200 code
     *    repsonse for `com.atproto.server.createSession'.
     *
     * SEE ALSO:
     *    - bsky api documentation for `com.atproto.server.createSession':
     * `https://docs.bsky.app/docs/api/com-atproto-server-create-session'
     */
    enum bsky_session_status {
        bsky_session_status_Null = 0,

        bsky_session_status_Active,
        bsky_session_status_Inactive, // status not provied, but not active.
        bsky_session_status_Takedown,
        bsky_session_status_Suspended,
        bsky_session_status_Deactivated,
    };


    /**
     * STRUCT                       BSKY SESSION EMAIL                   STRUCT
     *
     * Structure representing email of the session.
     *
     * NOTE:
     *    Fields of this structure represents fields `email', `emailConfirmed',
     *    and `emailAuthFactor' of 200 code response for
     *    `com.atproto.server.createSession'.
     * 
     * SEE ALSO:
     *    - bsky api documentation for `com.atproto.server.createSession':
     * `https://docs.bsky.app/docs/api/com-atproto-server-create-session'
     */
    struct bsky_session_email {
        struct bsky_str email;

        enum {
            bsky_session_email_c_Null = 0, 

            bsky_session_email_Confirmed,
            bsky_session_email_Not_confirmed,
        } confirmed;

        enum {
            bsky_session_email_f_Null = 0,

            bsky_session_email_Auth_factor,
            bsky_session_email_No_auth_factor,
        } auth_factor;
    };


    /**
     * STRUCT                        BSKY SESSION                        STRUCT
     *
     * Struct contains information about current session. 
     *
     * NOTE:
     *    The `bsky_session' do not contain authorization keys. The auth keys
     *    are contained by `bsky_auth'.
     * 
     * SEE ALSO:
     *    - bsky api documentation for `com.atproto.server.createSession':
     * `https://docs.bsky.app/docs/api/com-atproto-server-create-session'
     * 
     *    - bsky api documentation for `com.atproto.server.getSession':
     * `https://docs.bsky.app/docs/api/com-atproto-server-get-session'
     */
    struct bsky_session {
        struct bsky_str handle;
        struct bsky_str did;

        struct bsky_str didDoc;
        struct bsky_session_email  email;
        enum   bsky_session_status status;
    };



    /**
     * STRUCT                         BSKY AUTH                          STRUCT
     *
     * Struct contains information session's authorization. To get authorized
     * session you ned to create session by `bsky_create_session'.
     *
     * SEE ALSO:
     *    - bsky api documentation for `com.atproto.server.createSession':
     * `https://docs.bsky.app/docs/api/com-atproto-server-create-session'
     */
    struct bsky_auth {
        struct bsky_str accessJwt;
        struct bsky_str refreshJwt;
    };


    /**
     * FN                        BSKY CREATE SESSION                         FN
     * 
     * Create authorized session for user (Actor).
     *
     * PARAMS:
     *    - server     -- ATProto PDS Server url [The server where Actor
     *              hosted, for example: bsky.social -- is default PDS Server.]
     * 
     *    - identifier -- Identifier of a an Actor. It can be handle or any
     *              other identifier of the Actor allowed by bluesky api.
     * 
     *    - password   -- Password for authorization.
     *
     *    - auth_factor -- Auth factor token. Not required. If not provided
     *              must be (struct bsky_str) { NULL, NULL }.
     *
     *    - ec         -- Error code pointer. If session was created
     *              successfully this would be `bsky_ec_Ok'. Possible errors:
     *               Protocol errors:
     *                   - `bsky_ec_Invalid_request'
     *                   - `bsky_ec_Expired_token'
     *                   - `bsky_ec_Ivalid_token'
     *                   - `bsky_ec_Account_takedown'
     *                   - `bsky_ec_Auth_factor_token_required'
     *
     *               - `bsky_ec_Failed_to_init_curl' -- in case curl init
     *                                               will failed.
     *               - `bsky_ec_Invalid_id' -- provided identifier is invalid.
     *
     * SEE ALSO:
     *    - bsky api documentation for `com.atproto.server.createSession':
     * `https://docs.bsky.app/docs/api/com-atproto-server-create-session'
     */
    struct bsky_crses_resp;
    struct bsky_crses_resp 
    bsky_create_session(struct bsky_identifier identifier,
                        struct bsky_str        password,
                        struct bsky_str        auth_factor,
                        enum bsky_error_code  *ec);

    struct bsky_crses_resp {
        struct bsky_auth    auth;
        struct bsky_session session;
    };


/**
 * ============================================================================
 *                                RECORD
 * ============================================================================
 */

/*
 * ============================================================================
 *                             IMPLEMENTATION
 * ============================================================================
 *
 * To enable implementation predefine `BSKY_API_IMPLEMENTATION' macro.
 */
#ifdef BSKY_API_IMPLEMENTATION

    /*
     * BSKY ERROR CODE
     */
    const char *bsky_str_of_error_code(enum bsky_error_code code)
    {
        switch (code) {
        case bsky_ec_Ok:           return "Ok";
        case bsky_ec_Tmp_overflow: return "overflow of temporary arena!";

        case bsky_ec_Json_expect_CSB: 
            return "JSON: expect ']' at the end of array!";
        case bsky_ec_Json_expect_OSB: 
            return "JSON: expect '[' at the start of the array!";
        case bsky_ec_Json_expect_CCB: 
            return "JSON: expect '}' at the end of dictionary!";
        case bsky_ec_Json_expect_OCB: 
            return "JSON: expect '}' at the start of dictionary!";
        case bsky_ec_Json_expect_Bool: 
            return "JSON: expect 'true' or 'false'!";
        case bsky_ec_Json_expect_Null: 
            return "JSON: expect 'null'!";
        case bsky_ec_Json_expect_Number: 
            return "JSON: expect number!";
        case bsky_ec_Json_expect_OQ: 
            return "JSON: expect '\"' at the start of string!";
        case bsky_ec_Json_expect_CQ: 
            return "JSON: expect '\"' at the end of string!";
        case bsky_ec_Json_expect_Colon: 
            return "JSON: expect ':' between key and value!";
        case bsky_ec_Json_invalid_variant:
            return "JSON: parse invalid json variant!";

        case bsky_ec_Failed_to_init_curl: 
            return "CURL: failed to init curl easy instance.";
        case bsky_ec_Failed_to_perform_curl: 
            return "CURL: failed to perform curl easy request.";
        case bsky_ec_Failed_to_init_curlm: 
            return "CURL: failed to init curl multi instance.";
        case bsky_ec_Failed_to_poll_curlm:
            return "CURL: failed to poll curl multi instance.";
        case bsky_ec_Failed_to_perform_curlm: 
            return "CURL: failed to perform curl multi request.";
        case bsky_ec_Failed_to_get_response_code:
            return "CURL: failed to get response code.";

        case bsky_ec_Invalid_id: 
            return "API: Ivalid id";
        case bsky_ec_Invalid_request: 
            return "API: invalid request";
        case bsky_ec_Expired_token: 
            return "API: expired token";
        case bsky_ec_Ivalid_token: 
            return "API: ivalid token";
        case bsky_ec_Account_takedown: 
            return "API: account takedown";
        case bsky_ec_Auth_factor_token_required: 
            return "API: auth factor token required";
        }
    }

    /*
     * DEFAULT TMP ARENA
     */
    struct __bsky_default_tmp_arena {
        void *allocator; size_t len;
    } __bsky_default_tmp_arena = { 0 };

    void    bsky_default_tmp_reset(void) {
        __bsky_default_tmp_arena.len = 0;
    }

    void *__bsky_default_tmp_alloc(size_t size_to_alloc)
    {
        if (__bsky_default_tmp_arena.allocator == 0) {
            __bsky_default_tmp_arena.allocator =
                calloc(BSKY_DEFAULT_TMP_ARENA_CAPACITY, sizeof(char));
        }

        if (__bsky_default_tmp_arena.len + size_to_alloc >
            BSKY_DEFAULT_TMP_ARENA_CAPACITY) 
            return NULL;

        void *ret = __bsky_default_tmp_arena.allocator
                    + __bsky_default_tmp_arena.len;

        __bsky_default_tmp_arena.len += size_to_alloc;

        return ret;
    }

    /*
     * BKSY DYNAMIC ARRAY
     */
    void bsky_da_free(void *da) {
        struct bsky_dynamic_arr *self = (struct bsky_dynamic_arr*) da;

        if (self->data != NULL) free(self->data);
    }

    enum bsky_error_code 
    __bsky_da_push(void *self_gen, const void *elem, size_t elem_size)
    {
        struct bsky_dynamic_arr *self = (struct bsky_dynamic_arr*) self_gen;

        if (self->len >= self->cap) {
            self->cap  = self->cap ?  self->cap * 2 : 16;
            self->data = realloc(self->data, self->cap * elem_size);
            if (self->data == NULL) bsky_return_error(bsky_ec_Tmp_overflow);
        }

        memcpy(self->data + self->len++ * elem_size, elem, elem_size);

        return bsky_ec_Ok;
    }

    enum bsky_error_code
    __bsky_da_append(void *self_gen, const void *elems,
                     size_t elem_size, size_t len)
    {
        struct bsky_dynamic_arr *self = (struct bsky_dynamic_arr*) self_gen;

        if (self->len + len > self->cap) {
            self->cap  = (self->cap ? self->cap * 2 : 16) + len;
            self->data = realloc(self->data, self->cap * elem_size);

            if (self->data == NULL) bsky_return_error(bsky_ec_Tmp_overflow);
        }

        memcpy(self->data + self->len, elems, elem_size * len);
        self->len += len;

        return bsky_ec_Ok;
    }

    void bsky_clear_da(void *self_gen) {
        struct bsky_dynamic_arr *self = (struct bsky_dynamic_arr*) self_gen;

        *self = (struct bsky_dynamic_arr) { 0 }; 
    }

    /*
     * BSKY VIEW
     */
    struct bsky_view __bsky_view_of_da(void *_da, size_t elem_size)
    {
        struct bsky_dynamic_arr *da = (struct bsky_dynamic_arr *) _da;

        return (struct bsky_view) { da->data, da->data + da->len * elem_size };
    }

    struct bsky_view bsky_view_to_tmp(struct bsky_view view)
    {
        void *data = bsky_tmp_alloc(view.end - view.start);
        memcpy(data, view.start, view.end - view.start);

        return (struct bsky_view) { data, data +  (view.end - view.start) };
    }

    struct bsky_view __bsky_tmp_view_of_da(void *da, size_t elem_size)
    {
        struct bsky_view ret = bsky_view_to_tmp(
                                 __bsky_view_of_da(da, elem_size));

        bsky_da_free(da);
        bsky_clear_da(da);

        return ret;
    }

    /*
     * BSKY STRING
     */
    void bsky_sb_push(struct bsky_str_builder *sb, char c)
    {
        const char nullc = '\0';

        if (sb->len == 0) {
            char buf[] = { c, nullc };
            __bsky_da_append(sb, buf, sizeof(char), 2);
        }
        else {
            sb->data[sb->len-1] = c;
            __bsky_da_push(sb, &nullc, sizeof(char));
        }
    }

    void bsky_sb_push_str(struct bsky_str_builder *sb, struct bsky_str str)
    {
        const char nullc = '\0';

        if (sb->len != 0) sb->len--; // remove null character.

        __bsky_da_append(sb, str.start, sizeof(char), str.end - str.start);

        if (sb->data[sb->len-1] != '\0')
            __bsky_da_push(sb, &nullc, sizeof(char));
    }

    #include <stdarg.h>

    void bsky_sb_push_fmt(struct bsky_str_builder *sb, char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        int len = vsnprintf(NULL, 0, fmt, args);
        va_end(args);

        char *result = bsky_tmp_alloc(len + 1);
        va_start(args, fmt);
        vsnprintf(result, len + 1, fmt, args);
        va_end(args);

        bsky_sb_push_str(sb, (struct bsky_str) {result, result + len + 1});
    }

    struct bsky_str bsky_sb_build(struct bsky_str_builder *sb)
    {
        return (struct bsky_str) { sb->data, sb->data + sb->len-1 };
    }

    struct bsky_str bsky_sb_build_tmp(struct bsky_str_builder *sb)
    {
        struct bsky_view str = bsky_view_of_str(bsky_sb_build(sb));
        struct bsky_view tmp = bsky_view_to_tmp(str);

        bsky_da_free(sb);
        *sb = (struct bsky_str_builder) { 0 };

        return (struct bsky_str) { tmp.start, tmp.end-1 };
    }

    struct bsky_view bsky_view_of_str(struct bsky_str str)
    {
        return (struct bsky_view) { str.start, str.end + 1 };
    }

    static int __s_is_whitespace(char c) {
        return c == ' ' || c == '\n' || c == '\t';
    }

    struct bsky_str bsky_trim_left(struct bsky_str str)
    {
        while (str.start != str.end && __s_is_whitespace(*str.start)) {
            str.start++;
        }

        return str;
    }

    int bsky_str_starts_with(struct bsky_str fst, struct bsky_str pref)
    {
        size_t fst_len = bsky_str_len(fst), pref_len = bsky_str_len(pref);

        if (fst_len < pref_len) return 0;

        return memcmp(fst.start, pref.start, pref_len) == 0;
    }

    int bsky_str_ends_with(struct bsky_str fst, struct bsky_str post)
    {
        size_t fst_len = bsky_str_len(fst), post_len = bsky_str_len(post);

        if (fst_len < post_len) return 0;

        return memcmp(fst.start + fst_len - post_len,
                      post.start, post_len) == 0;
    }

    size_t bsky_str_len(struct bsky_str str) {
        return (str.end - str.start) / sizeof (char);
    }

    int bsky_str_eq(struct bsky_str fst, struct bsky_str snd) {
        return strcmp(fst.start, snd.start) == 0;
    }

    int bsky_str_cmp(struct bsky_str fst, struct bsky_str snd) {
        size_t fst_len = bsky_str_len(fst), snd_len = bsky_str_len(snd);

        for (int i = 0; i < (fst_len > snd_len ? fst_len : snd_len); ++i) {
            if (fst.start[i] > snd.start[i]) return  1 + i;
            if (fst.start[i] < snd.start[i]) return -1 - i;
        }

        return fst_len > snd_len ?   1 + snd_len :
               snd_len > fst_len ? - 1 - fst_len :
               0;
    }


    struct bsky_str bsky_shift_str(struct bsky_str str, size_t n) {
        if (n > bsky_str_len(str)) {
            return (struct bsky_str){str.end, str.end};
        }

        return (struct bsky_str) {str.start + n, str.end};
    }

    struct bsky_str bsky_mk_str(char *str) {
        return (struct bsky_str) { str, str + strlen(str) };
    }


    struct bsky_str bsky_str_from_file_tmp(const char *file_path)
    {
        struct bsky_str_builder sb = { 0 };
        char buffer[1024];
        FILE *file;

        file = fopen(file_path, "r");
        if (!file) return bsky_str_null();

        size_t len = 0;
        while ((len = fread(buffer, sizeof buffer, 1, file)) > 0) {
            bsky_sb_push_str(&sb, (struct bsky_str) { buffer, buffer + len });
        }

        fclose(file);

        return bsky_sb_build_tmp(&sb);
    }

    struct bsky_str bsky_str_null() {
        return (struct bsky_str) { NULL, NULL };
    }

    size_t bsky_sb_write(char *ptr, size_t size, size_t nmemb,
                         struct bsky_str_builder *sb)
    {
        struct bsky_str str = (struct bsky_str) {
            .start = ptr,
            .end   = ptr + size * nmemb,
        };

        bsky_sb_push_str(sb, str);

        return size * nmemb;
    }

    /*
     * BSKY JSON
     */
    void bsky_sb_push_json(struct bsky_str_builder *sb, struct bsky_json json)
    {
        switch (json.var) {
        case bsky_json_Arr: {
            bsky_sb_push(sb, '[');

            for (size_t i = 0; i < json.arr.len; ++i) {
                bsky_sb_push_json(sb, json.arr.data[i]);

                if (i != json.arr.len-1) bsky_sb_push(sb, ',');
            }

            bsky_sb_push(sb, ']');
        } break;
        case bsky_json_Dct: {
            bsky_sb_push(sb, '{');

            for (size_t i = 0; i < json.dct.len; ++i) {
                bsky_sb_push_fmt(sb, "\"%s\":", json.dct.data[i].name);
                bsky_sb_push_json(sb, json.dct.data[i].value);

                if (i < json.dct.len-1) bsky_sb_push(sb, ',');
            }

            bsky_sb_push(sb, '}');
        } break;
        case bsky_json_Num: {
            if (fabsl(json.num - (float)(int)json.num) < 0.0001) {
                bsky_sb_push_fmt(sb, "%d", (int)json.num);
            } else {
                bsky_sb_push_fmt(sb, "%f.3",    json.num);
            }
        }break;
        case bsky_json_Str:  bsky_sb_push_fmt(sb, "\"%s\"", json.str); break;
        case bsky_json_Null: bsky_sb_push_fmt(sb, "null"); break;
        case bsky_json_Bool: {
            bsky_sb_push_fmt(sb, "%s", json._bool ? "true" : "false"); 
        }break;
        }
    }

    struct bsky_str bsky_tmp_str_of_json(struct bsky_json json)
    {
        struct bsky_str_builder sb = { 0 };

        bsky_sb_push_json(&sb, json);

        return bsky_sb_build_tmp(&sb);
    }


    struct bsky_json bsky_parse_json_arr(struct bsky_str *data,
                                         enum bsky_error_code *ec)
    {
        *ec = bsky_ec_Ok;

        struct bsky_json_da arr_da = { 0 };
        struct bsky_json json = { 0 };

        *data = bsky_trim_left(*data);

        if (*data->start != '[') bsky_defer_ec(bsky_ec_Json_expect_OSB);

        do {
            *data = bsky_shift_str(*data, 1);
            *data = bsky_trim_left(*data);
            if (*data->start == ']') break;

            struct bsky_json elem = bsky_parse_json(data, ec);
            if (*ec != bsky_ec_Ok) bsky_defer_ec(*ec);

            bsky_da_push(&arr_da, elem);

            *data = bsky_trim_left(*data);
        } while(*data->start == ',');

        if (*data->start != ']') bsky_defer_ec(bsky_ec_Json_expect_CSB);

        *ec = bsky_ec_Ok;
        *data = bsky_shift_str(*data, 1);

        struct bsky_view arr = bsky_tmp_view_of_da(&arr_da);

        json.var = bsky_json_Arr;
        json.arr.data = arr.start;
        json.arr.len  = (arr.end - arr.start) / sizeof (struct bsky_json);

    defer:
        bsky_da_free(&arr_da);
        return json;
    }


    struct bsky_json bsky_parse_json_dct(struct bsky_str *data,
                                         enum bsky_error_code *ec)
    {
        *ec = bsky_ec_Ok;

        struct bsky_json_pair_da dct_da = { 0 };
        struct bsky_json json = { 0 };

        *data = bsky_trim_left(*data);

        if (*data->start != '{') bsky_defer_ec(bsky_ec_Json_expect_OCB);

        json.var = bsky_json_Dct;

        struct bsky_json elem = { 0 };
        struct bsky_json name = { 0 };
        struct bsky_json_pair pair = { 0 };

        do {
            *data = bsky_shift_str(*data, 1);
            *data = bsky_trim_left(*data);
            if (*data->start == '}') break;

            name = bsky_parse_json_str(data, ec);
            if (*ec != bsky_ec_Ok) bsky_defer_ec(*ec);

            *data = bsky_trim_left(*data);

            if (*data->start != ':') bsky_defer_ec(bsky_ec_Json_expect_Colon);

            *data = bsky_shift_str(*data, 1);

            elem = bsky_parse_json(data, ec);

            if (*ec != bsky_ec_Ok) bsky_defer_ec(*ec);

            pair.name  = name.str;
            pair.value = elem;
            bsky_da_push(&dct_da, pair);

            *data = bsky_trim_left(*data);
        } while(*data->start == ',');

        if (*data->start != '}') bsky_defer_ec(bsky_ec_Json_expect_CCB);

        *data = bsky_shift_str(*data, 1);

        struct bsky_view dct = bsky_tmp_view_of_da(&dct_da);

        json.dct.data = dct.start;
        json.dct.len  = (dct.end - dct.start) / sizeof (struct bsky_json_pair);

    defer:
        bsky_da_free(&dct_da);
        return json;
    }

    struct bsky_json bsky_parse_json_str(struct bsky_str *data,
                                         enum bsky_error_code *ec)
    {
        *ec = bsky_ec_Ok;

        struct bsky_str_builder sb = { 0 };
        struct bsky_json json = { 0 };

        *data = bsky_trim_left(*data);

        if (*data->start != '"') bsky_defer_ec(bsky_ec_Json_expect_OQ);

        *data = bsky_shift_str(*data, 1);

        char *end = data->start;

        while (end < data->end && *end != '"') {
            if (*end == '\\' && end+1 != data->end) end++;
            end++;
        }

        bsky_sb_push_str(&sb, (struct bsky_str) { data->start, end });
        data->start = end;

        if (end >= data->end) bsky_defer_ec(bsky_ec_Json_expect_CQ);

        data->start++;



        struct bsky_str str = bsky_sb_build_tmp(&sb);

        json.var = bsky_json_Str;
        json.str = str.start;

    defer:
        bsky_da_free(&sb);
        return json;
    }

    struct bsky_json bsky_parse_json_num(struct bsky_str *data,
                                         enum bsky_error_code *ec)
    {
        *ec = bsky_ec_Ok;

        struct bsky_json json = { 0 };

        *data = bsky_trim_left(*data);

        char *endptr = NULL;
        long double v = strtold(data->start, &endptr);

        if (endptr == data->start) bsky_defer_ec(bsky_ec_Json_expect_Number);

        data->start = endptr;

        json.var = bsky_json_Num;
        json.num = v;

    defer:
        return json;
    }

    struct bsky_json bsky_parse_json_null(struct bsky_str *data,
                                          enum bsky_error_code *ec)
    {
        *ec = bsky_ec_Ok;

        struct bsky_json json = { 0 };

        *data = bsky_trim_left(*data);

        if (!bsky_str_starts_with(*data, bsky_mk_str("null")))
            bsky_defer_ec(bsky_ec_Json_expect_Null);

        json.var = bsky_json_Null;

        data->start += 4;

	defer:
        return json;
    }

    struct bsky_json bsky_parse_json_bool(struct bsky_str *data,
                                          enum bsky_error_code *ec)
    {
        *ec = bsky_ec_Ok;

        struct bsky_json json = { 0 };

        *data = bsky_trim_left(*data);

        json.var = bsky_json_Bool;

        if (bsky_str_starts_with(*data, bsky_mk_str("false"))) {
            data->start += 5;
            json._bool = 0;

        } else if (bsky_str_starts_with(*data, bsky_mk_str("true"))) {
            data->start += 4;
            json._bool = 1;

        } else {
            bsky_defer_ec(bsky_ec_Json_expect_Bool);
        }

    defer:
        return json;
    }

    struct bsky_json bsky_parse_json(struct bsky_str *data,
                                     enum bsky_error_code* ec)
    {
        *ec = bsky_ec_Ok;
        struct bsky_json json = { 0 };

        *data = bsky_trim_left(*data);
        struct bsky_str data_s = *data;

        json = bsky_parse_json_null(data, ec);
        if (*ec != bsky_ec_Json_expect_Null) return json;

        *data = data_s;
        json = bsky_parse_json_bool(data, ec);
        if (*ec != bsky_ec_Json_expect_Bool) return json;

        *data = data_s;
        json = bsky_parse_json_num(data, ec);
        if (*ec != bsky_ec_Json_expect_Number) return json;

        *data = data_s;
        json = bsky_parse_json_str(data, ec);
        if (*ec != bsky_ec_Json_expect_OQ) return json;

        *data = data_s;
        json = bsky_parse_json_arr(data, ec);
        if (*ec != bsky_ec_Json_expect_OSB) return json;

        *data = data_s;
        json = bsky_parse_json_dct(data, ec);
        if (*ec != bsky_ec_Json_expect_OCB) return json;

        *ec = bsky_ec_Json_invalid_variant;

        return json;
    }

    /*
     * BSKY FUTURE
     */
    int bsky_fufure_poll(struct bsky_future self, void *data, void *result,
                         enum bsky_error_code *ec)
    {
        return self.poll(self.self, data, result, ec);
    }

    /*
     * BSKY CURL
     */
    struct bsky_http_response bsky_http_request(struct bsky_str server,
                                                struct bsky_view _headers,
                                                struct bsky_json request,
                                                enum bsky_error_code *ec)
    {
        struct bsky_http_response res = { 0 };
        struct bsky_str  req          = { 0 };
        struct bsky_str_builder sb    = { 0 };

        struct bsky_headers_view headers = { _headers.start, _headers.end };
        req = bsky_tmp_str_of_json(request);

        *ec = bsky_ec_Ok;

        struct curl_slist *headers_list = NULL;
        headers_list = curl_slist_append(headers_list,
                                    "Content-type: application/json");

        while (headers.end - headers.start > 0)
             headers_list = curl_slist_append(headers_list,
                                             (headers.start++)->start);
        
        CURL *curl = curl_easy_init();
        if (!curl) bsky_defer_ec(bsky_ec_Failed_to_init_curl);

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers_list);
        curl_easy_setopt(curl, CURLOPT_URL, server.start);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.start);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, bsky_sb_write);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sb);

        CURLcode ret = curl_easy_perform(curl);
        if (ret != 0) bsky_defer_ec(bsky_ec_Failed_to_perform_curl);

        struct bsky_str res_str = bsky_sb_build_tmp(&sb);
        res.json = bsky_parse_json(&res_str, ec);

        if (*ec != bsky_ec_Ok) bsky_defer_ec(*ec);

        curl_easy_cleanup(curl);

        ret = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res.code);
        if (ret != 0) bsky_defer_ec(bsky_ec_Failed_to_get_response_code);

    defer:
        bsky_da_free(&sb);
        return res;
    }

    struct __bsky_http_request_state {
        CURLM *multi; CURL *curl;
        struct bsky_str_builder sb;
        enum {
            __bsky_http_request_SEND,
            __bsky_http_request_RETURN,
        } state;
    };

    int __bsky_http_request_poll(void *_self, void *data, void *result,
                                 enum bsky_error_code* ec)
    {
        struct __bsky_http_request_state *self = _self;
        int ret = 0;

        switch (self->state) {
        case __bsky_http_request_SEND: {
            int still_running = 0;
            CURLMcode mc;

            mc = curl_multi_perform(self->multi, &still_running);
            if (mc) {
                ret = -1;
                bsky_defer_ec(bsky_ec_Failed_to_perform_curlm);
            }

            if (!still_running) self->state = __bsky_http_request_RETURN;

            if (data != NULL) {
                size_t ms = *(size_t*)data;
                mc = curl_multi_poll(self->multi, NULL, 0, ms, NULL);
                if (mc) {
                    ret = -1;
                    bsky_defer_ec(bsky_ec_Failed_to_poll_curlm);
                }
			}

            return 0; /* do not return with defer, cause don't wanna free
                         curl. */
        } break;
        case __bsky_http_request_RETURN: {
            struct bsky_http_response *res = result;
            struct bsky_str            str = bsky_sb_build_tmp(&self->sb);

            res->json = bsky_parse_json(&str, ec);
            if (*ec != bsky_ec_Ok) {
                ret = -1;
                bsky_defer_ec(*ec);
            }

            ret = curl_easy_getinfo(self->curl, CURLINFO_RESPONSE_CODE, 
                                    &res->code);
            if (ret != 0) {
                ret = -1;
                bsky_defer_ec(bsky_ec_Failed_to_get_response_code);
            }

            ret = 1;
        } break;
        }

    defer:
        curl_multi_remove_handle(self->multi, self->curl);
        curl_easy_cleanup(self->curl);
        curl_multi_cleanup(self->multi);

        return ret;
    }

    struct bsky_future bsky_http_request_async(struct bsky_str server,
                                               struct bsky_view _headers,
                                               struct bsky_json request,
                                               enum bsky_error_code* ec)
    {
        *ec = bsky_ec_Ok;

        struct bsky_future res = { 0 };

        struct __bsky_http_request_state *self = 
               bsky_tmp_alloc(sizeof(struct __bsky_http_request_state));

        *self = (struct __bsky_http_request_state) { 0 };
        self->state = __bsky_http_request_SEND;

        self->curl = curl_easy_init();
        if (!self->curl) bsky_defer_ec(bsky_ec_Failed_to_init_curl);

        self->multi = curl_multi_init();
        if (!self->multi) {
            curl_easy_cleanup(self->curl);
            bsky_defer_ec(bsky_ec_Failed_to_init_curlm);
        }
        curl_multi_add_handle(self->multi, self->curl);

        struct bsky_str req = { 0 };

        struct bsky_headers_view headers = { _headers.start, _headers.end };
        req = bsky_tmp_str_of_json(request);

        struct curl_slist *headers_list = NULL;
        headers_list = curl_slist_append(headers_list,
                                    "Content-type: application/json");

        while (headers.end - headers.start > 0)
             headers_list = curl_slist_append(headers_list,
                                             (headers.start++)->start);        

        curl_easy_setopt(self->curl, CURLOPT_HTTPHEADER, headers_list);
        curl_easy_setopt(self->curl, CURLOPT_URL, server.start);
        curl_easy_setopt(self->curl, CURLOPT_POSTFIELDS, req.start);
        curl_easy_setopt(self->curl, CURLOPT_WRITEFUNCTION, bsky_sb_write);
        curl_easy_setopt(self->curl, CURLOPT_WRITEDATA, &self->sb);

        res = (struct bsky_future) {
            .self = self,
            .poll = &__bsky_http_request_poll,
        };

    defer:
        return res;
    }


    /*
     * BSKY API TYPES
     */
    struct bsky_identifier bsky_mk_handle_tmp(char *_handle) {
        struct bsky_str_builder     sb = { 0 };
        struct bsky_identifier      id = { 0 };
        struct bsky_str         handle = { 0 };

        handle = bsky_mk_str(_handle);

        id.var = bsky_id_Handle;
        while (handle.start != handle.end && *handle.start != '.') {
            handle.start++;
        }

        if (handle.start == handle.end) {
            id.var = bsky_id_Invalid;
            return id;
        }

        bsky_sb_push_str(&sb, (struct bsky_str) { _handle, handle.start });
        id.handle.username = bsky_sb_build_tmp(&sb);

        bsky_sb_push_str(&sb,(struct bsky_str) { handle.start+1, handle.end });
        id.handle.server   = bsky_sb_build_tmp(&sb);

        return id;
    }

    /*
     * BSKY SESSION
     */
    struct bsky_crses_resp 
    bsky_create_session(struct bsky_identifier identifier,
                        struct bsky_str        password,
                        struct bsky_str        auth_factor,
                        enum bsky_error_code  *ec)
    {
        struct bsky_crses_resp  res = { 0 };

        return res;
    }

#endif

/**
 * If defined `BSKY_API_STRIP_PREFIX' will strip `bsky_' prefix from functions.
 */
#ifdef BSKY_API_STRIP_PREFIX
	/*
     * BSKY LOG
     */
    #define log(level, fmt, ...) bsky_log(level, fmt, __VA_ARGS__)

    #define bsky_log_None    bsky_log_None  
    #define bsky_log_Error   bsky_log_Error
    #define bsky_log_Warning bsky_log_Warning
    #define bsky_log_Info    bsky_log_Info

    /*
     * BSKY ERROR
     */
    #define ec_Ok                   bsky_ec_Ok
    #define ec_Tmp_overflow         bsky_ec_Tmp_overflow
    #define ec_Json_expect_CSB      bsky_ec_Json_expect_CSB
    #define ec_Json_expect_OSB      bsky_ec_Json_expect_OSB
    #define ec_Json_expect_CCB      bsky_ec_Json_expect_CCB
    #define ec_Json_expect_OCB      bsky_ec_Json_expect_OCB
    #define ec_Json_expect_Bool     bsky_ec_Json_expect_Bool
    #define ec_Json_expect_Null     bsky_ec_Json_expect_Null
    #define ec_Json_expect_Number   bsky_ec_Json_expect_Number
    #define ec_Json_expect_OQ       bsky_ec_Json_expect_OQ
    #define ec_Json_expect_CQ       bsky_ec_Json_expect_CQ
    #define ec_Json_expect_Colon    bsky_ec_Json_expect_Colon
    #define ec_Json_invalid_variant bsky_ec_Json_invalid_variant

    #define str_of_error_code(ec)     bsky_str_of_error_code(ec)
    #define log_error(ec)             bsky_log_error(ec)
    #define return_error(ec)          bsky_return_error(ec)
    #define return_error_v(ec_v, ret) bsky_return_error_v(ec_v, ret)
    #define defer_ec(ec)              bsky_defer_ec(ec)

    /*
     * BSKY TMP ARENA
     */
    #define tmp_alloc(size) bsky_tmp_alloc(size)
    #define default_tmp_reset() bsky_default_tmp_reset()

    /*
     * BSKY DYNAMIC ARRAY
     */
    #define da_free(da) bsky_da_free(da)
    #define __da_push(da, elem_size) __bsky_da_push(da, elem_size)
    #define da_push(da) bsky_da_push(da)
    #define __da_append(da, e, es, len) __bsky_da_append(da, e, es, len)
    #define bsky_da_append(da, e, es) bsky_da_append(da, e, es)
    #define clear_da(da) bsky_clear_da(da)
    #define da_free(da) bsky_da_free(da)

    /*
     * BSKY VIEW
     */
    #define __view_of_da(da, elem_size) __bsky_view_of_da(da, elem_size)
    #define view_of_da(da) bsky_view_of_da(da)
    #define view_to_tmp(view) bsky_view_to_tmp(view)
    #define __tmp_view_of_da(da, elem_size) __bsky_tmp_view_of_da(da,elem_size)
    #define tmp_view_of_da(da) bsky_tmp_view_of_da(da)

    /*
     * BSKY STRING
     */
    #define mk_str(cstr) bsky_mk_str(cstr)
    #define sb_push(sb, c) bsky_sb_push(sb, c)
    #define sb_push_str(sb, str) bsky_sb_push_str(sb, str)
    #define sb_push_fmt(sb, fmt, ... ) bsky_sb_push_fmt(sb, fmt, __VA_ARGS__)
    #define sb_build(sb) bsky_sb_build(sb)
    #define sb_build_tmp(sb) bsky_sb_build_tmp(sb)
    #define view_of_str(str) bsky_view_of_str(str)
    #define trim_left(str) bsky_trim_left(str)
    #define str_starts_with(fst, snd) bsky_str_starts_with(fst, snd)
    #define str_ends_with(fst, snd) bsky_str_ends_with(fst, snd)
    #define str_eq(fst, snd) bsky_str_eq(fst, snd)
    #define str_cmp(fst, snd) bsky_str_cmp(fst, snd)
    #define str_len(str) str_len(str)
    #define shift_str(str, n) bsky_shift_str(str, n)

    /*
     * BSKY JSON
     */
    #define json_Arr bsky_json_Arr
    #define json_Dct bsky_json_Dct
    #define json_Num bsky_json_Num
    #define json_Str bsky_json_Str
    #define json_Bool bsky_json_Bool
    #define json_Null bsky_json_Null

    #define tmp_str_of_json(json) bsky_tmp_str_of_json(json)
    #define sb_push_json(sb, json) bsky_sb_push_json(sb, json)
    #define parse_json(str, ec) bsky_parse_json(str, ec)

    #define parse_json_arr(str, ec) bsky_parse_json_arr(str, ec)
    #define parse_json_dct(str, ec) bsky_parse_json_dct(str, ec)
    #define parse_json_num(str, ec) bsky_parse_json_num(str, ec)
    #define parse_json_str(str, ec) bsky_parse_json_str(str, ec)
    #define parse_json_bool(str, ec) bsky_parse_json_bool(str, ec)
    #define parse_json_null(str, ec) bsky_parse_json_null(str, ec)

    #define Json      bsky_Json;
    #define Json_Pair bsky_Json_Pair;

#endif

#endif //GUARD
