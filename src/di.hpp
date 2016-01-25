/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * di.hpp
 * Copyright (C) 2016 Emilien Kia <emilien.kia@gmail.com>
 *
 * depinj is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * depinj is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _DI_HPP_
#define _DI_HPP_

#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace di
{

typedef size_t component_id;

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

/**
 * Component descriptor.
 * Internal structure used to keep component properties in registry.
 * Each component has:
 * - a unique numeric identifier 'id'
 * - a name 'name', which should be unique
 * - its shared pointer 'comp'
 */
struct component_descriptor
{
	component_id id;
	std::string name;
	std::shared_ptr<di::component> comp;

	component_descriptor(component_id id, const std::string& name, std::shared_ptr<component> comp):
	id(id), name(name), comp(comp)
	{}
};

/**
 * Component registry.
 * Central container which holds components.
 */
class registry
{
public:

	typedef std::vector<component_descriptor> comp_holder;
	typedef typename comp_holder::iterator iterator;
	typedef typename comp_holder::const_iterator const_iterator;
	typedef typename comp_holder::reverse_iterator reverse_iterator;
	typedef typename comp_holder::const_reverse_iterator const_reverse_iterator;

	/**
	 * Retrieve registry singleton.
	 */
	static registry& get();

	/**
	 * Retrieve number of registered components.
	 */
	std::size_t size()const;

	/**
	 * Find a component from its unique id.
	 */
	std::shared_ptr<component> get(component_id id) const;
	/**
	 * Find a component from its name (the first found).
	 */
	std::shared_ptr<component> get(const std::string& name) const;

	/**
	 * Find a component from its unique id.
	 */
	const_iterator find(component_id id) const;
	/**
	 * Find a component from it name.
	 */
	const_iterator find(const std::string& name) const;
	/**
	 * Find a component from its pointer.
	 */
	const_iterator find(const component* comp) const;

	const_iterator begin()const;
	const_iterator end()const;
	const_iterator cbegin()const;
	const_iterator cend()const;
	const_reverse_iterator rbegin()const;
	const_reverse_iterator rend()const;
	const_reverse_iterator crbegin()const;
	const_reverse_iterator crend()const;

	/**
	 * Register a new component.
	 */	
	const_iterator set(const std::string& name, std::shared_ptr<component> comp);
	/**
	 * Unregister an already registered component.
	 */
	void erase(const_iterator it);

	/**
	 * Find a component from a type.
	 */
	template<typename C>
	std::shared_ptr<C> find()
	{
		for(const_iterator it = begin(); it!=end(); ++it)
		{
			std::shared_ptr<C> ptr = std::dynamic_pointer_cast<C>(it->comp);
			if(ptr)
			{
				return ptr;
			}
		}
		return std::shared_ptr<C>();
		
	}

	/**
	 * Find a liszt of components from a type.
	 */
	template<typename C>
	std::vector<std::shared_ptr<C>> find_all()
	{
		std::vector<std::shared_ptr<C>> res;
		for(const_iterator it = begin(); it!=end(); ++it)
		{
			std::shared_ptr<C> ptr = std::dynamic_pointer_cast<C>(it->comp);
			if(ptr)
			{
				res.emplace_back(ptr);
			}
		}
		return res;		
	}

	/**
	 * Load a library (and register all component instances).
	 */
	void load(const std::string& path);

protected:
	registry();
	~registry();
	
private:
	comp_holder _components;
	component_id _idcount = 0;

	static registry _singleton;
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

	explicit component_instance(const std::string& name):component_instance(name, std::make_shared<component_type>())
	{
	}

	explicit component_instance(const std::string& name, C* comp):component_instance(name, std::shared_ptr<component_type>(comp))
	{
	}
	
	explicit component_instance():component_instance(typeid(component_type).name(), std::make_shared<component_type>())
	{
	}

	explicit component_instance(C* comp):component_instance(typeid(component_type).name(), std::shared_ptr<component_type>(comp))
	{
	}

	explicit component_instance(component_ptr comp):component_instance(typeid(component_type).name(), comp)
	{
	}
	
	explicit component_instance(const std::string& name, component_ptr comp):
		_name(name),_instance(comp)
	{
		_id = registry::get().set(_name, comp)->id;
	}

	template<class... Args >
	component_instance(const std::string& name, Args&&... args):component_instance(name, std::make_shared<component_type>(args...))
	{
	}

	~component_instance()
	{
		registry::get().erase(registry::get().find(_id));
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
	component_id _id;
};

} // namespace di
#endif // _DI_HPP_

