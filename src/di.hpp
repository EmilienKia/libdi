/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * di.hpp
 * 
 * Copyright (C) 2016 Emilien Kia <emilien.kia@gmail.com>
 *
 * libdi is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libdi is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */

#ifndef _DI_HPP_
#define _DI_HPP_

#include <cstddef>
#include <iostream>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>

namespace di
{

/**
 * Base component class.
 * This is an empty class used to avoid usage of void* for pointing to components.
 */
class component
{
public:
	component() = default;
	virtual ~component() = default;
};

typedef ssize_t component_id;
typedef std::shared_ptr<di::component> component_ptr_t;
typedef std::map<std::string, std::string> properties_t;
typedef std::initializer_list<std::pair<const std::string, std::string>> properties_init_list_t;

/**
 * Component descriptor.
 * Internal structure used to keep component properties in registry.
 * Each component has:
 * - a unique numeric identifier 'id'
 * - a name 'name', which should be unique
 * - its shared pointer 'comp'
 * - a key/value property map 'prop'
 */
struct component_descriptor
{
	component_id    id;
	std::string     name;
	component_ptr_t comp;
	properties_t    prop;

	component_descriptor(component_id id, const std::string& name, component_ptr_t comp):
	id(id), name(name), comp(comp)
	{}

	component_descriptor(component_id id, const std::string& name, component_ptr_t comp, const properties_t& prop):
	id(id), name(name), comp(comp), prop(prop)
	{}

	component_descriptor(component_id id, const std::string& name, component_ptr_t comp, properties_t&& prop):
	id(id), name(name), comp(comp), prop(std::move(prop))
	{}

	component_descriptor(component_id id, const std::string& name, component_ptr_t comp, properties_init_list_t prop):
	id(id), name(name), comp(comp), prop(prop)
	{}


	component_descriptor(const component_descriptor& desc):
		id(desc.id), name(desc.name), comp(desc.comp), prop(desc.prop)
	{}

	component_descriptor(component_descriptor&& desc):
		id(desc.id), name(std::move(desc.name)), comp(std::move(desc.comp)), prop(std::move(desc.prop))
	{}

	component_descriptor& operator = (const component_descriptor& desc)
	{
		id = desc.id;
		name = desc.name;
		comp = desc.comp;
		prop = desc.prop;
		return *this;
	}
		
	component_descriptor& operator = (component_descriptor&& desc)
	{
		id = desc.id;
		name = std::move(desc.name);
		comp = std::move(desc.comp);
		prop = std::move(desc.prop);
		return *this;
	}

	

};

/**
 * Component registry.
 * Container which holds components.
 */
class registry
{
public:
	registry(registry* parent = nullptr);
	~registry();

	typedef std::vector<component_descriptor> comp_holder;

	/**
	 * Retrieve default registry singleton.
	 */
	static registry& get();

	registry* parent(){return _parent;}
	const registry* parent()const{return _parent;}
	registry& parent(registry* parent){_parent = parent;}
	
	/**
	 * Retrieve number of registered components.
	 */
	std::size_t size()const;

	/**
	 * Find a component from its unique id.
	 */
	const component_descriptor* get(component_id id) const;
	/**
	 * Find a component from it name.
	 */
	const component_descriptor* get(const std::string& name) const;
	/**
	 * Find a component from its pointer.
	 */
	const component_descriptor* get(const component* comp) const;

	/**
	 * Register a new component.
	 */	
	const component_descriptor& set(const component_descriptor& desc);
	const component_descriptor& set(component_descriptor&& desc);
	const component_descriptor& set(const std::string& name, component_ptr_t comp);
	const component_descriptor& set(const std::string& name, component_ptr_t comp, const properties_t& prop);
	const component_descriptor& set(const std::string& name, component_ptr_t comp, properties_t&& prop);
	const component_descriptor& set(const std::string& name, component_ptr_t comp, properties_init_list_t prop);
	
	/**
	 * Unregister an already registered component.
	 */
	static void erase(component_id id);

	/**
	 * Find a component from its unique id.
	 */
	component_ptr_t find(component_id id) const;
	/**
	 * Find a component from its name (the first found).
	 */
	component_ptr_t find(const std::string& name) const;
	
	/**
	 * Find a component from a type.
	 */
	template<typename T>
	std::shared_ptr<T> find()const
	{
		for(const registry* reg=this; reg!=nullptr; reg = reg->parent())
		{
			for(comp_holder::const_iterator it = reg->_components.begin(); it!=reg->_components.end(); ++it)
			{
				std::shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(it->comp);
				if(ptr)
				{
					return ptr;
				}
			}
		}
		return std::shared_ptr<T>();
	}

	/**
	 * Find a list of components from a type.
	 */
	template<typename T>
	std::vector<std::shared_ptr<T>> find_all()const
	{
		std::vector<std::shared_ptr<T>> res;
		for(const registry* reg=this; reg!=nullptr; reg = reg->parent())
		{
			for(comp_holder::const_iterator it = reg->_components.begin(); it!=reg->_components.end(); ++it)
			{
				std::shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(it->comp);
				if(ptr)
				{
					res.emplace_back(ptr);
				}
			}
		}
		return res;		
	}

	/**
	 * Find a component from a type and a predicate.
	 */
	template<typename T, typename UnaryPredicate>
	std::shared_ptr<T> find_if(UnaryPredicate p)const
	{
		for(const registry* reg=this; reg!=nullptr; reg = reg->parent())
		{
			for(comp_holder::const_iterator it = reg->_components.begin(); it!=reg->_components.end(); ++it)
			{
				std::shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(it->comp);
				if(ptr && p(*it))
				{
					return ptr;
				}
			}
		}
		return std::shared_ptr<T>();
	}

