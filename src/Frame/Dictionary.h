#ifndef __DICTIONARY__
#define __DICTIONARY__

#include <map>
#include <assert.h>


template <typename KEY, typename VALUE>
class Dictionary
{
public:
	typedef std::map<KEY, VALUE>	MapType;
	typedef std::pair<KEY,VALUE>	PairType;

private:
	Dictionary(const Dictionary&);
	Dictionary& operator=(const Dictionary&);

	MapType m_map;
	typename MapType::iterator m_it;

public:
	Dictionary(){Reset();}
	virtual ~Dictionary(){}

	/// Add a data if the key is not exist.
	bool Add(const KEY& key, const VALUE& val);
	
	/// Is the key in this dictionary?
	bool ContainKey(const KEY& key)
	{return m_map.find(key)!=m_map.end();}
	
	/// Get the value by the key.
	VALUE& operator[](const KEY& key);
	
	/// Get the value by the key. If the key is exist, it will return true.
	bool TryGet(const KEY& key, VALUE& val);

	/// How many data are in this dictionary.
	int Size()
	{return m_map.size();}

	/// Remove the data by the key.
	/// Don't use it in the iterator mode.
	bool RemoveByKey(const KEY& key);

	/// Set the offset to first iterator.
	void Reset()
	{ m_it = m_map.begin(); }

	/// Get the key in the current iterator.
	KEY GetCurrentKey()
	{return (*m_it).first;}

	/// Get the value in the current iterator.
	VALUE GetCurrentValue()
	{return (*m_it).second;}
	
	/// Move the offset to next iterator.
	bool Next();

	/// Remove the data which was dicated by current iterator.
	void RemoveCurrentData();

	void Clear()
	{m_map.clear();}
};





template <typename KEY, typename VALUE>
void Dictionary<KEY, VALUE>::RemoveCurrentData()
{
	typename MapType::iterator _itRemove;
	_itRemove = m_it;
	m_it++;
	m_map.erase(_itRemove);
}


template <typename KEY, typename VALUE>
bool Dictionary<KEY, VALUE>::Next()
{
	if(m_it==m_map.end())
		return false;

	m_it++;
	if(m_it==m_map.end())
		return false;
	
	return true;
}


template <typename KEY, typename VALUE>
VALUE& Dictionary<KEY, VALUE>::operator[](const KEY& key)
{
	if(!ContainKey(key))
		throw FRAME_EXPCEPTION_1("don't have the key.");

	return m_map[key];
}


template <typename KEY, typename VALUE>
bool Dictionary<KEY, VALUE>::RemoveByKey(const KEY& key)
{
	if(!ContainKey(key))
		return false;

	m_map.erase(key);
	return true;
}


template <typename KEY, typename VALUE>
bool Dictionary<KEY, VALUE>::TryGet(const KEY& key, VALUE& val)
{
	if(!ContainKey(key))
		return false;

	val = m_map[key];
	return true;
}


template <typename KEY, typename VALUE>
bool Dictionary<KEY, VALUE>::Add(const KEY& key, const VALUE& val)
{
	if(ContainKey(key))
		return false;

	m_map.insert(PairType(key, val));
	return true;
}

#endif /*__DICTIONARY__*/
