// Written by Steve Coward
// PSU CS 202 Fall 2009 Project 1
//
// linkedList.h: interface of the CLinkedList class.
//  CLinkedList is a templated doubly linked list data structure.
//
//	ISSUE: asserts should be added to check sanity/consistency of list state.
//	ISSUE:
//
//	CLinkedList contains the following member variables:
//
//public:
//	const tnode<T>* getFirstNode(void) const;
//		returns a ptr to the front of the list
//
//	const tnode<T>* getLastNode(void) const;
//		returns a ptr to the back of the list
//
//
//public:
//
//	void CLinkedList<T>::clear(void)
//		remove all nodes from the list
//
//	void CLinkedList<T>::operator= (const CLinkedList<T>& rhs)
//		clear all nodes from lhs and then copy each node from rhs to lhs
//
//	bool CLinkedList<T>::operator== (const CLinkedList<T>& rhs) const
//		return true if each node of lhs == corresponding node of rhs
//
//	bool CLinkedList<T>::operator!= (const CLinkedList<T>& rhs) const
//		return true if any node of lhs != corresponding node of rhs
//
//	bool CLinkedList<T>::empty(void) const
//		return true if list is empty, otherwise return false
//
//	int CLinkedList<T>::size(void) const
//		return the number of nodes contained in the list
//
//	void CLinkedList<T>::swapPos(int nodePos1, int nodePos2)
//		swap the positions of two nodes whose zero-based indices are nodePos1 and nodePos2
//
//	void CLinkedList<T>::swap(tnode<T>* node1, tnode<T>* node2)
//		swap the positions of two nodes node1 and node2
//
//	void CLinkedList<T>::push_front(const T nodeData)
//		push nodeData onto the front of the list
//
//	void CLinkedList<T>::push_back(const T nodeData)
//		push nodeData onto the back of the list
//
//	const T CLinkedList<T>::front(void) const
//		return the front of the list
//
//	const T CLinkedList<T>::back(void) const
//		return the back of the list
//
//	const T CLinkedList<T>::pop_front(void)
//		remove and return the front of the list
//
//	const T CLinkedList<T>::pop_back(void)
//		remove and return the back of the list
//
//	T CLinkedList<T>::erase(int i)
//		remove and return the node whose zero-based indice is i
//
//	bool CLinkedList<T>::testDriveSwap(void) const
//		A debug routine that checks the correctness of the swap() routines.
//		All manipulations are performed on a temporary deck initially set equal to *this (deck).
//		Return false if an error is found.
//
//private:
//	tnode<T>* CLinkedList<T>::get(int i) const
//		return the node whose zero-based indice is i
//////////////////////////////////////////////////////////////////////

#include <iostream> 
#include <assert.h>

#if !defined(LINKEDLIST_H)
#define LINKEDLIST_H

template <typename T> 
struct tnode
{
	T data;
	tnode<T> *next;
	tnode<T> *prev;
};

template <class T>
class CLinkedList
{
private:
	tnode<T> *m_first;
public:
	const tnode<T>* getFirstNode(void) const { return(m_first); }

private:
	tnode<T> *m_last;
public:
	const tnode<T>* getLastNode(void) const { return(m_last); }

private:
	int m_size;

	tnode<T>* CLinkedList<T>::get(int i) const
	{
		int count = 0;
		tnode<T>* currentNode;

		currentNode = m_first;
		if (i < m_size) {
			while (count++ < i) {
				currentNode = currentNode->next;
			}

			// currentNode is in location pos
			return(currentNode);
		}
		else {
			return(NULL);
		}
	}

public:

#if defined(_DEBUG)
	// a debug routine that checks the correctness of the swap routines.
	// All manipulations are performed on a temporary deck initially set equal to *this (deck).
	// Return false if an error is found.
	bool CLinkedList<T>::testDriveSwap(void) const
	{
		int i, j;
		CLinkedList<T> origList;
	
		origList = *this;

		if (origList.m_size > 1) {
			for (i=0; i<origList.m_size; i++) {
				for (j=0; j<origList.m_size-1; j++) {
					origList.swapPos(j, (j+1) % origList.m_size);
				}
			}
			if (origList != *this) {
				assert(false);
				return(false);
			}

			origList.swapPos(0, origList.m_size-2);
			origList.swapPos(origList.m_size-2, 0);
			if (origList != *this) {
				assert(false);
				return(false);
			}

			origList.swapPos(1, origList.m_size-1);
			origList.swapPos(origList.m_size-1, 1);
			if (origList != *this) {
				assert(false);
				return(false);
			}
		}

		if (origList.m_size > 0) {
			origList.swapPos(0, origList.m_size-1);
			origList.swapPos(origList.m_size-1, 0);
			if (origList != *this) {
				assert(false);
				return(false);
			}
		}

		// this swap should not be performed
		origList.swapPos(-1, 0);
		if (origList != *this) {
			assert(false);
			return(false);
		}

		// this swap should not be performed
		origList.swapPos(origList.m_size, 0);
		if (origList != *this) {
			assert(false);
			return(false);
		}

		return(true);
	}
#endif // _DEBUG

