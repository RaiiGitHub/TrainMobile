#pragma once
namespace MobileStudio
{
	template<typename T>
	class Singleton {
	public:
		static T *GetInstance() {
			static  T _instance;
			return &_instance;
		}
	protected:
		Singleton(void) {};
		Singleton(const Singleton<T> &);
		Singleton& operator=(const Singleton<T> &);
	};
};
