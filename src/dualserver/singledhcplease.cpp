#include "stdafx.h"
#include "singledhcplease.h"

namespace utm
{

singledhcplease::singledhcplease()
{
}


singledhcplease::~singledhcplease()
{
}

unsigned int singledhcplease::get_id() const
{
	return id;
}

void singledhcplease::set_id(unsigned int id)
{
	this->id = id;
}

}