	CLinkedList<T>::CLinkedList(void)
	{
		m_first = NULL;
		m_last = NULL;
		m_size = 0;
		return;
	}

	// ISSUE: this causes (within Visual C++ 2008 Express Edition) -
	// ISSUE: fatal error C1001: An internal error has occurred in the compiler.
	// ISSUE: problem seems to be related to duplicate constructor definitions
	// ISSUE: even though argument list differs.
	/*
	CLinkedList<T>::CLinkedList(const CLinkedList<T>& oldList)
	{
		// only shallow copy performed here
		this->m_first = oldList.m_first;
		this->m_last = oldList.m_last;
		return;
	}
	*/

	CLinkedList<T>::~CLinkedList(void)
	{
		clear();
		return;
	}


	void CLinkedList<T>::operator= (const CLinkedList<T>& rhs)
	{
		clear();
		m_first = NULL;
		m_last = NULL;

		if (rhs.m_first != NULL) {
			// initialize a node to the first node of rhs
			tnode<T>* pCurrentNode = rhs.m_first;

			while (pCurrentNode != NULL) {				
				// push onto the front of lhs
				push_back(pCurrentNode->data);

				// get next node in rhs to copy into new list
				pCurrentNode = pCurrentNode->next;
			}
		}

		return;
	}

	bool CLinkedList<T>::operator== (const CLinkedList<T>& rhs) const
	{
		tnode<T>* pLHNode;
		tnode<T>* pRHNode;

		pLHNode = this->m_first;
		pRHNode = rhs.m_first;
		
		while ((pLHNode != NULL) && (pRHNode != NULL)) {
			if (pLHNode->data != pRHNode->data) {
				return(false);
			}

			pLHNode = pLHNode->next;
			pRHNode = pRHNode->next;
		}

		return(true);
	}

	bool CLinkedList<T>::operator!= (const CLinkedList<T>& rhs) const
	{
		return(!(*this == rhs));
	}

	bool CLinkedList<T>::empty(void) const
	{
		return(m_first == NULL);
	}

	int CLinkedList<T>::size(void) const
	{
		return(m_size);
	}

	// The process of swapping two nodes is fairly complex.
	// Boundary cases include swapping the front and/or back of the list,
	// swapping neighboring nodes and swapping non-neighboring nodes.
	// The number of pointer updates required for the swap can range from
	// a minimum of 3 (neighboring nodes at front/back of the list) to
	// a maximum of 8 (non neighboring nodes with neither node at
	// front/back of the list).
	//
	// To test this complexity, the function testDriveSwap() can be used.
	//
	void CLinkedList<T>::swap(tnode<T>* node1, tnode<T>* node2)
	{
		tnode<T> tmpNode;

		if ((node1 != NULL) && (node2 != NULL)) {
			// save node1 ptrs
			// data is not swapped, only position in list changes
			tmpNode.next = node1->next;
			tmpNode.prev = node1->prev;

			// adjust node1 neighbors to point to node2
			if ((node1->prev != NULL) && (node1->prev != node2)) {
				// node1 is not at front of the list and
				// node2 does not immediately precede node1
				node1->prev->next = node2;
			}
			else if (node1->prev == NULL) {
				// node1 is at the front of the list
				// so node2 will become the front of the list
				m_first = node2;
			}

			if ((node1->next != NULL) && (node1->next != node2)) {
				// node1 is not at back of the list and
				// node2 does not immediately follow node1
				node1->next->prev = node2;
			}
			else if (node1->next == NULL) {
				// node1 is at the back of the list
				// so node2 will become the back of the list
				m_last = node2;
			}

			// adjust node2 neighbors to point to node1
			if ((node2->prev != NULL) && (node2->prev != node1)) {
				// node2 is not at front of the list and
				// node1 does not immediately precede node2
				node2->prev->next = node1;
			}
			else if (node2->prev == NULL) {
				// node2 is at the front of the list
				// so node1 will become the front of the list
				m_first = node1;
			}

			if ((node2->next != NULL) && (node2->next != node1)) {
				// node2 is not at back of the list and
				// node1 does not immediately follow node2
				node2->next->prev = node1;
			}
			else if (node2->next == NULL) {
				// node2 is at the back of the list
				// so node1 will become the back of the list
				m_last = node1;
			}

			// adjust node1 ptrs to point to node2 neighbors
			if (node2->prev != node1) {
				node1->prev = node2->prev;
			}
			else {
				node1->prev = node2;
			}
			if (node2->next != node1) {
				node1->next = node2->next;
			}
			else {
				node1->next = node2;
			}

			// adjust node2 ptrs to point to node1 neighbors
			if (tmpNode.prev != node2) {
				node2->prev = tmpNode.prev;
			}
			else {
				node2->prev = node1;
			}
			if (tmpNode.next != node2) {
				node2->next = tmpNode.next;
			}
			else {
				node2->next = node1;
			}
		}

		return;
	}

