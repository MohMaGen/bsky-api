#define IMPLEMENT_TESTS
#define BSKY_API_IMPLEMENTATION
#include "json-tests.h"

#include <unity.h>

void setUp (void) {} 
void tearDown (void) {} 

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    run_json_tests();

	return UNITY_END();
}


