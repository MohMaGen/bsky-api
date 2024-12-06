#define IMPLEMENT_TESTS
#define BSKY_API_IMPLEMENTATION

#include "json-tests.h"
#include "string-tests.h"

#include <unity.h>

void setUp (void) {} 
void tearDown (void) {
    bsky_default_tmp_reset();
} 

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    run_json_tests();

    run_string_tests();


	return UNITY_END();
}


