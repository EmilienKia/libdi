// library01.cpp
//
// Copyright (C) 2016 - Emilien Kia
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "service01.hpp"

#include "di.hpp"

#include <iostream>

//
// HelloServiceImpl
//

class HelloServiceImpl : public HelloService
{
public:
	HelloServiceImpl() = default;
	virtual ~HelloServiceImpl() = default;

	virtual void sayHello(const std::string& name)const;
	virtual size_t count();
};

void HelloServiceImpl::sayHello(const std::string& name)const
{
	std::cout << "Hello " << name << " !" << std::endl;
}

size_t HelloServiceImpl::count()
{
	return 42;
}

di::component_instance<HelloServiceImpl> hello;



//
// TotoServiceImpl
// 
class TotoServiceLib1Impl : public TotoService
{
public:
	TotoServiceLib1Impl() = default;
	virtual void titi()
	{
		std::cout << "titi(library01) " << std::endl;
	}
};

di::component_instance<TotoServiceLib1Impl> Lib01TotoServiceImplInstance;

//
//
//

int test() {
	return 42;
}

