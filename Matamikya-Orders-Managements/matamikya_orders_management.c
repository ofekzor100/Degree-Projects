//
// Created by illus on 30/11/2021.
//

#include "matamikya_orders_management.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct order_t
{
    unsigned int order_id;
    AmountSet products;
};

void freeProductElement (void* product_ptr)
{
    free(product_ptr);
}

void* copyProductElement(void* to_copy)
{
    if (!to_copy)
    {
        return NULL;
    }
    unsigned int* to_copy_cast=(unsigned int*)to_copy;
    unsigned int* newProduct=malloc(sizeof(unsigned int));
    if (!newProduct)
    {
        return NULL;
    }
    *newProduct=*to_copy_cast;
    return newProduct;
}

int compareProductElement (void* product1, void* product2)
{
    if (!product1 && !product2)
    {
        return 0;
    }
    if (!product1 && product2)
    {
        return -*(unsigned int*)product2;
    }
    if (product1 && !product2)
    {
        return *(unsigned int*)product1;
    }
    return ((int)(*(unsigned int*)product1) - (int)(*(unsigned int*)product2));
}

unsigned int getAvailableId(Set orders)
{
    if (!orders)
    {
        return ELEMENT_NOT_EXISTS;
    }
    Order iterator=setGetFirst(orders);
    if(!iterator)
    {
        return MINIMUM_ID;
    }
    assert(iterator);
    unsigned int result_id=iterator->order_id;
    while (iterator)
    {
        assert(iterator);
        result_id=iterator->order_id+1;
        iterator=setGetNext(orders);
    }
    return result_id;
}

void* copyOrder(void* to_copy)
{
    if(!to_copy)
    {
        return NULL;
    }
    Order newOrder=malloc(sizeof(*newOrder));
    if(!newOrder)
    {
        return NULL;
    }
    Order to_copy_cast=(Order)to_copy;
    newOrder->products=asCopy(to_copy_cast->products);
    if(!newOrder->products)
    {
        free(newOrder);
        return NULL;
    }
    newOrder->order_id=to_copy_cast->order_id;
    return newOrder;
}

void freeOrder(void* to_free)
{
    if (!to_free)
    {
        return;
    }
    Order to_free_cast=(Order)to_free;
    asDestroy(to_free_cast->products);
    free(to_free);
}

int compareOrders (void* order1, void* order2)
{
    if (!order1 && !order2)
    {
        return 0;
    }
    Order order1_cast=(Order)order1, order2_cast=(Order)order2;
    if(order1 && !order2)
    {
        return (int)order1_cast->order_id;
    }
    if (!order1 && order2)
    {
        return -(int)order2_cast->order_id;
    }
    assert(order1 && order2);
    return (int)(order1_cast->order_id - order2_cast->order_id);
}

Order createOrder(Set orders, AmountSet products)
{
    if(!products || !orders)
    {
        return NULL;
    }
    Order newOrder=malloc(sizeof(*newOrder));
    if(!newOrder)
    {
        return NULL;
    }
    newOrder->products=products;
    newOrder->order_id=getAvailableId(orders);
    return newOrder;
}

unsigned int getOrderId(Order order)
{
    return order ? order->order_id : ELEMENT_NOT_EXISTS;
}

AmountSet getProductsInOrder(Set orders,unsigned int order_id)
{
    if(!orders)
    {
        return NULL;
    }
    Order order=setGetFirst(orders);
    while (order && order_id != order->order_id)
    {
        order=setGetNext(orders);
    }
    return order ? order->products : NULL;
}

Order getOrderByOrderId(Set orders,unsigned int order_id)
{
    Order order= setGetFirst(orders);
    if(!order)
    {
        return NULL;
    }
    while (order_id != getOrderId(order))
    {
        order=setGetNext(orders);
        if (!order)
        {
            return NULL;
        }
    }
    assert(order);
    return order;
}