#ifndef INDEX_SET_H
#define INDEX_SET_H

#include "GenericIndex.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>

class IndexSet
{
protected:
public:
	std::map<std::string, GenericIndex> indices;

public:
	IndexSet();
	IndexSet(IndexSet const&);

	friend bool operator==(IndexSet const&, IndexSet const&);
	friend bool operator!=(IndexSet const& lhs, IndexSet const& rhs){return !(lhs == rhs);}

	friend bool operator<(IndexSet const&, IndexSet const&);
	friend bool operator>(IndexSet const& lhs, IndexSet const& rhs){return rhs < lhs;}
	friend bool operator<=(IndexSet const& lhs, IndexSet const& rhs){return !(lhs > rhs);}
	friend bool operator>=(IndexSet const& lhs, IndexSet const& rhs){return !(lhs < rhs);}

	IndexSet& operator++(); 
	IndexSet& operator--(); 

	const IndexSet begin();
	const IndexSet end();

	void AddIndex(GenericIndex);
	void AddIndex(std::string s, int l, int u){AddIndex(GenericIndex(s, l, u));}

	int GetIndex(std::string, int&);
	int SetIndex(std::string, int);
};

#endif
