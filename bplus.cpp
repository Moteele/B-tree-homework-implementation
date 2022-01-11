#include "bplus.hpp"
//#include <stdlib.h>


void test_basic() {
    bplus<int, int, 3> t;
    bplus<int, int, 3> t2;
    bplus<int, int, 3> t3;
    bplus<int, int, 5> t5;
    for (int i = 0; i <= 20; ++i)
	assert(t.insert(i, i*100));
    for (int i = 0; i <= 20; ++i)
	assert(t.contains(i));
    assert(!t.contains(-1));
    assert(!t.contains(21));
    t.print();
    //try{t.at(100);} catch (std::out_of_range &ex) {std::cout<< "catched";}
    for (int i = 0; i <= 30; ++i)
	t[i];
    for (int i = 0; i <= 30; ++i)
	assert(t.contains(i));
    assert(t[20] == 2000);
    assert(t[21] == 0);
    assert(t[22] == 0);

    assert(!t.contains(-1));
    assert(!t.contains(31));
    t.print();

    for (int i = 20; i >= 0; --i)
	t2.insert(i, i*100);
    for (int i = 20; i >= 0; --i)
	assert(t2.contains(i));
    assert(!t2.contains(-1));
    assert(!t2.contains(21));
    t2.print();

    t3.insert(1,1);
    t3.insert(4,4);
    t3.insert(7,7);
    t3.insert(10,10);
    t3.insert(17,17);
    assert(t3.insert(21,21));
    assert(!t3.insert(21,22));
    assert(!t3.insert(21,22));
    assert(!t3.insert(21,21));
    t3.insert(31,31);
    t3.insert(25,25);
    t3.insert(19,19);
    t3.insert(20,20);
    t3.insert(28,28);
    int counter = 0;
    for(int i = 0; i < 29; i++) {
	try {
	    auto p = t3.path(i);
	    auto val = t3.at(i);
	    assert(val == t3.fetch(p));
	    const auto valx = t3.at(i);
	    assert(val == valx);
	    assert(val == i);
	} catch (std::out_of_range& ex) {
	    //std::cout << ex.what() << i << std::endl;
	    counter++;
	}
    }
    assert(counter == 19);
    t3.print();
    std::cout << "STARTING ERASE!!!\n";

    t3.erase(1);
    t3.erase(1);
    assert( t3.contains(4));
    t3.erase(4);
    assert( ! t3.contains(4));
    t3.erase(31);
    assert( ! t3.contains(1) );
    for(int i = 0; i < 29; i++) {
	if(t3.contains(i))
	    t3.erase(i);
    }
    t3.print();
    t3.insert(1,1);
    t3.insert(4,4);
    t3.insert(31,31);
    t3.insert(7,7);
    t3.insert(10,10);
    t3.insert(17,17);
    t3.insert(21,21);
    t3.insert(25,25);
    t3.insert(19,19);
    t3.insert(28,28);
    t3.insert(20,20);

    counter = 0;
    for(int i = 0; i < 29; i++) {
	try {
	    auto p = t3.path(i);
	    auto val = t3.at(i);
	    assert(val == t3.fetch(p));
	    const auto valx = t3.at(i);
	    assert(val == valx);
	    assert(val == i);
	} catch (std::out_of_range& ex) {
	    //std::cout << ex.what() << i << std::endl;
	    counter++;
	}
    }
    assert(counter == 19);
    t3.print();
    for(int i = 0; i < 29; i++) {
	if(t3.contains(i))
	    assert(t3.erase(i));
	assert( ! t3.erase(i));
    }
    for (int i = 0; i <= 20; ++i)
	t3.insert(i, i*100);
    t3.print();


    for(int i = 0; i < 20; i++) {
	assert(t5.insert(i, i));
    }
    for(int i = 0; i < 20; i++) {
	auto p = t5.path(i);
	for (auto x : p)
	    std::cout << x << " ";
	std::cout<< std::endl;
    }
    t5.print();
    bplus<int, char, 3> t6;
    const auto &b_const = t6;
    t6.insert(4, 'x');
    assert(b_const.at(4) == 'x');
}
void test_nonint() {
    bplus<char, double, 3> t;
    t.insert('a', 5.5);
    t.insert('d', 6.5);
    t.insert('b', 7.5);
    auto &x = t['d'];
    x = 1.1;
    t.print();
}
void test_advanced() {
}
int main()
{
    test_basic();
    test_nonint();
}
