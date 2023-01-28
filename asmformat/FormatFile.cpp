
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
 *
*/

#include "pch.hpp"
#include "FormatFile.hpp"
#include "StringCast.hpp"
#include "error.hpp"
#include "ErrorCode.hpp"


/**
 * @brief MASM directives
 * https://learn.microsoft.com/en-us/cpp/assembler/masm/directives-reference
 *
 * We use underscore to denote a dot.
 * Only directives used by formatter are listed.
*/
enum class Directive
{
	none,	// Not a diretive
	proc,
	endp,
	_data,
	_code,
	_const,
	end
};

/**
 * @brief Instruction mnemonics
 *
 * Only mnemonics used by formatter are listed
*/
enum class Mnemonic
{
	none,	// Noa a mnemonic
	call
};

/**
 * @brief Provides information about a line
*/
struct LineInfo
{
	bool comment = false;
	bool blank = false;
	bool label = false;
	Mnemonic mnemonic = Mnemonic::none;
	Directive directive = Directive::none;
};

/**
 * @brief			Implementation for STRING macro
 * @tparam CharType Character type, either char or wchar_t
 * @param ansi		Exclusive ANSI string literal
 * @param wide		Exclusive Wide string literal
 * @return			Either Wide or ANSI version of string literal
*/
template<typename CharType>
[[nodiscard]] constexpr auto ConditionalString([[maybe_unused]] const char* ansi, [[maybe_unused]] const wchar_t* wide)
{
	if constexpr (std::is_same_v<CharType, char>)
		return ansi;

	if constexpr (std::is_same_v<CharType, wchar_t>)
		return wide;
}

// TODO: This string macro can be used to reduce code bloat and have a single formatting function
// Conditional Wide or ANSI string literal
#define STRING(StringType, string) ConditionalString<typename StringType::value_type>(string, L ## string)

/**
 * @brief				Get detailed information about a line
 * @tparam StringType	std::string
 * @tparam RegexType	std::regex
 * @param line			line which to check
 * @return				LineInfo struct
*/
template<typename RegexType, typename StringType>
requires std::is_same_v<typename RegexType::value_type, typename StringType::value_type>
[[nodiscard]] LineInfo GetLineInfo(const StringType& line)
{
	// Not testing if comment or blank because handled in std::getline loops
	LineInfo lineinfo = { 0 };

	//
	// The order of regex tests must begin with more likely toward less likely matches
	//
	if (std::regex_search(line, RegexType(STRING(StringType, "^call"), std::regex_constants::icase)))
		lineinfo.mnemonic = Mnemonic::call;

	else if (std::regex_search(line, RegexType(STRING(StringType, "^\\w+\\s+proc"), std::regex_constants::icase)))
		lineinfo.directive = Directive::proc;

	else if (std::regex_search(line, RegexType(STRING(StringType, "^\\w+\\s+endp"), std::regex_constants::icase)))
		lineinfo.directive = Directive::endp;

	else if (std::regex_search(line, RegexType(STRING(StringType, "^\\w+:"), std::regex_constants::icase)))
		lineinfo.label = true;

	else if (std::regex_search(line, RegexType(STRING(StringType, "^\\.data"), std::regex_constants::icase)))
		lineinfo.directive = Directive::_data;

	else if (std::regex_search(line, RegexType(STRING(StringType, "^\\.code"), std::regex_constants::icase)))
		lineinfo.directive = Directive::_code;

	else if (std::regex_search(line, RegexType(STRING(StringType, "^\\.const"), std::regex_constants::icase)))
		lineinfo.directive = Directive::_const;

	else if (std::regex_search(line, RegexType(STRING(StringType, "^end"), std::regex_constants::icase)))
		lineinfo.directive = Directive::end;

	return lineinfo;
}

/**
 * @brief		Check if line should be indented
 * @param line	Line which to check
 * @return		true if the line should be indented
*/
[[nodiscard]] inline bool TestIndentLine(const LineInfo& lineinfo) noexcept
{
	switch (lineinfo.directive)
	{
	case Directive::proc:
	case Directive::endp:
	case Directive::_code:
	case Directive::_data:
	case Directive::_const:
		return false;
	default:
		return !lineinfo.label;
	}
}

