//
// Created by illus on 30/11/2021.
//

#ifndef SINGLE_ORDER_AMOUNT_SET_ORDERS_MANAGEMENT_H
#define SINGLE_ORDER_AMOUNT_SET_ORDERS_MANAGEMENT_H

#include <stdlib.h>
#include "amount_set.h"
#include <assert.h>
#include "set.h"
#ifndef ELEMENT_NOT_EXISTS
#define ELEMENT_NOT_EXISTS 0
#define MINIMUM_ID 1
#endif

/** Type for defining the order database */
typedef struct order_t *Order;

/** Free one product (element) from the order */
void freeProductElement (void* product_ptr);


/** Returns a copy of the input product element
 *
 * @param to_copy - the product element you want to copy
 * @return a pointer to the new copied product element
 */
void* copyProductElement(void* to_copy);

/** Returns a positive number if the first product is greater than the second.
 *
 * @param string1 - First product
 * @param string2 - Second product
 * @return A positive number if product1 is greater, negative if product2 is greater or 0 if equal
 */
int compareProductElement (void* product1, void* product2);

/** Create the order Id.
 * Input:
 * the set of all orders
 * Output: orderId, if set is NULL returns 0*/
unsigned int getAvailableId(Set set);

/** Create a new order, if unsuccessful NULL is returned
 *
 * @param orders - the set of all orders
 * @param products - an amount set of products the order will use
 * @return a new order, if unsuccessful NULL is returned
 */
Order createOrder(Set orders, AmountSet products);

/** Copies an order
 * Inputs: The order you wish to copy (to_copy)
 * Output: The exact same order as to_copy
 * */
void* copyOrder(void* to_copy);

/**Free a single order (element of the set
 * @param to_free - the order you wish to remove
 */
void freeOrder(void* to_free);


/** Delivers the products pointer in an order
 *
 * @param orders - All orders there are
 * @param order_id - The id of the order you wish to receive it's products
 * @return Pointer to the products (AmountSet), or NULL if there is an error (orders doesn't exist or there isn't an order with such ID)
 */
AmountSet getProductsInOrder(Set orders,unsigned int order_id);

/** compare two orders inside the set
 * @param order1 - If it has a greater ID than order 2, then order1's order ID will be returned
 * @param order2 - If it has a greater ID than order 1, then minus of order1's order ID will be returned
 * @return a positive number if order1 has a greater ID, a negative number if order2 has a greater ID, and 0 if they are equal
 */
int compareOrders (void* order1, void* order2);

/** Fetch an input order's Id
 *
 * @param order - input order
 * @return the ID of the order, if the order is empty return 0
 */
unsigned int getOrderId(Order order);

/** Delivers the order pointer given the order id
 * @param orders - All orders there are
 * @param order_id - The id of the order you wish to receive it's pointer
 * @return Pointer to the order (*order), or NULL if there is an error (orders doesn't exist or there isn't an order with such ID)
 */
Order getOrderByOrderId(Set orders,unsigned int order_id);

/** Free an order structure
 * @param order - the order which needs to be freed
 */
void freeOrder(void* order);

#endif //SINGLE_ORDER_AMOUNT_SET_ORDERS_MANAGEMENT_H
