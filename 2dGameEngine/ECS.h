#pragma once

#include <vector>
#include <bitset>
#include <array>
#include <memory>
class Component;
class Entity;
class Manager;

using ComponentID = std::size_t;
using Group = std::size_t;
inline ComponentID getNewComponentTypeID()
{
	static ComponentID lastID = 0u;
	return lastID++;
}

template <typename T> inline ComponentID getComponentTypeID() noexcept
{
	static ComponentID typeID = getNewComponentTypeID();
	return typeID;
}

constexpr std::size_t maxComponents = 32;
constexpr std::size_t maxGroups = 32;

using ComponentBitSet = std::bitset<maxComponents>;	//To determine whether an entity has certain components
using GroupBitset = std::bitset<maxGroups>;
using ComponentArray = std::array<Component*, maxComponents>;	//Holds components

class Component
{
public:
	Entity* entity;

	virtual void init() {}
	virtual void update() {}
	virtual void draw() {}

	virtual ~Component() {}

};

class alignas(16) Entity
{
private:
	Manager& manager;
	bool active = true;
	std::vector<std::unique_ptr<Component>> components;

	ComponentArray componentArray;
	ComponentBitSet componentBitset;
	GroupBitset groupBitset;
public:
	Entity(Manager& man) : manager{man} {}

	void update()	//update components held by entity
	{													
		for (auto& c : components) c->update();
	}
	void draw()										
	{
		for (auto& c : components) c->draw();
	}

	bool isActive() const { return active; }
	void destroy() { active = false; }

	bool hasGroup(Group G)
	{
		return groupBitset[G];
	}

	void addGroup(Group G);
	void delGroup(Group G)
	{
		groupBitset[G] = false;
	}

	template <typename T> bool hasComponent() const
	{
		return componentBitset[getComponentTypeID<T>()];
	}

	template <typename T, typename... TArgs>
	T& addComponent(TArgs&&... mArgs)
	{
		T* c(new T(std::forward<TArgs>(mArgs)...));
		c->entity = this;								//Give this entity address to each component
		std::unique_ptr<Component> uPtr{ c };
		components.emplace_back(std::move(uPtr));
		//Components of specific type always put in same pos. of array
		componentArray[getComponentTypeID<T>()] = c;
		componentBitset[getComponentTypeID<T>()] = true;

		c->init();
		return *c;
	}
	template<typename T> T& getComponent() const
	{
		auto ptr(componentArray[getComponentTypeID<T>()]);
		return *static_cast<T*>(ptr);
	}
	//This is how we will use this component system
	//gameObject.getComponent<TransformComponent>().setXpos(25);
	
	//~Entity(){}
};

class Manager
{
private:
	std::vector<std::unique_ptr<Entity>> entities;
	std::array<std::vector<Entity*>, maxGroups> groupedEntities; //Array of group vectors
public:
	void update()				
	{
		for (auto& e : entities) e->update();
	}
	void draw()									
	{
		for (auto& e : entities) e->draw();
	}
	void refresh()
	{	
		for (auto i(0u); i < maxGroups; ++i)
		{
			auto& v(groupedEntities[i]);
			v.erase(std::remove_if(std::begin(v), std::end(v),
				[i](Entity* mEntity)
			{
				return !mEntity->isActive() || !mEntity->hasGroup(i);
			}),
				std::end(v));
		}

		//removes inactive entities
		entities.erase(std::remove_if(std::begin(entities), std::end(entities),
			[](const std::unique_ptr<Entity> &mEntity)
		{
			return !mEntity->isActive();
		}),
			std::end(entities));
	}

	void AddToGroup(Entity* mEntity, Group mGroup)
	{
		groupedEntities[mGroup].emplace_back(mEntity);
	}

	std::vector<Entity*>& getGroup(Group mGroup)
	{
		return groupedEntities[mGroup];
	}

	//Entity& addEntity()///////////////////DBg
	Entity& addEntity(Manager* man)
	{
		//Entity* e = new Entity(*this);///////////////////DBG
		//std::cout << std::alignment_of<Entity>() << std::endl;
		alignas(16) Entity* e = new Entity(*man);
		std::unique_ptr<Entity> uPtr{ e };
		entities.emplace_back(std::move(uPtr));
		return *e;
	}

	//~Manager(){}
};