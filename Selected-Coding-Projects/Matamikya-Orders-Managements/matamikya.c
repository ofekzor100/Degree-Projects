#include <stdio.h>
#include "matamikya.h"
#include "set.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "amount_set.h"
#include "matamikya_orders_management.h"
#include <math.h>
#include "matamikya_print.h"
#include "list.h"
#define STR_SIZE(string) sizeof(char)*(strlen(string)+1)
#define ALMOST_ZERO 0.0000001
#define PRECISION 0.001
#define CHECK_RESULT_AND_ROLLBACK_IF_NEEDED(result) \
        if (result!=MATAMIKYA_SUCCESS)\
            {\
                rollBackShipmentFromWarehouse(matamikya,orderId,products_in_order,current_product_id);\
                return result;\
            }


#ifndef ELEMENT_NOT_EXISTS
#define ELEMENT_NOT_EXISTS 0
#endif

typedef struct MtmProduct_t
{
    char* name;
    unsigned int id;
    MtmProductData additional_data;
    MtmCopyData copy_additional_data;
    MtmFreeData free_additional_data;
    MtmGetProductPrice get_product_price;
    MatamikyaAmountType scale;
    double quantity;
    double profit;
} *MtmProduct;

struct Matamikya_t
{
    List warehouse;
    Set orders;
};

static MatamikyaResult changeProductAttributes(MtmProduct toChange, const unsigned int id, const char *name,
                                               const double amount, const MatamikyaAmountType amountType,
                                               const MtmProductData customData, MtmCopyData copyData,
                                               MtmFreeData freeData, MtmGetProductPrice prodPrice, double profit)
{
    toChange->name=malloc(STR_SIZE(name));
    if (!toChange->name)
    {
        return MATAMIKYA_OUT_OF_MEMORY;
    }
    if(name)
    {
        strcpy(toChange->name,name);
    }
    else
    {
        toChange->name=NULL;
    }
    toChange->id=id;
    toChange->quantity=amount;
    toChange->scale=amountType;
    toChange->additional_data=customData;
    toChange->copy_additional_data=copyData;
    toChange->free_additional_data=freeData;
    toChange->get_product_price=prodPrice;
    toChange->profit=profit;
    return MATAMIKYA_SUCCESS;
}

void* copyMtmProduct(void* to_copy)
{
    if (!to_copy)
    {
        return NULL;
    }
    MtmProduct to_copy_cast=(MtmProduct)to_copy;
    MtmProduct newProduct=malloc(sizeof(*newProduct));
    if (!newProduct)
    {
        return NULL;
    }
    MtmProductData copy_custom_data=to_copy_cast->copy_additional_data(to_copy_cast->additional_data);
    MatamikyaResult change_attributes_result=changeProductAttributes(newProduct,to_copy_cast->id,to_copy_cast->name,to_copy_cast->quantity,
                            to_copy_cast->scale,copy_custom_data,to_copy_cast->copy_additional_data,
                            to_copy_cast->free_additional_data, to_copy_cast->get_product_price,to_copy_cast->profit);
    if (change_attributes_result!=MATAMIKYA_SUCCESS)
    {
        free(newProduct);
        return NULL;
    }
    return newProduct;
}

void freeMtmProduct(void* to_free)
{

    MtmProduct to_free_cast=(MtmProduct)to_free;
    if (!to_free_cast)
    {
        return;
    }
    free(to_free_cast->name);
    to_free_cast->free_additional_data(to_free_cast->additional_data);
    free(to_free_cast);
}

Matamikya matamikyaCreate()
{
    Matamikya matamikya=malloc(sizeof(*matamikya));
    if(!matamikya)
    {
        return NULL;
    }
    matamikya->orders=setCreate(copyOrder, freeOrder, compareOrders);
    if(!matamikya->orders)
    {
        free(matamikya);
        return NULL;
    }
    matamikya->warehouse=listCreate(copyMtmProduct, freeMtmProduct);
    if(!matamikya->warehouse)
    {
        setDestroy(matamikya->orders);
        free(matamikya);
        return NULL;
    }
    return matamikya;
}

void matamikyaDestroy(Matamikya matamikya)
{
    listDestroy(matamikya->warehouse);
    setDestroy(matamikya->orders);
    free(matamikya);
}

static bool isProductInWarehouse (Matamikya matamikya, const unsigned int id)
{
    if (!matamikya->warehouse)
    {
        return false;
    }
    MtmProduct product=listGetFirst(matamikya->warehouse);
    while(product)
    {
        if(product->id == id)
        {
            return true;
        }
        product=listGetNext(matamikya->warehouse);
    }
    return false;
}

