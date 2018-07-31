#pragma once
/////�������ֲ�ͬ���͵ĵ�����������Differenceʱ����ݲ�ͬ���͵ĵ�����������Ӧ�ĺ���
struct InputIteratorTag
{};

struct OutputIteratorTag
{};

struct ForwardIteratorTag : public InputIteratorTag
{};

struct BidirectionalIteratorTag : public ForwardIteratorTag
{};

struct RandomAccessIteratorTag : public BidirectionalIteratorTag
{};


template <class T, class Distance> 
struct BidirectionalIterator
{
	typedef BidirectionalIteratorTag IteratorCategory;
	typedef T                          ValueType;
	typedef Distance                   DifferenceType;
	typedef T*                         Pointer;
	typedef T&                         Reference;
};

template <class T, class Distance> 
struct RandomAccessIterator
{
	typedef RandomAccessIteratorTag IteratorCategory;
	typedef T                          ValueType;
	typedef Distance                   DifferenceType;
	typedef T*                         Pointer;
	typedef T&                         Reference;
};

// ���������Զ���ĵ�����
template<class Iterator>
struct IteratorTraits
{
	typename typedef Iterator::IteratorCategory IteratorCategory;
	typename typedef Iterator::ValueType ValueType;
	typename typedef Iterator::Reference Reference;
	typename typedef Iterator::Pointer Pointer;
	typename typedef Iterator::DifferenceType DifferenceType;
};

// ƫ�ػ�
/////////�����������͵ĵ�������ԭ��ָ�룬����Ҫ���⴦��
template<class T>
struct IteratorTraits<T*>
{
	typedef RandomAccessIteratorTag IteratorCategory;
	typedef T ValueType;
	typedef T& Reference;
	typedef T* Pointer;
	typedef int  DifferenceType;
};

// STL: ׷��Ч��
// ˫�� O(N)
template<class Iterator>
typename IteratorTraits<Iterator>::DifferenceType Difference(Iterator first, Iterator last, BidirectionalIteratorTag)
{
	typename IteratorTraits<Iterator>::DifferenceType diff = 0;
	while(first != last)
	{
		++diff;
		++first;
	}
	return diff;
}

// O(1)
template<class Iterator>
typename IteratorTraits<Iterator>::DifferenceType Difference(Iterator first, Iterator last, RandomAccessIteratorTag)
{
	return last - first;
}

// STL: Ч��
template<class Iterator>
typename IteratorTraits<Iterator>::DifferenceType Difference(Iterator first, Iterator last)
{
	return Difference(first, last, IteratorTraits<Iterator>::IteratorCategory());
}


// ReverseIterator: �����������������
template<class Iterator>
class ReverseIterator
{
	typedef ReverseIterator<Iterator> Self;

public:

	typename typedef IteratorTraits<Iterator>::IteratorCategory IteratorCategory;
	typename typedef IteratorTraits<Iterator>::ValueType ValueType;
	typename typedef IteratorTraits<Iterator>::Reference Reference;
	typename typedef IteratorTraits<Iterator>::Pointer Pointer;
	typename typedef IteratorTraits<Iterator>::DifferenceType DifferenceType;

public:
	ReverseIterator()
	{}

	ReverseIterator(Iterator it)
		: _it(it)
	{}

	ReverseIterator(Self& s)
		: _it(s._it)
	{}

	Reference operator*()
	{
		Iterator temp(_it);
		--temp;
		return *temp;
	}

	Pointer operator->()
	{
		return &(operator*());
	}

	Self& operator++()
	{
		--_it;
		return *this;
	}

	Self operator++(int)
	{
		Iterator temp(_it);
		_it--;
		return temp;
	}

	Self& operator--()
	{
		++_it;
		return *this;
	}

	Self operator--(int)
	{
		Iterator temp(_it);
		_it++;
		return temp;
	}

	bool operator!=(const Self& s)
	{
		return _it != s._it;
	}

	bool operator==(const Self& s)
	{
		return _it == s._it;
	}

private:
	Iterator _it;
};
