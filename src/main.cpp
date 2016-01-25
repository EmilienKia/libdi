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

#include "di.hpp"

#include "service01.hpp"

//
// IntegratedHelloServiceImpl
// 

class IntegratedHelloServiceImpl : public HelloService
{
public:
	IntegratedHelloServiceImpl() = default;
	virtual ~IntegratedHelloServiceImpl() = default;

	virtual void sayHello(const std::string& name)const;
	virtual size_t count();
private:
	size_t _count;
};

void IntegratedHelloServiceImpl::sayHello(const std::string& name)const
{
	std::cout << "(integrated) Hello " << name << " !" << std::endl;
}

size_t IntegratedHelloServiceImpl::count()
{
	return _count++;
}

di::component_instance<IntegratedHelloServiceImpl> IntegratedHello;


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

di::component_instance<TotoServiceImpl> TotoServiceImplInstance;


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


int main()
{
	std::cout << "Hello world!" << std::endl;

	std::cout << ">> " << IntegratedHello.name() << std::endl;

	
	test();

	di::registry& registry = di::registry::get();


	std::cout << "===== STATIC =====" << std::endl;
	dump();

	
	std::cout << "===== LOADED =====" << std::endl;
	registry.load("module01");
	registry.load("module02");
	dump();

	return 0;
}

