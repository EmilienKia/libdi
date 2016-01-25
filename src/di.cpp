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

registry registry::_singleton;

registry::registry()
{
	if(lt_dlinit()!=0)
	{
		std::cerr << "Error while initializing libltdl" << std::endl;
	}
}
		
registry::~registry()
{
	if(lt_dlexit()!=0)
	{
		std::cerr << "Error while exiting libltdl" << std::endl;
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

std::shared_ptr<component> registry::get(component_id id) const
{
	for(auto comp : _components)
	{
		if(comp.id == id)
		{
			return comp.comp;
		}
	}
	return std::shared_ptr<component>();
}

std::shared_ptr<component> registry::get(const std::string& name) const
{
	for(auto comp : _components)
	{
		if(comp.name == name)
		{
			return comp.comp;
		}
	}
	return std::shared_ptr<component>();
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

registry::const_iterator registry::set(const std::string& name, std::shared_ptr<component> component)
{
	_components.emplace_back(_idcount++, name, component);
	return --end();
}

void registry::erase(registry::const_iterator it)
{
	if(it!=end())
	{
		_components.erase( _components.begin() + (it-cbegin()) );
	}
}

void registry::load(const std::string& path)
{
	lt_dlhandle handle = lt_dlopenext(path.c_str());
	if(handle==nullptr)
	{
		std::cerr << "Error while loading " << path << std::endl;
	}
}

} // namespace di