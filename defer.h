#ifndef DEFER_H
#define DEFER_H

#include <functional>

class DeferDummy
{
public:
    DeferDummy(std::function<void(void)> &&cb) : _cb(std::move(cb)) { }
    ~DeferDummy() { if (_cb) _cb(); }
private:
    std::function<void(void)> _cb;
};


#ifndef defer
#define _contact(a, b) a##b
#define _contact2(a, b) _contact(a, b)
#define defer(codes) DeferDummy _contact2(defer, __LINE__)([&]()codes);
#else
#warning "can not use defer."
#endif

#endif // DEFER_H
