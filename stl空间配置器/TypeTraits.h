#pragma once

// POD
struct TrueType
{};

struct FalseType
{};

template<class T>
struct TypeTraits
{
	typedef FalseType PODType;
};

//////ƫ�ػ� ��ȡ���������ͣ�����destory()��ʱ������Ч��
template<>
struct TypeTraits<int>
{
	typedef TrueType PODType;
};

template<>
struct TypeTraits<char>
{
	typedef TrueType PODType;
};



