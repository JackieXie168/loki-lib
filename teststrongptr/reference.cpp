#include <iostream>
void foo(int &)
{
	std::cout<<"hello,world\n";
}
void foo(const int &)
{
	std::cout<<"HELLO,WORLD\n";
}
//int main()
//{
//	int i = 100;
//	foo(i);
//}