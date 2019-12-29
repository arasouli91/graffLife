#include "stdafx.h"
#include "ECS.h"

void Entity::addGroup(Group G)
{
	groupBitset[G] = true;
	manager.AddToGroup(this, G);
}