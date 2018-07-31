#include"alloc.h"
template<class T,class alloc>
class SimpleAlloc
{
public:
	//����n��T���͵Ĵ�С�Ŀռ�
	static T*Allocate(size_t n)
	{
		return n == 0 ? 0 : (T*)alloc::Allocate(n*sizeof(T));
	}
	//����1��T���͵Ĵ�С�Ŀռ�
	static T*Allocate()
	{
		return (T*)alloc::Allocate(sizeof(T));
	}
	//�ͷ�n��T���͵Ĵ�С�Ŀռ�
	static void Deallocate(T*p, size_t n)
	{
		if (n != 0)
			alloc::Deallocate(p, n*sizeof(T));
	}
	//�ͷ�1��T���͵Ĵ�С�Ŀռ�
	static void Deallocate(T*p)
	{
		alloc::Deallocate(p, sizeof(T));
	}
};