
/*
 *	Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file asmformat\FormatFile.cpp
 *
 * File formatting function definitions
 *
*/

#include "pch.hpp"
#include "FormatFile.hpp"
#include "StringCast.hpp"


// Was previous line blank?
static bool previous_blank = false;

// Insert new blank line?
static bool insert_blankline = false;

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
		if (line.empty())
		{
			previous_blank = true;
		}
		else
		{
			previous_blank = false;

			// Trim trailing spaces at the end of line
			reg = L"\\s+$";
			line = std::regex_replace(line, reg, L"");

			if (line.starts_with(L";"))
			{
				// Make so that between semicolon and comment is only one space
				reg = L";\\s+";
				line = std::regex_replace(line, reg, L";");
				line.insert(1, L" ");
			}
			else
			{
				// Insert tab to beginning of each line
				line.insert(0, L"\t");

				reg = L"\t\\w+\\s+proc";
				const bool is_proc = std::regex_search(line, reg);

				reg = L"\t\\w+\\s+endp";
				const bool is_endproc = std::regex_search(line, reg);

				// Is code line indented with tab?
				// Do not indent procedure labels
				const bool isindented = !(is_proc || is_endproc);

				// if previous line is not blank and this is procedure inset blank line so that procedure blocks are sectioned
				if (is_proc && !previous_blank)
				{
					result += L"\n";
				}
				else if (is_endproc)
				{
					// Insert blank line later when done processing current line
					insert_blankline = true;
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

					// Make so that between semicolon and comment is only one space
					reg = L";\\s+";
					comment = std::regex_replace(comment, reg, L";");
					comment.insert(1, L" ");

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
		}

		result += line.append(L"\n");
		if (insert_blankline)
		{
			result += L"\n";
			insert_blankline = false;
		}
	}

	// Make sure first line is blank
	if (result.front() != L'\n')
	{
		result.insert(0, L"\n");
	}
	else
	{
		// Make sure only one blank line is at the top of file
		reg = L"^\n+";
		result = std::regex_replace(result, reg, L"\n");

		// Remove surplus blank lines at the end of file
		reg = L"\n+$";
		result = std::regex_replace(result, reg, L"\n");
	}

	filedata.str(result);
	filedata.clear();

	#ifdef _DEBUG
	std::cout << wsl::StringCast(filedata.str()) << std::endl;
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
		if (line.empty())
		{
			previous_blank = true;
		}
		else
		{
			previous_blank = false;

			// Trim trailing spaces at the end of line
			reg = "\\s+$";
			line = std::regex_replace(line, reg, "");

			if (line.starts_with(";"))
			{
				// Make so that between semicolon and comment is only one space
				reg = ";\\s+";
				line = std::regex_replace(line, reg, ";");
				line.insert(1, " ");
			}
			else
			{
				// Insert tab to beginning of each line
				line.insert(0, "\t");

				reg = "\t\\w+\\s+proc";
				const bool is_proc = std::regex_search(line, reg);

				reg = "\t\\w+\\s+endp";
				const bool is_endproc = std::regex_search(line, reg);

				// Is code line indented with tab?
				// Do not indent procedure labels
				const bool isindented = !(is_proc || is_endproc);

				// if previous line is not blank and this is procedure inset blank line so that procedure blocks are sectioned
				if (is_proc && !previous_blank)
				{
					result += "\n";
				}
				else if (is_endproc)
				{
					// Insert blank line later when done processing current line
					insert_blankline = true;
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

					// Make so that between semicolon and comment is only one space
					reg = ";\\s+";
					comment = std::regex_replace(comment, reg, ";");
					comment.insert(1, " ");

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
		}

		result += line.append("\n");
		if (insert_blankline)
		{
			result += "\n";
			insert_blankline = false;
		}
	}

	// Make sure first line is blank
	if (result.front() != '\n')
	{
		result.insert(0, "\n");
	}
	else
	{
		// Make sure only one blank line is at the top of file
		reg = "^\n+";
		result = std::regex_replace(result, reg, "\n");

		// Remove surplus blank lines at the end of file
		reg = "\n+$";
		result = std::regex_replace(result, reg, "\n");
	}

	filedata.str(result);
	filedata.clear();

	#ifdef _DEBUG
	std::cout << filedata.str() << std::endl;
	std::cout << "longest code line is: " << maxcodelen << std::endl;
	#endif
}
