#include <stdio.h>
#include "amount_set_str_tests.h"

int main() {
    RUN_TEST(testAsCreate);
    RUN_TEST(testAsDestroy);
    RUN_TEST(testAsRegister);
    RUN_TEST(testAsDelete);
    RUN_TEST(testAsChangeAmountAndGetAmount);
    RUN_TEST(testAsGetSize);
    RUN_TEST(testAsContains);
    RUN_TEST(testAsGetFirst);
    RUN_TEST(testAsGetNext);
    RUN_TEST(testAsClear);
    RUN_TEST(testAsCopy);
    return 0;
}
