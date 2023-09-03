#pragma once
#include <cstdlib>
#include <iostream>
template <class T>
class SingletonHolder
{
public:
	static T* instance()
	{
		static bool _initialized = false;
		if (!_initialized)
		{
			if (std::atexit(&SingletonHolder<T>::_destroy))
			{
				std::cerr << "Singleton registration failed\n";
				std::exit(EXIT_FAILURE);
			}
			_construct();
			_initialized = true;
		}
		return _pInstance;
	}
private:
	inline static T* _pInstance = nullptr;
	static void _destroy()
	{
		delete _pInstance;
	}
	static void _construct()
	{
		_pInstance = new T;
	}
};