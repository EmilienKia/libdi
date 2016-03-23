/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * module01.cpp
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

#include "service01.hpp"

//
// Module01HelloServiceImpl
//

class Module01HelloServiceImpl : public HelloService
{
public:
	Module01HelloServiceImpl() = default;
	virtual ~Module01HelloServiceImpl() = default;

	virtual void sayHello(const std::string& name)const;
	virtual size_t count();
private:
	size_t _count;
};

void Module01HelloServiceImpl::sayHello(const std::string& name)const
{
	std::cout << "(module01) Hello " << name << " !" << std::endl;
}

size_t Module01HelloServiceImpl::count()
{
	return _count++;
}

di::component_instance<Module01HelloServiceImpl> IntegratedHello("mod01-hello", {{"titi", "toto"}, {"tata", "tutu"}});


//
// Module01TotoServiceImpl
//
class Module01TotoServiceImpl : public TotoService
{
public:
	Module01TotoServiceImpl() = default;
	virtual void titi()
	{
		std::cout << "titi(module01) " << std::endl;
	}
};

di::component_instance<Module01TotoServiceImpl> TotoServiceImplInstance;
