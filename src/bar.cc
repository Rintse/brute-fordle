#include "bar.h"

#include <exception>
#include <iostream>
#include <cmath>
#include <iterator>

LoadingBar::LoadingBar() : cur(0), max(0) {}
LoadingBar::LoadingBar(size_t max) : cur(0), max(max) {}
        
void LoadingBar::update(const size_t val) {
    cur = val;
    show();
}

void LoadingBar::inc(const size_t val) { 
    cur += val; 
    show();
}

void LoadingBar::inc() { 
    cur++; 
    show();
}

void LoadingBar::show() const {
    std::cout << "[";

    size_t cur_c = floor(LOADINGBAR_W * ((double)cur/max));
    size_t rest = LOADINGBAR_W - cur_c;

    std::cout << std::string(cur_c, '=');
    std::cout << std::string(rest, ' ');
    
    std::cout 
        << "] ("
        << cur << "/" << max 
        << ")\r";

    std::cout.flush();
}
