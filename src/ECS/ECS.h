#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <set>
#include <unordered_map>
#include <typeindex>
#include <memory>

#include "../logger/logger.h"

namespace ecs
{
	const unsigned int MAX_COMPONENTS = 32;

	// Signature
	// We use a bitset (1s and 0s) to keep track of which components an entity has,
	// and also helps keep track of which entities a system is interested in.
	typedef std::bitset<MAX_COMPONENTS> signature;

	struct i_component
	{
		protected:
			static int next_id;
	};

	// Used to assign a unique id to component tyoe
	template <typename T>
	class component : public i_component
	{
		public:
			// Returns the unique id of Component<T>
			static int get_id()
			{
				static auto id = next_id++;
				return id;
			}
	};

	class entity 
	{
		private:
			int id;

		public:
			entity(int id): id(id) {};
			entity(const entity& entity) = default;
			int get_id() const;
			
			entity& operator =(const entity& other) = default;
			bool operator ==(const entity& other) const { return id  == other.id; }
			bool operator !=(const entity& other) const { return id != other.id; }
			bool operator >(const entity& other) const { return id > other.id; }
			bool operator <(const entity& other) const { return id < other.id; }

			template<typename t_component, typename ...t_args> void add_component(t_args&& ...args);
			template<typename t_component> void remove_component();
			template<typename t_component> bool has_component() const;
			template<typename t_component> t_component& get_component() const;

			// Hold a pointer to the entity's owner registry
			class registry* registry;
	};

	// The system processes entities that contain a specific signature
	class system 
	{
		private:
			signature component_signature;
			std::vector<entity> entities;

		public:
			system() = default;
			~system() = default;

			void add_entity_to_system(entity entity);
			void remove_entity_from_system(entity entity);
			std::vector<entity> get_system_entities() const;
			const signature& get_component_signature() const;

			// Define the component type T that entities must have to be considered by the system
			template <typename t_component> void require_component();
	};

	// Pool
	// A pool is just a vector (contigous data) of objects of type T
	class i_pool
	{
		public:
			virtual ~i_pool() {}
	};

	template <typename T>
	class pool : public i_pool
	{
		private:
			std::vector<T> data;

		public:
			pool(int size = 100) { data.resize(size); }
			virtual ~pool() = default;

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
	class registry 
	{
		private:
			int num_entities = 0;

			// Vector of component pools, each pool contains all the data for a certain component type
			// [Vector index = component type id]
			// [Pool index = entity id]
			std::vector<std::shared_ptr<i_pool>> component_pools;

			// Vecotr of component signatures per entity, saying which component is turned "on" for a given entity
			// [Vector index = entity id]
			std::vector<signature> entity_component_signatures;

			std::unordered_map<std::type_index, std::shared_ptr<system>> systems;

			// Set of entities that are flaggeds to be added or removed in the next registry Update()
			std::set<entity> entities_to_be_added;
			std::set<entity> entities_to_be_killed;

		public:
			registry()
			{ 
				logger::log("Registry constructor called");
			}

			~registry()
			{ 
				logger::log("Registry destructor called");
			}

			// The registry Update() finally process the entities that are waiting to be added/killed to the systems
			void update();

			// Entity management
			entity create_entity();

			// Componet management
			template <typename t_component, typename ...t_args> void add_component(entity entity, t_args&& ...args);
			template <typename t_component> void remove_component(entity entity);
			template <typename t_component> bool has_component(entity entity) const;
			template <typename t_component> t_component& get_component(entity entity) const;

			// System management
			template <typename t_system, typename ...t_args> void add_system(t_args&& ...args);
			template <typename t_system> void remove_system();
			template <typename t_system> bool has_system() const;
			template <typename t_system> t_system& get_system() const;

			// Checks the component signature of an entity and add the entity to the systems
			// that are interested in it
			void add_entity_to_systems(entity entity);
	};

	// Implementation of the function template
	template <typename t_component> 
	void system::require_component() 
	{
		const auto component_id = component<t_component>::get_id();
		component_signature.set(component_id);
	}

	template <typename t_system, typename ...t_args>
	void registry::add_system(t_args&& ...args)
	{
		std::shared_ptr<t_system> new_system = std::make_shared<t_system>(std::forward<t_args>(args)...);
		systems.insert(std::make_pair(std::type_index(typeid(t_system)), new_system));
	}

	template <typename t_system>
	void registry::remove_system()
	{
		auto system = systems.find(std::type_index(typeid(t_system)));
		systems.erase(system);
	}

	template <typename t_system>
	bool registry::has_system() const
	{
		return systems.find(std::type_index(typeid(t_system))) != systems.end();
	}

	template <typename t_system>
	t_system& registry::get_system() const
	{
		auto system = systems.find(std::type_index(typeid(t_system)));
		return *(std::static_pointer_cast<t_system>(system->second));
	}

	template <typename t_component, typename ...t_args> 
	void registry::add_component(entity entity, t_args&& ...args)
	{
		const auto component_id = component<t_component>::get_id();
		const auto entity_id = entity.get_id();

		if (component_id >= static_cast<int>(component_pools.size()))
		{
			component_pools.resize(component_id + 1, nullptr);
		}

		if (!component_pools[component_id])
		{
			std::shared_ptr<pool<t_component>> new_component_pool = std::make_shared<pool<t_component>>();
			component_pools[component_id] = new_component_pool;
		}

		std::shared_ptr<pool<t_component>> component_pool = std::static_pointer_cast<pool<t_component>>(component_pools[component_id]);

		if (entity_id >= component_pool->get_size())
		{
			component_pool->resize(num_entities);
		}

		t_component new_component(std::forward<t_args>(args)...);

		component_pool->set(entity_id, new_component);
		entity_component_signatures[entity_id].set(component_id);

		logger::log("Component id = " + std::to_string(component_id) + " was added to entity id " + std::to_string(entity_id));
	}

	template <typename t_component>
	void registry::remove_component(entity entity)
	{
		const auto component_id = component<t_component>::get_id();
		const auto entity_id = entity.get_id();
		entity_component_signatures[entity_id].set(component_id, false);

		logger::log("Component id = " + std::to_string(component_id) + " was removed from entity id " + std::to_string(entity_id));
	}

	template <typename t_component>
	bool registry::has_component(entity entity) const
	{
		const auto component_id = component<t_component>::get_id();
		const auto entity_id = entity.get_id();
		return entity_component_signatures[entity_id].test(component_id);
	}

	template <typename t_component>
	t_component& registry::get_component(entity entity) const
	{
		const auto component_id = component<t_component>::get_id();
		const auto entity_id = entity.get_id();
		auto component_pool = std::static_pointer_cast<pool<t_component>>(component_pools[component_id]);
		return component_pool->get(entity_id); 
	}

	template <typename t_component, typename ...t_args> 
	void entity::add_component(t_args&& ...args)
	{
		registry->add_component<t_component>(*this, std::forward<t_args>(args)...);
	}

	template <typename t_component>
	void entity::remove_component()
	{
		registry->remove_component<t_component>(*this);
	}

	template <typename t_component>
	bool entity::has_component() const
	{
		return registry->has_component<t_component>(*this);
	}

	template <typename t_component>
	t_component& entity::get_component() const
	{
		return registry->get_component<t_component>(*this);
	}
}
#endif