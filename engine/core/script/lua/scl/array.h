////////////////////////////////////////////////////////////////////////////////
//	array and parray
//	2010.04.24 caolei
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "scl/type.h"
#include "scl/assert.h"
#include "scl/math.h"

//#include <stdlib.h> //for qsort
#include <algorithm> //for std sort
#if defined(SCL_LINUX) || defined(SCL_APPLE)
#include <string.h>
#endif

#ifdef min
#undef min
#endif
#ifdef max 
#undef max 
#endif
namespace scl{

////////////////////////////////////////////////////////////////////////////////
//	class array
////////////////////////////////////////////////////////////////////////////////
template<typename T, int MAX_SIZE>
class array
{
public:

	array(): m_size(0) {}

	//stl���ݽӿڡ�iterator��ʱ��ΪT*
	typedef T*			iterator;
	typedef const T*	const_iterator;
	typedef T			value_type;
	typedef int			size_type;
	void	zero_memory	()						{ memset(m_array, 0, sizeof(m_array)); }
	void	push_back	(const T& elem);
	void	erase		(const int index);
	void	clear		()						{ m_size = 0; }
	int		size		() const 				{ return m_size; };
	bool	full		() const				{ return m_size == MAX_SIZE; }
	int&	size_ref	()  					{ return m_size; };
	int		max_size	() const 				{ return MAX_SIZE; }
	T*		begin		()						{ return m_array; }
	T*		end			()						{ return &m_array[m_size]; }
	const T* begin		() const				{ return m_array; }
	const T* end		() const				{ return &(m_array[m_size]); }
	T*		rbegin		()						{ return &(m_array[m_size - 1]); }
	T*		rend		()						{ return &m_array[-1]; }
	void	resize		(const int size)		{ assert(size <= MAX_SIZE); m_size = size; };
	void	reserve		(const int capacity)	{ assert(0); } //��̬���鲻��reserve
	bool	empty		() const				{ return m_size == 0; }

	template<class _Pr>
	inline void	sort (_Pr _Pred)
	{ 
		std::sort(m_array, m_array + m_size, _Pred);
	}

	inline void	sort ()
	{ 
		std::sort(m_array, m_array + m_size);
	}

	template <class _Pr>
	inline int min(_Pr _Pred) const
	{
		int minIndex = -1;
		int minValue = scl::MAX_INT;
		for (int i = 0; i < size(); ++i)
		{
			int v = _Pred(m_array[i]);
			if (v >= minValue)
				continue;
			minValue = v;
			minIndex = i;
		}
		return minIndex;
	}

	template <class _Pr>
	inline int max(_Pr _Pred) const
	{
		int maxIndex = -1;
		int maxValue = scl::MIN_INT;
		for (int i = 0; i < size(); ++i)
		{
			int v = _Pred(m_array[i]);
			if (v <= maxValue)
				continue;
			maxValue = v;
			maxIndex = i;
		}
		return maxIndex;
	}

	//stl��û����Ӧ�Ĺ��ܽӿ�
	T&		push_back_fast		();
	void	erase_fast			(const int index);
	void	erase_element		(const T& elem);
	void	erase_element_fast	(const T& elem);
	int		find				(const T& elem) const;
	bool	contains			(const T& elem) const { return find(elem) != -1; }
	void	assign				(const T* other, const int count);
	int		capacity			() const { return MAX_SIZE; }

	const T& 	operator[]	(int index) const { if (index >= MAX_SIZE || index < 0) { assert(0);throw 1; } return m_array[index]; }
	T&		 	operator[]	(int index)		 { if (index >= MAX_SIZE || index < 0) { assert(0);throw 1; } return m_array[index]; }
	T*		 	c_array		()			{ return m_array; }
	const T* 	c_array		() const	{ return m_array; }
	bool		operator==	(const array<T, MAX_SIZE>& a) const;
	bool		operator!=	(const array<T, MAX_SIZE>& a) const;
	void	reverse		()
	{
		for (int i = 0; i < m_size / 2; ++i)
		{
			swapmem2(m_array[i], m_array[m_size - 1 - i]);
		}
	}

