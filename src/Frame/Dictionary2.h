#ifndef __DICTIONARY2__
#define __DICTIONARY2__


/********************************************************************
	Dictionary �C�����J(add) �P���X(operator[])������
	�|�ˬd�O�_�w�s�b,���ʧ@�|�y���Ĳv�C��
	
	Dictionary2 ���|�i�榹�ˬd,�ҥH�ϥήɽЪ`�Nkey�ȬO�_�s�b�����D
	�p���ü{�Цۦ�ϥ�ContainKey �ˬd

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

	/// �s�W�@����,���ˬd�O�_�s�b
	/// �s�W�w������,�|�л\�¦�����
	void Add(const K& key, const V& val);
	
	/// �����Ӥ���,���ˬd�O�_�s�b
	void RemoveByKey(const K& key);

	/// �O�_�]�t�o��key��
	bool ContainKey(const K& key);
	
	/// �^�Ǹ�key����������,���ˬd�O�_�s�b
	/// �s�����s�b�����|�y���Ť������s�b,�ϥνФp��
	V& operator[](const K& key);
	
	/// �^�Ǹ�key����������,�ˬd�O�_�s�b
	bool TryGet(const K& key, V& val);

	/// �ثe�������ƶq
	int Size();

	/// �M���Ҧ����
	void Clear();

	/// �H�U�����^���ɭԨϥ�
	/// ���i�H�b�h������U�ϥ�
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
