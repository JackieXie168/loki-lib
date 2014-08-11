#include "singleton.h"

using namespace ::Loki;


struct A { ~A(){} };
struct B { ~B(){} };
struct C { ~C(){} };

unsigned int GetLongevity(C*)
{
    return 1;
}

int main()
{
    typedef SingletonHolder<A> SingletonHolder1_t;
    typedef SingletonHolder<B, CreateStatic> SingletonHolder2_t;
    typedef SingletonHolder<C, CreateUsingNew, SingletonWithLongevity> SingletonHolder3_t;

    SingletonHolder1_t::Instance();
    SingletonHolder2_t::Instance();
    SingletonHolder3_t::Instance();
}

