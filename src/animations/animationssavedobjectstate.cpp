#include "animations/animationssavedobjectstate.h"

sad::animations::SavedObjectState::SavedObjectState(sad::db::Object* o) : m_object(o), m_references(1)
{
    
}

sad::animations::SavedObjectState::~SavedObjectState()
{
    
}

void sad::animations::SavedObjectState::storeBodyState(sad::p2d::Body* b)
{

}