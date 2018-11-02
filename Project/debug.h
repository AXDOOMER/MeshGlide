#include <iostream>
#include <string>
#include <cstdarg>

using namespace std;

void Print(string info, auto var)
{
	cout << info << ": " << var << endl;
}

template<typename... Arguments>  
void Printer(string info, const Arguments&... args)  
{  
	unsigned int numargs = sizeof...(Arguments);

	cout << info << ":" << endl;

	for (int i = 0; i < numargs; i++)
	{
		cout << "\t" << args[i] << endl;
	}

	cout << endl;
}  

/*
void Print(string info, ...)
{
	va_list args;
	va_start(args, info);

	cout << info << ":";

	for (int i = 0; i < count; ++i) {
		arg = va_arg(args, int);
		cout "\t" << arg;
	}

	cout << endl;

	va_end(args);
}
*/
void Break(string info, auto var)
{
	Print(info, var);
	cin.get();
}
