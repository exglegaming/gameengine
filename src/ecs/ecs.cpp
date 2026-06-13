#include "ecs.h"
#include "../logger/logger.h"
#include <algorithm>

int ecs::i_component::next_id = 0;

int ecs::entity::get_id() const 
{
	return id;
}

void ecs::system::add_entity_to_system(entity entity)
{
	entities.push_back(entity);
}

void ecs::system::remove_entity_from_system(entity entity)
{
	entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](ecs::entity other) 
	{
		return entity == other;
	}), entities.end());
}

std::vector<ecs::entity> ecs::system::get_system_entities() const
{
	return entities;
}

const ecs::signature& ecs::system::get_component_signature() const
{
	return component_signature;
}

ecs::entity ecs::registry::create_entity()
{
	int entity_id;

	entity_id = num_entities++;

	entity entity(entity_id);
	entity.registry = this;
	entities_to_be_added.insert(entity);

	if (entity_id >= static_cast<int>(entity_component_signatures.size()))
	{
		entity_component_signatures.resize(entity_id + 1);
	}

	logger::log("Entity created with id = " + std::to_string(entity_id));
	return entity;
}

void ecs::registry::add_entity_to_systems(entity entity)
{
	const auto entity_id = entity.get_id();
	const auto& entity_component_signature = entity_component_signatures[entity_id];

	for (auto& system: systems)
	{
		const auto& system_component_signature = system.second->get_component_signature();

		bool isInterested = (entity_component_signature & system_component_signature) == system_component_signature;

		if (isInterested)
		{
			system.second->add_entity_to_system(entity);
		}
	}
}

void ecs::registry::update()
{
	// Add the entites that are waiting to be created to the active Systems
	for (auto entity: entities_to_be_added)
	{
		add_entity_to_systems(entity);
	}
	entities_to_be_added.clear();

	// TODO: Remove the entities that are waiting to be killed from the active Systems
}
