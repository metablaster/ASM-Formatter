
/*
 *	Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file asmformat\main.cpp
 *
 * Defines the entry point for application
 * Debug command arguments: $(SolutionDir)assets\utf8-BOM.asm
 * Debug working directory: $(SolutionDir)Build\$(Platform)\$(Configuration)
 * Debugger type: Native Only
 *
*/

#include "pch.hpp"
#include "console.hpp"
#include "FormatFile.hpp"
#include "SourceFile.hpp"
#include "error.hpp"
#include "ErrorCode.hpp"
using namespace wsl;
namespace fs = std::filesystem;


int main(int argc, char* argv[])
{
	if (!RegisterConsoleHandler())
	{
		return ExitCode(ErrorCode::FunctionFailed);
	}

	fs::path executable_path = argv[0];

	if (argc < 2)
	{
		std::cerr << "Usage: " << executable_path.filename().string()
			<< " path\\file1.asm path\\file2.asm ... [--encoding ansi|utf8|utf16|utf16le] [--tabwidth N]"
			<< std::endl;

		return ExitCode(ErrorCode::InvalidCommand);
	}

	// Show help if --help was specified
	std::vector<std::string> all_params(argv + 1, argv + argc);

	if (std::find(all_params.begin(), all_params.end(), "--help") != all_params.end())
	{
		std::cout << std::endl << executable_path.filename().string()
			<< " path\\file1.asm path\\file2.asm ... [--encoding ansi|utf8|utf16|utf16le] [--tabwidth N] [--spaces]"
			<< std::endl << std::endl;

		std::cout << " --encoding\tspecifies encoding of source files (defalt: utf8)" << std::endl;
		std::cout << " --tabwidth\tspecifies tab width used in source files (defalt: 4)" << std::endl;
		std::cout << " --spaces\tuse spaces instead of tabs (by defalt tabs are used)" << std::endl << std::endl;

		return 0;
	}

	// Default values
	bool spaces = false;
	unsigned tab_width = 4;
	Encoding encoding = Encoding::UTF8;

	std::vector<fs::path> files;

	for (int i = 1; i < argc; ++i)
	{
		std::string param = argv[i];

		if (param.starts_with("--"))
		{
			if (param == "--spaces")
			{
				spaces = true;
				continue;
			}

			// Make sure we aren't at the end of argv
			if (i + 1 == argc)
			{
				ShowError(ErrorCode::InvalidParameter, param + " option requires one argument");
				return ExitCode(ErrorCode::InvalidParameter);
			}

			std::string arg = argv[++i];

			if (param == "--encoding")
			{
				if (arg == "ansi")
				{
					encoding = Encoding::ANSI;
				}
				else if (arg == "utf16")
				{
					encoding = Encoding::UTF16;
				}
				else if (arg == "utf16le")
				{
					encoding = Encoding::UTF16LE;
				}
				else if (arg != "utf8")
				{
					ShowError(ErrorCode::InvalidParameter, "The specified encoding '" + arg + "' was not recognized");
					return ExitCode(ErrorCode::InvalidParameter);
				}
			}
			else if (param == "--tabwidth")
			{
				const long width = std::stoi(arg);
				if (width < 1)
				{
					ShowError(ErrorCode::InvalidParameter, "Tab width must be a number grater than zero");
					return ExitCode(ErrorCode::InvalidParameter);
				}

				tab_width = width;
			}
			else
			{
				ShowError(ErrorCode::UnknownOption, "option '" + param + "' was not recognized");
				return ExitCode(ErrorCode::UnknownOption);
			}
		}
		else
		{
			fs::path file_path = param;

			if (fs::exists(file_path))
			{
				files.push_back(file_path);
			}
			else
			{
				// Check if file is in current working directory
				file_path = executable_path.parent_path().append(file_path.string());

				if (fs::exists(file_path))
				{
					files.push_back(file_path);
				}
				else
				{
					ShowError(ErrorCode::InvalidParameter, "File '" + file_path.filename().string() + "' was not found");
					return ExitCode(ErrorCode::InvalidParameter);
				}
			}
		}
	}

	for (const auto& file_path : files)
	{
		std::cout << "Formatting file " << file_path.filename() << std::endl;

		if (encoding == Encoding::ANSI)
		{
			std::stringstream filedata = LoadFileA(file_path.string());
			FormatFileA(filedata, tab_width, spaces);
			WriteFileA(file_path, filedata);
		}
		else
		{
			std::wstringstream filedata = LoadFileW(file_path, encoding);
			FormatFileW(filedata, tab_width, spaces);
			WriteFileW(file_path, filedata, encoding);
		}
	}

	return 0;
}
