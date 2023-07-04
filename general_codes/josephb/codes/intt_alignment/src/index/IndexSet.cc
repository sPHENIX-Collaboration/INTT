#include "IndexSet.h"

IndexSet::IndexSet()
{
	//Do nothing
}

IndexSet::IndexSet(IndexSet const& b)
{
	indices.clear();

	auto itr = b.indices.begin();
	for(itr = b.indices.begin(); itr != b.indices.end(); ++itr)
	{
		indices.insert({itr->first, itr->second});
	}
}

bool operator==(IndexSet const& lhs, IndexSet const& rhs)
{
	if(lhs.indices.size() != rhs.indices.size())return false;

	auto lhs_itr = lhs.indices.begin();
	auto rhs_itr = rhs.indices.begin();

	while(true)
	{
		if(lhs_itr->second != rhs_itr->second)return false;

		++lhs_itr;
		++rhs_itr;

		if(lhs_itr == lhs.indices.end())break;
		if(rhs_itr == rhs.indices.end())break;
	}

	return true;
}

bool operator<(IndexSet const& lhs, IndexSet const& rhs)
{
	if(lhs.indices.size() < rhs.indices.size())return true;
	if(lhs.indices.size() > rhs.indices.size())return false;

	auto lhs_itr = lhs.indices.begin();
	auto rhs_itr = rhs.indices.begin();

	while(true)
	{
		if(lhs_itr->second < rhs_itr->second)return true;
		if(lhs_itr->second > rhs_itr->second)return false;

		++lhs_itr;
		++rhs_itr;

		if(lhs_itr == lhs.indices.end())break;
		if(rhs_itr == rhs.indices.end())break;
	}

	return false;
}

IndexSet& IndexSet::operator++()
{
	bool carry = false;
	auto itr = indices.begin();

	for(itr = indices.begin(); itr != indices.end(); ++itr)
	{
		++(itr->second);
		carry = itr->second.index == itr->second.upper;

		if(carry)itr->second.index = itr->second.lower;
		else break;
	}

	if(carry)
	{
		for(itr = indices.begin(); itr != indices.end(); ++itr)itr->second.index = itr->second.upper;
	}

	return *this;
}

IndexSet& IndexSet::operator--()
{
	bool carry = false;
	auto itr = indices.begin();

	for(itr = indices.begin(); itr != indices.end(); ++itr)
	{
		--(itr->second);
		carry = itr->second.index == itr->second.lower - 1;

		if(carry)itr->second.index = itr->second.upper - 1;
		else break;
	}

	if(carry)
	{
		for(itr = indices.begin(); itr != indices.end(); ++itr)itr->second.index = itr->second.upper;
	}

	return *this;
}

const IndexSet IndexSet::begin()
{
	IndexSet b = *this;
	for(auto itr = b.indices.begin(); itr != b.indices.end(); ++itr)itr->second.index = itr->second.lower;

	return b;
}

const IndexSet IndexSet::end()
{
	IndexSet b = *this;
	for(auto itr = b.indices.begin(); itr != b.indices.end(); ++itr)itr->second.index = itr->second.upper;

	return b;
}

void IndexSet::AddIndex(GenericIndex i)
{
	indices.insert({i.label, i});
}

int IndexSet::GetIndex(std::string s, int& i)
{
	auto itr = indices.find(s);
	if(itr == indices.end())return 1;

	i = itr->second.index;

	return 0;
}

int IndexSet::SetIndex(std::string s, int i)
{
	auto itr = indices.find(s);
	if(itr == indices.end())return 1;

	itr->second.index = i;

	if(!(itr->second.lower <= itr->second.index && itr->second.index < itr->second.upper))itr->second.index = itr->second.upper;

	return 0;
}
