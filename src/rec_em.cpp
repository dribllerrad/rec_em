/*
 *	Copyright (c) 2019 Darrell Bird.  All Rights Reserved.
 *
 *  No part of this file may be reproduced or transmitted in any form by
 *  any means or for any purpose without expressed written permission.
 *
 *
 *
 *
 *  rec_em:
 *  - Creates a c++ header from an input file as an unsigned char
 *    array which can be used as an embedded resource in another binary.
 *    The unsigned char array will be wrapped in a namespace.  Also in the
 *    namespace will be the name of the input file used to generate it and
 *    the length of the array data as an unsigned int.
 *
 *  - Can recursively convert all the input files of a given folder into a
 * 	  single header.
 *
 *  Author:
 *  - Darrell Bird (bird.darrell@gmail.com)
 */

#include "rec_em_version.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>
#include <filesystem>
#include <algorithm>
//#include "rec_em_version.h"eee

struct header_parameters
{

	std::vector<std::string> input_files;

	std::filesystem::path output_directory;

	std::string header_filename;

	std::string namespace_name;
};

const std::string defaults = "rec_em";

void print_version()
{
	std::cout << "\n"
			  << rec_em::version.project_name << "\n\n"
			  << "Version: " << rec_em::version.major_version << "."
			  << rec_em::version.minor_version << "."
			  << rec_em::version.patch_version << "\n"
			  << "Build: " << rec_em::version.build_number << "\n"
			  << "git commit: [" << rec_em::version.git_commit << "]\n\n";

}

void print_help()
{
	print_version();

	std::cout << R"delimiter(
        Creates a c++ header from an input file as an unsigned char
     	array which can be used as an embedded resource in another binary.
     	The unsigned char array will be wrapped in a namespace.  Also in the
     	namespace will be the name of the input file used to generate it and
     	the length of the array data as an unsigned int.

        Usage:
            rec_em INPUT [OPTIONS]

        INPUT:  Can be a single file or a directory containing multiple files.
                If a directory is given all files within the directory will
                be added to the generated header.


        OPTIONS:
            -o, --option
			    NAME : Output filename for generated header. The default is 'rec_em'.
            -d, --directory
			    PATH : Path to write the generated header. Path will be recursively
					   created if it doesn't exist.
            -n, --namespace
			    NAME : Namespace to be used in the generated header.
            -h, --help
			         : Prints this help message.
            -v, --version
			         : Print version.

        Example 1:
            rec_em File1.txt -o file1_txt -d embedded_include -n embedded_file
                - This would create the header file 'file1_txt.hpp' in the
                  directory 'embedded_include' with the unsigned char array
                  inside the namespace 'embedded_file{}'.

        Example 2:
            rec_em ../input_directory -o all_txt_files -d emb_include -n embedded_file
                - Assuming the directory ../input_directory contains three
                  files (a.txt, b.txt, c.txt), this would create a single file
                  all_txt_files.hpp in the emb_include directory in the embedded_file{}
                  namespace. Each file's char array will be uniquely named based
                  on the input filenames.

    )delimiter"
			  << std::endl;
}

void set_argument_values(const std::string &argument_name, const std::string &argument_value, header_parameters &parameters)
{
	assert(argument_name.front() == '-');
	assert(!argument_value.empty());

	if (argument_name == "-d" || argument_name == "--directory")
	{
		parameters.output_directory = argument_value;

		if (!parameters.output_directory.empty())
		{
			if (!std::filesystem::exists(parameters.output_directory))
			{
				std::filesystem::create_directories(parameters.output_directory);

				if (!std::filesystem::exists(parameters.output_directory))
				{
					throw std::runtime_error{"Error creating directories."};
				}
			}
		}
	}
	else if (argument_name == "-o" || argument_name == "--output")
	{
		parameters.header_filename = argument_value + ".hpp";
	}
	else if (argument_name == "-n" || argument_name == "--namespace")
	{
		parameters.namespace_name = argument_value;
	}
	else
	{
		throw std::runtime_error{"Invalid option: " + argument_name};
	}
}

void set_input(const std::string &value, header_parameters &parameters)
{
	if (std::filesystem::is_directory(value))
	{

		for (auto path : std::filesystem::recursive_directory_iterator{value})
		{
			if (std::filesystem::is_regular_file(path))
			{
				parameters.input_files.push_back(path.path().generic_string());
			}
		}
	}
	else if (std::filesystem::is_regular_file(value))
	{
		parameters.input_files.push_back(value);
	}
	else
	{
		throw std::runtime_error{"Cannot find file or directory: " + value};
	}
}

