
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
*/

/**
 * @file asmformat\FormatFile.cpp
 *
 * ASM source file formatting function definitions
 * TODO: Maybe using macros to conditionally use wide and ansi strings to reduce code bloat?
 *
*/

#include "pch.hpp"
#include "FormatFile.hpp"
#include "StringCast.hpp"
#include "error.hpp"
#include "ErrorCode.hpp"


/**
 * @brief				Check if line should be indented
 * @tparam RegexType	std::regex
 * @tparam StringType	std::string
 * @param line			line which to check
 * @return				true if the line should be indented
*/
template<typename RegexType, typename StringType>
[[nodiscard]] inline bool TestIndentLine(const StringType& line)
{
	RegexType regex;

	if constexpr (std::is_base_of_v<std::basic_string<wchar_t>, StringType>)
	{
		regex = L"\\w+\\s+(proc|endp)", std::regex_constants::icase;
	}
	else
	{
		regex = "\\w+\\s+(proc|endp)", std::regex_constants::icase;
	}

	const bool proc = std::regex_search(line, regex);
	return !proc;
}

/**
 * @brief				Get next line in file without affecting stream position
 * @tparam StreamType	std::stringstream
 * @tparam StringType	std::string
 * @param filedata		string stream holding file data
 * @param nextline		string which receives next line
*/
template<typename StreamType, typename StringType>
inline void PeekNextLine(StreamType& filedata, StringType& nextline)
{
	const std::streampos pos = filedata.tellg();
	std::getline(filedata, nextline);

	filedata.clear();
	filedata.seekg(pos);
}

/**
 * @brief				Get line break used in file
 * @tparam StringType	std::string
 * @tparam StreamType	std::stringstream
 * @param filedata		File contents
 * @return				LineBreak enum
*/
template<typename StringType, typename StreamType>
[[nodiscard]] inline LineBreak GetLineBreak(StreamType& filedata)
{
	StringType nextline;
	// TODO: What if next line is EOF?
	PeekNextLine(filedata, nextline);

	if (nextline.empty() || (*(nextline.cend() - 1) != L'\r'))
		return LineBreak::LF;

	return LineBreak::CRLF;
}

/**
 * Get next code line (skipping comments) in file without affecting stream position.
 * If a blank line is reached then the function breaks and does not reach next code line
 * 
 * @tparam StreamType	std::stringstream
 * @tparam StringType	std::string
 * @param filedata		string stream holding file data
 * @param codeline		string which receives next code line
 * @param crlf			Is line break CRLF?
 * @return				true if blank line was reached before code line, false otherwise
*/
template<typename StreamType, typename StringType>
[[nodiscard]] inline bool PeekNextCodeLine(StreamType& filedata, StringType& codeline, bool crlf)
{
	bool isblank = false;
	StringType semicolon;
	const std::streampos pos = filedata.tellg();

	if constexpr (std::is_base_of_v<std::basic_string<wchar_t>, StringType>)
	{
		semicolon = L";";
	}
	else
	{
		semicolon = ";";
	}

	while (std::getline(filedata, codeline).good())
	{
		if (!codeline.starts_with(semicolon))
		{
			if (!codeline.empty() && crlf)
			{
				// Drop \r
				codeline.erase(codeline.cend() - 1);
			}

			if (codeline.empty())
				isblank = true;

			break;
		}
	}

	filedata.clear();
	filedata.seekg(pos);

	return isblank;
}

/**
 * @brief				Get count of blank lines that follow
 * @tparam StringType	std::string
 * @tparam StreamType	std::stringstream
 * @param filedata		string stream holding file data
 * @param crlf			Is line break CRLF?
 * @return				Count of blank lines that follow
*/
template<typename StringType, typename StreamType>
[[nodiscard]] inline std::size_t GetBlankCount(StreamType& filedata, bool crlf)
{
	StringType line;
	std::size_t count = 0;
	const std::streampos pos = filedata.tellg();

	while (std::getline(filedata, line).good())
	{
		if (!line.empty() && crlf)
		{
			// Drop \r
			line.erase(line.cend() - 1);
		}

		if (line.empty())
			++count;
		else break;
	}

	filedata.clear();
	filedata.seekg(pos);

	return count;
}

// Minimum capacity for strings
constexpr std::size_t MIN_CAPACITY = 1000;