	/**
	 * Find a list of components from a type and a predicate.
	 */
	template<typename T, typename UnaryPredicate>
	std::vector<std::shared_ptr<T>> find_all_if(UnaryPredicate p)const
	{
		std::vector<std::shared_ptr<T>> res;
		for(const registry* reg=this; reg!=nullptr; reg = reg->parent())
		{
			for(comp_holder::const_iterator it = reg->_components.begin(); it!=reg->_components.end(); ++it)
			{
				std::shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(it->comp);
				if(ptr && p(*it))
				{
					res.emplace_back(ptr);
				}
			}
		}
		return res;
	}

	/**
	 * Iterate on components and recursivly.
	 */
	template<typename Action>
	void foreach(Action a)const
	{
		for(const registry* reg=this; reg!=nullptr; reg = reg->parent())
		{
			for(comp_holder::const_iterator it = reg->_components.begin(); it!=reg->_components.end(); ++it)
			{
				a(*it);
			}
		}
	}

	template<typename UnaryPredicate, typename Action>
	void foreach_if(UnaryPredicate p, Action a)const
	{
		for(const registry* reg=this; reg!=nullptr; reg = reg->parent())
		{
			for(comp_holder::const_iterator it = reg->_components.begin(); it!=reg->_components.end(); ++it)
			{
				if(p(*it))
				{
					a(*it);
				}
			}
		}
	}

	template<typename T, typename Action>
	void foreach(Action a)const
	{
		for(const registry* reg=this; reg!=nullptr; reg = reg->parent())
		{
			for(comp_holder::const_iterator it = reg->_components.begin(); it!=reg->_components.end(); ++it)
			{
				std::shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(it->comp);
				if(ptr)
				{
					a(*it);
				}
			}
		}
	}

	template<typename T, typename UnaryPredicate, typename Action>
	void foreach_if(UnaryPredicate p, Action a)const
	{
		for(const registry* reg=this; reg!=nullptr; reg = reg->parent())
		{
			for(comp_holder::const_iterator it = reg->_components.begin(); it!=reg->_components.end(); ++it)
			{
				std::shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(it->comp);
				if(ptr && p(*it))
				{
					a(*it);
				}
			}
		}
	}
	
	/**
	 * Load a library (and register all component instances).
	 */
	void load(const std::string& path);
	
private:
	registry*   _parent;
	comp_holder _components;
	static component_id _idcount;
	static std::vector<registry*> _registries;
	static registry _singleton;
};


/**
 * Helper to manage just loaded components from modules.
 * Should not be used by users.
 */
class component_loader
{
public:	
	/**
	 * Add a new component.
	 */
	static component_id set(const component_descriptor& desc);
	static component_id set(component_descriptor&& desc);
	static component_id set(const std::string& name, component_ptr_t comp);
	static component_id set(const std::string& name, component_ptr_t comp, const properties_t& prop);
	static component_id set(const std::string& name, component_ptr_t comp, properties_t&& prop);
	static component_id set(const std::string& name, component_ptr_t comp, properties_init_list_t prop);

	static void push_registry(registry& reg);
	static void pop_registry();

	class locker
	{
	public:
		locker(registry& reg);
		~locker();
	};
	
private:
	static std::stack<registry*> _registries;
};


/**
 * Helper to automatically instantiate and register a component.
 */
template <typename C>
class component_instance
{
public:
	typedef C component_type;
	typedef std::shared_ptr<C> component_ptr;

	component_instance():component_instance(typeid(component_type).name(), std::make_shared<component_type>())
	{
	}
	
	component_instance(const std::string& name):component_instance(name, std::make_shared<component_type>())
	{
	}

	component_instance(const std::string& name, C* comp):component_instance(name, std::shared_ptr<component_type>(comp))
	{
	}

	component_instance(C* comp):component_instance(typeid(component_type).name(), std::shared_ptr<component_type>(comp))
	{
	}

	component_instance(component_ptr comp):component_instance(typeid(component_type).name(), comp)
	{
	}
	
	component_instance(const std::string& name, component_ptr comp):
		_name(name),_instance(comp)
	{
		_id = component_loader::set(_name, comp);
	}

	component_instance(const std::string& name, component_ptr comp, const properties_t& prop):
		_name(name),_instance(comp)
	{
		_id = component_loader::set(_name, comp, prop);		
	}

	component_instance(const std::string& name, component_ptr comp, properties_t&& prop):
		_name(name),_instance(comp)
	{
		_id = component_loader::set(_name, comp, prop);		
	}

	component_instance(const std::string& name, component_ptr comp, properties_init_list_t& prop):
		_name(name),_instance(comp)
	{
		_id = component_loader::set(_name, comp, prop);		
	}



	template<class... Args >
	component_instance(const std::string& name, Args&&... args):component_instance(name, std::make_shared<component_type>(args...))
	{
	}

	template<class... Args >
	component_instance(const std::string& name, const properties_t& prop, Args&&... args):component_instance(name, std::make_shared<component_type>(args...), prop)
	{
	}

	template<class... Args >
	component_instance(const std::string& name, properties_t&& prop, Args&&... args):component_instance(name, std::make_shared<component_type>(args...), prop)
	{
	}

	template<class... Args >
	component_instance(const std::string& name, properties_init_list_t prop, Args&&... args):component_instance(name, std::make_shared<component_type>(args...), prop)
	{
	}

	~component_instance()
	{
		if(_id!=-1)
		{
			registry::erase(_id);
		}
	}

	component_ptr& get()
	{
		return _instance;
	}

	const component_ptr& get()const
	{
		return _instance;
	}

	const std::string& name()const
	{
		return _name;
	}

private:
	std::string _name;
	component_ptr _instance;
	component_id _id = -1;
};

} // namespace di
#endif // _DI_HPP_

