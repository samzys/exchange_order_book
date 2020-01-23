cmake_minimum_required(VERSION 3.10)
project(order_book)
set(Boost_USE_STATIC_LIBS   ON)

#enable_testing()

set(CMAKE_CXX_STANDARD 17)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(order_book main.cpp OrderBook.h OrderBook.cc OrderNode.h PriceLevel.h CommonDef.h)
TARGET_LINK_LIBRARIES(order_book ${Boost_LIBRARIES})

add_executable(order_book_test order_book_test.cc OrderBook.h OrderBook.cc OrderNode.h PriceLevel.h CommonDef.h)
TARGET_LINK_LIBRARIES(order_book_test ${Boost_LIBRARIES})