static bool isValidName (const char* name)
{
    assert(name);
    if ((name[0]<'a' || name[0]>'z') && (name[0]<'A' || name[0]>'Z') && (name[0] < '0' || name[0]>'9'))
    {
        return false;
    }
    return true;
}

static bool isAmountValid (MatamikyaAmountType scaleToCheck,const double amount)
{
    assert(scaleToCheck==MATAMIKYA_INTEGER_AMOUNT || scaleToCheck==MATAMIKYA_HALF_INTEGER_AMOUNT || scaleToCheck==MATAMIKYA_ANY_AMOUNT);
    if((fabs(amount - round(amount)) > PRECISION) &&
       (scaleToCheck==MATAMIKYA_INTEGER_AMOUNT))
    {
        return false;
    }
    if(((fabs(amount*2 - round(amount*2)) > PRECISION*2))
       && (scaleToCheck==MATAMIKYA_HALF_INTEGER_AMOUNT))
    {
        return false;
    }
    return true;
}

static int compareMtmProducts (void* product1, void* product2)
{
    if (!product1 && !product2)
    {
        return 0;
    }
    MtmProduct product1_cast=(MtmProduct)product1,
            product2_cast=(MtmProduct)product2;
    if (!product1)
    {
        return (int)product2_cast->id;
    }
    if (!product2)
    {
        return -((int)product1_cast->id);
    }
    return (int)((int)product1_cast->id - (int)product2_cast->id);
}

MatamikyaResult  mtmNewProduct(Matamikya matamikya, const unsigned int id, const char *name,
const double amount, const MatamikyaAmountType amountType,
const MtmProductData customData, MtmCopyData copyData,
MtmFreeData freeData, MtmGetProductPrice prodPrice)
{
    if (!matamikya || !name || !customData || !copyData || !freeData || !prodPrice || !matamikya->warehouse)
    {
        return MATAMIKYA_NULL_ARGUMENT;
    }
    if (!isValidName(name))
    {
        return MATAMIKYA_INVALID_NAME;
    }
    if (amount<0 || !isAmountValid(amountType, amount))
    {
        return MATAMIKYA_INVALID_AMOUNT;
    }
    if (isProductInWarehouse(matamikya, id))
    {
        return MATAMIKYA_PRODUCT_ALREADY_EXIST;
    }
    MtmProduct new_product=malloc(sizeof(*new_product));
    if (!new_product)
    {
        return MATAMIKYA_OUT_OF_MEMORY;
    }
    MatamikyaResult change_attributes_result=changeProductAttributes(new_product, id, name,
                            amount, amountType,customData, copyData,freeData, prodPrice,0);
    if (change_attributes_result!=MATAMIKYA_SUCCESS)
    {
        free(new_product);
        return change_attributes_result;
    }
    ListResult list_result=listInsertFirst(matamikya->warehouse, new_product);
    if (list_result==LIST_OUT_OF_MEMORY)
    {
        freeMtmProduct(new_product);
        return MATAMIKYA_OUT_OF_MEMORY;
    }
    ListResult list_sort_result=listSort(matamikya->warehouse,compareMtmProducts);
    if (list_sort_result==LIST_OUT_OF_MEMORY)
    {
        MtmProduct set_list_to_first_element=listGetFirst(matamikya->warehouse);
        if (!set_list_to_first_element)
        {
            freeMtmProduct(new_product);
            return MATAMIKYA_NULL_ARGUMENT;
        }
        list_result=listRemoveCurrent(matamikya->warehouse);
        if (list_result!=LIST_SUCCESS)
        {
            freeMtmProduct(new_product);
            return MATAMIKYA_NULL_ARGUMENT;
        }
        freeMtmProduct(new_product);
        return MATAMIKYA_OUT_OF_MEMORY;
    }
    free(new_product->name);
    free(new_product);
    return MATAMIKYA_SUCCESS;
}

MatamikyaResult mtmChangeProductAmount(Matamikya matamikya, const unsigned int id, const double amount)
{
    if(!matamikya || !matamikya->warehouse)
    {
        return MATAMIKYA_NULL_ARGUMENT;
    }
    MtmProduct product=listGetFirst(matamikya->warehouse);
    while(product && product->id!=id)
    {
        product=listGetNext(matamikya->warehouse);
    }
    if(!product)
    {
        return MATAMIKYA_PRODUCT_NOT_EXIST;
    }
    if(!isAmountValid (product->scale,amount))
    {
        return MATAMIKYA_INVALID_AMOUNT;
    }
    if(amount>=0)
    {
        product->quantity+=amount;
        return MATAMIKYA_SUCCESS;
    }
    if(amount<0)
    {
        if(product->quantity+amount<0)
        {
            return MATAMIKYA_INSUFFICIENT_AMOUNT;
        }
        else
        {
            product->quantity+=amount;
            return MATAMIKYA_SUCCESS;
        }
    }
    return MATAMIKYA_SUCCESS;
}