// Was previous line blank?
static bool previous_blank = false;

// Insert new blank line?
static bool insert_blankline = false;

void FormatFileW(std::wstringstream& filedata, unsigned tab_width, bool spaces, bool compact, LineBreak line_break)
{
	#ifdef _DEBUG
	std::wstring maxlenline;
	#endif // DEBUG

	std::wregex regex;
	std::wstring line;
	std::wstring result;
	std::wstring tab = spaces ? std::wstring(tab_width, L' ') : L"\t";

	line.reserve(MIN_CAPACITY);
	result.reserve(filedata.str().capacity() + MIN_CAPACITY);

	// Count of characters of the longest code line which contains inline comment
	// inline comments will be shifted according to longest code line
	std::size_t maxcodelen = 0;

	const bool crlf = GetLineBreak<std::wstring>(filedata) == LineBreak::CRLF;
	const std::wstring linebreak = crlf ? L"\r\n" : L"\n";

	// crlf != (line_break != LineBreak::CRLF)
	// true (crlf) != false (CRLF) = true
	// true (crlf) != true (LF)    = false
	// false (lf)  != true (LF)    = true
	// false (lf)  != false (CRLF) = false
	const bool preserve = (line_break == LineBreak::Preserve) || (crlf != (line_break != LineBreak::CRLF));
	
	// Formatting a soure file consists of 2 while loops, each looping trough lines in file,
	// First loop trims leading and trailing spaces and tabs and calculates the widest code line containing an inline comment
	// Second loop (later) uses this information (compacted lines and length) to perform accurate formatting
	while (std::getline(filedata, line).good())
	{
		if (!line.empty() && crlf)
		{
			// Drop \r
			line.erase(line.cend() -1);
		}

		if (!line.empty())
		{
			// Shift line to beginning by trimming leading spaces and tabs
			regex = L"^\\s+(.*)";
			line = std::regex_replace(line, regex, L"$1");

			// Trim trailing spaces and tabs
			regex = L"\\s+$";
			line = std::regex_replace(line, regex, L"");

			// Calculate longest code line with inline comment, excluding indentation
			if (!line.starts_with(L";"))
			{
				regex = L"^(.*?)(?=\\s*;)";
				std::wsmatch match;

				if (std::regex_search(line, match, regex))
				{
					const std::wssub_match code = match[1];
					const std::size_t codelen = code.str().length();

					if (codelen > maxcodelen)
					{
						maxcodelen = codelen;

						#ifdef _DEBUG
						maxlenline = line;
						#endif // DEBUG
					}
				}
			}
		}

		// getline dropped \n and \r dropped manually
		result += line.append(linebreak);
	}

	if (filedata.bad() || (!filedata.eof() && filedata.fail()))
	{
		ShowError(wsl::ErrorCode::ParseFailure, ("Processing source file data failed after line: " + wsl::StringCast(line)).c_str());
	}

	// set good bit (remove eofbit)
	assert(filedata.eof());
	filedata.clear();
	filedata.str(result);

	// clear contents but not memory
	result.clear();
	assert(result.capacity() > MIN_CAPACITY);

	// Count of characters missing to make a full tab of the max length code line
	const std::size_t maxmissing = tab_width - maxcodelen % tab_width;

	// Count of lines to skip
	std::size_t skiplines = 0;

	while (std::getline(filedata, line).good())
	{
		if (skiplines > 0)
		{
			--skiplines;
			continue;
		}

		if (!line.empty() && crlf)
		{
			// Drop \r
			line.erase(line.cend() - 1);
		}

		if (line.empty())
		{
			previous_blank = true;
		}
		else
		{
			// How comments are indented depends on what's after those comments
			if (line.starts_with(L";"))
			{
				std::wstring nextline;

				// Peek at next code line unless blank line is reached
				const bool isblank = PeekNextCodeLine(filedata, nextline, crlf);

				// Will next line be indented?
				const bool next_indent = !isblank && TestIndentLine<std::wregex>(nextline);

				// Make only one space between semicolon and comment
				regex = L";\\s*";
				const std::wstring replacement = next_indent ? tab + L"; " : L"; ";
				line = std::regex_replace(line, regex, replacement);
			}
			else
			{
				const bool is_proc = std::regex_search(line, std::wregex(L"\\w+\\s+proc", std::regex_constants::icase));
				const bool is_endproc = std::regex_search(line, std::wregex(L"\\w+\\s+endp", std::regex_constants::icase));

				// Is code line indented with tab?
				// Do not indent procedure labels
				const bool indent = !(is_proc || is_endproc);

				if (indent)
				{
					// Indent line by inserting tab
					line.insert(0, tab);
				}

				// if previous line is not blank and this is procedure insert blank line so that procedure blocks are sectioned
				if (is_proc)
				{
					if (!previous_blank)
						result += linebreak;

					// Remove blank lines that follow proc label
					skiplines = GetBlankCount<std::wstring>(filedata, crlf);
				}
				else if (is_endproc)
				{
					// Insert blank line later when done processing current line
					insert_blankline = true;
				}

				// Format inline comments to start on same column
				// On which column depends on the longest code line containing inline comment
				regex = L"^(" + tab + L")?(.*?)(?=\\s*;)(\\s*)(;.*)";
				std::wsmatch match;

				if (std::regex_search(line, match, regex))
				{
					// Character length of the current code line, excluding indentation
					const std::size_t codelen = match[2].str().length();

					std::wstring code = std::regex_replace(line, regex, L"$1$2");
					std::wstring comment = std::regex_replace(line, regex, L"$4");

					// Make between semicolon and comment only one space
					regex = L";\\s*";
					comment = std::regex_replace(comment, regex, L"; ");

					// Character length difference of current code line compared to max length code line
					std::size_t diff = maxcodelen - codelen;
					// Also include characters which will be added to max length code line
					diff += maxmissing;

					if (spaces)
					{
						if (!indent)
						{
							// This accounts for removed tab at the start of line
							diff += tab_width;
						}

						code.append(diff, L' ');
					}
					else
					{
						std::size_t tabcount = diff / tab_width;

						if (!indent)
						{
							// This accounts for removed tab at the start of line
							++tabcount;
						}

						// Tab count must be multiple of tab width
						if (diff % tab_width != 0)
						{
							++tabcount;
						}

						code.append(tabcount, L'\t');
					}

					line = code.append(comment);
				}
			}

			previous_blank = false;
		}

		// \n dropped by getline \r dropped manually
		result += line.append(linebreak);

		if (insert_blankline)
		{
			result += linebreak;
			insert_blankline = false;
		}
	}

	// Make sure first line is blank
	if (!result.starts_with(linebreak))
	{
		result.insert(0, linebreak);
	}

	regex = L"^(" + linebreak + L"){2,}";
	if (compact)
	{
		// Remove all surplus blank lines
		// TODO: This should handle blank lines at the end of file but it doesn't
		result = std::regex_replace(result, regex, linebreak);
	}
	else
	{
		// Remove surplus blank lines at the top of a file
		result = std::regex_replace(result, regex, linebreak, std::regex_constants::match_continuous);
	}

	// Remove all blank lines before endproc
	regex = L"^(" + linebreak + L")+(?=\\w+\\s+endp)";
	result = std::regex_replace(result, regex, L"");

	// Remove surplus blank lines at the end of a file
	regex = L"(" + linebreak + L"){2,}$";
	result = std::regex_replace(result, regex, linebreak);

	if (!preserve)
	{
		switch (line_break)
		{
		case LineBreak::LF:
			wsl::ReplaceAll(result, linebreak, L"\n");
			break;
		case LineBreak::CRLF:
			wsl::ReplaceAll(result, linebreak, L"\r\n");
			break;
		case LineBreak::CR:
			ShowError(wsl::ErrorCode::NotImplemented, "CR line break not implemeted");
			break;
		case LineBreak::Preserve:
		default:
			break;
		}
	}
	
	if (filedata.bad() || (!filedata.eof() && filedata.fail()))
	{
		ShowError(wsl::ErrorCode::ParseFailure, ("Processing source file data failed after line: " + wsl::StringCast(line)).c_str());
	}

	assert(filedata.eof());
	filedata.clear();
	filedata.str(result);

	#ifdef _DEBUG
	// TODO: Reason why output is incorrect in the console is because not all tabs consume tab_width spaces
	std::string str = wsl::StringCast(result);
	wsl::ReplaceAll(str, "\t", std::string(tab_width, ' '));
	std::cout << str << std::endl;
	std::cout << "longest code line " << "(" << maxcodelen << ")" << " is: " << wsl::StringCast(maxlenline) << std::endl;
	#endif
}

