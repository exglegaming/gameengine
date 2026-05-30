#ifndef ECS_H
#define ECS_H

#include "../Logger/Logger.h"
#include <bitset>
#include <vector>
#include <set>
#include <unordered_map>
#include <typeindex>
#include <memory>

const unsigned int MAX_COMPONENTS = 32;

// Signature
// We use a bitset (1s and 0s) to keep track of which components an entity has,
// and also helps keep track of which entities a system is interested in.
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent
{
	protected:
		static int next_id;
};

// Used to assign a unique id to component tyoe
template <typename T>
class Component : public IComponent
{
	public:
		// Returns the unique id of Component<T>
		static int get_id()
		{
			static auto id = next_id++;
			return id;
		}
};

class Entity 
{
	private:
		int id;

	public:
		Entity(int id): id(id) {};
		Entity(const Entity& entity) = default;
		int get_id() const;
		
		Entity& operator =(const Entity& other) = default;
		bool operator ==(const Entity& other) const { return id  == other.id; }
		bool operator !=(const Entity& other) const { return id != other.id; }
		bool operator >(const Entity& other) const { return id > other.id; }
		bool operator <(const Entity& other) const { return id < other.id; }

		template<typename TComponent, typename ...TArgs> void add_component(TArgs&& ...args);
		template<typename TComponent> void remove_component();
		template<typename TComponent> bool has_component() const;
		template<typename TComponent> TComponent& get_component() const;

		// Hold a pointer to the entity's owner registry
		class Registry* registry;
};

// The system processes entities that contain a specific signature
class System 
{
	private:
		Signature component_signature;
		std::vector<Entity> entities;

	public:
		System() = default;
		~System() = default;

		void add_entity_to_system(Entity entity);
		void remove_entity_from_system(Entity entity);
		std::vector<Entity> get_system_entities() const;
		const Signature& get_component_signature() const;

		// Define the component type T that entities must have to be considered by the system
		template <typename TComponent> void require_component();
};

// Pool
// A pool is just a vector (contigous data) of objects of type T
class IPool
{
	public:
		virtual ~IPool() {}
};

template <typename T>
class Pool : public IPool
{
	private:
		std::vector<T> data;

	public:
		Pool(int size = 100) { data.resize(size); }
		virtual ~Pool() = default;

		bool is_empty() const { return data.empty(); }
		int get_size() const { return data.size(); }
		void resize(int n) { data.resize(n); }
		void clear() { data.clear(); }
		void add(T object) { data.push_back(object); }
		void set(int index, T object) { data[index] = object; }
		T& get(int index) { return static_cast<T&>(data[index]); }
		T& operator [](unsigned int index) { return data[index]; }
};

// Registry
// The registry manages the creation and destruction of enitities, add systems,
// and components
class Registry 
{
	private:
		int num_entities = 0;

		// Vector of component pools, each pool contains all the data for a certain component type
		// [Vector index = component type id]
		// [Pool index = entity id]
		std::vector<std::shared_ptr<IPool>> component_pools;

		// Vecotr of component signatures per entity, saying which component is turned "on" for a given entity
		// [Vector index = entity id]
		std::vector<Signature> entity_component_signatures;

		std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

		// Set of entities that are flaggeds to be added or removed in the next registry Update()
		std::set<Entity> entities_to_be_added;
		std::set<Entity> entities_to_be_killed;

	public:
		Registry()
		{ 
			Logger::log("Registry constructor called");
		}

		~Registry()
		{ 
			Logger::log("Registry destructor called");
		}

		// The registry Update() finally process the entities that are waiting to be added/killed to the systems
		void update();

		// Entity management
		Entity create_entity();

		// Componet management
		template <typename TComponent, typename ...TArgs> void add_component(Entity entity, TArgs&& ...args);
		template <typename TComponent> void remove_component(Entity entity);
		template <typename TComponent> bool has_component(Entity entity) const;
		template <typename TComponent> TComponent& get_component(Entity entity) const;

		// System management
		template <typename TSystem, typename ...TArgs> void add_system(TArgs&& ...args);
		template <typename TSystem> void remove_system();
		template <typename TSystem> bool has_system() const;
		template <typename TSystem> TSystem& get_system() const;

		// Checks the component signature of an entity and add the entity to the systems
		// that are interested in it
		void add_entity_to_systems(Entity entity);
};

// Implementation of the function template
template <typename TComponent> 
void System::require_component() 
{
	const auto component_id = Component<TComponent>::get_id();
	component_signature.set(component_id);
}

template <typename TSystem, typename ...TArgs>
void Registry::add_system(TArgs&& ...args)
{
	std::shared_ptr<TSystem> new_system = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), new_system));
}

template <typename TSystem>
void Registry::remove_system()
{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}

template <typename TSystem>
bool Registry::has_system() const
{
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::get_system() const
{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent, typename ...TArgs> 
void Registry::add_component(Entity entity, TArgs&& ...args)
{
	const auto component_id = Component<TComponent>::get_id();
	const auto entity_id = entity.get_id();

	if (component_id >= static_cast<int>(component_pools.size()))
	{
		component_pools.resize(component_id + 1, nullptr);
	}

	if (!component_pools[component_id])
	{
		std::shared_ptr<Pool<TComponent>> new_component_pool = std::make_shared<Pool<TComponent>>();
		component_pools[component_id] = new_component_pool;
	}

	std::shared_ptr<Pool<TComponent>> component_pool = std::static_pointer_cast<Pool<TComponent>>(component_pools[component_id]);

	if (entity_id >= component_pool->get_size())
	{
		component_pool->resize(num_entities);
	}

	TComponent new_component(std::forward<TArgs>(args)...);

	component_pool->set(entity_id, new_component);
	entity_component_signatures[entity_id].set(component_id);

	Logger::log("Component id = " + std::to_string(component_id) + " was added to entity id " + std::to_string(entity_id));
}

template <typename TComponent>
void Registry::remove_component(Entity entity)
{
	const auto component_id = Component<TComponent>::get_id();
	const auto entity_id = entity.get_id();
	entity_component_signatures[entity_id].set(component_id, false);

	Logger::log("Component id = " + std::to_string(component_id) + " was removed from entity id " + std::to_string(entity_id));
}

template <typename TComponent>
bool Registry::has_component(Entity entity) const
{
	const auto component_id = Component<TComponent>::get_id();
	const auto entity_id = entity.get_id();
	return entity_component_signatures[entity_id].test(component_id);
}

template <typename TComponent>
TComponent& Registry::get_component(Entity entity) const
{
	const auto component_id = Component<TComponent>::get_id();
	const auto entity_id = entity.get_id();
	auto component_pool = std::static_pointer_cast<Pool<TComponent>>(component_pools[component_id]);
	return component_pool->get(entity_id); 
}

template <typename TComponent, typename ...TArgs> 
void Entity::add_component(TArgs&& ...args)
{
	registry->add_component<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::remove_component()
{
	registry->remove_component<TComponent>(*this);
}

template <typename TComponent>
bool Entity::has_component() const
{
	return registry->has_component<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::get_component() const
{
	return registry->get_component<TComponent>(*this);
}

#endif