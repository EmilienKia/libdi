/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.cc
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

#include <iostream>

#include <map>

#include "di.hpp"

#include "service01.hpp"

//
// IntegratedHelloServiceImpl
// 

class IntegratedHelloServiceImpl : public HelloService
{
public:
	IntegratedHelloServiceImpl() = default;
	IntegratedHelloServiceImpl(const std::string& str):_str(str){}
	virtual ~IntegratedHelloServiceImpl() = default;

	virtual void sayHello(const std::string& name)const;
	virtual size_t count();
private:
	size_t _count;
	std::string _str = "<<default>>";
};

void IntegratedHelloServiceImpl::sayHello(const std::string& name)const
{
	std::cout << "(integrated) Hello " << _str << " " << name << " !" << std::endl;
}

size_t IntegratedHelloServiceImpl::count()
{
	return _count++;
}

di::component_instance<IntegratedHelloServiceImpl> IntegratedHello {"hello", {{"Titi", "Toto"}}, "big" };


//
// TotoServiceImpl
// 
class TotoServiceImpl : public TotoService
{
public:
	TotoServiceImpl() = default;
	virtual void titi()
	{
		std::cout << "titi(main) " << std::endl;
	}
};

di::component_instance<TotoServiceImpl> TotoServiceImplInstance {"toto", {{"Titi", "Toto"}} };


//
// Main
//

int test();


void dump()
{
	di::registry& registry = di::registry::get();
	std::cout << "count = " << registry.size()  << std::endl;

	{
		auto arr = registry.find_all<HelloService>();
		for(auto comp : arr)
		{
			std::cout << "found HelloService : " << comp->count() << " ";
			comp->sayHello("World");
			std::cout << std::endl;
		}
	}

	{
		auto arr = registry.find_all<TotoService>();
		for(auto comp : arr)
		{
			std::cout << "found : TotoService ";
			comp->titi();
			std::cout << std::endl;
		}
	}
}


void dumpRegistry()
{
	di::registry& registry = di::registry::get();
	std::cout << "Registry: " << registry.size()  << std::endl;

	{
		for(auto& comp : registry)
		{
			std::cout
				<< comp.id << " "
				<< comp.name << " "
				<< comp.comp.get()
				<< std::endl;
			for(auto prop : comp.prop)
			{
				std::cout << "  " << prop.first << " : " << prop.second << std::endl;
			}
		}
	}
}



int main()
{
	std::cout << "Hello world!" << std::endl;


	std::map<std::string, std::string> map{{"titi", "toto"}, {"Boule", "Bill"}};

	for(auto it : map)
	{
		std::cout << " - " << it.first << " / " << it.second << std::endl;
	}
	std::cout << std::endl;
	
	map.insert({{"plic", "ploc"}, {"tarte", "enpion"}});
	for(auto it : map)
	{
		std::cout << " - " << it.first << " / " << it.second << std::endl;
	}
	std::cout << std::endl;
	

	std::cout << ">> " << IntegratedHello.name() << std::endl;

	
	test();

	di::registry& registry = di::registry::get();


	std::cout << "===== STATIC =====" << std::endl;
	dump();
	
	std::cout << "===== LOADED =====" << std::endl;
	registry.load("module01");
	registry.load("module02");
	dump();

	std::cout << std::endl << std::endl;
	dumpRegistry();
	std::cout << std::endl << std::endl;

	for(auto it : registry.find_all_if<di::component>([](const di::component_descriptor& desc){std::cout << desc.name << std::endl; return desc.name == "hello";}))
	{
		std::cout << it.get() << std::endl;
	}
	
	
	return 0;
}

