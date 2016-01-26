/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * di.cpp
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
   
#include "di.hpp"

#include <algorithm>
#include <map>
#include <iostream>

#include <ltdl.h>

namespace di
{

//
// Registry
//

component_id registry::_idcount = 0;
std::vector<registry*> registry::_registries;
registry registry::_singleton;

registry::registry()
{
	if(_registries.size()==0)
	{
		if(lt_dlinit()!=0)
		{
			std::cerr << "Error while initializing libltdl" << std::endl;
		}
	}
	_registries.push_back(this);
}
		
registry::~registry()
{
	for(auto it = _registries.begin(); it!=_registries.end();)
	{
		if(*it == this)
		{
			_registries.erase(it);
		}
		else
		{
			++it;
		}
	}

	if(_registries.size()==0)
	{
		if(lt_dlexit()!=0)
		{
			std::cerr << "Error while exiting libltdl" << std::endl;
		}
	}
}
	
registry& registry::get()
{
	return registry::_singleton;
}

std::size_t registry::size()const
{
	return _components.size();
}

component_ptr_t registry::get(component_id id) const
{
	for(auto comp : _components)
	{
		if(comp.id == id)
		{
			return comp.comp;
		}
	}
	return component_ptr_t();
}

component_ptr_t registry::get(const std::string& name) const
{
	for(auto comp : _components)
	{
		if(comp.name == name)
		{
			return comp.comp;
		}
	}
	return component_ptr_t();
}

registry::const_iterator registry::find(component_id id) const
{
	return std::find_if(cbegin(), cend(), [&](const component_descriptor& desc){return desc.id == id;});
}

registry::const_iterator registry::find(const std::string& name) const
{
	return std::find_if(cbegin(), cend(), [&](const component_descriptor& desc){return desc.name == name;});
}

registry::const_iterator registry::find(const component* comp) const
{
	return std::find_if(cbegin(), cend(), [&](const component_descriptor& desc){return desc.comp.get() == comp;});
}

registry::const_iterator registry::begin()const
{
	return _components.begin();
}

registry::const_iterator registry::end()const
{
	return _components.end();
}

registry::const_iterator registry::registry::cbegin()const
{
	return _components.cbegin();
}

registry::const_iterator registry::cend()const
{
	return _components.cend();
}

registry::const_reverse_iterator registry::rbegin()const
{
	return _components.rbegin();
}

registry::const_reverse_iterator registry::rend()const
{
	return _components.rend();
}

registry::const_reverse_iterator registry::crbegin()const
{
	return _components.crbegin();
}

registry::const_reverse_iterator registry::crend()const
{
	return _components.crend();
}

registry::const_iterator registry::set(const component_descriptor& desc)
{
	return _components.emplace(_components.end(), _idcount++, desc.name, desc.comp, desc.prop);
}

registry::const_iterator registry::set(component_descriptor&& desc)
{
	return _components.emplace(_components.end(), _idcount++, std::move(desc.name), std::move(desc.comp), std::move(desc.prop));
}

registry::const_iterator registry::set(const std::string& name, component_ptr_t comp)
{
	return _components.emplace(_components.end(), _idcount++, name, comp);
}

registry::const_iterator registry::set(const std::string& name, component_ptr_t comp, const properties_t& prop)
{
	return _components.emplace(_components.end(), _idcount++, name, comp, prop);
}

registry::const_iterator registry::set(const std::string& name, component_ptr_t comp, properties_t&& prop)
{
	return _components.emplace(_components.end(), _idcount++, name, comp, prop);
}

registry::const_iterator registry::set(const std::string& name, component_ptr_t comp, properties_init_list_t prop)
{
	return _components.emplace(_components.end(), _idcount++, name, comp, prop);
}

void registry::erase(registry::const_iterator it)
{
	if(it!=end())
	{
		_components.erase( _components.begin() + (it-cbegin()) );
	}
}

void registry::erase(component_id id)
{
	for(registry* reg : _registries)
	{
		if(reg!=nullptr && reg->_components.size()>0)
		{
			for(auto it = reg->_components.begin(); it != reg->_components.end(); )
			{
				if(it->id == id)
				{
					reg->erase(it);
				}
				else
				{
					++it;
				}
			}
		}
	}
}

void registry::load(const std::string& path)
{
	component_loader::locker lock(*this);
	
	lt_dlhandle handle = lt_dlopenext(path.c_str());
	if(handle==nullptr)
	{
		std::cerr << "Error while loading " << path << std::endl;
	}
}



//
// component_loader 
//

std::stack<registry*> component_loader::_registries;

void component_loader::push_registry(registry& reg)
{
	_registries.push(&reg);
}

void component_loader::pop_registry()
{
	_registries.pop();
}

component_id component_loader::set(const component_descriptor& desc)
{
	return ( _registries.empty() ? &registry::get() : _registries.top() )->set(desc)->id;
}

component_id component_loader::set(component_descriptor&& desc)
{
	return ( _registries.empty() ? &registry::get() : _registries.top() )->set(desc)->id;
}

component_id component_loader::set(const std::string& name, component_ptr_t comp)
{
	return ( _registries.empty() ? &registry::get() : _registries.top() )->set(name, comp)->id;
}

component_id component_loader::set(const std::string& name, component_ptr_t comp, const properties_t& prop)
{
	return ( _registries.empty() ? &registry::get() : _registries.top() )->set(name, comp, prop)->id;
}

component_id component_loader::set(const std::string& name, component_ptr_t comp, properties_t&& prop)
{
	return ( _registries.empty() ? &registry::get() : _registries.top() )->set(name, comp, prop)->id;
}

component_id component_loader::set(const std::string& name, component_ptr_t comp, properties_init_list_t prop)
{
	return ( _registries.empty() ? &registry::get() : _registries.top() )->set(name, comp, prop)->id;
}

//
// component_loader::locker
//

component_loader::locker::locker(registry& reg)
{
	component_loader::push_registry(reg);
}

component_loader::locker::~locker()
{
	component_loader::pop_registry();
}




} // namespace di