#ifndef bar_h
#define bar_h

#include <cstddef>

// The width in characters
#define LOADINGBAR_W 50

class LoadingBar {
    public:
        LoadingBar();
        LoadingBar(const size_t max);

        void update(const size_t val);
        void inc(const size_t val);
        void inc();
        void show() const;

    private:
        size_t cur;
        const size_t max;
};

#endif
