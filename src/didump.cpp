/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * didump.cpp
 *
 * Copyright (C) 2016 Emilien Kia <emilien.kia@gmail.com>
 *
 * didump is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * didump is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */
#include "../config.h"

#include "di.hpp"

#include <iostream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#define DIDUMP_NAME		"didump"

void introspect(const std::string& filename)
{
	di::registry reg;
	di::simple_component_loader loader(reg);
	loader.load(filename);

	std::cout << filename << std::endl << std::endl;
	reg.foreach([](const di::component_descriptor& desc){
			std::cout << '\t' << desc.name << std::endl;
			for(const auto& prop : desc.prop)
			{
				std::cout << '\t' << '\t' << prop.first << " = " << prop.second << std::endl;
			}
			std::cout << std::endl;
		});
}

void introspect_path(const fs::path& path, bool recursive)
{
	if(fs::is_regular_file(path))
	{
		introspect(path.string());
	}
	else if(fs::is_directory(path))
	{
		std::vector<fs::path> paths;
		if(recursive)
		{
			for(fs::recursive_directory_iterator dir(path); dir!=fs::recursive_directory_iterator(); dir++)
			{
				paths.push_back(fs::directory_entry(*dir).path());
			}
		}
		else
		{
			for(fs::directory_iterator dir(path); dir!=fs::directory_iterator(); dir++)
			{
				paths.push_back(fs::directory_entry(*dir).path());
			}
		}
		for(fs::path& p : paths)
		{
			if(fs::is_regular_file(p))
			{
				introspect(p.string());
			}
		}
	}
}

int main(int argc, const char** argv)
{
	std::vector<std::string> files;
	std::string directory;

	po::options_description inputs("Input files or directory");
	inputs.add_options()
		("input-file,i", po::value< std::vector<std::string> >(&files), "input file")
		("directory,d",  po::value< std::string >(&directory),          "directory to introspect")
		("recursive,r",                                                 "introspect subdirectories recursively")
	;
	po::positional_options_description p;
	p.add("input-file", -1);

	po::options_description others("Other options");
	others.add_options()
		("help,h",    "display this help and exit")
		("version,v", "output version information and exit")
	;

	po::options_description cmdline_options;
	cmdline_options.add(inputs).add(others);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		po::options_description help_options;
		help_options.add(inputs).add(others);
		std::cout
			<< "Usage: " << DIDUMP_NAME << " [options]... [file]..." << std::endl
			<< "   or: " << DIDUMP_NAME << " [options]... -i <file>" << std::endl
			<< "   or: " << DIDUMP_NAME << " [options]... -d <directory>" << std::endl
			<< "List libdi declared dependencies in specified modules." << std::endl
			<< help_options << std::endl;
		return 1;
	}

	if (vm.count("version")) {
		std::cout
			<< DIDUMP_NAME << " " << VERSION << std::endl
			<< "Copyright (C) 2016 Emilien Kia <emilien.kia@gmail.com>" << std::endl
			<< "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>." << std::endl
			<< "This is free software: you are free to change and redistribute it." << std::endl
			<< "There is NO WARRANTY, to the extent permitted by law." << std::endl
			;
		return 1;
	}

	// Directory traversal
	if(!directory.empty())
	{
		bool recursive = vm.count("recursive")>0;
		introspect_path(fs::path(directory), recursive);

	}
	else // File enumeration
	{
		for(std::string file : files)
		{
			introspect(file);
		}
	}

	return 0;
}
