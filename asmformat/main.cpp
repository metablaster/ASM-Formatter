
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
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
#include "StringCast.hpp"
using namespace wsl;
namespace fs = std::filesystem;


int main(int argc, char* argv[])
{
	if (!RegisterConsoleHandler())
	{
		return ExitCode(ErrorCode::FunctionFailed);
	}

	#ifdef _DEBUG
	const auto old_CP = GetConsoleCodePage();
	#endif

	fs::path executable_path = argv[0];
	constexpr const char* syntax = " path\\file1.asm path\\file2.asm ... [--encoding ansi|utf8|utf16le] [--tabwidth N] [--spaces] [--linebreaks crlf|lf] [--compact]";

	if (argc < 2)
	{
		std::cerr << "Usage: " << executable_path.filename().string() << syntax << std::endl;
		return ExitCode(ErrorCode::InvalidCommand);
	}

	// Show help if --help was specified
	std::vector<std::string> all_params(argv + 1, argv + argc);

	if (std::find(all_params.begin(), all_params.end(), "--help") != all_params.end())
	{
		std::cout << std::endl << executable_path.filename().string() << syntax << std::endl << std::endl;

		std::cout << " --encoding\tspecifies encoding of source files if no BOM is present (defalt: utf8)" << std::endl;
		std::cout << " --tabwidth\tspecifies tab width used in source files (defalt: 4)" << std::endl;
		std::cout << " --spaces\tuse spaces instead of tabs (by defalt tabs are used)" << std::endl;
		std::cout << " --linebreaks\tperform line breaks conversion (by defalt line breaks are preserved)" << std::endl;
		std::cout << " --compact\treplaces all surplus blank lines with single blank line" << std::endl;

		return 0;
	}

	// Default values
	bool spaces = false;
	// TODO: There could multiple verbosities of compact
	bool compact = false;
	unsigned tabwidth = 4;
	Encoding encoding = Encoding::UTF8;
	LineBreak linebreaks = LineBreak::Preserve;

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
			else if (param == "--compact")
			{
				compact = true;
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

				tabwidth = width;
			}
			else if (param == "--linebreaks")
			{
				if (arg == "crlf")
				{
					linebreaks = LineBreak::CRLF;
				}
				else if (arg == "lf")
				{
					linebreaks = LineBreak::LF;
				}
				else if (arg == "cr")
				{
					ShowError(ErrorCode::NotImplemented, "CR linebreak is not implemented");
					return ExitCode(ErrorCode::NotImplemented);
				}
				else
				{
					ShowError(ErrorCode::InvalidParameter, "The specified linebreak '" + arg + "' was not recognized");
					return ExitCode(ErrorCode::InvalidParameter);
				}
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
			if (fs::is_directory(file_path))
			{
				ShowError(Exception(ErrorCode::InvalidCommand, param + " is directory and was ignored"), ERROR_INFO, MB_ICONINFORMATION);
				continue;
			}

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
					ShowError(ErrorCode::InvalidCommand, "File '" + file_path.filename().string() + "' was not found");
					return ExitCode(ErrorCode::InvalidCommand);
				}
			}
		}
	}

	#ifdef _DEBUG
	// MSDN: It is recommended for all new and updated command-line applications to avoid code pages and use Unicode.
	// UTF-16 formatted text can be sent to the W family of console APIs.
	// UTF-8 formatted text can be sent to the A family of console APIs after ensuring the code page is first set to 65001 (CP_UTF8)
	// with the SetConsoleCP and SetConsoleOutputCP functions.
	if (!SetConsoleCodePage(old_CP.first, CP_UTF8))
	{
		return ExitCode(ErrorCode::FunctionFailed);
	}
	#endif // _DEBUG

	for (const auto& file_path : files)
	{
		std::cout << "Formatting file " << file_path.filename() << std::endl;

		#ifdef _DEBUG
		std::vector<unsigned char> bom_string;
		const BOM bom = GetBOM(file_path, bom_string);
		static_cast<void>(bom);
		#endif

		switch (encoding)
		{
		case Encoding::ANSI:
		{
			std::stringstream filedata(LoadFileA(file_path.string()));

			FormatFileA(filedata, tabwidth, spaces, compact, linebreaks);
			WriteFileBytes(file_path, filedata.str(), false);
			break;
		}
		case Encoding::UTF8:
		{
			std::string filebytes = LoadFileBytes(file_path);
			std::wstringstream filedata(StringCast(filebytes));

			FormatFileW(filedata, tabwidth, spaces, compact, linebreaks);

			filebytes = StringCast(filedata.str());
			WriteFileBytes(file_path, filebytes, false);
			break;
		}
		case Encoding::UTF16LE:
		{
			std::wstringstream filedata(LoadFileW(file_path, encoding));
			FormatFileW(filedata, tabwidth, spaces, compact, linebreaks);

			#if TRUE
			// TODO: Converts from LF to CRLF
			WriteFileW(file_path, filedata.str(), encoding);
			#else
			std::string coverted = StringCast(filedata.str());
			WriteFileBytes(file_path, coverted, false);
			#endif
			break;
		}
		default:
			break;
		}
	}

	#ifdef _DEBUG
	if (!SetConsoleCodePage(old_CP.first, old_CP.second))
	{
		return ExitCode(ErrorCode::FunctionFailed);
	}
	#endif // _DEBUG

	return 0;
}
