%module ScriptEngine
%include "typemaps.i"

%{
#include "CSayCheese.h"

CSayCheese* getCSayCheese() {
	return SayCheeseStonT::getInstancePtr();
};
%}

template <typename T> class Singleton {
	static T& getInstance();
	static T* getInstancePtr();
};

%template(CSayCheeseSton) Singleton<CSayCheese>;
class CSayCheese : public Singleton<CSayCheese> {
public:
	CSayCheese() : m_nCnt(0) { }
	~CSayCheese() { }

	void SayIt(char *str)
	{
		cout << "I am saying \"say cheese\" for the " << ++m_nCnt << " time." << endl;
	}

protected:
	int m_nCnt;
};

CSayCheese* getCSayCheese();

