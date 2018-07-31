#pragma once
#include <new>
#include "TypeTraits.h"
#include "IteratorTraits.h"


template<class T1, class T2>
void Construct(T1* p, const T2& value)
{
	cout << "����" << typeid(T1).name() << "���캯��" << endl;
	new(p) T1(value);
}

template<class T>
void Destroy(T* p)
{
	cout << "����" << typeid(T).name() << "��������" << endl;
	p->~T();
}



// O(N)
// List---->[Begin(), End() )
template<class Iterator>
void _Destroy(Iterator first, Iterator last, FalseType)
{
	while(first != last)
		Destroy(&(*first++));
}


// O(1)
//����洢�����������ͣ��Ͳ��õ���������
template<class Iterator>
void _Destroy(Iterator first, Iterator last, TrueType)
{}


template<class Iterator>
void Destroy(Iterator first, Iterator last)
{
	_Destroy(first, last, TypeTraits<IteratorTraits<Iterator>::ValueType>::PODType());
}



