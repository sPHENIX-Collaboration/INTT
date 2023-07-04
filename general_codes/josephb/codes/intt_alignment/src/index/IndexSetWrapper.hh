#ifndef INDEX_SET_WRAPPER_HH
#define INDEX_SET_WRAPPER_HH

template <typename T>
IndexSetWrapper<T>::IndexSetWrapper()
{
	wrapper.clear();
	index_set = IndexSet();
}

template <typename T>
IndexSetWrapper<T>::IndexSetWrapper(const IndexSet& s)
{
	wrapper.clear();
	index_set = s;

	for(index_set = index_set.begin(); index_set != index_set.end(); ++index_set)
	{
		wrapper.insert({index_set, T()});
	}
}

template <typename T>
IndexSetWrapper<T>::IndexSetWrapper(const IndexSetWrapper<T>& o)
{
	wrapper.clear();
	index_set = o.index_set;

	typename std::map<IndexSet, T>::const_iterator itr = o.wrapper.begin();
	for(itr = o.wrapper.begin(); itr != o.wrapper.end(); ++itr)
	{
		wrapper.insert({itr->first, itr->second});
	}
}

template <typename T>
IndexSetWrapper<T>& IndexSetWrapper<T>::operator++()
{
	++index_set;

	return *this;
}

template <typename T>
IndexSetWrapper<T>& IndexSetWrapper<T>::operator--()
{
	--index_set;

	return *this;
}

template <typename T>
void IndexSetWrapper<T>::GotoBegin()
{
	index_set = index_set.begin();
}

template <typename T>
void IndexSetWrapper<T>::GotoEnd()
{
	index_set = index_set.end();
}

template <typename T>
bool IndexSetWrapper<T>::AtBegin()
{
	return index_set == index_set.begin();
}

template <typename T>
bool IndexSetWrapper<T>::AtEnd()
{
	return index_set == index_set.end();
}

template <typename T>
int IndexSetWrapper<T>::Get(T& t)
{
	auto itr = wrapper.find(index_set);
	if(itr == wrapper.end())return 1;

	t = itr->second;

	return 0;
}

template <typename T>
int IndexSetWrapper<T>::Set(T t)
{
	auto itr = wrapper.find(index_set);
	if(itr == wrapper.end())return 1;

	itr->second = t;

	return 0;
}

template <typename T>
int IndexSetWrapper<T>::GetIndex(std::string s, int& i)
{
	return index_set.GetIndex(s, i);
}

template <typename T>
int IndexSetWrapper<T>::SetIndex(std::string s, int i)
{
	return index_set.SetIndex(s, i);
}


#endif
