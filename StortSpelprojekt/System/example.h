#ifndef EXAMPLE_H
#define EXAMPLE_H

#define SYSTEM_EXPORT __declspec(dllexport)

namespace System
{
	class SYSTEM_EXPORT Example 
	{
	private:
		int _value;
	public:
		Example(int value);
		int GetValue()const;
	};
}

#endif