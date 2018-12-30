#include <scene.h>

struct LandscapeModel::Impl {
};

LandscapeModel::LandscapeModel ()
{
  impl = new Impl;  
}

LandscapeModel::~LandscapeModel ()
{
  delete impl;
}

void LandscapeModel::visit (EntityVisitor* visitor)
{
  visitor->visit (this);
}
