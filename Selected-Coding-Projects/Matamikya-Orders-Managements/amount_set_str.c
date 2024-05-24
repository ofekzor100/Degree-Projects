#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "amount_set_str.h"
#define STR_SIZE(string) sizeof(char)*(strlen(string)+1)

struct AmountSet_t
{
    char* name;
    double amount;
    AmountSet next;
    AmountSet iterator;
};

AmountSet asCreate()
{
    AmountSet set=malloc(sizeof(*set));
    if(!set)
    {
        return NULL;
    }
    set->next=NULL;
    set->amount=0;
    set->name=NULL;
    set->iterator=NULL;
    return set;
}

void asDestroy(AmountSet set)
{
    if (!set)
    {
        return;
    }
    AmountSet temp_set;
    while(set)
    {
        temp_set=set;
        set=set->next;
        free(temp_set->name);
        free(temp_set);
    }
}

AmountSet asCopy(AmountSet set)
{
    if (!set)
    {
        return NULL;
    }
    AmountSet new_set=asCreate();
    if(!new_set)
    {
        return NULL;
    }
    new_set->amount=set->amount;
    new_set->iterator=new_set;
    char* first_element=asGetFirst(set);
    new_set->name=malloc(STR_SIZE(first_element));
    if (!new_set->name)
    {
        asDestroy(new_set);
        return NULL;
    }
    strcpy(new_set->name,first_element);
    while(set->iterator)
    {
        AmountSet new_node=malloc(sizeof(*new_node));
        if (!new_node)
        {
            asDestroy(new_set);
            return NULL;
        }
        new_node->amount=set->iterator->amount;
        char* next_name=asGetNext(set);
        new_node->name=malloc(STR_SIZE(next_name));
        if (!new_node->name)
        {
            free(new_node);
            asDestroy(new_set);
            return NULL;
        }
        strcpy(new_node->name,next_name);
        assert(new_set->iterator);
        new_set->iterator->next=new_node;
        new_set->iterator=new_set->iterator->next;
    }
    new_set->iterator->next=NULL;
    return new_set;
}

int asGetSize(AmountSet set)
{
    if (!set)
    {
        return -1;
    }
    int count_size_of_set=0;
    AmountSet temp_set=set;
    while(temp_set && temp_set->name)
    {
        temp_set=temp_set->next;
        count_size_of_set++;
    }
    return count_size_of_set;
}

bool asContains(AmountSet set, const char* element)
{
    if (!set || !element)
    {
        return false;
    }
    AmountSet temp_set=set;
    while(temp_set)
    {
        if(temp_set->name && strcmp(temp_set->name,element)==0)
        {
            return true;
        }
        temp_set=temp_set->next;
    }
    return false;
}

AmountSetResult asGetAmount(AmountSet set,const char* element, double *outAmount)
{
    if (!set || !element || !outAmount)
    {
        return AS_NULL_ARGUMENT;
    }
    if (!set->name)
    {
        return AS_ITEM_DOES_NOT_EXIST;
    }
    AmountSet temp_set=set;
    while(temp_set)
    {
        if(strcmp(temp_set->name,element)==0)
        {
            *outAmount=temp_set->amount;
            return AS_SUCCESS;
        }
        temp_set=temp_set->next;
    }
    return AS_ITEM_DOES_NOT_EXIST;
}

