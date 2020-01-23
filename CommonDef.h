//
// Created by sam on 23/1/2020.
//

#ifndef ORDER_BOOK_COMMONDEF_H
#define ORDER_BOOK_COMMONDEF_H
#include <iostream>
#include <unordered_map>
#include <list>
#include <memory>
#include <cmath>
//#include <boost/multiprecision/cpp_dec_float.hpp>

using std::string;
using std::unordered_map;
using std::list;
using std::cout;
using std::endl;
//using dec_50 = boost::multiprecision::cpp_dec_float_50;

class PriceLevel;
class OrderNode;

enum class BUY_SELL
{
    BUY, SELL
};

enum class OrderStatus
{
    New, Filled, Partial_Filled, Cancelled
};

inline std::ostream &operator<<(std::ostream &os, const OrderStatus& s) {
    switch(s)
    {
        case OrderStatus::New:
        {
            os<< "new";
            break;
        }
        case OrderStatus::Filled:
        {
            os<< "Filled";
            break;
        }
        case OrderStatus::Partial_Filled:
        {
            os<< "Partial_Filled";
            break;
        }
        case OrderStatus::Cancelled:
            os<< "Cancelled";
            break;
        default:
            os << "unknown";
    }

    return os;
}

constexpr int power_of_10(int digit)
{
    if(digit == 0) return 1;
    return 10* power_of_10(digit-1);
}

#endif //ORDER_BOOK_COMMONDEF_H
