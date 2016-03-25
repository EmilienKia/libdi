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

#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#define DIDUMP_NAME		"didump"

int main(int argc, const char** argv)
{
	std::vector<std::string> filenames;
	std::string directory;

	po::options_description inputs("Input files or directory");
	inputs.add_options()
		("input,i", po::value< std::vector<std::string> >(&filenames), "input file or directory")
		("recursive,R",                                                "introspect subdirectories recursively")
	;
	po::positional_options_description p;
	p.add("input", -1);

	po::options_description reports("Report generation");
	reports.add_options()
		("def,d", "Generate di definition files (.didef)")
		("rep,r", "Generate di repository definition file (.direp)")
	;

	po::options_description others("Other options");
	others.add_options()
		("help,h",    "display this help and exit")
		("version,v", "output version information and exit")
	;

	po::options_description cmdline_options;
	cmdline_options.add(inputs).add(reports).add(others);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout
			<< "Usage: " << DIDUMP_NAME << " [options]... [file-or-directory]..." << std::endl
			<< "List libdi declared dependencies in specified modules." << std::endl
			<< cmdline_options << std::endl;
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

	// Flag generate definition files (.didef)
	bool generate_didef = false;
	if(vm.count("def")>0)
	{
		generate_didef = true;
	}

	// Flag generate repository definition file
	bool generate_direp = false;
	if(vm.count("rep")>0)
	{
		generate_direp = true;
	}
	std::ofstream repository_file;
	if(generate_direp)
	{
		repository_file.open("./.direp", std::ios_base::trunc);
	}

	// If no specified file, assume process current directory.
	if(filenames.size()==0)
	{
		filenames.push_back(".");
	}

	// List of paths to introspect
	std::vector<fs::path> paths;
	bool recursive = vm.count("recursive")>0;
	for(std::string filename : filenames)
	{
		fs::path path(filename);
		if(fs::is_regular_file(path))
		{
			paths.push_back(path);
		}
		else if(fs::is_directory(path))
		{
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
		}
	}

	// Introspect all files
	for(const fs::path& path : paths)
	{
		std::string filename = path.string();
		di::registry reg;
		di::simple_component_loader loader(reg);
		if(loader.load(filename) && reg.size()>0)
		{
			std::cout << filename << ':' << std::endl;
			reg.foreach([](const di::component_descriptor& desc){
					std::cout << desc.name << std::endl;
					for(const auto& prop : desc.prop)
					{
						std::cout << '\t' << prop.first << "=" << prop.second << std::endl;
					}
					std::cout << std::endl;
				});

			if(generate_didef)
			{
				std::ofstream file(filename+".didef", std::ios_base::trunc);
				file << "(" << filename << ")" << std::endl;
				reg.foreach([&file](const di::component_descriptor& desc){
						file << '[' << desc.name << ']'<< std::endl;
						for(const auto& prop : desc.prop)
						{
							file << prop.first << "=" << prop.second << std::endl;
						}
						file << std::endl;
					});
			}
			if(generate_direp)
			{
				repository_file << "(" << filename << ")" << std::endl;
				reg.foreach([&repository_file](const di::component_descriptor& desc){
						repository_file << '[' << desc.name << ']'<< std::endl;
						for(const auto& prop : desc.prop)
						{
							repository_file << prop.first << "=" << prop.second << std::endl;
						}
						repository_file << std::endl;
					});
			}
		}
	}

	return 0;
}
