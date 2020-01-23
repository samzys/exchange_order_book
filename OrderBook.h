#include <iomanip>
#include "CommonDef.h"
#include "OrderNode.h"
#include "PriceLevel.h"

class OrderBook
{
private:
    int orderBookId=-1;
    int denominator = 1;
    int decimalPoints = 2; //need to be adjusted when ticksize change.

//    list<PriceLevel*> askPrices_;
    list<PriceLevel*>::iterator  bestBidLevel_;
    list<PriceLevel*>::iterator  bestAskLevel_;

    //small -> large price.
    list<PriceLevel*> prices_levels_;
    unordered_map<int, std::unique_ptr<PriceLevel>> levels_map_;
    unordered_map<int, list<PriceLevel*>::iterator> prices_iter_;

    //order_id, orderNode
    unordered_map<int, std::unique_ptr<OrderNode>>  orders_m_;

    PriceLevel* getPriceLevel(int price, BUY_SELL buy);

public:
    OrderBook(int orderBookId, const string& tick);

    bool addOrder(int oId, BUY_SELL buy, int qty, double price);
    bool addOrderToPrice(int oId, BUY_SELL buy, int qty, double price);

    bool amendOrder(int oId, int new_qty);

    bool cancelOrder(int oId);

    void adjustBestAskLevelMatch(int px){
        if (bestAskLevel_ == prices_levels_.end() || px < (*bestAskLevel_)->getPrice())
            bestAskLevel_ = prices_iter_[px];
    }
    void adjustBestBidLevelMatch(int px)
    {
        if (bestBidLevel_ == prices_levels_.end() || px > (*bestBidLevel_)->getPrice())
            bestBidLevel_ = prices_iter_[px];
    }

    void adjustBestAskLevelUp(){
        while(bestAskLevel_ != prices_levels_.end())
        {
            if((*bestAskLevel_)->getQty() > 0)
                break;
            bestAskLevel_ = next(bestAskLevel_);
        }
    }
    void adjustBestBidLevelDown(){
        while(bestBidLevel_ != prices_levels_.end())
        {
            if((*bestBidLevel_)->getQty() > 0)
                break;
            bestBidLevel_  = prev(bestBidLevel_ );
        }

    }

    /// query functions
    /// \param oId
    int queryOrder(int oId);

    int queryLevel(BUY_SELL buy, int i);

};