AmountSetResult asRegister(AmountSet set,const char* element)
{
    if (!set || !element)
    {
        return AS_NULL_ARGUMENT;
    }
    if (asContains(set, element))
    {
        return AS_ITEM_ALREADY_EXISTS;
    }
    if (!set->name)
    {
        set->name=malloc(STR_SIZE(element));
        if (!set->name)
        {
            return AS_OUT_OF_MEMORY;
        }
        strcpy(set->name,element);
        return AS_SUCCESS;
    }
    assert(set);
    set->iterator=set;
    AmountSet previous_element=NULL;
    while(set->iterator && (strcmp(set->iterator->name,element)<0))
    {
        previous_element=set->iterator;
        assert(set->iterator);
        set->iterator=set->iterator->next;
    }
    if(!previous_element)
    {
        AmountSet set_copy=asCopy(set);
        if(!set_copy)
        {
            return AS_OUT_OF_MEMORY;
        }
        AmountSetResult result=asClear(set);
        if (result==AS_NULL_ARGUMENT)
        {
            asDestroy(set_copy);
            return result;
        }
        set->name=malloc(STR_SIZE(element));
        if (!set->name)
        {
            asDestroy(set_copy);
            return AS_OUT_OF_MEMORY;
        }
        strcpy(set->name,element);
        set->amount=0;
        set->next=set_copy;
        return AS_SUCCESS;
    }
    AmountSet new_node=malloc(sizeof(*new_node));
    if (!new_node)
    {
        return AS_OUT_OF_MEMORY;
    }
    new_node->name=malloc(STR_SIZE(element));
    if (!new_node->name)
    {
        free(new_node);
        return AS_OUT_OF_MEMORY;
    }
    strcpy(new_node->name,element);
    new_node->amount=0;
    assert(previous_element);
    previous_element->next=new_node;
    new_node->next=set->iterator;
    return AS_SUCCESS;
}

AmountSetResult asChangeAmount(AmountSet set, const char* element, double amount)
{
    if (!set || !element)
    {
        return AS_NULL_ARGUMENT;
    }
    if (!set->name)
    {
        return AS_ITEM_DOES_NOT_EXIST;
    }
    AmountSet temp_set=set;
    while (temp_set)
    {
        if (strcmp(temp_set->name,element)==0)
        {
            if(temp_set->amount + amount < 0)
            {
                return AS_INSUFFICIENT_AMOUNT;
            }
            temp_set->amount+=amount;
            return AS_SUCCESS;
        }
        temp_set=temp_set->next;
    }
    return AS_ITEM_DOES_NOT_EXIST;
}


AmountSetResult asDelete(AmountSet set, const char* element)
{
    if(!element || !set)
    {
        return AS_NULL_ARGUMENT;
    }
    set->iterator=set;
    AmountSet previous_element=NULL;
    while(set->iterator)
    {
        if (strcmp(set->iterator->name, element)==0)
        {
            if (set->iterator->next)
            {
                AmountSet next_element=set->iterator->next;
                free(set->iterator->name);
                set->iterator->name=malloc(STR_SIZE(next_element->name));
                if (!set->iterator->name)
                {
                    return AS_OUT_OF_MEMORY;
                }
                strcpy(set->iterator->name,next_element->name);
                free(next_element->name);
                set->iterator->amount=next_element->amount;
                set->iterator->next=next_element->next;
                free(next_element);
                return AS_SUCCESS;
            }
            if (previous_element)
            {
                free(set->iterator->name);
                free(set->iterator);
                previous_element->next=NULL;
                return AS_SUCCESS;
            }
            else
            {
                asClear(set);
                return AS_SUCCESS;
            }
        }
        previous_element=set->iterator;
        set->iterator=set->iterator->next;
    }
    return AS_ITEM_DOES_NOT_EXIST;
}


AmountSetResult asClear(AmountSet set)
{
    if (!set)
    {
        return AS_NULL_ARGUMENT;
    }
    set->amount=0;
    free(set->name);
    set->name=NULL;
    set->iterator=set->next;
    while (set->iterator)
    {
        AmountSet set_to_free=set->iterator;
        set->iterator=set->iterator->next;
        free(set_to_free->name);
        free(set_to_free);
    }
    set->next=NULL;
    return AS_SUCCESS;
}

char* asGetFirst(AmountSet set)
{
    if (!set)
    {
        return NULL;
    }
    set->iterator=set;
    return asGetNext(set);
}

char* asGetNext(AmountSet set)
{
    if (!set || !set->iterator)
    {
        return NULL;
    }
    char* next_element=set->iterator->name;
    set->iterator=set->iterator->next;
    return next_element;
}