#include <iostream>
#include "OrderBook.h"
#include <vector>
using std::vector;

static void split(string& input, char c, vector<string>& result)
{
    // remove ending newline characters
    input.erase(input.find_last_not_of(" \n\r\t") + 1);
    const char *s = input.c_str();
    do
    {
        const char *begin = s;
        while (*s != c && *s)
            s++;
        result.push_back(string(begin, s));
    } while (0 != *s++);
}

int main()
{

    //  buf.fd = STDIN_FILENO;
    string tmp;
    vector<string> input;
    OrderBook book(1,"0.05");

    while(1)
    {
        input.clear();

        std::getline(std::cin, tmp);
        split(tmp, ' ', input);
        if(input.empty())
        {
            cout <<"Invalid input"<<endl;
            break;
        }

        if(input[0] == "order")
        {
            int id = stoi(input[1]);
            BUY_SELL b = input[2] == "buy" ? BUY_SELL ::BUY : BUY_SELL ::SELL;

            int qty = stoi(input[3]);
            double price = stod(input[4]);
            book.addOrder(id, b, qty, price);
        }
        else if(input[0] == "cancel")
        {
            int id = stoi(input[1]);
            book.cancelOrder(id);
        }
        else if(input[0] == "amend")
        {
            int id = stoi(input[1]);
            int qty = stoi(input[2]);

            book.amendOrder(id, qty);
        }
        else if(input[0] == "q")
        {
            if(input[1] == "order")
            {
                int id = stoi(input[2]);
                book.queryOrder(id);
            }else if(input[1] == "level")
            {
                BUY_SELL b = BUY_SELL ::BUY;
                if(input[2] == "ask")
                    b = BUY_SELL ::SELL;

                int index = stoi(input[3]);
                book.queryLevel(b, index);
            }
        }else
        {
            cout <<"Invalid input"<<endl;
            break;
        }

    }

    cout << "end of order book"<<endl;


//    OrderBook book(1,"0.05");
//
//    book.addOrder(1001, BUY_SELL::BUY, 100, 12.30);
//    book.addOrder(1002, BUY_SELL::BUY, 200, 12.30);
//    book.addOrder(1003, BUY_SELL::SELL, 200, 12.10);
//    book.queryOrder(1001)/*, 1)*/;
//    book.queryOrder(1002)/*, 2)*/;
//    book.queryOrder(1003)/*, 1)*/;
//
////    book.addOrder(1002, BUY_SELL::SELL, 100, 12.40);
////    book.addOrder(1004, BUY_SELL::BUY, 300, 12.15);
//
//    book.amendOrder(1002, 400);
//    book.queryOrder(1002);
//    book.amendOrder(1002, 50);
//    book.queryOrder(1001);
//
//    book.cancelOrder(1001);
//
//    std::cout << "Hello, World! End" << std::endl;
//    my_class test_object("qwerty");
//    BOOST_CHECK(test_object.is_valid());
    return 0;
}
