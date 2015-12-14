///////////////////////////////////////////////////////////////////////
//
//	File	: SmartPtr.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Template: SmartPtr
//
//	Purpose	: A reference counted smart pointer. The reference count
//			  is set to 1 when the pointer is assigned, and when the
//			  reference count reaches 0 the memory is released.
//
//			  Both single objects and arrays are supported using the
//			  second template parameter. If this parameter is false
//			  (or omitted) then delete is used to release the memory,
//			  but if the second parameter is true then delete[] is
//			  used instead.
//
//			  Pointers to polymorphic types can be used with care.
//			  Attempting to assign a smart pointer of one type to that
//			  of a different type will not behave as expected - a
//			  conversion to and from a normal pointer will be used,
//			  resulting in known independent smart pointers having
//			  "unique" ownership. The SmartPtrCast template should be
//			  used for this purpose.
//
//	Modification history:
//	
//		1.1		27:Sep:00	at	File created.
//		1.2		05:Oct:00	at	Completely rewritten to avoid use of
//								a separate SmartPtrCount template.
//		1.3		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.4		23:Jul:01	at	Updated copyright message.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/SmartPtr.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef SMARTPTR_H
#define SMARTPTR_H

#if _MSC_VER > 1000
#pragma once
#endif

// Disable warning for operator->() when used with built-in type
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4284)
#endif

// SmartPtr template
template<class T, bool isArray = false>
class SmartPtr
{
public:

	// Constructors
	SmartPtr();
	explicit SmartPtr(T *ptr, int *count = 0);
	SmartPtr(const SmartPtr<T, isArray> &ptr);

	// Destructor
	~SmartPtr();

	// Assignment
	SmartPtr &operator=(const SmartPtr<T, isArray> &ptr);

	// Accessors
	operator T *() const;
	T &operator*() const;
	T *operator->() const;
	T &operator[](size_t index) const;
	int *GetCount() const;

private:

	// The pointer being managed
	T *ptr;

	// Pointer to the corresponding reference counter
	int *count;

	// Unassign any existing pointer
	void Unassign();

	// Assign a new pointer
	void Assign(T *ptr = 0, int *count = 0);
};

// Constructors
template<class T, bool isArray>
SmartPtr<T, isArray>::SmartPtr()
{
	Assign();
}

template<class T, bool isArray>
SmartPtr<T, isArray>::SmartPtr(T *ptr, int *count)
{
	Assign(ptr, count);
}

template<class T, bool isArray>
SmartPtr<T, isArray>::SmartPtr(const SmartPtr<T, isArray> &ptr)
{
	Assign(ptr.ptr, ptr.count);
}

// Destructor
template<class T, bool isArray>
SmartPtr<T, isArray>::~SmartPtr()
{
	Unassign();
}

// Assignment
template<class T, bool isArray>
SmartPtr<T, isArray> &SmartPtr<T, isArray>::operator=(const SmartPtr<T, isArray> &ptr)
{
	if (this->ptr != ptr.ptr)
	{
		Unassign();
		Assign(ptr.ptr, ptr.count);
	}
	return *this;
}

// Accessors
template<class T, bool isArray>
SmartPtr<T, isArray>::operator T *() const
{
	return ptr;
}

template<class T, bool isArray>
T &SmartPtr<T, isArray>::operator*() const
{
	return *ptr;
}

template<class T, bool isArray>
T *SmartPtr<T, isArray>::operator->() const
{
	return ptr;
}

template<class T, bool isArray>
T &SmartPtr<T, isArray>::operator[](size_t index) const
{
	return ptr[index];
}

template<class T, bool isArray>
int *SmartPtr<T, isArray>::GetCount() const
{
	return count;
}

// Unassign any existing pointer
template<class T, bool isArray>
void SmartPtr<T, isArray>::Unassign()
{
	if (count)
	{
		if (--*count == 0)
		{
			if (isArray) delete[] ptr;
			else delete ptr;
			delete count;
		}
		ptr = 0;
		count = 0;
	}
}

// Assign a new pointer
template<class T, bool isArray>
void SmartPtr<T, isArray>::Assign(T *ptr, int *count)
{
	this->ptr = ptr;
	if (count)
	{
		this->count = count;
		++*count;
	}
	else
	{
		this->count = ptr ? new int(1) : 0;
	}
}

// Casting between smart pointers for non-array polymorphic types
template<class U, class V>
void SmartPtrCast(SmartPtr<U, false> &u, const SmartPtr<V, false> &v)
{
	u = SmartPtr<U, false>((V *) v, v.GetCount());
}

// Restore previous warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
