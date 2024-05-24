//
// Created by illus on 15/12/2021.
//

#include "amount_set_str.h"
#include <stdbool.h>
#include <string.h>
#define ALMOST_ZERO 0.000001
#define MAX_NAME_SIZE 20

#define ASSERT_TEST_WITH_FREE(expr)                                      \
    do {                                                                          \
        if (!(expr)) {                                                            \
            printf("\nAssertion failed at %s:%d %s \n", __FILE__, __LINE__, #expr); \
            asDestroy(set);                                                              \
            return false;                                                         \
        }                                                                         \
    } while (0)

#define TEST_ADD(to_add, num_of_strings) \
    if(asRegister(set,to_add)!=AS_SUCCESS) \
        return false; \
    double expected_amounts[num_of_strings]={0}; \
    return (compareSetWithExpected(set,expected_names,num_of_strings,expected_amounts));

#define TEST_DELETE(to_delete, num_of_strings) \
        if (asDelete(set,to_delete)!=AS_SUCCESS)\
            return false;\
        double expected_amounts[num_of_strings]={0};\
        return compareSetWithExpected(set,expected_names,num_of_strings,expected_amounts);

#define TEST_CHANGE_AMOUNT(to_change,amount,expected_change_result) \
        if (asChangeAmount(set,to_change,amount)!=expected_change_result)\
            return false;\
        char expected_elements[4][MAX_NAME_SIZE]={"Box","Bull","Chair","Door"};\
        return compareSetWithExpected(set,expected_elements,4,expected_amounts);\


#define TEST_COMPLETED \
        asDestroy(set);\
        return true;

bool testAsCreate()
{
    AmountSet set=asCreate();
    ASSERT_TEST_WITH_FREE(set!=NULL);
    TEST_COMPLETED;
}

bool testAsDestroy()
{
    AmountSet set=asCreate();
    ASSERT_TEST_WITH_FREE(set!=NULL);
    asDestroy(set);
    set=asCreate();
    ASSERT_TEST_WITH_FREE(set!=NULL);
    TEST_COMPLETED;
}

static bool compareSetWithExpected(AmountSet set, char names[][MAX_NAME_SIZE], int num_of_strings, double expected_amounts[])
{
    double current_amount;
    char* current_element=asGetFirst(set);
    int i;
    for (i=0; current_element && i<num_of_strings; i++)
    {
        if(asGetAmount(set,current_element,&current_amount)!=AS_SUCCESS)
            return false;
        if(strcmp(current_element, names[i])!=0)
            return false;
        if(!(current_amount < expected_amounts[i] + ALMOST_ZERO && current_amount > expected_amounts[i] - ALMOST_ZERO))
            return false;
        current_element=asGetNext(set);
    }
    if(i<num_of_strings-1)
        return false;
    return true;
}



static bool testAddAtEnd(AmountSet set)
{
    char expected_names[4][MAX_NAME_SIZE]={"Box","Bull","Chair","Door"};
    TEST_ADD("Door",4);
}

static bool testAddSpecialChars(AmountSet set)
{
    char expected_names[5][MAX_NAME_SIZE]={"Box","Bull","Bz53+$@#^%","Chair","Door"};
    TEST_ADD("Bz53+$@#^%",5);
}

static bool testAddAtMiddle(AmountSet set)
{
    char expected_names[3][MAX_NAME_SIZE]={"Box","Bull","Chair"};
    TEST_ADD("Bull",3);
}

static bool testAddFirst(AmountSet set)
{
    char expected_names[2][MAX_NAME_SIZE]={"Box","Chair"};
    TEST_ADD("Box",2);
}

static bool testAsRegisterSanity(AmountSet set)
{
    return asRegister(set,"Chair")==AS_SUCCESS;
}

bool testAsRegister()
{
    AmountSet set=NULL;
    ASSERT_TEST_WITH_FREE(asRegister(set,"Chair")==AS_NULL_ARGUMENT);
    set=asCreate();
    ASSERT_TEST_WITH_FREE(set!=NULL);
    ASSERT_TEST_WITH_FREE(asRegister(set,NULL)==AS_NULL_ARGUMENT);
    ASSERT_TEST_WITH_FREE(testAsRegisterSanity(set));
    ASSERT_TEST_WITH_FREE(testAddFirst(set));
    ASSERT_TEST_WITH_FREE(testAddAtMiddle(set));
    ASSERT_TEST_WITH_FREE(testAddAtEnd(set));
    ASSERT_TEST_WITH_FREE(testAddSpecialChars(set));
    TEST_COMPLETED;
}

static bool createTestSet(AmountSet set)
{
    if(asRegister(set,"Box")!=AS_SUCCESS)
        return false;
    if(asRegister(set,"Bull")!=AS_SUCCESS)
        return false;
    if(asRegister(set,"Chair")!=AS_SUCCESS)
        return false;
    if(asRegister(set,"Door")!=AS_SUCCESS)
        return false;
    return true;
}

static bool testDeleteFromMiddle(AmountSet set)
{
    char expected_names[3][MAX_NAME_SIZE]={"Box","Bull","Door"};
    TEST_DELETE("Chair",3);
}

static bool testDeleteFromEnd(AmountSet set)
{
    char expected_names[2][MAX_NAME_SIZE]={"Box","Bull"};
    TEST_DELETE("Door",2);
}

static bool testDeleteBeginning(AmountSet set)
{
    char expected_names[1][MAX_NAME_SIZE]={"Bull"};
    TEST_DELETE("Box",1);
}

static bool testDeleteLastRemaining(AmountSet set)
{
    if(asDelete(set,"Bull")!=AS_SUCCESS)
        return false;
    if (asGetFirst(set)!=NULL)
        return false;
    return true;
}

bool testAsDelete()
{
    AmountSet set=NULL;
    ASSERT_TEST_WITH_FREE(asDelete(set,"Box")==AS_NULL_ARGUMENT);
    set=asCreate();
    ASSERT_TEST_WITH_FREE(set!=NULL);
    ASSERT_TEST_WITH_FREE(asDelete(set,NULL)==AS_NULL_ARGUMENT);
    ASSERT_TEST_WITH_FREE(createTestSet(set));
    ASSERT_TEST_WITH_FREE(testDeleteFromMiddle(set));
    ASSERT_TEST_WITH_FREE(testDeleteFromEnd(set));
    ASSERT_TEST_WITH_FREE(testDeleteBeginning(set));
    ASSERT_TEST_WITH_FREE(testDeleteLastRemaining(set));
    TEST_COMPLETED;
}

static bool testChangeAmountAtMiddle (AmountSet set)
{
    double expected_amounts[4]={0,0,3,0};
    TEST_CHANGE_AMOUNT("Chair",3,AS_SUCCESS);
}

static bool testChangeAmountAtBeginning (AmountSet set)
{
    double expected_amounts[4]={10.55,0,3,0};
    TEST_CHANGE_AMOUNT("Box",10.55,AS_SUCCESS);
}

static bool testChangeAmountAtEnd (AmountSet set)
{
    double expected_amounts[4]={10.55,0,3,1};
    TEST_CHANGE_AMOUNT("Door",1,AS_SUCCESS);
}

static bool testReduceMoreThanPossibleSimple(AmountSet set)
{
    double expected_amounts[4]={10.55,0,3,1};
    TEST_CHANGE_AMOUNT("Door",-2,AS_INSUFFICIENT_AMOUNT);
}

static bool testReduceMoreThanPossibleComplicated(AmountSet set)
{
    double expected_amounts[4]={10.55,0,3,1};
    TEST_CHANGE_AMOUNT("Box",-10.56,AS_INSUFFICIENT_AMOUNT);
}

static bool testChangeAmountOfNoneExisting(AmountSet set)
{
    double expected_amounts[4]={10.55,0,3,1};
    TEST_CHANGE_AMOUNT("Test",3,AS_ITEM_DOES_NOT_EXIST);
}

static bool testChangeAmountBig(AmountSet set)
{
    double expected_amounts[4]={10.55,0,3414.592653,1};
    TEST_CHANGE_AMOUNT("Chair",3411.592653,AS_SUCCESS);
}

bool testAsChangeAmountAndGetAmount()
{
    AmountSet set=NULL;
    ASSERT_TEST_WITH_FREE(asChangeAmount(set,"Box",3)==AS_NULL_ARGUMENT);
    set=asCreate();
    ASSERT_TEST_WITH_FREE(set!=NULL);
    ASSERT_TEST_WITH_FREE(asChangeAmount(set,NULL,3)==AS_NULL_ARGUMENT);
    ASSERT_TEST_WITH_FREE(createTestSet(set));
    ASSERT_TEST_WITH_FREE(testChangeAmountAtMiddle(set));
    ASSERT_TEST_WITH_FREE(testChangeAmountAtBeginning(set));
    ASSERT_TEST_WITH_FREE(testChangeAmountAtEnd(set));
    ASSERT_TEST_WITH_FREE(testReduceMoreThanPossibleSimple(set));
    ASSERT_TEST_WITH_FREE(testReduceMoreThanPossibleComplicated(set));
    ASSERT_TEST_WITH_FREE(testChangeAmountOfNoneExisting(set));
    ASSERT_TEST_WITH_FREE(testChangeAmountBig(set));
    TEST_COMPLETED;
}

bool testAsGetSize()
{
    AmountSet set=NULL;
    ASSERT_TEST_WITH_FREE(asGetSize(set)==-1);
    set=asCreate();
    ASSERT_TEST_WITH_FREE(set!=NULL);
    ASSERT_TEST_WITH_FREE(asGetSize(set)<ALMOST_ZERO && asGetSize(set)>-ALMOST_ZERO);
    ASSERT_TEST_WITH_FREE(asRegister(set,"New")==AS_SUCCESS);
    ASSERT_TEST_WITH_FREE(asGetSize(set)==1);
    ASSERT_TEST_WITH_FREE(createTestSet(set));
    ASSERT_TEST_WITH_FREE(asGetSize(set)==5);
    TEST_COMPLETED;
}

bool testAsContains()
{
    AmountSet set=NULL;
    ASSERT_TEST_WITH_FREE(asContains(set,"Box")==false);
    set=asCreate();
    ASSERT_TEST_WITH_FREE(set!=NULL);
    ASSERT_TEST_WITH_FREE(asContains(set,NULL)==false);
    ASSERT_TEST_WITH_FREE(createTestSet(set));
    ASSERT_TEST_WITH_FREE(asContains(set,"Box")==true);
    ASSERT_TEST_WITH_FREE(asContains(set,"Bull")==true);
    ASSERT_TEST_WITH_FREE(asContains(set,"Chair")==true);
    ASSERT_TEST_WITH_FREE(asContains(set,"Door")==true);
    ASSERT_TEST_WITH_FREE(asContains(set,"Test")==false);
    ASSERT_TEST_WITH_FREE(asRegister(set,"CRAZY%$&@#")==AS_SUCCESS);
    ASSERT_TEST_WITH_FREE(asContains(set,"CRAZY%$&@#")==true);
    TEST_COMPLETED;
}

bool testAsGetFirst()
{
    AmountSet set=NULL;
    ASSERT_TEST_WITH_FREE(asGetFirst(set)==NULL);
    set=asCreate();
    ASSERT_TEST_WITH_FREE(set!=NULL);
    ASSERT_TEST_WITH_FREE(createTestSet(set));
    ASSERT_TEST_WITH_FREE(strcmp(asGetFirst(set),"Box")==0);
    TEST_COMPLETED;
}

bool testAsGetNext()
{
    AmountSet set=NULL;
    ASSERT_TEST_WITH_FREE(asGetNext(set)==NULL);
    set=asCreate();
    ASSERT_TEST_WITH_FREE(set!=NULL);
    ASSERT_TEST_WITH_FREE(createTestSet(set));
    ASSERT_TEST_WITH_FREE(strcmp(asGetFirst(set),"Box")==0);
    ASSERT_TEST_WITH_FREE(strcmp(asGetNext(set),"Bull")==0);
    ASSERT_TEST_WITH_FREE(strcmp(asGetNext(set),"Chair")==0);
    ASSERT_TEST_WITH_FREE(strcmp(asGetNext(set),"Door")==0);
    ASSERT_TEST_WITH_FREE(asGetNext(set)==NULL);
    TEST_COMPLETED;
}

bool testAsClear()
{
    AmountSet set=NULL;
    asClear(set);
    ASSERT_TEST_WITH_FREE(set==NULL);
    set=asCreate();
    ASSERT_TEST_WITH_FREE(set!=NULL);
    ASSERT_TEST_WITH_FREE(createTestSet(set));
    char expected_elements[4][MAX_NAME_SIZE]={"Box","Bull","Chair","Door"};
    double expected_amounts[4]={0};
    ASSERT_TEST_WITH_FREE(compareSetWithExpected(set,expected_elements,4,expected_amounts));
    asClear(set);
    ASSERT_TEST_WITH_FREE(asGetFirst(set)==NULL);
    TEST_COMPLETED;
}

bool testAsCopy()
{
    AmountSet set=NULL;
    ASSERT_TEST_WITH_FREE(asCopy(set)==NULL);
    set=asCreate();
    ASSERT_TEST_WITH_FREE(createTestSet(set));
    ASSERT_TEST_WITH_FREE(asChangeAmount(set,"Box",10.55)==AS_SUCCESS);
    ASSERT_TEST_WITH_FREE(asChangeAmount(set,"Chair",3414.592653)==AS_SUCCESS);
    ASSERT_TEST_WITH_FREE(asChangeAmount(set,"Door",1)==AS_SUCCESS);
    AmountSet set_copy=asCopy(set);
    ASSERT_TEST_WITH_FREE(set_copy!=NULL);
    char expected_elements[4][MAX_NAME_SIZE]={"Box","Bull","Chair","Door"};
    double expected_amounts[4]={10.55,0,3414.592653,1};
    ASSERT_TEST_WITH_FREE(compareSetWithExpected(set_copy,expected_elements,4,expected_amounts));
    asDestroy(set_copy);
    TEST_COMPLETED;
}