/**
 * @brief				Get next line in file without affecting stream position
 * @tparam StreamType	std::stringstream
 * @tparam StringType	std::string
 * @param filedata		string stream holding file data
 * @param nextline		string which receives next line
*/
template<typename StreamType, typename StringType>
requires std::is_same_v<typename StreamType::char_type, typename StringType::value_type>
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
requires std::is_same_v<typename StringType::value_type, typename StreamType::char_type>
[[nodiscard]] LineBreak GetLineBreak(StreamType& filedata)
{
	typename StringType::value_type cr;

	if constexpr (std::is_base_of_v<std::basic_string<wchar_t>, StringType>)
		cr = L'\r';
	else cr = '\r';

	StringType nextline{ };
	PeekNextLine(filedata, nextline);

	if (nextline.empty() || (*(nextline.cend() - 1) != cr))
		return LineBreak::LF;

	return LineBreak::CRLF;
}

/**
 * Get next code line (skipping comments) in file without affecting stream position.
 * If a blank line is reached then the function breaks and does not reach next code line.
 * Optionally skip blank lines as well, if eof is reached the function returns true.
 * If next code line is not reached the codeline parameter contains last line processed.
 *
 * @tparam StreamType	std::stringstream
 * @tparam StringType	std::string
 * @param filedata		string stream holding file data
 * @param codeline		string which receives next code line if any
 * @param crlf			Is line break CRLF?
 * @param skip_blanks	Skip blank lines too?
 * @return				true if blank line (by default) or EOF was reached before code line, false otherwise
*/
template<typename StreamType, typename StringType>
requires std::is_same_v<typename StreamType::char_type, typename StringType::value_type>
[[nodiscard]] bool PeekNextCodeLine(StreamType& filedata, StringType& codeline, bool crlf, bool skip_blanks)
{
	bool fail = false;
	StringType semicolon;
	const std::streampos pos = filedata.tellg();

	if constexpr (std::is_base_of_v<std::basic_string<wchar_t>, StringType>)
		semicolon = L";";
	else semicolon = ";";

	while (std::getline(filedata, codeline).good())
	{
		// Skip comments
		if (!codeline.starts_with(semicolon))
		{
			if (!codeline.empty() && crlf)
			{
				// Drop \r
				codeline.erase(codeline.cend() - 1);
			}

			if (codeline.empty())
				if (skip_blanks)
					continue;
				else fail = true;

			break;
		}
	}

	fail = fail || filedata.eof();
	filedata.clear();
	filedata.seekg(pos);

	return fail;
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
requires std::is_same_v<typename StringType::value_type, typename StreamType::char_type>
[[nodiscard]] std::size_t GetBlankCount(StreamType& filedata, bool crlf)
{
	StringType line{ };
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

// Information about previous line
static LineInfo previous_line;

// Insert new blank line?
static bool insert_blankline = false;

void FormatFileW(std::wstringstream& filedata, std::size_t tab_width, bool spaces, bool compact, LineBreak line_break)
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

		// filedata is unchanged
		return;
	}

	assert(filedata.eof());
	// set good bit (remove eof bit)
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
			previous_line.blank = true;
		}
		else
		{
			std::wstring nextcode;

			// How comments are indented depends on what's after those comments
			if (line.starts_with(L";"))
			{

				// Peek at next code line unless blank line is reached
				const bool isblank = PeekNextCodeLine(filedata, nextcode, crlf, false);
				const LineInfo nextcodeinfo = isblank ? LineInfo{ 0 } : GetLineInfo<std::wregex>(nextcode);

				// Will next code line be indented?
				const bool next_indent = !isblank && TestIndentLine(nextcodeinfo);

				// Make only one space between semicolon and comment
				regex = L"^;\\s*";
				const std::wstring replacement = next_indent ? tab + L"; " : L"; ";
				line = std::regex_replace(line, regex, replacement);

				// For sectionaing purposes blank line must be inserted before next code that requires it is reached
				if (!(previous_line.blank || previous_line.comment))
					switch (nextcodeinfo.directive)
					{
					case Directive::proc:
					case Directive::_data:
					case Directive::_code:
					case Directive::_const:
						result += linebreak;
						break;
					default:
						break;
					}

				previous_line.comment = true;
			}
			else
			{
				const bool isend = PeekNextCodeLine(filedata, nextcode, crlf, true);
				const LineInfo lineinfo = GetLineInfo<std::wregex>(line);
				const LineInfo nextcodeinfo = GetLineInfo<std::wregex>(nextcode);

				switch (lineinfo.directive)
				{
				case Directive::proc:
				case Directive::_data:
				case Directive::_code:
				case Directive::_const:
				{
					// if previous line is not blank and this is non indented directive insert blank line to section these directives
					// if previous line is comment, blank line was already inserted
					if (!(previous_line.blank || previous_line.comment))
						result += linebreak;

					// Remove blank lines that follow
					skiplines = GetBlankCount<std::wstring>(filedata, crlf);
					break;
				}
				case Directive::endp:
				{
					if (!isend)
					{
						const std::size_t blanks = GetBlankCount<std::wstring>(filedata, crlf);
						if (nextcodeinfo.directive == Directive::end)
						{
							// Remove blank lines that follow endp label up until a comment if any
							skiplines = blanks;
						}
						else if (blanks == 0)
						{
							// Insert blank line later when done processing current line
							insert_blankline = true;
						}
					}
					break;
				}
				case Directive::none:
				default:
				{
					switch (lineinfo.mnemonic)
					{
					case Mnemonic::call:
						// Section code by call to function or procedure
						insert_blankline = true;
						break;
					case Mnemonic::none:
					default:
						if (!isend)
						{
							if (nextcodeinfo.label)
								insert_blankline = true;
						}
						break;
					}
				}
					break;
				}

				// Is code line indented with tab?
				const bool indent = TestIndentLine(lineinfo);

				if (indent)
				{
					// Indent line by inserting tab
					line.insert(0, tab);
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
					regex = L"^;\\s*";
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

				previous_line.comment = false;
			}

			previous_line.blank = false;
		}

		// \n dropped by getline \r dropped manually
		result += line.append(linebreak);

		if (insert_blankline)
		{
			result += linebreak;
			insert_blankline = false;
			previous_line.blank = true;
		}
	}

	if (filedata.bad() || (!filedata.eof() && filedata.fail()))
	{
		ShowError(wsl::ErrorCode::ParseFailure, ("Processing source file data failed after line: " + wsl::StringCast(line)).c_str());

		// filedata is unchanged
		return;
	}

	// Make sure first line is blank
	if (!result.starts_with(linebreak))
	{
		result.insert(0, linebreak);
	}

	regex = L"^(" + linebreak + L"){2,}";
	if (compact)
	{
		// TODO: This should be commented out during testing to make sure surplus blanks are not created by previous code,
		// we want to remove only blanks that already existed without creating additional surplus blanks for performance reasons.
		// TODO: This should handle blank lines at the end of file but it doesn't.
		// Remove all surplus blank lines
		result = std::regex_replace(result, regex, linebreak);
	}
	else
	{
		// Remove surplus blank lines at the top of a file
		result = std::regex_replace(result, regex, linebreak, std::regex_constants::match_continuous);
	}

	// Remove all blank lines before endproc
	regex = std::wregex(L"^(" + linebreak + L")+(?=\\w+\\s+endp)", std::regex_constants::icase);
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

	assert(filedata.eof());
	// set good bit (remove eof bit)
	filedata.clear();
	filedata.str(result);

	#if FALSE
	// TODO: Reason why output is incorrect in the console is because not all tabs consume tab_width spaces
	std::string str = wsl::StringCast(result);
	wsl::ReplaceAll(str, "\t", std::string(tab_width, ' '));
	std::cout << str << std::endl;
	std::cout << "longest code line " << "(" << maxcodelen << ")" << " is: " << wsl::StringCast(maxlenline) << std::endl;
	#endif
}

