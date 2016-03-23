/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.cpp
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


void dumpRegistry(di::registry& reg)
{
	std::cout << "Registry: " << reg.size()  << std::endl;

	{
		reg.foreach([](const di::component_descriptor& comp)
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
		});
	}
}



int main()
{
	std::cout << "Hello world!" << std::endl;


	test();

	std::cout << "===== STATIC =====" << std::endl;
	dumpRegistry(di::registry::get());
	std::cout << std::endl << std::endl;


	std::cout << "===== LOADED =====" << std::endl;
	di::registry reg(&di::registry::get());

	di::simple_component_loader loader(reg);
	loader.load("module01");
	loader.load("module02");

	di::simple_component_loader::filter_t filter([](const std::string& filename)->bool{return filename.find("lib")!=std::string::npos;});
	loader.load_all(".", filter);
	dumpRegistry(reg);
	std::cout << std::endl << std::endl;


	for(auto it : reg.find_all_if<di::component>([](const di::component_descriptor& desc){std::cout << desc.name << std::endl; return desc.name == "hello";}))
	{
		std::cout << it.get() << std::endl;
	}

	return 0;
}