static MatamikyaResult deleteFromOrders(Matamikya matamikya,const unsigned int id)
{
    assert(matamikya && matamikya->orders);
    SET_FOREACH(Order,current_order,matamikya->orders)
    {
        unsigned int current_order_id=getOrderId(current_order);
        assert(current_order_id!=ELEMENT_NOT_EXISTS);
        AmountSet products=getProductsInOrder(matamikya->orders,current_order_id);
        assert(products);
        unsigned int* id_copy=malloc(sizeof(unsigned int));
        if (!id_copy)
        {
            return MATAMIKYA_OUT_OF_MEMORY;
        }
        *id_copy=id;
        if (asContains(products,id_copy))
        {
            asDelete(products,id_copy);
        }
        free(id_copy);
    }
    return MATAMIKYA_SUCCESS;
}


static MatamikyaResult changeProfitByIdAndAmount(Matamikya matamikya,unsigned int id,double quantity)
{
    assert(matamikya->warehouse);
    MtmProduct product=listGetFirst(matamikya->warehouse);
    while(product)
    {
        if(product->id==id)
        {
            product->profit+=product->get_product_price(product->additional_data,quantity);
            return MATAMIKYA_SUCCESS;
        }
        product=listGetNext(matamikya->warehouse);
    }
    return MATAMIKYA_PRODUCT_NOT_EXIST;
}



MatamikyaResult mtmClearProduct(Matamikya matamikya, const unsigned int id)
{
    if(!matamikya || !matamikya->warehouse)
    {
        return MATAMIKYA_NULL_ARGUMENT;
    }
    MtmProduct product=listGetFirst(matamikya->warehouse);
    while(product && ((MtmProduct)listGetCurrent(matamikya->warehouse))->id!=id)
    {
        product=listGetNext(matamikya->warehouse);
    }
    if(!product)
    {
        return MATAMIKYA_PRODUCT_NOT_EXIST;
    }
    MatamikyaResult ordersRemoveResult=deleteFromOrders(matamikya,id);
    if (ordersRemoveResult!=MATAMIKYA_SUCCESS)
    {
        return ordersRemoveResult;
    }
    ListResult listRemoveResult=listRemoveCurrent(matamikya->warehouse);
    if (listRemoveResult==LIST_NULL_ARGUMENT)
    {
        return MATAMIKYA_NULL_ARGUMENT;
    }
    assert(listRemoveResult==LIST_SUCCESS);
    assert(ordersRemoveResult==MATAMIKYA_SUCCESS);
    return MATAMIKYA_SUCCESS;
}

static void rollBackShipmentFromWarehouse(Matamikya matamikya, const unsigned int orderId,AmountSet productsInOrder, unsigned int* last_product_id)
{
    AS_FOREACH(unsigned int*,product_id,productsInOrder)
    {
        if (*product_id==*last_product_id)
        {
            break;
        }
        MtmProduct current_mtm_product=listGetFirst(matamikya->warehouse);
        while(current_mtm_product)
        {
            if (current_mtm_product->id == *product_id)
            {
                double amount;
                asGetAmount(productsInOrder,product_id,&amount);
                assert(amount>=0);
                mtmChangeProductAmount(matamikya,current_mtm_product->id,amount);
                changeProfitByIdAndAmount(matamikya,current_mtm_product->id,-amount);
            }
            current_mtm_product=listGetNext(matamikya->warehouse);
        }
    }
}

