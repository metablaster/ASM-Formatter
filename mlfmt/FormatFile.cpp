
/*
 *	Project: "masm-formatter" https://github.com/metablaster/masm-formatter
 *
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *
 *	Licensed under the MIT license
 *
*/

/**
 * @file mlfmt\FormatFile.cpp
 *
 * Source file function definitions
 *
*/

#include "pch.hpp"
#include "FormatFile.hpp"
#include "StringCast.hpp"


void FormatFileW(std::wstringstream& filedata)
{
	std::wregex reg{ };
	std::wstring line;
	std::wstring fileline{ };
	std::wstring result;

	while (std::getline(filedata, fileline).good())
	{
		// Remove all starting spaces and tabs
		reg = L"^(\\s|\t)+(.*)";
		line = std::regex_replace(fileline, reg, L"$2");

		// Insert tab to beginning of each line
		line.insert(0, L"\t");

		// Shift comments to start
		reg = L"^(\\s|\t)+(;.*)";
		line = std::regex_replace(line, reg, L"$2");

		// Shift procedures to start
		reg = L"\t\\w+\\s+(proc|endp)$";
		if (std::regex_match(line, reg))
		{
			line.erase(0, 1);
		}

		// NOTE: \r is preserved with getline
		line += L"\n";
		result += line;
	}

	filedata.str(result);
	std::cout << StringCast(filedata.str()) << std::endl;
}

void FormatFileA(std::stringstream& filedata)
{
	std::string line{};
	while (std::getline(filedata, line).good())
	{
		std::cout << line << std::endl;
	}
}
