//
// Created by sam on 22/1/2020.
//

#include "OrderBook.h"

PriceLevel::PriceLevel(int price, int qty/*, int askQty*/) : price_(price) /*qty(qty), askQty_(askQty)*/
{}

int PriceLevel::getPrice() const
{
    return price_;
}

int PriceLevel::getQty() const
{
    return bidQty> 0? bidQty: askQty;
}

int PriceLevel::getBidQty() const
{
    return bidQty;
}

int PriceLevel::getAskQty() const
{
    return askQty;
}

OrderNode::OrderNode(int orderId, int price, int qty, int remainQty, BUY_SELL side, PriceLevel *level) : orderId(orderId),
                                                                                             price(price), qty(qty),
                                                                                             remainQty(remainQty),
                                                                                             side(side), level(level)
{
}

void OrderNode::setState(OrderStatus state)
{
    OrderNode::state = state;
}

OrderBook::OrderBook(int orderBookId, const string &tick)
        : orderBookId(orderBookId)
{
    auto pos = tick.find_first_of('.');

    denominator = 0;

    if(pos != std::string::npos)
    {
        string  decimal = tick.substr(pos+1);
        denominator  =  stoi(decimal);
        decimalPoints = decimal.length();
    }

    if(denominator == 0)
    {
        denominator = 1;
        decimalPoints  = 0;
    }

    bestAskLevel_ = prices_levels_.end();
    bestBidLevel_ =  prices_levels_.end();
}

PriceLevel *OrderBook::getPriceLevel(int price, BUY_SELL buy)
{
    auto it = levels_map_.find(price);
    if (it != levels_map_.end())
        return it->second.get();

    //alternatively can get from object pool
    auto level = std::make_unique<PriceLevel>(price, 0);

    auto p_it = prices_levels_.begin();
    while (p_it != prices_levels_.end())
    {
        if ((*p_it)->getPrice() > price)
            break;
        ++p_it;
    }
    auto raw_level_ptr = level.get();

    auto it2 = prices_levels_.insert(p_it, level.get());
    prices_iter_[price] = it2;
    levels_map_.emplace(price, std::move(level));

    return raw_level_ptr;
}

bool OrderBook::addOrder(int oId, BUY_SELL buy, int qty, double price)
{
    //check price is tick size aligned
    int p =  round(price*power_of_10(decimalPoints));
    if((p % denominator) != 0)
    {
        cout << "price is not tick size aligned!"<<endl;
        return false;
    }

    auto level = getPriceLevel(p, buy);

    //do the matching.
    if(buy == BUY_SELL::BUY)
    {
        //no ask level, simply add them
        if(bestAskLevel_ == prices_levels_.end())
        {
            /*add order to queue*/
            auto node = std::make_unique<OrderNode>(oId, p, qty,qty, buy, level);
            level->addOrder(node.get());
            orders_m_.emplace(oId,std::move(node));

            adjustBestBidLevelMatch(p);

            return true;
        }

        if(p < (*bestAskLevel_)->getPrice())
        {
            /*add order to queue*/
            auto node = std::make_unique<OrderNode>(oId, p, qty,qty, buy, level);
            level->addOrder(node.get());
            orders_m_.emplace(oId,std::move(node));

            adjustBestBidLevelMatch(p);
        }
        else
        {
            //cross orders and adjust best bid and best ask price.
            int remainQty = qty;

            //move bestAsk up.
            auto it = bestAskLevel_;

            while(remainQty>0 && it != prices_levels_.end())
            {
                remainQty = (*it)->matchOrders(p, remainQty);

                adjustBestAskLevelUp();

                if (bestAskLevel_ == prices_levels_.end()||(*it)->getPrice() > p)
                    break;
            }


            auto node = std::make_unique<OrderNode>(oId, p, qty, remainQty, buy, level);
            node->setState(OrderStatus::Filled);

            //add remain order to the book
            if(remainQty >0){
                /*add order to queue*/
                level->addOrder(node.get());
                node->setState(OrderStatus::Partial_Filled);
                /*move best bid up to current level*/
                adjustBestBidLevelMatch(p);
            }
            orders_m_.emplace(oId,std::move(node));
        }
    }
    else
    {
        //first order
        if(bestBidLevel_ == prices_levels_.end())
        {
            /*add order to queue*/
            auto node = std::make_unique<OrderNode>(oId, p, qty,qty, buy, level);
            level->addOrder(node.get());
            orders_m_.emplace(oId,std::move(node));

            adjustBestAskLevelMatch(p);
            return true;
        }

        //Sell Orders.
        if(p > (*bestBidLevel_)->getPrice())
        {
            /*add order to queue*/
            auto node = std::make_unique<OrderNode>(oId, p, qty, qty, buy, level);
            level->addOrder(node.get());
            orders_m_.emplace(oId,std::move(node));

            /*move best ask down to current level*/
            adjustBestAskLevelMatch(p);
        }
        else
        {
            //cross orders and adjust best bid and best ask price.
            int remainQty = qty;

            //move bestAsk up.
            while(remainQty>0 && (*bestBidLevel_)->getQty()>0)
            {
                remainQty = (*bestBidLevel_)->matchOrders(p, remainQty);

                adjustBestBidLevelDown();

                if (bestBidLevel_ == prices_levels_.end() ||
                (*bestBidLevel_)->getPrice() < p)
                    break;
            }

            auto node = std::make_unique<OrderNode>(oId, p, qty, remainQty, buy, level);
            node->setState(OrderStatus::Filled);
            //add remain order to the book
            if(remainQty >0){
                /*add order to queue*/
                node->setState(OrderStatus::Partial_Filled);
                level->addOrder(node.get());

                /*move best ask up to current level*/
                adjustBestAskLevelMatch(p);
            }
            orders_m_.emplace(oId,std::move(node));
        }
    }
    return true;
}

