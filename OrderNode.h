//
// Created by sam on 23/1/2020.
//

#ifndef ORDER_BOOK_ORDERNODE_H
#define ORDER_BOOK_ORDERNODE_H
#include "CommonDef.h"

class OrderNode
{
public:

    OrderNode(int orderId, int price, int qty, int remainQty, BUY_SELL side, PriceLevel *level);
    int orderId;
    int price;
    int qty;
    int remainQty;
    BUY_SELL side;
    PriceLevel* level;

    OrderStatus state;

    void setState(OrderStatus state);

    friend std::ostream &operator<<(std::ostream &os, const OrderNode &node)
    {
        os  << node.orderId << "," << node.price << "," << node.qty << ","
            << (node.side == BUY_SELL::BUY? "buy": "sell") << "," << node.state;
        return os;
    }
};

#endif //ORDER_BOOK_ORDERNODE_H