	void CLinkedList<T>::swapPos(int nodePos1, int nodePos2)
	{
		tnode<T>* node1;
		tnode<T>* node2;

		node1 = get(nodePos1);
		node2 = get(nodePos2);

		swap(node1, node2);

		return;
	}

	void CLinkedList<T>::push_front(const T nodeData)
	{
		tnode<T>* newNode;

		newNode = new tnode<T>;

		if (newNode != NULL) {
			newNode->data = nodeData;
			newNode->next = m_first;
			newNode->prev = NULL;
			if (m_size == 0) {
				m_first = newNode;
				m_last = newNode;
			}
			else {
				m_first->prev = newNode;
				m_first = newNode;
			}

			m_size++;
		}

		return;
	}

	void CLinkedList<T>::push_back(const T nodeData)
	{
		tnode<T>* newNode;

		newNode = new tnode<T>;

		if (newNode != NULL) {
			newNode->data = nodeData;
			newNode->next = NULL;
			newNode->prev = m_last;
			if (m_size == 0) {
				m_first = newNode;
				m_last = newNode;
			}
			else {
				m_last->next = newNode;
				m_last = newNode;
			}

			m_size++;
		}

		return;
	}

	const T CLinkedList<T>::front(void) const
	{
		if (m_first != NULL) {
			return(m_first->data);
		}
		else {
			return(NULL);
		}
	}

	const T CLinkedList<T>::back(void) const
	{
		if (m_last != NULL) {
			return(m_last->data);
		}
		else {
			return(NULL);
		}
	}

	const T CLinkedList<T>::pop_front(void)
	{
		T front_data;
		tnode<T>* tmpNode;

		if (m_first != NULL) {
			front_data = m_first->data;
			m_size--;
			tmpNode = m_first->next;
			delete m_first;
			if (tmpNode != NULL) {
				tmpNode->prev = NULL;
			}
			m_first = tmpNode;

			if (m_first == NULL) {
				m_last = NULL;
			}

			return(front_data);
		}

		return(NULL);
	}

	const T CLinkedList<T>::pop_back(void)
	{
		T back_data;
		tnode<T>* tmpNode;

		if (m_last != NULL) {
			back_data = m_last->data;
			m_size--;
			tmpNode = m_last->prev;
			delete m_last;
			if (tmpNode != NULL) {
				tmpNode->next = NULL;
			}
			m_last = tmpNode;

			if (m_last == NULL) {
				m_first = NULL;
			}

			return(back_data);
		}

		return(NULL);
	}

	void CLinkedList<T>::clear(void)
	{
		while (m_first != NULL) {
			erase(0);
		}

		assert(m_size == 0);
		assert(m_first == NULL);
		assert(m_last == NULL);

		return;
	}

	// i is zero based index
	T CLinkedList<T>::erase(int i)
	{
		T data;
		int count = 0;
		tnode<T>* currentNode = m_first;

		if (i < m_size) {
			while (++count < i) {
				currentNode = currentNode->next;
			}

			// currentNode now holds node that will be erased
			data = currentNode->data;

			if (currentNode->prev == NULL) {
				// deleting front of list
				m_first = currentNode->next;
			}
			if (currentNode->next == NULL) {
				// deleting back of list
				m_last = currentNode->prev;
			}

			if (m_size > 1) {
				if (i > 0) {
					currentNode->prev->next = currentNode->next;
				}
				if (i < m_size) {
					currentNode->next->prev = currentNode->prev;
				}
			}

			m_size--;
			delete currentNode->data;
			delete currentNode;
			return(data);
		}

		return(NULL);
	}
};

#endif // !defined(LINKEDLIST_H)