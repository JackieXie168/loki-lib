#include <loki/MinMax.h>
#include <iostream>

#include <boost/>
//int foo()
//{
//	return 0;
//}
//void goo( int & a)
//{
//}
//template<class Ty> inline
//void Destroy_Test(Ty  *ptr)
//{
//	ptr->~Ty(); //调用当前类型的析构函数
//}

//int main()
//{
//	Derived d;
//	const Base & b = d;
//	int *pInt = new int(0);
//	//pInt->~int(); //这样编译不通过
//	Destroy_Test(pInt); //这样就没问题
//}
//int main()
//{
//	goo(foo());
//}
int main()
{
	int a = 10;
	int b =20;
	int i = Max(a, b);
	std::cout<<i<<"\n";
}