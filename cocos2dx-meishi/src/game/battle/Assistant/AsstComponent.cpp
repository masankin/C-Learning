#include "AsstComponent.h"

AsstComponent::AsstComponent(void)
: mOwner(nullptr)
, mEnabled(true)
{

}

AsstComponent::~AsstComponent(void)
{
}

bool AsstComponent::init()
{
	return true;
}

void AsstComponent::run()
{

}

void AsstComponent::update(float delta)
{

}

void AsstComponent::clean()
{

}

const std::string& AsstComponent::getName() const
{
	return mName;
}

void AsstComponent::setName(const std::string& name)
{
	mName = name;
}

Assistant* AsstComponent::getOwner() const
{
	return mOwner;
}

void AsstComponent::setOwner(Assistant *owner)
{
	mOwner = owner;
}

bool AsstComponent::isEnabled() const
{
	return mEnabled;
}

void AsstComponent::setEnabled(bool b)
{
	mEnabled = b;
}
