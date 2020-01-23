//
// Created by sam on 23/1/2020.
//

#ifndef ORDER_BOOK_PRICELEVEL_H
#define ORDER_BOOK_PRICELEVEL_H
#include "CommonDef.h"

class PriceLevel
{
private:
    //at one time, only one queue should have value.  auction is not implemented
    std::list<OrderNode*> q;
    //orderId,
    std::unordered_map<int, std::list<OrderNode*>::iterator > m_;

    int price_=0;
//    int qty = 0;
    //for auction period
    int bidQty = 0;
    int askQty = 0;
public:
    int getBidQty() const;

    int getAskQty() const;

    int getQty() const;

    int getPrice() const;

    PriceLevel(int price, int qty);

    int matchOrders(int price, int remainQty)
    {
        for(auto it = q.begin(); it != q.end() && remainQty>0;/* ++it*/)
        {
            OrderNode* node = (*it);
            auto& qty =  node->side == BUY_SELL ::BUY? bidQty: askQty;

            if(remainQty >= node->remainQty)
            {
                node->state = OrderStatus::Filled;
                remainQty -= node->remainQty;
                qty -= node->remainQty;
                node->remainQty = 0;

                //remove the filled orderNode from list.
                m_.erase(node->orderId);
                q.erase(it++);
            }
            else
            {
                node->state = OrderStatus::Partial_Filled;
                node->remainQty -= remainQty;
                qty -= remainQty;
                remainQty = 0;
                break;
            }
        }
        return remainQty;
    }

    bool addOrder(OrderNode* node)
    {
        //put it at the end of the list.
        node->level = this;

        q.push_back(node);

        m_[node->orderId]= std::prev(q.end());

        auto& qty =  node->side == BUY_SELL ::BUY? bidQty: askQty;

        qty += node->remainQty;

        node->state = OrderStatus::New;
    }

    bool amendOrder(OrderNode* node, int new_qty)
    {
        const auto it = m_.find(node->orderId);
        if(it== m_.cend())
            return false;

        if(node->state != OrderStatus::New)
            return false;

        //qty increase,
        if(node->qty < new_qty){
            q.splice(q.end(), q, it->second);

            //update  m_
            m_[node->orderId] = std::prev(q.end());
        }

        auto& qty =  node->side == BUY_SELL ::BUY? bidQty: askQty;

        qty += new_qty - node->qty;

        // update node qty
        node->qty = new_qty;
        node->remainQty = new_qty;

        return true;
    }

    bool cancelOrder(OrderNode* node)
    {
        auto it = m_.find(node->orderId);
        if(it== m_.end())
            return false;

        if(node->remainQty ==0)
            return false;

        auto& qty =  node->side == BUY_SELL ::BUY? bidQty: askQty;

        qty -= node->remainQty;
        node->remainQty = 0;

        q.erase(it->second);
        m_.erase(it);

        return true;
    }

    int queryOrder(OrderNode* node)
    {
        auto it = m_.find(node->orderId);
        if(it== m_.end())
            return -1;

        return std::distance(q.begin(), it->second) + 1;
    }

};

#endif //ORDER_BOOK_PRICELEVEL_H
