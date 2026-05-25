#pragma once

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
	// Returns the unique id of Component<T>
	static int GetId()
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
		int GetId() const;
		
		Entity& operator =(const Entity& other) = default;
		bool operator ==(const Entity& other) const { return id  == other.id; }
		bool operator !=(const Entity& other) const { return id != other.id; }
		bool operator >(const Entity& other) const { return id > other.id; }
		bool operator <(const Entity& other) const { return id < other.id; }
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

		void AddEntityToSystem(Entity entity);
		void RemoveEntityFromSystem(Entity entity);
		std::vector<Entity> GetSystemEntities() const;
		const Signature& GetComponentSignature() const;

		// Define the component type T that entities must have to be considered by the system
		template <typename TComponent> void RequireComponent();
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
		Pool(int size = 100) 
		{ 
			data.resize(size); 
		}

		virtual ~Pool() = default;

		bool isEmpty() const 
		{ 
			return data.empty(); 
		}

		int GetSize() const 
		{ 
			return data.size(); 
		}

		void Resize(int n) 
		{ 
			data.resize(n); 
		}

		void Clear() 
		{ 
			data.clear(); 
		}

		void Add(T object) 
		{ 
			data.push_back(object); 
		}

		void Set(int index, T object) 
		{ 
			data[index] = object; 
		}

		T& Get(int index) 
		{ 
			return static_cast<T&>(data[index]); 
		}

		T& operator [](unsigned int index) 
		{ 
			return data[index]; 
		}
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
		// [Vector incex = entity id]
		std::vector<Signature> entity_component_signatures;

		std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

		// Set of entities that are flaggeds to be added or removed in the next registry Update()
		std::set<Entity> entities_to_be_added;
		std::set<Entity> entities_to_be_killed;

	public:
		Registry()
		{ 
			Logger::Log("Registry constructor called");
		}

		~Registry()
		{ 
			Logger::Log("Registry destructor called");
		}

		// The registry Update() finally process the entities that are waiting to be added/killed to the systems
		void Update();

		// Entity management
		Entity CreateEntity();

		// Componet management
		template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
		template <typename T> void RemoveComponent(Entity entity);
		template <typename T> bool HasComponent(Entity entity);

		// System management
		template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
		template <typename TSystem> void RemoveSystem();
		template <typename TSystem> bool HasSystem() const;
		template <typename TSystem> TSystem& GetSystem() const;

		// Checks the component signature of an entity and add the entity to the systems
		// that are interested in it
		void AddEntityToSystems(Entity entity);
};

// Implementation of the function template
template <typename TComponent> 
void System::RequireComponent() 
{
	const auto component_id = Component<TComponent>::GetId();
	component_signature.set(component_id);
}

template <typename TSystem, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args)
{
	std::shared_ptr<TSystem> new_system = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), new_system));
}

template <typename TSystem>
void Registry::RemoveSystem()
{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}

template <typename TSystem>
bool Registry::HasSystem() const
{
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const
{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent, typename ...TArgs> 
void Registry::AddComponent(Entity entity, TArgs&& ...args)
{
	const auto component_id = Component<TComponent>::GetId();
	const auto entity_id = entity.GetId();

	if (component_id >= component_pools.size())
	{
		component_pools.resize(component_id + 1, nullptr);
	}

	if (!component_pools[component_id])
	{
		std::shared_ptr<Pool<TComponent>> new_component_pool = std::make_shared<Pool<TComponent>>();
		component_pools[component_id] = new_component_pool;
	}

	std::shared_ptr<Pool<TComponent>> component_pool = std::static_pointer_cast<Pool<TComponent>>(component_pools[component_id]);

	if (entity_id >= component_pool->GetSize())
	{
		component_pool->Resize(num_entities);
	}

	TComponent new_component(std::forward<TArgs>(args)...);

	component_pool->Set(entity_id, new_component);
	entity_component_signatures[entity_id].set(component_id);
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
	const auto component_id = Component<TComponent>::GetId();
	const auto entity_id = entity.GetId();
	entity_component_signatures[entity_id].set(component_id, false);
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity)
{
	const auto component_id = Component<TComponent>::GetId();
	const auto entity_id = entity.GetId();
	return entity_component_signatures[entity_id].test(component_id);
}