MatamikyaResult mtmShipOrder(Matamikya matamikya, const unsigned int orderId)
{
    if(!matamikya || !matamikya->warehouse || !matamikya->orders)
    {
        return MATAMIKYA_NULL_ARGUMENT;
    }
    AmountSet products_in_order=getProductsInOrder(matamikya->orders,orderId);
    if(!products_in_order)
    {
        return MATAMIKYA_ORDER_NOT_EXIST;
    }
    double amount;
    AS_FOREACH(unsigned int*,current_product_id,products_in_order)
    {
        AmountSetResult getAmountResult=asGetAmount(products_in_order,current_product_id,&amount);
        if(getAmountResult!=AS_SUCCESS || amount<0)
        {
            rollBackShipmentFromWarehouse(matamikya,orderId,products_in_order, current_product_id);
            return getAmountResult==AS_NULL_ARGUMENT ? MATAMIKYA_NULL_ARGUMENT : MATAMIKYA_PRODUCT_NOT_EXIST;
        }
        MatamikyaResult resultChangeAmount=mtmChangeProductAmount(matamikya,*current_product_id,-amount);
        CHECK_RESULT_AND_ROLLBACK_IF_NEEDED(resultChangeAmount);
        MatamikyaResult changeProfitResult= changeProfitByIdAndAmount(matamikya,*current_product_id,amount);
        CHECK_RESULT_AND_ROLLBACK_IF_NEEDED(changeProfitResult);
    }
    assert(getOrderByOrderId(matamikya->orders,orderId));
    SetResult set_remove_result=setRemove(matamikya->orders,getOrderByOrderId(matamikya->orders,orderId));
    if (set_remove_result==SET_NULL_ARGUMENT)
    {
        return MATAMIKYA_NULL_ARGUMENT;
    }
    if (set_remove_result==SET_ITEM_DOES_NOT_EXIST)
    {
        return MATAMIKYA_ORDER_NOT_EXIST;
    }
    assert(set_remove_result==SET_SUCCESS);
    return MATAMIKYA_SUCCESS;
}

unsigned int mtmCreateNewOrder(Matamikya matamikya)
{
    if (!matamikya)
    {
        return 0;
    }
    AmountSet newOrderProducts=asCreate(copyProductElement,freeProductElement,compareProductElement);
    if (!newOrderProducts)
    {
        return 0;
    }
    Order newOrder=createOrder(matamikya->orders, newOrderProducts);
    if(!newOrder)
    {
        asDestroy(newOrderProducts);
        return 0;
    }
    SetResult result=setAdd(matamikya->orders, newOrder);
    if(result==SET_OUT_OF_MEMORY || result==SET_NULL_ARGUMENT)
    {
        freeOrder(newOrder);
        return 0;
    }
    assert(result==SET_SUCCESS || result==SET_ITEM_ALREADY_EXISTS);
    int new_order_id=getOrderId(newOrder);
    freeOrder(newOrder);
    return new_order_id;
}

MatamikyaResult mtmChangeProductAmountInOrder(Matamikya matamikya, const unsigned int orderId,
                                              const unsigned int productId, const double amount)
{
    if (!matamikya || !matamikya->warehouse || !matamikya->orders)
    {
        return MATAMIKYA_NULL_ARGUMENT;
    }
    AmountSet productsInOrder=getProductsInOrder(matamikya->orders,orderId);
    if (!productsInOrder)
    {
        return MATAMIKYA_ORDER_NOT_EXIST;
    }
    MtmProduct productInWarehouse=listGetFirst(matamikya->warehouse);
    while (productInWarehouse && productInWarehouse->id != productId)
    {
        productInWarehouse=listGetNext(matamikya->warehouse);
    }
    if (!productInWarehouse)
    {
        return MATAMIKYA_PRODUCT_NOT_EXIST;
    }
    assert(productInWarehouse->id==productId);
    if (!isAmountValid(productInWarehouse->scale,amount))
    {
        return MATAMIKYA_INVALID_AMOUNT;
    }
    Order order=setGetFirst(matamikya->orders);
    while (order && getOrderId(order)!=orderId)
    {
        order=setGetNext(matamikya->orders);
    }
    if(!order)
    {
        return MATAMIKYA_ORDER_NOT_EXIST;
    }
    assert(getOrderId(order)==orderId);
    AmountSet orderProducts=getProductsInOrder(matamikya->orders,orderId);
    unsigned int* productInOrder=asGetFirst(orderProducts);
    while (productInOrder && *productInOrder!=productInWarehouse->id)
    {
        productInOrder=asGetNext(orderProducts);
    }
    if(!productInOrder)
    {
        AmountSetResult result=asRegister(orderProducts, &productInWarehouse->id);
        if(result!=AS_SUCCESS)
        {
            return result==AS_NULL_ARGUMENT ? MATAMIKYA_NULL_ARGUMENT : MATAMIKYA_ORDER_NOT_EXIST;
        }
        assert(result!=AS_ITEM_DOES_NOT_EXIST && result!=AS_NULL_ARGUMENT);
    }
    AmountSetResult result=asChangeAmount(orderProducts,&productInWarehouse->id,amount);
    assert(result!=AS_ITEM_DOES_NOT_EXIST);
    if (result==AS_INSUFFICIENT_AMOUNT)
    {
        result=asDelete(orderProducts,&productInWarehouse->id);
        assert(result!=AS_ITEM_DOES_NOT_EXIST && result!=AS_NULL_ARGUMENT);
    }
    return MATAMIKYA_SUCCESS;
}

