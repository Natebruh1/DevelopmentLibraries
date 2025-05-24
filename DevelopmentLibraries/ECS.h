#pragma once
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <vector>
#include <typeindex>
#include <type_traits>
#include <cassert>

//Type alias for entity ID
using Entity = std::uint32_t;

//Forward dec
class ECS;

//Component storage base for type-erased access
struct IComponentStorage
{
    virtual void remove(Entity) = 0; //Pure Virtual
    virtual ~IComponentStorage() = default;
};
//Component storage template for type-specific access
template<typename T>
class ComponentStorage : public IComponentStorage
{
public:
	std::unordered_map<Entity, T> components;

	void add(Entity entity, T value) //Add a component to the storage
	{
		components[entity] = std::move(value);
	}
	T& get(Entity entity) //Get a component from the storage
	{
		assert(components.find(entity) != components.end() && "Entity does not have this component"); //Compile time check
		return components[entity];
	}
	bool has(Entity entity) const //Check if the storage has a component for the  requested entity
	{
		return components.find(entity) != components.end();
	}
	void remove(Entity entity) override //Remove a component from the storage
	{
		components.erase(entity);
	}
	auto begin() { return components.begin(); } //Iterator begin
	auto end() { return components.end(); } //Iterator end
	auto begin() const { return components.begin(); } //Const iterator begin
	auto end() const { return components.end(); } //Const iterator end
};

//ECS class for managing entities and components
class ECS {
    //Next available ID (incremented per entity)
    Entity next_entity = 1;

    //Map of type to storage interface
    mutable std::unordered_map<std::type_index, IComponentStorage*> components; //Note the use of the keyword mutable here to signify that it is ok to modify the map inside of const functions which is especially useful when using the [] operator



    /* Retrieves or lazily creates the storage for a specific component type T.
    All component storages inherit from IComponentStorage for type-erased handling.
    Each ComponentStorage<T> maps entities to component data (unordered_map).
    This function ensures a single storage instance exists per component type.
    Used internally by all ECS operations involving component access.               */
    template<typename T>
    ComponentStorage<T>& get_storage() const
    {
        std::type_index index(typeid(T)); //Get type index of T for type-erasure (std::type_index is used to store type information at runtime, and we use it to map types to their respective storage)
        if (components.find(index) == components.end())
        {
            components[index] = new ComponentStorage<T>();
        }
        return *static_cast<ComponentStorage<T>*>(components[index]);
    }

public:
    ~ECS() {
        //Cleanup all component storages
        for (auto& [_, ptr] : components)
        {
            delete ptr;
        }
    }

    Entity create_entity()
    {
        return next_entity++;
    }

    template<typename T>
    void add_component(Entity e, T value)
    {
        get_storage<T>().add(e, std::move(value));
    }

    template<typename T>
    T& get(Entity e)
    {
        return get_storage<T>().get(e);
    }

    template<typename T>
    bool has(Entity e) const
    {
        return get_storage<T>().has(e);
    }

    template<typename... Ts, typename Func> //Variadic Template for multiple component types and a function that takes an entity and those components
    void each(Func&& func) { //R-Value Reference to a function that takes an entity and components (i.e. Func(Entity, Ts...)) such as a lambda
        auto& first = get_storage<std::tuple_element_t<0, std::tuple<Ts...>>>();
        for (auto& [e, _] : first.components)
        {
            /*Use the first component type in Ts... to seed the iteration.
            This limits the search space to only entities that already have that component,
            which improves performance and avoids scanning every entity.
            For each of those, we check if it also has the other components.
            This is a practical early-out optimization for ECS queries.     */
            if ((get_storage<Ts>().has(e) && ...))
            {
                func(e, get_storage<Ts>().get(e)...);
            }
        }
    }

    // Executes the given function with the components of entity e,
// only if it has *all* of the specified component types.
    template<typename... Ts, typename Func>
    void with(Entity e, Func&& func)
    {
        // Check if the entity has all required components
        if ((has<Ts>(e) && ...))
        {
            // Pass the components to the function
            func(get<Ts>(e)...);
        }
    }

};