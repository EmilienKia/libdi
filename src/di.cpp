/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * di.cpp
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

registry::registry(registry* parent):
_parent(parent)
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
	return _components.size() + (_parent!=nullptr?_parent->size():0);
}

component_ptr_t registry::find(component_id id) const
{
	for(const registry* reg=this; reg!=nullptr; reg = reg->parent())
	{
		for(comp_holder::const_iterator it = reg->_components.begin(); it!=reg->_components.end(); ++it)
		{
			if(it->id == id)
			{
				return it->comp;
			}
		}
	}
	return component_ptr_t();
}

component_ptr_t registry::find(const std::string& name) const
{
	for(const registry* reg=this; reg!=nullptr; reg = reg->parent())
	{
		for(comp_holder::const_iterator it = reg->_components.begin(); it!=reg->_components.end(); ++it)
		{
			if(it->name == name)
			{
				return it->comp;
			}
		}
	}
	return component_ptr_t();
}

const component_descriptor* registry::get(component_id id) const
{
	auto it = std::find_if(_components.cbegin(), _components.cend(), [&](const component_descriptor& desc){return desc.id == id;});
	return it != _components.cend() ? &*it : nullptr;
}

const component_descriptor* registry::get(const std::string& name) const
{
	auto it = std::find_if(_components.cbegin(), _components.cend(), [&](const component_descriptor& desc){return desc.name == name;});
	return it != _components.cend() ? &*it : nullptr;
}

const component_descriptor* registry::get(const component* comp) const
{
	auto it = std::find_if(_components.cbegin(), _components.cend(), [&](const component_descriptor& desc){return desc.comp.get() == comp;});
	return it != _components.cend() ? &*it : nullptr;
}

const component_descriptor& registry::set(const component_descriptor& desc)
{
	return *_components.emplace(_components.end(), _idcount++, desc.name, desc.comp, desc.prop);
}

const component_descriptor& registry::set(component_descriptor&& desc)
{
	return *_components.emplace(_components.end(), _idcount++, std::move(desc.name), std::move(desc.comp), std::move(desc.prop));
}

const component_descriptor& registry::set(const std::string& name, component_ptr_t comp)
{
	return *_components.emplace(_components.end(), _idcount++, name, comp);
}

const component_descriptor& registry::set(const std::string& name, component_ptr_t comp, const properties_t& prop)
{
	return *_components.emplace(_components.end(), _idcount++, name, comp, prop);
}

const component_descriptor& registry::set(const std::string& name, component_ptr_t comp, properties_t&& prop)
{
	return *_components.emplace(_components.end(), _idcount++, name, comp, prop);
}

const component_descriptor& registry::set(const std::string& name, component_ptr_t comp, properties_init_list_t prop)
{
	return *_components.emplace(_components.end(), _idcount++, name, comp, prop);
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
					reg->_components.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
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
	return ( _registries.empty() ? &registry::get() : _registries.top() )->set(desc).id;
}

component_id component_loader::set(component_descriptor&& desc)
{
	return ( _registries.empty() ? &registry::get() : _registries.top() )->set(desc).id;
}

component_id component_loader::set(const std::string& name, component_ptr_t comp)
{
	return ( _registries.empty() ? &registry::get() : _registries.top() )->set(name, comp).id;
}

component_id component_loader::set(const std::string& name, component_ptr_t comp, const properties_t& prop)
{
	return ( _registries.empty() ? &registry::get() : _registries.top() )->set(name, comp, prop).id;
}

component_id component_loader::set(const std::string& name, component_ptr_t comp, properties_t&& prop)
{
	return ( _registries.empty() ? &registry::get() : _registries.top() )->set(name, comp, prop).id;
}

component_id component_loader::set(const std::string& name, component_ptr_t comp, properties_init_list_t prop)
{
	return ( _registries.empty() ? &registry::get() : _registries.top() )->set(name, comp, prop).id;
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


//
// simple_component_loader
//

simple_component_loader::simple_component_loader(registry& reg):
_reg(reg)
{
}

bool simple_component_loader::load(const std::string& filename)
{
	component_loader::locker lock(_reg);
	return lt_dlopenext(filename.c_str()) != nullptr;
}

void simple_component_loader::load(const std::vector<std::string>& filenames)
{
	component_loader::locker lock(_reg);
	for(std::string filename : filenames)
	{
		lt_dlhandle handle = lt_dlopenext(filename.c_str());
		if(handle==nullptr)
		{
			std::cerr << "Error while loading " << filename << " : " << lt_dlerror() << std::endl;
		}
	}
}

static int load_all_cb(const char *filename, std::vector<std::string>* paths)
{
	paths->push_back(filename);
	return 0;
}

void simple_component_loader::load_all(const std::string& dirname)
{
	std::vector<std::string> paths;
	lt_dlforeachfile(dirname.c_str(), (int(*)(const char *, void*))load_all_cb, (void*)&paths);
	load(paths);
}

struct load_all_test_st
{
	simple_component_loader::filter_t& filter;
	std::vector<std::string> paths;
};

static int load_all_test_cb(const char *filename, load_all_test_st* st)
{
	std::string path(filename);
	if(st->filter(path))
	{
		st->paths.push_back(path);
	}
	return 0;
}

void simple_component_loader::load_all(const std::string& dirname, filter_t& filter)
{
	load_all_test_st test{filter};
	lt_dlforeachfile(dirname.c_str(), (int(*)(const char *, void*))load_all_test_cb, (void*)&test);
	load(test.paths);

}

} // namespace di
