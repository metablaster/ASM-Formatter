
/*
 *	Project: "masm-formatter" https://github.com/metablaster/masm-formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
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
	std::wregex reg;
	std::wstring line;
	std::wstring result;

	// Count of characters of the longest code line that contains inline comment
	// inline comments will be shifted according to longest code line
	std::size_t maxcodelen = 0;
	
	while (std::getline(filedata, line).good())
	{
		if (!line.empty())
		{
			// Shift line to start by removing all starting spaces and tabs
			reg = L"^\\s+(.*)";
			line = std::regex_replace(line, reg, L"$1");

			// Calculate longest code line with inline comment, excluding indentation
			if (!line.starts_with(L";"))
			{
				reg = L"^(.*?)(?=\\s*;)";
				std::wsmatch match;

				if (std::regex_search(line, match, reg))
				{
					std::wssub_match code = match[1];
					const std::size_t codelen = code.str().length();

					if (codelen > maxcodelen)
					{
						maxcodelen = codelen;
					}
				}
			}
		}

		// TODO: Is \r preserved with getline?
		result += line.append(L"\n");
	}

	filedata.str(result);
	filedata.clear();
	result.clear();

	// Count of characters missing to make a full tab of the max length code line
	const std::size_t maxmissing = 4 - maxcodelen % 4;

	while (std::getline(filedata, line).good())
	{
		if (!line.empty() && !line.starts_with(L";"))
		{
			// Insert tab to beginning of each line
			line.insert(0, L"\t");

			// Is code line indented with tab?
			bool isindented = true;

			// Do not indent procedure labels
			reg = L"\t\\w+\\s+(proc|endp)";
			if (std::regex_search(line, reg))
			{
				isindented = false;
			}

			// Format inline comments to start on same column
			// On which column depends on the longest code line containing inline comment
			reg = L"^(\t)(.*?)(?=\\s*;)(\\s*)(;.*)";
			std::wsmatch match;

			if (std::regex_search(line, match, reg))
			{
				// Character length of current code line, excluding indentation
				const std::size_t codelen = match[2].str().length();

				std::wstring code = std::regex_replace(line, reg, L"$1$2");
				std::wstring comment = std::regex_replace(line, reg, L"$4");

				// Character length difference of current code line compared to max length code line
				std::size_t diff = maxcodelen - codelen;
				// Also include characters that will be added to max length code line
				diff += maxmissing;

				std::size_t tabcount = diff / tab_width;

				// Tab count must be multiple of tab width
				if (diff % tab_width != 0)
				{
					++tabcount;
				}

				if (!isindented)
				{
					// This accounts for removed tab at the start of line (later)
					++tabcount;
				}

				code.append(tabcount, L'\t');
				line = code.append(comment);
			}

			if (!isindented)
			{
				// Shift back to start by removing starting tab
				line.erase(0, 1);
			}
		}

		result += line.append(L"\n");
	}

	filedata.str(result);
	filedata.clear();

	#ifdef _DEBUG
	std::cout << StringCast(filedata.str()) << std::endl;
	std::cout << "longest code line is: " << maxcodelen << std::endl;
	#endif
}

void FormatFileA(std::stringstream& filedata)
{
	std::regex reg;
	std::string line;
	std::string result;

	// Count of characters of the longest code line that contains inline comment
	// inline comments will be shifted according to longest code line
	std::size_t maxcodelen = 0;

	while (std::getline(filedata, line).good())
	{
		if (!line.empty())
		{
			// Shift line to start by removing all starting spaces and tabs
			reg = "^\\s+(.*)";
			line = std::regex_replace(line, reg, "$1");

			// Calculate longest code line with inline comment, excluding indentation
			if (!line.starts_with(";"))
			{
				reg = "^(.*?)(?=\\s*;)";
				std::smatch match;

				if (std::regex_search(line, match, reg))
				{
					std::ssub_match code = match[1];
					const std::size_t codelen = code.str().length();

					if (codelen > maxcodelen)
					{
						maxcodelen = codelen;
					}
				}
			}
		}

		// TODO: Is \r preserved with getline?
		result += line.append("\n");
	}

	filedata.str(result);
	filedata.clear();
	result.clear();

	// Count of characters missing to make a full tab of the max length code line
	const std::size_t maxmissing = 4 - maxcodelen % 4;

	while (std::getline(filedata, line).good())
	{
		if (!line.empty() && !line.starts_with(";"))
		{
			// Insert tab to beginning of each line
			line.insert(0, "\t");

			// Is code line indented with tab?
			bool isindented = true;

			// Do not indent procedure labels
			reg = "\t\\w+\\s+(proc|endp)";
			if (std::regex_search(line, reg))
			{
				isindented = false;
			}

			// Format inline comments to start on same column
			// On which column depends on the longest code line containing inline comment
			reg = "^(\t)(.*?)(?=\\s*;)(\\s*)(;.*)";
			std::smatch match;

			if (std::regex_search(line, match, reg))
			{
				// Character length of current code line, excluding indentation
				const std::size_t codelen = match[2].str().length();

				std::string code = std::regex_replace(line, reg, "$1$2");
				std::string comment = std::regex_replace(line, reg, "$4");

				// Character length difference of current code line compared to max length code line
				std::size_t diff = maxcodelen - codelen;
				// Also include characters that will be added to max length code line
				diff += maxmissing;

				std::size_t tabcount = diff / tab_width;

				// Tab count must be multiple of tab width
				if (diff % tab_width != 0)
				{
					++tabcount;
				}

				if (!isindented)
				{
					// This accounts for removed tab at the start of line (later)
					++tabcount;
				}

				code.append(tabcount, '\t');
				line = code.append(comment);
			}

			if (!isindented)
			{
				// Shift back to start by removing starting tab
				line.erase(0, 1);
			}
		}

		result += line.append("\n");
	}

	filedata.str(result);
	filedata.clear();

	#ifdef _DEBUG
	std::cout << filedata.str() << std::endl;
	std::cout << "longest code line is: " << maxcodelen << std::endl;
	#endif
}
