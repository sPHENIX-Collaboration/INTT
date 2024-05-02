#include "InttVertex3DMapv1.h"

#include "InttVertex3D.h"

#include <iterator>  // for reverse_iterator
#include <utility>   // for pair, make_pair

InttVertex3DMapv1::~InttVertex3DMapv1()
{
  clear();
}

void InttVertex3DMapv1::identify(std::ostream& os) const
{
  os << "InttVertex3DMapv1: size = " << _map.size() << std::endl;
  for (auto& m : _map)
  {
    m.second->identify(os);
  }

  return;
}

void InttVertex3DMapv1::clear()
{
  for (auto& iter : _map)
  {
    delete iter.second;
  }
  _map.clear();
  return;
}

const InttVertex3D* InttVertex3DMapv1::get(unsigned int id) const
{
  ConstIter iter = _map.find(id);
  if (iter == _map.end())
  {
    return nullptr;
  }
  return iter->second;
}

InttVertex3D* InttVertex3DMapv1::get(unsigned int id)
{
  Iter iter = _map.find(id);
  if (iter == _map.end())
  {
    return nullptr;
  }
  return iter->second;
}

InttVertex3D* InttVertex3DMapv1::insert(InttVertex3D* vertex)
{
  unsigned int index = 0;
  if (!_map.empty())
  {
    index = _map.rbegin()->first + 1;
  }
  _map.insert(std::make_pair(index, vertex));
  _map[index]->set_id(index);
  return _map[index];
}
