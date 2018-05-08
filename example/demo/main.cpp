/**
 *
 *  作者: hm
 *
 *  说明: 主函数
 *
 */


#include "main.h"


class SafeBool
{
public:
	typedef void (SafeBool::*type)() const;

	static type makeSafe(bool value)
	{
		return value ? &SafeBool::trueValue : nullptr;
	}

private:
	void trueValue() const {}
};

template<typename T>
class A
{
public:
	A() = default;

	explicit A(T * p) : m_p(p)
	{

	}

	operator SafeBool::type() const
	{
		return SafeBool::makeSafe(m_p != nullptr);
	}

private:
	T * m_p{ nullptr };
};


int main(int argc, char const * argv[])
{
	int x = 1;

	A<int> a;

	if (a)
	{
		std::cout << "a is init" << std::endl;
	}
	else
	{
		std::cout << "a no init" << std::endl;
	}

	A<int> b(&x);

	if (b)
	{
		std::cout << "b is init" << std::endl;
	}
	else
	{
		std::cout << "b no init" << std::endl;
	}

	return 0;
}
