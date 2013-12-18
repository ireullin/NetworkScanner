#ifndef __DICTIONARY2__
#define __DICTIONARY2__


/********************************************************************
	Dictionary 每次插入(add) 與取出(operator[])元素時
	會檢查是否已存在,此動作會造成效率低落
	
	Dictionary2 不會進行此檢查,所以使用時請注意key值是否存在的問題
	如有疑慮請自行使用ContainKey 檢查

	Example:
	Dictionary2<std::string,std::string> _d;
	for(_d.Begin(); !_d.End(); ++_d)
	{
		printf("key=%s value=%s", _d.Key().c_str(), _d.Value().c_str());	
		_d.Remove();
	}

*********************************************************************/
template <typename K, typename V>
class Dictionary2
{
private:
	typedef std::map<K,V>	MapType;
	typedef std::pair<K,V>	PairType;

	Dictionary2(const Dictionary2&);
	Dictionary2& operator=(const Dictionary2&);

	MapType						m_map;
	typename MapType::iterator	m_it;
	bool						m_reset;

public:
	Dictionary2();
	virtual ~Dictionary2();

	/// 新增一元素,不檢查是否存在
	/// 新增已有元素,會覆蓋舊有元素
	void Add(const K& key, const V& val);
	
	/// 移除該元素,不檢查是否存在
	void RemoveByKey(const K& key);

	/// 是否包含這個key值
	bool ContainKey(const K& key);
	
	/// 回傳該key對應的元素,不檢查是否存在
	/// 存取不存在元素會造成空元素的存在,使用請小心
	V& operator[](const K& key);
	
	/// 回傳該key對應的元素,檢查是否存在
	bool TryGet(const K& key, V& val);

	/// 目前元素的數量
	int Size();

	/// 清除所有資料
	void Clear();

	/// 以下為遞回的時候使用
	/// 不可以在多執行緒下使用
	V& Value();
	const K& Key();
	void Begin();
	bool End();
	void Remove();
	Dictionary2<K,V> & operator++();
	const Dictionary2<K,V> operator++(int i);

};


template <typename K, typename V>
Dictionary2<K,V>::Dictionary2():m_reset(false)
{}

template <typename K, typename V>
Dictionary2<K,V>::~Dictionary2()
{}

template <typename K, typename V>
void Dictionary2<K,V>::Add(const K& key, const V& val)
{
	m_map.insert(PairType(key, val));
}

template <typename K, typename V>
void Dictionary2<K,V>::RemoveByKey(const K& key)
{
	m_map.erase(key);
}

template <typename K, typename V>
bool Dictionary2<K,V>::ContainKey(const K& key)
{
	return m_map.find(key)!=m_map.end();
}
	
template <typename K, typename V>
V& Dictionary2<K,V>::operator[](const K& key)
{
	return m_map[key];
}

template <typename K, typename V>
bool Dictionary2<K,V>::TryGet(const K& key, V& val)
{
	if(!ContainKey(key))
		return false;

	val = m_map[key];
	return true;
}

template <typename K, typename V>
int Dictionary2<K,V>::Size()
{
	return m_map.size();
}

template <typename K, typename V>
void Dictionary2<K,V>::Clear()
{
	m_map.clear();
}

template <typename K, typename V>
V& Dictionary2<K,V>::Value()
{
	return (*m_it).second;
}

template <typename K, typename V>
const K& Dictionary2<K,V>::Key()
{
	return (*m_it).first;
}

template <typename K, typename V>
void Dictionary2<K,V>::Begin()
{
	m_reset = false;
	m_it = m_map.begin();
}

template <typename K, typename V>
bool Dictionary2<K,V>::End()
{
	return m_it == m_map.end();
}

template <typename K, typename V>
void Dictionary2<K,V>::Remove()
{
	typename MapType::iterator _tmp;
	_tmp = m_it;
	
	if(m_it==m_map.begin())
		m_reset = true;
	else
		--m_it;
	
	m_map.erase(_tmp);

	if(m_reset)
		m_it = m_map.begin();
}

template <typename K, typename V>
Dictionary2<K,V> & Dictionary2<K,V>::operator++()
{
	if(m_reset) m_reset=false;
	else		++m_it;

	return (*this);
}

template <typename K, typename V>
const Dictionary2<K,V> Dictionary2<K,V>::operator++(int i)
{
	if(m_reset) m_reset=false;
	else		++m_it;

	++m_it;
	Dictionary2<K,V> _tmp;
	return _tmp;
}

#endif