	template <typename StreamerT>
	void map(StreamerT& s)
	{
		s << m_size;
		if (m_size < 0 || m_size > MAX_SIZE)
			return;
		for (int i = 0; i < size(); ++i)
		{
			s << m_array[i];
		}
	}

protected:
	T	m_array[MAX_SIZE];
	int m_size;
};

template<typename T, int MAX_SIZE>
bool array<T, MAX_SIZE>::operator==(const array<T, MAX_SIZE>& other) const
{
	if (this->size() != other.size())
		return false;
	for (int i = 0; i < size(); ++i)
	{
		if (m_array[i] != other[i])
			return false;
	}
	return true;
}

template<typename T, int MAX_SIZE>
bool array<T, MAX_SIZE>::operator!=(const array<T, MAX_SIZE>& other) const
{
	return !(this->operator==(other));
}

template<typename T>
inline void swapmem(T& a, T& b)
{
	if (&a == &b)
		return;
	//if (a == b)  //operator==ֻ�ṩ���߼��ϵ�������ж�
	//	return;
	int* pIntA = reinterpret_cast<int*>(&a);
	int* pIntB = reinterpret_cast<int*>(&b);
	for (uint i = 0; i < sizeof(T) / sizeof(int); ++i)
	{
		int t = *pIntA;
		*pIntA = *pIntB;
		*pIntB = t;
		++pIntA;
		++pIntB;
	}

	byte* pByteA = reinterpret_cast<byte*>(pIntA);
	byte* pByteB = reinterpret_cast<byte*>(pIntB);
	for (uint j = 0; j < sizeof(T) % sizeof(int); ++j)
	{
		byte t = *pByteA;
		*pByteA = *pByteB;
		*pByteB = t;
		++pByteA;
		++pByteB;
	}
}

template<typename T, int MAX_SIZE>
void array<T, MAX_SIZE>::push_back(const T& elem)
{
	if (m_size >= MAX_SIZE)
	{
		assert(0);
		return;
	}
	m_array[m_size++] = elem;
}

template<typename T, int MAX_SIZE>
T& array<T, MAX_SIZE>::push_back_fast()
{
	if (m_size >= MAX_SIZE)
	{
		assert(0);
		return m_array[MAX_SIZE - 1];
	}
	m_size++;
	return m_array[m_size - 1];
}


template<typename T, int MAX_SIZE>
void array<T, MAX_SIZE>::erase_element(const T& elem)
{
	int delIdx = find(elem);
	if (-1 == delIdx)
		return;

	erase(delIdx);
}

template<typename T, int MAX_SIZE>
void array<T, MAX_SIZE>::erase(const int index)
{
	if (index < 0 || index >= m_size)
		return;

	for (int i = index; i < m_size - 1; ++i)
	{
		//m_array[i] = m_array[i + 1];
		swapmem(m_array[i], m_array[i + 1]);
	}
	m_size--;
}

template<typename T, int MAX_SIZE>
void array<T, MAX_SIZE>::erase_element_fast(const T& elem)
{
	int delIdx = find(elem);
	if (-1 == delIdx)
		return;

	erase_fast(delIdx);
}

template<typename T, int MAX_SIZE>
void array<T, MAX_SIZE>::erase_fast(const int index)
{
	if (index < 0 || index >= m_size)
	{
		assert(0);
		return;
	}

	//����Ҫ������������T�а���ָ���ʱ��
	//�ᵼ��m_array[m_count - 1]��������deleteͬһ��ָ�����Σ�m_array[index]�л���һ����ͬ��ָ�룩
	//ͬʱ��������һ���µ�T temp���������м��������Ϊ�����ᵼ��temp������
	//ʹ��ͬ����ͬһ��ָ�뱻����delete�����⣨temp�ڵ�ָ��ͱ�temp�������elem�ڵ�ָ�룩
	//2013.10.15 ��T������Ϊָ��ʱ��swapmem�ڶ��߳��²���ȫ��ֱ�ӽ�����ȫ�������ᵽ�����⣬����T�����Ŀ������캯����������Ǳ����������
	//m_array[index] = m_array[m_count - 1];
	if (index != m_size - 1)
	{
		//m_array[index] = m_array[m_size - 1];
		swapmem(m_array[index], m_array[m_size - 1]);
	}
	m_size--;
}

template<typename T, int MAX_SIZE>
int array<T, MAX_SIZE>::find(const T& elem) const
{
	int foundIdx = -1;
	for (int i = 0; i < m_size; ++i)
	{
		if (m_array[i] == elem)
		{
			foundIdx = i;
			break;
		}
	}
	return foundIdx;
}

template<typename T, int MAX_SIZE>
void array<T, MAX_SIZE>::assign(const T* other, const int count)
{
	if (count == 0)
	{
		m_size = 0;
		return;
	}
	if (count > MAX_SIZE)
	{
		return;
	}

	//��ֹ�ڴ��غ�
	assert(m_array != other);
	if (m_array < other)
		assert(m_array + count < other);
	if (other < m_array)
		assert(other + count < m_array);

	memcpy(m_array, other, count * sizeof(T));
	m_size = count;
}

//template<typename T, int MAX_SIZE>
//void array<T, MAX_SIZE>::assign(const T* other, const int count)
//{
//	if (count > m_maxSize)
//	{
//		assert(false);
//		return;
//	}
//	memcpy(m_array, other, count * sizeof(T));
//	m_size = count;
//}

template <typename T>
class parray
{
public:
	parray() { m_array = NULL; m_size = 0; m_capacity = 0; }

	template <int N>
	parray(array<T, N>& a) { m_array = a.c_array(); m_size = a.size(); m_capacity = a.capacity(); }

	void		assign	(T* arr, const int size, const int capacity) { m_array = arr; m_size = size; m_capacity = capacity; }
	int			capacity() const	{ return m_capacity;	}
	int			size	() const	{ return m_size;		}
	T*			c_array	()			{ return m_array;		}
	const T*	c_array	() const	{ return m_array;		}

	const T&	operator[]	(int index) const	{ if (index >= m_capacity || index < 0) { assert(0);throw 1; } return m_array[index]; }
	T&		 	operator[]	(int index)			{ if (index >= m_capacity || index < 0) { assert(0);throw 1; } return m_array[index]; }

private:
	int			m_size;
	int			m_capacity;
	T*			m_array;
};

template <typename T>
class parray_const
{
public:
	parray_const() { m_array = NULL; m_size = 0; m_capacity = 0; }

	template <int N>
	parray_const(const array<T, N>& a) { m_array = a.c_array(); m_size = a.size(); m_capacity = a.capacity(); }

	void		assign	(T* arr, const int size, const int capacity) { m_array = arr; m_size = size; m_capacity = capacity; }
	int			capacity() const	{ return m_capacity;	}
	int			size	() const	{ return m_size;		}
	const T*	c_array	() const	{ return m_array;		}

	const T&	operator[]	(int index) const	{ if (index >= m_capacity || index < 0) { assert(0);throw 1; } return m_array[index]; }

private:
	int			m_size;
	int			m_capacity;
	const T*	m_array;
};



} //namespace scl
