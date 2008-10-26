#include <iostream>
#include "Singleton.h"

using namespace std;

class CSayCheese : public Singleton<CSayCheese>
{
public:
	CSayCheese() : m_nCnt(0) { }
	~CSayCheese() { }

	void SayIt(char *str)
	{
		cout << "I am saying \"" << str << "\" for the " << ++m_nCnt << " time." << endl;
	}

protected:
	int m_nCnt;
};

typedef Singleton<CSayCheese> SayCheeseStonT;