void FormatFileA(std::stringstream& filedata, std::size_t tab_width, bool spaces, bool compact, LineBreak line_break)
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

		// filedata is unchanged
		return;
	}

	assert(filedata.eof());
	// set good bit (remove eof bit)
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
			previous_line.blank = true;
		}
		else
		{
			std::string nextcode;

			// How comments are indented depends on what's after those comments
			if (line.starts_with(";"))
			{
				// Peek at next code line unless blank line is reached
				const bool isblank = PeekNextCodeLine(filedata, nextcode, crlf, false);
				const LineInfo nextcodeinfo = isblank ? LineInfo{ 0 } : GetLineInfo<std::regex>(nextcode);

				// Will next code line be indented?
				const bool next_indent = !isblank && TestIndentLine(nextcodeinfo);

				// Make only one space between semicolon and comment
				regex = "^;\\s*";
				const std::string replacement = next_indent ? tab + "; " : "; ";
				line = std::regex_replace(line, regex, replacement);

				// For sectionaing purposes blank line must be inserted before next code that requires it is reached
				if (!(previous_line.blank || previous_line.comment))
					switch (nextcodeinfo.directive)
					{
					case Directive::proc:
					case Directive::_data:
					case Directive::_code:
					case Directive::_const:
						result += linebreak;
						break;
					default:
						break;
					}

				previous_line.comment = true;
			}
			else
			{
				const bool isend = PeekNextCodeLine(filedata, nextcode, crlf, true);
				const LineInfo lineinfo = GetLineInfo<std::regex>(line);
				const LineInfo nextcodeinfo = GetLineInfo<std::regex>(nextcode);

				switch (lineinfo.directive)
				{
				case Directive::proc:
				case Directive::_data:
				case Directive::_code:
				case Directive::_const:
				{
					// if previous line is not blank and this is non indented directive insert blank line to section these directives
					// if previous line is comment, blank line was already inserted
					if (!(previous_line.blank || previous_line.comment))
						result += linebreak;

					// Remove blank lines that follow
					skiplines = GetBlankCount<std::string>(filedata, crlf);
					break;
				}
				case Directive::endp:
				{
					if (!isend)
					{
						const std::size_t blanks = GetBlankCount<std::string>(filedata, crlf);
						if (nextcodeinfo.directive == Directive::end)
						{
							// Remove blank lines that follow endp label up until a comment if any
							skiplines = blanks;
						}
						else if (blanks == 0)
						{
							// Insert blank line later when done processing current line
							insert_blankline = true;
						}
					}
					break;
				}
				case Directive::none:
				default:
				{
					switch (lineinfo.mnemonic)
					{
					case Mnemonic::call:
						// Section code by call to function or procedure
						insert_blankline = true;
						break;
					case Mnemonic::none:
					default:
						if (!isend)
						{
							if (nextcodeinfo.label)
								insert_blankline = true;
						}
						break;
					}
				}
				break;
				}

				// Is code line indented with tab?
				const bool indent = TestIndentLine(lineinfo);

				if (indent)
				{
					// Indent line by inserting tab
					line.insert(0, tab);
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
					regex = "^;\\s*";
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

				previous_line.comment = false;
			}

			previous_line.blank = false;
		}

		// \n dropped by getline \r dropped manually
		result += line.append(linebreak);

		if (insert_blankline)
		{
			result += linebreak;
			insert_blankline = false;
			previous_line.blank = true;
		}
	}

	if (filedata.bad() || (!filedata.eof() && filedata.fail()))
	{
		ShowError(wsl::ErrorCode::ParseFailure, ("Processing source file data failed after line: " + line).c_str());

		// filedata is unchanged
		return;
	}

	// Make sure first line is blank
	if (!result.starts_with(linebreak))
	{
		result.insert(0, linebreak);
	}

	regex = "^(" + linebreak + "){2,}";
	if (compact)
	{
		// TODO: This should be commented out during testing to make sure surplus blanks are not created by previous code,
		// we want to remove only blanks that already existed without creating additional surplus blanks for performance reasons.
		// TODO: This should handle blank lines at the end of file but it doesn't.
		// Remove all surplus blank lines
		result = std::regex_replace(result, regex, linebreak);
	}
	else
	{
		// Remove surplus blank lines at the top of a file
		result = std::regex_replace(result, regex, linebreak, std::regex_constants::match_continuous);
	}

	// Remove all blank lines before endproc
	regex = std::regex("^(" + linebreak + ")+(?=\\w+\\s+endp)", std::regex_constants::icase);
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

	assert(filedata.eof());
	// set good bit (remove eof bit)
	filedata.clear();
	filedata.str(result);

	#if FALSE
	// TODO: Reason why output is incorrect in the console is because not all tabs consume tab_width spaces
	wsl::ReplaceAll(result, "\t", std::string(tab_width, ' '));
	std::cout << result << std::endl;
	std::cout << "longest code line " << "(" << maxcodelen << ")" << " is: " << maxlenline << std::endl;
	#endif
}
