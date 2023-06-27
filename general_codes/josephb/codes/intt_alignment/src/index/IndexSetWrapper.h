#ifndef INDEX_SET_WRAPPER_H
#define INDEX_SET_WRAPPER_H

#include <string>
#include <vector>
#include <map>

#include "IndexSet.h"

template <typename T>
class IndexSetWrapper
{
protected:
	IndexSet index_set;
	std::map<IndexSet, T> wrapper;

public:
	IndexSetWrapper();
	IndexSetWrapper(const IndexSet&);
	IndexSetWrapper(const IndexSetWrapper&);

	IndexSetWrapper& operator++();
	IndexSetWrapper& operator--();

	void GotoBegin();
	void GotoEnd();

	bool AtBegin();
	bool AtEnd();

	int Get(T& t);
	int Set(T t);

	int GetIndex(std::string, int&);
	int SetIndex(std::string, int);
};

#include "IndexSetWrapper.hh"

#endif
