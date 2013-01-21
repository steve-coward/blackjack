///////////////////////////////////////////////////////////////////////////////
// Written by Steve Coward
// PSU CS 202 Fall 2009 Project 3
//
// Iterator class to accompany CList class.
//
//	Supports forward and reverse iteration through a list.
//
//  Contains the following functions:
//
//	int GetFirst();
//	int GetLast();
//		Iteration initialization.
//
//	int GetNext();
//	int operator++(); // prefix
//	int operator++(int); // postfix
//	int GetPrev();
//	int operator--(); // prefix
//	int operator--(int); // postfix
//		Updating of iterator.
//
//	void Iterate(void (*f)(T *ip));
//		Apply function f to each node of list.
//
//	T* CurrentItem();
//		Get current item pointed to by iterator.
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(LISTITER_H)
#define LISTITER_H

//#include "stdafx.h"
#include "linkedList.h"

template <typename T>
class listIter
{
public:
	listIter(const CList<T> *ptr);
	listIter(const CList<T> &ref);
	int GetFirst();
	int GetLast();
	int GetNext();
	int GetPrev();
	int operator++(); // prefix
	int operator++(int); // postfix
	int operator--(); // prefix
	int operator--(int); // postfix
	void Iterate(void (*f)(T *ip));
	T* CurrentItem();

private:
	const CList<T> *m_pList;
	node<T>* m_currNode;
};


template <typename T>
listIter<T>::listIter(const CList<T> *ptr) : m_pList(ptr)
{
	GetFirst(); // set m_currNode to head
}

template <typename T>
listIter<T>::listIter(const CList<T> &ref) : m_pList(&ref)
{
	m_currNode = ref.GetHeadNode();
}

template <typename T>
int listIter<T>::GetFirst()
{
	if (m_pList == NULL) {
		m_currNode = NULL;
	}
	else {
		m_currNode = m_pList->GetHeadNode();
	}

	return(m_currNode != NULL);
}

template <typename T>
int listIter<T>::GetLast()
{
	if (m_pList == NULL) {
		m_currNode = NULL;
	}
	else {
		m_currNode = m_pList->GetTailNode();
	}

	return(m_currNode != NULL);
}

template <typename T>
int listIter<T>::operator++()
{
	if (m_currNode != NULL) {
		m_currNode = m_currNode->m_next;
	}

	return(m_currNode != NULL);
}
	
template <typename T>
int listIter<T>::operator++(int)
{
	if (m_currNode != NULL) {
		m_currNode = m_currNode->m_next;
	}

	return(m_currNode != NULL);
}

template <typename T>
int listIter<T>::operator--()
{
	if (m_currNode != NULL) {
		m_currNode = m_currNode->m_prev;
	}

	return(m_currNode != NULL);
}
	
template <typename T>
int listIter<T>::operator--(int)
{
	if (m_currNode != NULL) {
		m_currNode = m_currNode->m_prev;
	}

	return(m_currNode != NULL);
}

template <typename T>
int listIter<T>::GetNext()
{
	if (m_currNode != NULL) {
		m_currNode = m_currNode->m_next;
	}

	return(m_currNode != NULL);
}

template <typename T>
int listIter<T>::GetPrev()
{
	if (m_currNode != NULL) {
		m_currNode = m_currNode->m_prev;
	}

	return(m_currNode != NULL);
}

template <typename T>
void listIter<T>::Iterate(void (*f)(T *ip))
{
	if (!GetFirst()) {
		return;
	}

	do {
		f(CurrentItem());
	} while (GetNext());
}

template <typename T>
T* listIter<T>::CurrentItem()
{
	if (m_currNode == NULL) {
		return(NULL);
	}
	else {
		return(m_currNode->m_ip);
	}
}

#endif // LISTITER_H