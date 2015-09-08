#include <iostream>
class Base{
public:
	~Base()
	{
		std::cout<<"Base::~Base()\n";
	}
};
class Derived:public Base
{
public:
	~Derived()
	{
		std::cout<<"Derived::~Derived()\n";
	}
};
Derived getderived()
{
	return Derived();
}
//int main()
//{
//	const Base & b = getderived();
//}