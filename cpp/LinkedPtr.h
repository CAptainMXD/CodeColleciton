/*******************************************************
 * Copyright (c) 2014, QingYun All rights reserved.
 * 
 * FileName: LinkedPtr.h
 * Summary: 将被其他地方使用的指针链接在一起管理，预防野指针
 * Author: fengqk
 * Date: 2014/08/11  23:06
 *******************************************************/

template<class T>
class LinkedPtr
{
	public:
		LinkedPtr();
		LinkedPtr(const LinkedPtr<T>& other);
		virtual ~LinkedPtr();
	public:
		inline T* operator->();
		inline T& operator* ();
		inline operator T*();
		inline void operator =(const LinkedPtr<T>& other);
		inline bool operator ==(const LinkedPtr<T>& other);
		inline T* Get();
	public:
		inline void Init(T* pObject);
		inline void Reset();
	private:
		inline void Link(const LinkedPtr<T>& other);
	private:
		T *m_pObject;
		LinkedPtr<T>* m_pNext;
		LinkedPtr<T>* m_pPrev;
};
template<class T>
LinkedPtr<T>::LinkedPtr()
	: m_pObject(NULL)
	, m_pNext(NULL)
	  , m_pPrev(NULL)
{
}

template<class T>
LinkedPtr<T>::LinkedPtr(const LinkedPtr<T>& other)
	: m_pObject(NULL)
	, m_pNext(NULL)
	  , m_pPrev(NULL)
{
	this->Link(other);
}




template<class T>
LinkedPtr<T>::~LinkedPtr()
{
	this->Reset();
}



template<class T>
T* LinkedPtr<T>::Get()
{
	return m_pObject;
}


template<class T>
void LinkedPtr<T>::Init( T* pObject )
{
	m_pObject = pObject;
	m_pPrev = NULL;
	m_pNext = NULL;
}


template<class T>
void LinkedPtr<T>::operator =(const LinkedPtr<T>& other)
{
	this->Link(other);
}

template<class T>
bool LinkedPtr<T>::operator ==(const LinkedPtr<T>& other)
{
	return m_pObject == other.m_pObject;
}


template<class T>
LinkedPtr<T>::operator T*()
{
	return m_pObject;
}


template<class T>
T& LinkedPtr<T>::operator*()
{
	return *m_pObject;
}


template<class T>
T* LinkedPtr<T>::operator->()
{
	return m_pObject;
}


template<class T>
void LinkedPtr<T>::Reset()
{
	if(m_pPrev == NULL)
	{
		while(m_pNext)
		{
			m_pNext->Reset();
		}
	}
	if(m_pPrev != NULL)
	{
		m_pPrev->m_pNext = m_pNext;
	}
	if(m_pNext != NULL)
	{
		m_pNext->m_pPrev = m_pPrev;
	}
	m_pPrev = NULL;
	m_pNext = NULL;
	m_pObject = NULL;
}


template<class T>
void LinkedPtr<T>::Link(const LinkedPtr<T>& other)
{
	LinkedPtr<T>& _other = const_cast<LinkedPtr<T>&>(other);

	if(_other.m_pObject == m_pObject)
		return;
	if(_other.m_pObject == NULL)
		return;

	this->Reset();

	m_pPrev = &_other;
	m_pNext = _other.m_pNext;
	if(m_pNext)
	{
		m_pNext->m_pPrev = this;
	}
	m_pObject = _other.m_pObject;
	_other.m_pNext = this;
}
