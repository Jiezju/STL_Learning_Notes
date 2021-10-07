#include "MyString.h"
#include <typeinfo> // typeid()
#include <ctime>
#include <vector>

template<typename T>
void output_static_data(const T& myStr)
{
	cout << typeid(myStr).name() << "--" << endl;
	cout << "CCtor = " << T::CCtor << endl;
	cout << "MCtor = " << T::MCtor << endl;
	cout << "CAsgn = " << T::CAsgn << endl;
	cout << "MAsgn = " << T::MAsgn << endl;
	cout << "Dtor = " << T::Dtor << endl;
	cout << "Ctor = " << T::Ctor << endl;
	cout << "DCtor = " << T::DCtor << endl;
}

template<typename M>
void test_moveable(M c1, long& num)
{
	char buf[10];
	//  测试moveable
	// typename iterator_traits<typename M::iterator>::value_type
	// 查询 M::iterator 的 value_type 并重命名为 Vtype
	typedef typename iterator_traits<typename M::iterator>::value_type Vtype;

	clock_t timeStart = clock();

	for (long i=0;i< num;i++)
	{
		snprintf(buf, 10, "%d", rand());
		auto ite = c1.end();
		c1.insert(ite, Vtype(buf));
	}

	cout << "Construction, milli-seconds: " << (clock() - timeStart) << endl;
	cout << "size: " << c1.size() << endl;
	output_static_data(*(c1.begin()));

	timeStart = clock();
	M c11(c1); // 深拷贝 涉及到数据的搬运
	cout << "CopyConstruction, milli-seconds: " << (clock() - timeStart) << endl;
	cout << "size: " << c1.size() << endl;
	output_static_data(*(c1.begin()));

	timeStart = clock();
	M c12(std::move(c1)); // 浅拷贝 只是指针移动
	cout << "MoveCopyConstruction, milli-seconds: " << (clock() - timeStart) << endl;
	cout << "size: " << c12.size() << endl;
	output_static_data(*(c12.begin()));

	timeStart = clock();
	c11.swap(c12);
	cout << "MoveCopyConstruction, milli-seconds: " << (clock() - timeStart) << endl;
	cout << "size: " << c11.size() << endl;
	output_static_data(*(c11.begin()));
}

int main()
{
	long num = 300000;
	test_moveable(vector<MyString>(), num);
	return 0;
}