std::string remove_hyphen(std::string name)
{

	std::replace(name.begin(), name.end(), '-', '_');

	return name;
}

header_parameters get_arguments(int argc, char **argv)
{
	header_parameters parameters;

	if (argc == 1)
	{
		print_help();
		std::exit(0);
	}

	for (int i = 1; i < argc; ++i)
	{
		const std::string arg{argv[i]};

		if (arg.front() == '-')
		{
			if (arg == "-h" | arg == "--help")
			{
				print_help();
				std::exit(0);
			}
			else if (arg == "-v" | arg == "--version")
			{
				print_version();
				std::exit(0);
			}
			else if (i == argc - 1)
			{
				throw std::runtime_error{"Missing value for option " + arg};
			}
			else
			{
				set_argument_values(arg, argv[i + 1], parameters);
				i += 1;
			}
		}
		else
		{
			set_input(arg, parameters);
		}
	}

	if (parameters.header_filename.empty())
	{
		parameters.header_filename = defaults + ".hpp";
	}

	return parameters;
}

void binary_to_char_array(const std::string &input_file_name, const std::string &file_name_variable, std::ostream &stream)
{

	assert(std::filesystem::is_regular_file(input_file_name));

	std::filesystem::path input_file_path(input_file_name);

	std::ifstream input_file_stream{input_file_name, std::ios_base::in | std::ios_base::binary};
	if (!input_file_stream)
	{
		throw std::runtime_error{std::string("Error opening file") + input_file_name};
	}

	std::ios::fmtflags flags(stream.flags());

	const auto binary_length = static_cast<unsigned int>(std::filesystem::file_size(input_file_name));

	std::string file_extension;

	if (input_file_path.has_extension())
	{
		file_extension = input_file_path.extension();

		file_extension.erase(std::remove(file_extension.begin(), file_extension.end(), '.'));
	}
	else
	{
		file_extension = "no_ext";
	}
	stream << "\tconst char * " << remove_hyphen(input_file_path.stem().generic_string()) << "_" << file_extension << "_filename = \"" << input_file_path.filename().generic_string() << "\";\n";
	stream << "\tconst unsigned int " << remove_hyphen(input_file_path.stem().generic_string()) << "_" << file_extension << "_data_size = " << binary_length << ";\n";
	stream << "\tunsigned char " << remove_hyphen(input_file_path.stem().generic_string()) << "_data[" << remove_hyphen(input_file_path.stem().generic_string()) << "_" << file_extension << "_data_size] = {";

	size_t char_count{0};

	char c;

	while (input_file_stream.get(c))
	{
		if (char_count % 20 == 0)
		{
			stream << "\n\t\t";
		}

		char_count += 1;

		stream << "0x" << std::hex << (static_cast<int>(c) & 0xFF) << ",";
	}
	assert(char_count == binary_length);

	stream << "\n\t};\n";

	stream.flags(flags);
}

void create_header(const header_parameters &parameters)
{
	std::filesystem::path input_file_name;

	if (parameters.output_directory.empty())
	{
		input_file_name = static_cast<std::filesystem::path>(parameters.header_filename);
	}
	else
	{
		input_file_name = static_cast<std::filesystem::path>(parameters.output_directory / parameters.header_filename);
	}

	std::cout << "Generating " << input_file_name.generic_string() << "\n";
	std::ofstream stream{input_file_name};
	if (stream)
	{
		stream << "#pragma once\n";
		stream << "\n";

		if (!parameters.namespace_name.empty())
		{
			stream << "\n";
			stream << "namespace " << remove_hyphen(parameters.namespace_name) << " {";
		}

		stream << "\n";

		std::vector<std::string> file_names;

		int i = 0;

		for (auto path : parameters.input_files)
		{

			const std::string file_name_variable = "file" + std::to_string(file_names.size());

			std::cout << "  " << path << "\n";
			binary_to_char_array(path, file_name_variable, stream);
			file_names.emplace_back(file_name_variable);
			++i;
		}

		if (!parameters.namespace_name.empty())
		{
			stream << "}\n";
		}
		stream << "\n";
	}
	else
	{
		throw std::runtime_error{"Error creating header file."};
	}
}

int main(int argc, char **argv)
{
	try
	{
		const auto parameters = get_arguments(argc, argv);
		if (parameters.input_files.empty())
		{
			std::cerr << "Error: no input file to was given.\n";
		}
		else
		{
			std::cout << "Ready to process " << parameters.input_files.size() << " file(s).\n";
		}

		create_header(parameters);
	}
	catch (const std::exception &err)
	{
		std::cerr << "Error: " << err.what() << std::endl;
		return 1;
	}

	return 0;
}
