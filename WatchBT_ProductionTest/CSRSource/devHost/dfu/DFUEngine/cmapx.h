///////////////////////////////////////////////////////////////////////
//
//	File	: CMapX.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: J. Cooper
//
//	Class	: CMapX
//
//	Purpose	: A drop in replacement for the CMap object implemented using a STL map.
//            All implemented methods perform the same function as those in CMap.
//            The iterator type is CMapX::POSITION rather than simply ::POSITION.
//
//	$Id: //depot/bc/main/devHost/dfu/DFUEngine/CMapX.h#27 $
//
///////////////////////////////////////////////////////////////////////


#ifndef CMAPX_H
#define CMAPX_H

#pragma warning ( disable : 4786 )

#include <map>

template <class Key, class Arg_Key, class Value, class Arg_Value> class CMapX
{
public:
	class POSITION {
		friend class CMapX<Key, Arg_Key, Value, Arg_Value>;
	public:
		operator bool()
		{
			return it_!=owner_.impl.end();
		}

	private:
		POSITION(CMapX<Key, Arg_Key, Value, Arg_Value> &owner) : owner_(owner) {}

		CMapX<Key, Arg_Key, Value, Arg_Value> &owner_;
		typename std::map<Key, Value>::iterator it_;
	};
	friend class POSITION;
	bool Lookup(Arg_Key key, Value &val) const;
	void SetAt(Arg_Key key, Arg_Value val);
	bool RemoveKey(Arg_Key key);
	int GetCount() const;
	POSITION GetStartPosition();
	void GetNextAssoc(POSITION&, Key&, Value&);

private:
	std::map<Key, Value> impl;
};

template <class Key, class Arg_Key, class Value, class Arg_Value>
bool CMapX<Key, Arg_Key, Value, Arg_Value>::Lookup(Arg_Key key, Value &value) const
{
	std::map<Key, Value>::const_iterator it = impl.find(key);
	if(it != impl.end())
	{
		value = it->second;
		return true;
	}
	else
	{
		return false;
	}
}

template <class Key, class Arg_Key, class Value, class Arg_Value>
void CMapX<Key, Arg_Key, Value, Arg_Value>::SetAt(Arg_Key key, Arg_Value value)
{
	std::pair<std::map<Key, Value>::iterator, bool> r = impl.insert(std::make_pair(key, value));
	if(!r.second) r.first->second = value;
}

template <class Key, class Arg_Key, class Value, class Arg_Value>
bool CMapX<Key, Arg_Key, Value, Arg_Value>::RemoveKey(Arg_Key key)
{
	return impl.erase(key) != 0;
}

template <class Key, class Arg_Key, class Value, class Arg_Value>
int CMapX<Key, Arg_Key, Value, Arg_Value>::GetCount() const
{
	return impl.size();
}

template <class Key, class Arg_Key, class Value, class Arg_Value>
typename CMapX<Key, Arg_Key, Value, Arg_Value>::POSITION CMapX<Key, Arg_Key, Value, Arg_Value>::GetStartPosition()
{
	POSITION r(*this);
	r.it_ = impl.begin();
	return r;
}

template <class Key, class Arg_Key, class Value, class Arg_Value>
void CMapX<Key, Arg_Key, Value, Arg_Value>::GetNextAssoc(typename CMapX<Key, Arg_Key, Value, Arg_Value>::POSITION &pos, Key &key, Value &value)
{
	key = pos.it_->first;
	value = pos.it_->second;
	++pos.it_;
}

#endif