bool OrderBook::amendOrder(int oId, int new_qty)
{
    auto it = orders_m_.find(oId);
    if(it == orders_m_.end()){
        cout << "invalid oid"<<endl;
        return false;
    }

    auto& node = it->second;

    if(node->state != OrderStatus::New)
        return false;

    auto level = it->second->level;
    level->amendOrder(node.get(), new_qty);
    return true;
}

bool OrderBook::cancelOrder(int oId)
{
    auto it = orders_m_.find(oId);
    if(it == orders_m_.end()){
        cout << "invalid oid:"<< oId<<endl;
        return false;
    }

    auto& node = it->second;
    if(node->state == OrderStatus::Filled ||
            node->state == OrderStatus::Cancelled )
        return false;

    auto level = it->second->level;
    level->cancelOrder(node.get());

    if(node->side == BUY_SELL::BUY)
        adjustBestBidLevelDown();
    if(node->side == BUY_SELL::SELL)
        adjustBestAskLevelUp();

    orders_m_.erase(oId);
    return true;
}

int OrderBook::queryOrder(int oId)
{
    auto it = orders_m_.find(oId);
    if(it == orders_m_.end()){
        cout << "invalid oid"<<endl;
        return -1;
    }
    auto& node = it->second;
    auto level = it->second->level;

    int pos = level->queryOrder(node.get());

    cout<<  std::setprecision(decimalPoints) << std::fixed << oId
        << ","<< double(node->price*1.0/power_of_10(decimalPoints))
        << ","<< node->qty
        << ","<< node->remainQty
        << ","<< (node->side == BUY_SELL::BUY? "buy":"sell")
        << ", "<< node->state
        << ","<< pos  <<endl;

    return pos;
}

int OrderBook::queryLevel(BUY_SELL buy, int i)
{
    auto it = bestBidLevel_;
    if(buy == BUY_SELL::BUY)
    {
        int j=i;
        while(j-- && it!= prices_levels_.end())
        {
            it =prev(it);
        }
//            --it;
    }
    else{
        it = bestAskLevel_;
        int j=i;
        while(j-- && it!= prices_levels_.end())
            ++it;
    }

    if(it!= prices_levels_.end())
    {
        auto& p = (*it);

        cout << std::setprecision(decimalPoints) <<std::fixed
             << ((buy == BUY_SELL::BUY)? "bid" : "ask")
             <<", " << i
             <<", " << double(p->getPrice()*1.0/power_of_10(decimalPoints))
             <<","  << p->getQty() << endl;
        return p->getQty();
    }
    return -1;
}



