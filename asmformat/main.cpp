
/*
 *	Project: "masm-formatter" https://github.com/metablaster/ASM-Formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file mlfmt\main.cpp
 *
 * Defines the entry point for application
 *
*/

#include "pch.hpp"
#include "console.hpp"
#include "FormatFile.hpp"
#include "SourceFile.hpp"
#include "console.hpp"
namespace fs = std::filesystem;


int main(int argc, char* argv[])
{
	if (!RegisterConsoleHandler())
	{
		return EXIT_FAILURE;
	}

	if (argc < 2)
	{
		std::string executable = fs::path(argv[0]).filename().string();
		std::cerr << "Usage: " << executable << " path\\filename.asm [-encoding ansi|utf8|utf16|utf16le]" << std::endl;
		return EXIT_FAILURE;
	}

	fs::path file_path;
	Encoding encoding = Encoding::ANSI;

	for (int i = 1; i < argc; ++i)
	{
		if (std::string(argv[i]) == "--encoding")
		{
			// Make sure we aren't at the end of argv!
			if (i + 1 < argc)
			{
				std::string param = argv[++i];

				if (param == "utf8")
				{
					encoding = Encoding::UTF8;
				}
				else if (param == "utf16")
				{
					encoding = Encoding::UTF16;
				}
				else if (param == "utf16le")
				{
					encoding = Encoding::UTF16LE;
				}
				else if (param != "ansi")
				{
					std::cerr << "The specified encoding '" << param << "' is not recognized" << std::endl;
					return EXIT_FAILURE;
				}
			}
			else
			{
				std::wcerr << L"--encoding option requires one argument" << std::endl;
				return EXIT_FAILURE;
			}
		}
		else if (std::string(argv[i]).starts_with("--"))
		{
			std::cerr << "option " << argv[i] << " was not recognized" << std::endl;
			return EXIT_FAILURE;
		}
		else if (file_path.empty())
		{
			file_path = argv[i];

			if (!fs::exists(file_path))
			{
				std::cerr << "File " << argv[i] << " was not found" << std::endl;
				return EXIT_FAILURE;
			}
		}
		else
		{
			std::cerr << "Only one file path can be specified" << std::endl;
			return EXIT_FAILURE;
		}
	}

	if (encoding == Encoding::ANSI)
	{
		std::stringstream filedata = LoadFileA(file_path.string());
		FormatFileA(filedata);
		WriteFileA(file_path, filedata);
	}
	else
	{
		std::wstringstream filedata = LoadFileW(file_path, encoding);
		FormatFileW(filedata);
		WriteFileW(file_path, filedata, encoding);
	}

	return 0;
}
