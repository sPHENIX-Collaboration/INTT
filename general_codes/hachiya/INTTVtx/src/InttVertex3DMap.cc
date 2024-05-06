#include "InttVertex3DMap.h"

// class BbcVertex;

std::map<unsigned int, InttVertex3D*> DummyInttVertex3DMap;

InttVertex3DMap::ConstIter InttVertex3DMap::begin() const
{
  return DummyInttVertex3DMap.end();
}

InttVertex3DMap::ConstIter InttVertex3DMap::find(unsigned int /*idkey*/) const
{
  return DummyInttVertex3DMap.end();
}

InttVertex3DMap::ConstIter InttVertex3DMap::end() const
{
  return DummyInttVertex3DMap.end();
}

InttVertex3DMap::Iter InttVertex3DMap::begin()
{
  return DummyInttVertex3DMap.end();
}

InttVertex3DMap::Iter InttVertex3DMap::find(unsigned int /*idkey*/)
{
  return DummyInttVertex3DMap.end();
}

InttVertex3DMap::Iter InttVertex3DMap::end()
{
  return DummyInttVertex3DMap.end();
}
