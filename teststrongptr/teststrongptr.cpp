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
//	ptr->~Ty(); //���õ�ǰ���͵���������
//}

//int main()
//{
//	Derived d;
//	const Base & b = d;
//	int *pInt = new int(0);
//	//pInt->~int(); //�������벻ͨ��
//	Destroy_Test(pInt); //������û����
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