
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
 * TODO: Implement --codepage option
 * TODO: Implement --path and directory option
 * TODO: Implement --recurse option
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
	// TODO: Figure out which code page to set depending on contents of a source file
	// It's needed when there is an error parsing characters which may result in printing them
	const auto old_CP = GetConsoleCodePage();
	#endif

	fs::path executable_path = argv[0];
	const std::string executable_name = executable_path.stem().string();
	std::vector<std::string> all_params(argv + 1, argv + argc);

	constexpr const char* version = "0.4.0";
	const bool nologo = std::find(all_params.begin(), all_params.end(), "--nologo") != all_params.end();
	constexpr const char* syntax = " path\\file1.asm path\\file2.asm ... [--encoding ansi|utf8|utf16le] [--tabwidth N] [--spaces] [--linebreaks crlf|lf] [--compact] [--help] [--nologo]";

	if (!nologo)
	{
		std::cout << std::endl << "ASM Formatter " << version << " https://github.com/metablaster/ASM-Formatter" << std::endl;
		std::cout << "Copyright (C) 2023 metablaster (zebal@protonmail.ch)" << std::endl;
	}

	if (argc < (nologo ? 3 : 2))
	{
		std::cerr << std::endl << "Usage: " << std::endl << std::endl;
		std::cerr << executable_name << syntax << std::endl;
		return ExitCode(ErrorCode::InvalidCommand);
	}

	// Show help if --help was specified
	if (std::find(all_params.begin(), all_params.end(), "--help") != all_params.end())
	{
		std::cout << std::endl << "Syntax:" << std::endl;
		std::cout << std::endl << executable_name << syntax << std::endl << std::endl;

		std::cout << " --encoding\tSpecifies encoding used to read and write files (default: ansi)" << std::endl;
		std::cout << " --tabwidth\tSpecifies tab width used in source files (default: 4)" << std::endl;
		std::cout << " --spaces\tUse spaces instead of tabs (by default tabs are used)" << std::endl;
		std::cout << " --linebreaks\tPerform line breaks conversion (by default line breaks are preserved)" << std::endl;
		std::cout << " --compact\tReplaces all surplus blank lines with single blank line" << std::endl;
		std::cout << " --nologo\tSuppresses the display of the program banner, version and Copyright when the " << executable_name << " starts up" << std::endl;
		std::cout << " --help\t\tDisplays this help" << std::endl;

		std::cout << std::endl << "Notes:" << std::endl << std::endl;
		std::cout << "--linebreaks option doesn't have any effect on UTF-16 encoded files, UTF-16 files are always formatted with CRLF" << std::endl;
		std::cout << "By default surplus blank lines are removed at the topand at the end of a file," << std::endl;
		std::cout << "as well as surplus blank lines around procedure labels to make them compacted to code." << std::endl;
		std::cout << "If you whish to replace all surplus blank lines entirely with a single blank line specify --compact option." << std::endl;
		return 0;
	}

	// Default values
	bool spaces = false;
	// TODO: There could multiple verbosities of compact
	bool compact = false;
	std::size_t tabwidth = 4;
	Encoding encoding = Encoding::ANSI;
	LineBreak linebreaks = LineBreak::Preserve;

	std::vector<fs::path> files;
	std::cout << std::endl;

	for (int i = 1; i < argc; ++i)
	{
		std::string param = argv[i];

		if (param.starts_with("--"))
		{
			if (param == "--nologo")
			{
				continue;
			}
			else if (param == "--spaces")
			{
				spaces = true;
				std::cout << "using --spaces option" << std::endl;
				continue;
			}
			else if (param == "--compact")
			{
				compact = true;
				std::cout << "using --compact option" << std::endl;
				continue;
			}

			// Make sure we aren't at the end of argv
			if ((i + 1) == argc)
			{
				ShowError(ErrorCode::InvalidParameter, param + " option requires one argument");
				return ExitCode(ErrorCode::InvalidParameter);
			}

			std::string arg = argv[++i];

			if (param == "--encoding")
			{
				if (arg == "utf8")
				{
					encoding = Encoding::UTF8;
				}
				else if (arg == "utf16le")
				{
					encoding = Encoding::UTF16LE;
				}
				else if (arg != "ansi")
				{
					ShowError(ErrorCode::InvalidParameter, "The specified encoding '" + arg + "' was not recognized");
					return ExitCode(ErrorCode::InvalidParameter);
				}
			}
			else if (param == "--tabwidth")
			{
				const std::size_t width = static_cast<std::size_t>(std::stoi(arg));

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

				std::cout << "forcing " << arg << " line breaks" << std::endl;
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

	std::cout << "using tab width of " << tabwidth << std::endl;
	std::cout << "using "<< EncodingToString(encoding) << " encoding" << std::endl << std::endl;

	std::vector<unsigned char> bom_bytes;

	for (const auto& file_path : files)
	{
		std::cout << "Formatting file " << file_path.filename() << std::endl;

		const BOM bom = GetBOM(file_path, bom_bytes);

		switch (encoding)
		{
		case Encoding::ANSI:
		{
			// TODO: Since we may detect encoding we should use that rather than reporting an error
			if (bom != BOM::none)
				goto invalid_encoding;

			std::stringstream filedata(LoadFileBytes(file_path.string()));

			FormatFileA(filedata, tabwidth, spaces, compact, linebreaks);
			WriteFileBytes(file_path, filedata.str(), false);
			break;
		}
		case Encoding::UTF8:
		{
			if ((bom != BOM::utf8) && (bom != BOM::none))
				goto invalid_encoding;

			std::string filebytes = LoadFileBytes(file_path);
			std::wstringstream filedata(StringCast(filebytes));

			FormatFileW(filedata, tabwidth, spaces, compact, linebreaks);

			if (bom == BOM::utf8)
				WriteFileBytes(file_path, bom_bytes, false);

			filebytes = StringCast(filedata.str());
			WriteFileBytes(file_path, filebytes, true);
			break;
		}
		case Encoding::UTF16LE:
		{
			if (bom != BOM::utf16le)
				goto invalid_encoding;

			std::wstringstream filedata(LoadFileW(file_path, encoding));
			FormatFileW(filedata, tabwidth, spaces, compact, linebreaks);

			#if TRUE
			// TODO: Converts from LF to CRLF
			WriteFileW(file_path, filedata.str(), encoding);
			#else
			// TODO: Not working and unclear if LoadFileW loads BOM
			if (bom == BOM::utf16le)
				WriteFileBytes(file_path, bom_bytes, false);

			std::string converted = StringCast(filedata.str());
			WriteFileBytes(file_path, converted, true);
			#endif
			break;
		}
		default:
			break;
		}

		continue;

	invalid_encoding:
		ShowError(ErrorCode::InvalidParameter, EncodingToString(encoding) + " was specified but file " + file_path.filename().string() + " is encoded as " + BomToString(bom));
	}

	#ifdef _DEBUG
	if (!SetConsoleCodePage(old_CP.first, old_CP.second))
	{
		return ExitCode(ErrorCode::FunctionFailed);
	}
	#endif // _DEBUG

	return 0;
}