MatamikyaResult mtmCancelOrder(Matamikya matamikya, const unsigned int orderId)
{
    if (!matamikya || !matamikya->orders)
    {
        return MATAMIKYA_NULL_ARGUMENT;
    }
    Order orderToCancel=getOrderByOrderId(matamikya->orders, orderId);
    if(!orderToCancel)
    {
        return MATAMIKYA_ORDER_NOT_EXIST;
    }
    SetResult result=setRemove(matamikya->orders, orderToCancel);
    if (result==SET_NULL_ARGUMENT)
    {
        return MATAMIKYA_NULL_ARGUMENT;
    }
    if (result==SET_ITEM_DOES_NOT_EXIST)
    {
        return MATAMIKYA_ORDER_NOT_EXIST;
    }
    assert(result!=SET_NULL_ARGUMENT && result!=SET_ITEM_DOES_NOT_EXIST);
    return MATAMIKYA_SUCCESS;
}

MatamikyaResult mtmPrintInventory(Matamikya matamikya, FILE *output)
{
    if(!matamikya || !output || !matamikya->warehouse)
    {
        return MATAMIKYA_NULL_ARGUMENT;
    }
    fprintf(output,"Inventory Status:\n");
    MtmProduct to_print=listGetFirst(matamikya->warehouse);
    double price=0;
    while (to_print)
    {
        price=to_print->get_product_price(to_print->additional_data, 1.0);
        mtmPrintProductDetails(to_print->name, to_print->id, to_print->quantity, price, output);
        to_print=listGetNext(matamikya->warehouse);
    }
    return MATAMIKYA_SUCCESS;
}

MatamikyaResult mtmPrintOrder(Matamikya matamikya, const unsigned int orderId, FILE *output)
{
    if (!matamikya || !matamikya->warehouse || !matamikya->orders || !output)
    {
        return MATAMIKYA_NULL_ARGUMENT;
    }
    Order order=getOrderByOrderId(matamikya->orders,orderId);
    if (!order)
    {
        return MATAMIKYA_ORDER_NOT_EXIST;
    }
    mtmPrintOrderHeading(orderId,output);
    double total_order_cost=0;
    MtmProduct product_to_print=listGetFirst(matamikya->warehouse);
    AmountSet productsInOrder=getProductsInOrder(matamikya->orders,orderId);
    while (product_to_print)
    {
        if(asContains(productsInOrder,&product_to_print->id))
        {
            double amountOfProductInOrder;
            asGetAmount(productsInOrder,&product_to_print->id,&amountOfProductInOrder);
            assert(amountOfProductInOrder>=0);
            double price=product_to_print->get_product_price(product_to_print->additional_data, amountOfProductInOrder);
            mtmPrintProductDetails(product_to_print->name,product_to_print->id, amountOfProductInOrder,price,output);
            total_order_cost+=price;
        }
        product_to_print=listGetNext(matamikya->warehouse);
    }
    mtmPrintOrderSummary(total_order_cost,output);
    return MATAMIKYA_SUCCESS;
}

MatamikyaResult mtmPrintBestSelling(Matamikya matamikya,FILE *output)
{
    if(!matamikya || !matamikya->warehouse || !output)
    {
        return MATAMIKYA_NULL_ARGUMENT;
    }
    MtmProduct find_max=listGetFirst(matamikya->warehouse);
    double max_profit=0;
    while(find_max)
    {
        if(find_max->profit>max_profit)
        {
            max_profit=find_max->profit;
        }
        find_max=listGetNext(matamikya->warehouse);
    }
    if(max_profit< ALMOST_ZERO && max_profit>-ALMOST_ZERO)
    {
        fprintf(output, "Best Selling Product:\nnone\n");
        return MATAMIKYA_SUCCESS;
    }
    fprintf(output,"Best Selling Product:\n");
    find_max=listGetFirst(matamikya->warehouse);
    while (find_max)
    {
        if(find_max->profit < max_profit + ALMOST_ZERO && find_max->profit > max_profit - ALMOST_ZERO)
        {
            mtmPrintIncomeLine(find_max->name, find_max->id, find_max->profit, output);
            return MATAMIKYA_SUCCESS;
        }
        find_max=listGetNext(matamikya->warehouse);
    }
    return MATAMIKYA_SUCCESS;
}