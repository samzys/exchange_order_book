//
// Created by sam on 22/1/2020.
//
#include <iostream>
#include "OrderBook.h"
#define BOOST_TEST_MODULE order_book_test
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(LimitOrder)
{
    OrderBook book(1,"0.05");
    book.addOrder(1001, BUY_SELL::BUY, 100, 12.30);
    book.addOrder(1002, BUY_SELL::BUY, 200, 12.30);
    book.addOrder(1003, BUY_SELL::SELL, 200, 12.40);
    BOOST_CHECK_EQUAL(book.queryOrder(1001), 1);
    BOOST_CHECK_EQUAL(book.queryOrder(1002), 2);
    BOOST_CHECK_EQUAL(book.queryOrder(1003), 1);
};

BOOST_AUTO_TEST_CASE(LimitOrder_Incorrect_ticksize)
{
    OrderBook book(1,"0.05");
    BOOST_CHECK_EQUAL(book.addOrder(1001, BUY_SELL::BUY, 100, 12.30), true);
    BOOST_CHECK_EQUAL(book.addOrder(1002, BUY_SELL::BUY, 200, 12.11), false);

    BOOST_CHECK_EQUAL(book.queryOrder(1001), 1);
    BOOST_CHECK_EQUAL(book.queryOrder(1002), -1);
};

BOOST_AUTO_TEST_CASE(CancelOrder)
{
    OrderBook book(1,"0.05");
    BOOST_CHECK_EQUAL(book.addOrder(1001, BUY_SELL::BUY, 100, 12.30), true);
    BOOST_CHECK_EQUAL(book.addOrder(1002, BUY_SELL::BUY, 200, 12.30), true);
    BOOST_CHECK_EQUAL(book.addOrder(1003, BUY_SELL::SELL, 100, 12.10), true);

    BOOST_CHECK_EQUAL(book.cancelOrder(1002), true);
    //at this point order 1001 is exectued
    BOOST_CHECK_EQUAL(book.cancelOrder(1001), false);
};

BOOST_AUTO_TEST_CASE(CancelOrder2)
{
    OrderBook book(1,"0.05");
    BOOST_CHECK_EQUAL(book.addOrder(1001, BUY_SELL::BUY, 200, 12.30), true);
    BOOST_CHECK_EQUAL(book.addOrder(1002, BUY_SELL::BUY, 200, 12.30), true);
    BOOST_CHECK_EQUAL(book.addOrder(1003, BUY_SELL::SELL, 100, 12.10), true);

    //at this point order 1001 is partial filled, so still can cancel
    BOOST_CHECK_EQUAL(book.cancelOrder(1001), true);
};
//
BOOST_AUTO_TEST_CASE(AmendOrder)
{
    OrderBook book(1,"0.05");
    BOOST_CHECK_EQUAL(book.addOrder(1001, BUY_SELL::SELL, 200, 12.30), true);
    BOOST_CHECK_EQUAL(book.addOrder(1002, BUY_SELL::SELL, 200, 12.30), true);
    BOOST_CHECK_EQUAL(book.addOrder(1003, BUY_SELL::SELL, 200, 12.30), true);
    BOOST_CHECK_EQUAL(book.addOrder(1004, BUY_SELL::SELL, 200, 12.30), true);
//
//    //qty down, queue pos no change.
    BOOST_CHECK_EQUAL(book.amendOrder(1001, 50), true);
    BOOST_CHECK_EQUAL(book.queryOrder(1001), 1);

    //qty up, queue pos no change.
    BOOST_CHECK_EQUAL(book.amendOrder(1002, 400), true);
    BOOST_CHECK_EQUAL(book.queryOrder(1001), 1);
    BOOST_CHECK_EQUAL(book.queryOrder(1003), 2);
    BOOST_CHECK_EQUAL(book.queryOrder(1004), 3);
    BOOST_CHECK_EQUAL(book.queryOrder(1002), 4);
};

BOOST_AUTO_TEST_CASE(QueryOrder)
{
    OrderBook book(1, "0.05");
    BOOST_CHECK_EQUAL(book.addOrder(1001, BUY_SELL::SELL, 200, 12.30), true);
    BOOST_CHECK_EQUAL(book.addOrder(1002, BUY_SELL::SELL, 200, 12.30), true);
    BOOST_CHECK_EQUAL(book.addOrder(1003, BUY_SELL::BUY, 200, 12.10), true);
    BOOST_CHECK_EQUAL(book.addOrder(1004, BUY_SELL::BUY, 200, 12.10), true);

    BOOST_CHECK_EQUAL(book.queryOrder(1001), 1);
    BOOST_CHECK_EQUAL(book.queryOrder(1002), 2);
    BOOST_CHECK_EQUAL(book.queryOrder(1003), 1);
    BOOST_CHECK_EQUAL(book.queryOrder(1004), 2);
}

BOOST_AUTO_TEST_CASE(QueryLevel)
{
    OrderBook book(1, "0.05");
    BOOST_CHECK_EQUAL(book.addOrder(1001, BUY_SELL::SELL, 200, 12.30), true);
    BOOST_CHECK_EQUAL(book.addOrder(1002, BUY_SELL::SELL, 200, 12.30), true);
    BOOST_CHECK_EQUAL(book.addOrder(1003, BUY_SELL::BUY, 200, 12.10), true);
    BOOST_CHECK_EQUAL(book.addOrder(1004, BUY_SELL::BUY, 200, 12.10), true);
    BOOST_CHECK_EQUAL(book.addOrder(1004, BUY_SELL::BUY, 200, 12.05), true);

    BOOST_CHECK_EQUAL(book.queryLevel(BUY_SELL::BUY, 0), 400);
    BOOST_CHECK_EQUAL(book.queryLevel(BUY_SELL::BUY, 1), 200);
    BOOST_CHECK_EQUAL(book.queryLevel(BUY_SELL::BUY, 2), -1);
}

