#include <iostream>
#include <boost/version.hpp>

int main(void) {
    std::cout << "Using Boost " << BOOST_VERSION / 100000 << "."
                   << BOOST_VERSION / 100 % 1000 << "."
                   << BOOST_VERSION % 100 << std::endl;
}
