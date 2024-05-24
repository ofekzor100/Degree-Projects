//
// Created by illus on 15/12/2021.
//

#ifndef AMOUNT_SET_STR_MAIN_AMOUNT_SET_STR_TESTS_H
#define AMOUNT_SET_STR_MAIN_AMOUNT_SET_STR_TESTS_H
#include <stdbool.h>

/**
 * Macro used for running a test from the main function
 */
#define RUN_TEST(test)                   \
    do {                                 \
        printf("Running " #test "... "); \
        if (test()) {                    \
            printf("[OK]\n");            \
        } else {                         \
            printf("[Failed]\n");        \
        }                                \
    } while (0)


bool testAsCreate();

bool testAsDestroy();

bool testAsRegister();

bool testAsDelete();

bool testAsChangeAmountAndGetAmount();

bool testAsGetSize();

bool testAsContains();

bool testAsGetFirst();

bool testAsGetNext();

bool testAsClear();

bool testAsCopy();

#endif //AMOUNT_SET_STR_MAIN_AMOUNT_SET_STR_TESTS_H
