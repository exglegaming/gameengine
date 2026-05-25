#include "ECS.h"
#include "../Logger/Logger.h"
#include <algorithm>

int IComponent::next_id = 0;

int Entity::GetId() const 
{
	return id;
}

void System::AddEntityToSystem(Entity entity)
{
	entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity)
{
	entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) 
	{
		return entity == other;
	}), entities.end());
}

std::vector<Entity> System::GetSystemEntities() const
{
	return entities;
}

const Signature& System::GetComponentSignature() const
{
	return component_signature;
}

Entity Registry::CreateEntity()
{
	int entity_id;

	entity_id = num_entities++;

	Entity entity(entity_id);
	entities_to_be_added.insert(entity);

	if (entity_id >= static_cast<int>(entity_component_signatures.size()))
	{
		entity_component_signatures.resize(entity_id + 1);
	}

	Logger::Log("Entity created with id = " + std::to_string(entity_id));
	return entity;
}

void Registry::AddEntityToSystems(Entity entity)
{
	const auto entity_id = entity.GetId();
	const auto& entity_component_signature = entity_component_signatures[entity_id];

	for (auto& system: systems)
	{
		const auto& system_component_signature = system.second->GetComponentSignature();

		bool isInterested = (entity_component_signature & system_component_signature) == system_component_signature;

		if (isInterested)
		{
			system.second->AddEntityToSystem(entity);
		}
	}
}

void Registry::Update()
{
	// Add the entites that are waiting to be created to the active Systems
	for (auto entity: entities_to_be_added)
	{
		AddEntityToSystems(entity);
	}
	entities_to_be_added.clear();

	// TODO: Remove the entities that are waiting to be killed from the active Systems
}