void FormatFileA(std::stringstream& filedata, unsigned tab_width, bool spaces, bool compact, LineBreak line_break)
{
	#ifdef _DEBUG
	std::string maxlenline;
	#endif // DEBUG

	std::regex regex;
	std::string line;
	std::string result;
	std::string tab = spaces ? std::string(tab_width, ' ') : "\t";

	line.reserve(MIN_CAPACITY);
	result.reserve(filedata.str().capacity() + MIN_CAPACITY);

	// Count of characters of the longest code line which contains inline comment
	// inline comments will be shifted according to longest code line
	std::size_t maxcodelen = 0;

	const bool crlf = GetLineBreak<std::string>(filedata) == LineBreak::CRLF;
	const std::string linebreak = crlf ? "\r\n" : "\n";

	// crlf != (line_break != LineBreak::CRLF)
	// true (crlf) != false (CRLF) = true
	// true (crlf) != true (LF)    = false
	// false (lf)  != true (LF)    = true
	// false (lf)  != false (CRLF) = false
	const bool preserve = (line_break == LineBreak::Preserve) || (crlf != (line_break != LineBreak::CRLF));

	// Formatting a soure file consists of 2 while loops, each looping trough lines in file,
	// First loop trims leading and trailing spaces and tabs and calculates the widest code line containing an inline comment
	// Second loop (later) uses this information (compacted lines and length) to perform accurate formatting
	while (std::getline(filedata, line).good())
	{
		if (!line.empty() && crlf)
		{
			// Drop \r
			line.erase(line.cend() - 1);
		}

		if (!line.empty())
		{
			// Shift line to beginning by trimming leading spaces and tabs
			regex = "^\\s+(.*)";
			line = std::regex_replace(line, regex, "$1");

			// Trim trailing spaces and tabs
			regex = "\\s+$";
			line = std::regex_replace(line, regex, "");

			// Calculate longest code line with inline comment, excluding indentation
			if (!line.starts_with(";"))
			{
				regex = "^(.*?)(?=\\s*;)";
				std::smatch match;

				if (std::regex_search(line, match, regex))
				{
					const std::ssub_match code = match[1];
					const std::size_t codelen = code.str().length();

					if (codelen > maxcodelen)
					{
						maxcodelen = codelen;

						#ifdef _DEBUG
						maxlenline = line;
						#endif // DEBUG
					}
				}
			}
		}

		// getline dropped \n and \r dropped manually
		result += line.append(linebreak);
	}

	if (filedata.bad() || (!filedata.eof() && filedata.fail()))
	{
		ShowError(wsl::ErrorCode::ParseFailure, ("Processing source file data failed after line: " + line).c_str());
	}

	// set good bit (remove eofbit)
	assert(filedata.eof());
	filedata.clear();
	filedata.str(result);

	// clear contents but not memory
	result.clear();
	assert(result.capacity() > MIN_CAPACITY);

	// Count of characters missing to make a full tab of the max length code line
	const std::size_t maxmissing = 4 - maxcodelen % 4;

	// Count of lines to skip
	std::size_t skiplines = 0;

	while (std::getline(filedata, line).good())
	{
		if (skiplines > 0)
		{
			--skiplines;
			continue;
		}

		if (!line.empty() && crlf)
		{
			// Drop \r
			line.erase(line.cend() - 1);
		}

		if (line.empty())
		{
			previous_blank = true;
		}
		else
		{
			// How comments are indented depends on what's after those comments
			if (line.starts_with(";"))
			{
				std::string nextline;

				// Peek at next code line unless blank line is reached
				const bool isblank = PeekNextCodeLine(filedata, nextline, crlf);

				// Will next line be indented?
				const bool next_indent = !isblank && TestIndentLine<std::regex>(nextline);

				// Make only one space between semicolon and comment
				regex = ";\\s*";
				const std::string replacement = next_indent ? tab + "; " : "; ";
				line = std::regex_replace(line, regex, replacement);
			}
			else
			{
				const bool is_proc = std::regex_search(line, std::regex("\\w+\\s+proc", std::regex_constants::icase));
				const bool is_endproc = std::regex_search(line, std::regex("\\w+\\s+endp", std::regex_constants::icase));

				// Is code line indented with tab?
				// Do not indent procedure labels
				const bool indent = !(is_proc || is_endproc);

				if (indent)
				{
					// Indent line by inserting tab
					line.insert(0, tab);
				}

				// if previous line is not blank and this is procedure insert blank line so that procedure blocks are sectioned
				if (is_proc)
				{
					if (!previous_blank)
						result += linebreak;

					// Remove blank lines that follow proc label
					skiplines = GetBlankCount<std::string>(filedata, crlf);
				}
				else if (is_endproc)
				{
					// Insert blank line later when done processing current line
					insert_blankline = true;
				}

				// Format inline comments to start on same column
				// On which column depends on the longest code line containing inline comment
				regex = "^(" + tab + ")?(.*?)(?=\\s*;)(\\s*)(;.*)";
				std::smatch match;

				if (std::regex_search(line, match, regex))
				{
					// Character length of the current code line, excluding indentation
					const std::size_t codelen = match[2].str().length();

					std::string code = std::regex_replace(line, regex, "$1$2");
					std::string comment = std::regex_replace(line, regex, "$4");

					// Make between semicolon and comment only one space
					regex = ";\\s*";
					comment = std::regex_replace(comment, regex, "; ");

					// Character length difference of current code line compared to max length code line
					std::size_t diff = maxcodelen - codelen;
					// Also include characters that will be added to max length code line
					diff += maxmissing;

					if (spaces)
					{
						if (!indent)
						{
							// This accounts for removed tab at the start of line
							diff += tab_width;
						}

						code.append(diff, ' ');
					}
					else
					{
						std::size_t tabcount = diff / tab_width;

						if (!indent)
						{
							// This accounts for removed tab at the start of line
							++tabcount;
						}

						// Tab count must be multiple of tab width
						if (diff % tab_width != 0)
						{
							++tabcount;
						}

						code.append(tabcount, '\t');
					}

					line = code.append(comment);
				}
			}

			previous_blank = false;
		}

		// \n dropped by getline \r dropped manually
		result += line.append(linebreak);

		if (insert_blankline)
		{
			result += linebreak;
			insert_blankline = false;
		}
	}

	// Make sure first line is blank
	if (!result.starts_with(linebreak))
	{
		result.insert(0, linebreak);
	}

	regex = "^(" + linebreak + "){2,}";
	if (compact)
	{
		// Remove all surplus blank lines
		result = std::regex_replace(result, regex, linebreak);
	}
	else
	{
		// Remove surplus blank lines at the top of a file
		result = std::regex_replace(result, regex, linebreak, std::regex_constants::match_continuous);
	}

	// Remove all blank lines before endproc
	regex = "^(" + linebreak + ")+(?=\\w+\\s+endp)";
	result = std::regex_replace(result, regex, "");

	// Remove surplus blank lines at the end of a file
	regex = "(" + linebreak + "){2,}$";
	result = std::regex_replace(result, regex, linebreak);

	if (!preserve)
	{
		switch (line_break)
		{
		case LineBreak::LF:
			wsl::ReplaceAll(result, linebreak, "\n");
			break;
		case LineBreak::CRLF:
			wsl::ReplaceAll(result, linebreak, "\r\n");
			break;
		case LineBreak::CR:
			ShowError(wsl::ErrorCode::NotImplemented, "CR line break not implemeted");
			break;
		case LineBreak::Preserve:
		default:
			break;
		}
	}

	if (filedata.bad() || (!filedata.eof() && filedata.fail()))
	{
		ShowError(wsl::ErrorCode::ParseFailure, ("Processing source file data failed after line: " + line).c_str());
	}
	
	assert(filedata.eof());
	filedata.clear();
	filedata.str(result);

	#ifdef _DEBUG
	// TODO: Reason why output is incorrect in the console is because not all tabs consume tab_width spaces
	wsl::ReplaceAll(result, "\t", std::string(tab_width, ' '));
	std::cout << result << std::endl;
	std::cout << "longest code line " << "(" << maxcodelen << ")" << " is: " << maxlenline << std::endl;
	#endif
}
