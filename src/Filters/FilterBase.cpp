#include "FilterBase.h"

SYS_DEFINE_MODULE(DM_FILTER);

Filter::FilterBase::FilterBase(Group & group):
    myGroup(group)
{
 SYS_DEBUG_MEMBER(DM_FILTER);
 myGroup.insert(this);
}

Filter::FilterBase::~FilterBase()
{
}

void Filter::Group::Reset(void)
{
 for (Linked<Filter::FilterBase>::iterator i = begin(); i != end(); ++i) {
    i->Reset();
    i->At(0);
 }
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
