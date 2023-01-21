
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
*/

/**
 * @file asmformat\StringCast.cpp
 *
 * String conversion function definitions
 *
*/

#include "pch.hpp"
#include "StringCast.hpp"
#include "error.hpp"
#include "ErrorCode.hpp"


namespace wsl
{
	std::wstring StringCast(const std::string& param, UINT code_page)
	{
		if (param.empty())
			return std::wstring();

		// MSDN: Determines if a specified code page is valid
		// Returns a nonzero value if the code page is valid, or 0 if the code page is invalid
		if (IsValidCodePage(code_page) == FALSE)
		{
			ShowError(ErrorCode::BadArgument, "Code page is not valid");
			return std::wstring();
		}

		// Always use precomposed characters, this in line with WC_COMPOSITECHECK in WideCharToMultiByte
		DWORD flags = MB_PRECOMPOSED;
		// Use glyph characters instead of control characters
		// flags |= MB_USEGLYPHCHARS;

		switch (code_page)
		{
		case 50220:
		case 50221:
		case 50222:
		case 50225:
		case 50227:
		case 50229:
		case 65000:
		case 42:
			// Must be 0, otherwise, the function fails with ERROR_INVALID_FLAGS
			flags = 0;
			break;
		case 54936:
		case CP_UTF8:
			// Fail if an invalid input character is encountered
			// MSDN: This flag only applies when CodePage is specified as CP_UTF8 or 54936.
			// It cannot be used with other code page values.
			flags = MB_ERR_INVALID_CHARS; // or 0
			break;
		default:
			// Must be 0, otherwise, the function fails with ERROR_INVALID_FLAGS
			if ((code_page >= 57002) && (code_page <= 57011))
				flags = 0;
			break;
		}

		const int source_string_size = static_cast<int>(param.size());
		const int wchar_size_needed = MultiByteToWideChar(code_page, flags, param.c_str(),
			// Size, in bytes, of the string indicated by the lpMultiByteStr parameter
			source_string_size,
			// Pointer to a buffer that receives the converted string
			nullptr,
			// If this value is 0, the function returns the required buffer size
			0
		);

		// The function returns 0 if it does not succeed
		if (wchar_size_needed == 0)
		{
			ShowError(ERROR_INFO);
			return std::wstring();
		}

		std::wstring result(static_cast<const unsigned int>(wchar_size_needed), 0);
		// Returns the number of characters written to the buffer indicated by lpWideCharStr if successful
		const int wchars_written = MultiByteToWideChar(code_page, flags, param.c_str(), source_string_size,	&result[0],
			// Size, in characters, of the buffer indicated by lpWideCharStr.
			wchar_size_needed
		);
		
		if (wchars_written == 0)
		{
			ShowError(ERROR_INFO);
			return std::wstring();
		}

		return result;
	}

	std::string StringCast(const std::wstring& param, UINT code_page)
	{
		if (param.empty())
			return std::string();

		// MSDN: Determines if a specified code page is valid
		// Returns a nonzero value if the code page is valid, or 0 if the code page is invalid
		if (IsValidCodePage(code_page) == FALSE)
		{
			ShowError(ErrorCode::BadArgument, "Code page is not valid");
			return std::string();
		}

		// Translate any Unicode characters that do not translate directly to multibyte equivalents to the default character specified by lpDefaultChar
		DWORD flags = WC_NO_BEST_FIT_CHARS;
		// Windows normally represents Unicode strings with precomposed data, making the use of the WC_COMPOSITECHECK flag unnecessary
		// flags |= WC_COMPOSITECHECK;

		// MSDN: For the CP_UTF7 and CP_UTF8 settings for CodePage, this parameter must be set to NULL.
		LPCCH default_char = ((code_page == CP_UTF7) || (code_page == CP_UTF8)) ? nullptr : nullptr;

		switch (code_page)
		{
		case 50220:
		case 50221:
		case 50222:
		case 50225:
		case 50227:
		case 50229:
		case 65000:
		case 42:
			// Must be 0, otherwise, the function fails with ERROR_INVALID_FLAGS
			flags = 0;
			break;
		case 54936:
		case CP_UTF8:
			// Fail if an invalid input character is encountered
			// MSDN: This flag only applies when CodePage is specified as CP_UTF8 or 54936.
			// It cannot be used with other code page values.
			flags = WC_ERR_INVALID_CHARS; // or 0
			break;
		default:
			// Must be 0, otherwise, the function fails with ERROR_INVALID_FLAGS
			if ((code_page >= 57002) && (code_page <= 57011))
				flags = 0;
			break;
		}

		const int source_wchar_size = static_cast<int>(param.size());

		// If the function succeeds and cbMultiByte is 0, the return value is the required size, in bytes, for the buffer indicated by lpMultiByteStr
		const int char_size_needed = WideCharToMultiByte(code_page, flags, param.c_str(),
			// Size, in characters, of the string indicated by lpWideCharStr
			source_wchar_size,
			// Pointer to a buffer that receives the converted string
			nullptr,
			// If this value is 0, the function returns the required buffer size, in bytes, including any terminating null character
			0,
			default_char, nullptr);

		if (char_size_needed == 0)
		{
			ShowError(ERROR_INFO);
			return std::string();
		}

		BOOL default_used = FALSE;
		std::string result(static_cast<const unsigned int>(char_size_needed), 0);

		// If successful, returns the number of bytes written to the buffer pointed to by lpMultiByteStr
		const int bytes_written = WideCharToMultiByte(code_page, flags, param.c_str(), source_wchar_size, &result[0],
			// Size, in bytes, of the buffer indicated by lpMultiByteStr.
			char_size_needed,
			// Pointer to the character to use if a character cannot be represented in the specified code page
			default_char,
			// Pointer to a flag that indicates if the function has used a default character in the conversion.
			&default_used
		);

		// The function returns 0 if it does not succeed
		if (bytes_written == 0)
		{
			ShowError(ERROR_INFO);
			return std::string();
		}

		// The flag is set to TRUE if one or more characters in the source string cannot be represented in the specified code page
		if (default_used == TRUE)
		{
			ShowError(Exception(ErrorCode::ParseFailure, "One or more characters in the source string could not be represented in the specified code page"), ERROR_INFO, MB_ICONINFORMATION);
		}

		return result;
	}

	#if 0
	std::wstring StringCast(const std::u8string& param)
	{
		char char_buff[MB_LEN_MAX]{};

		std::mbstate_t state{};
		std::stringstream string_buff{};

		for (char8_t c8 : param)
		{
			std::size_t ret = std::c8rtomb(char_buff, c8, &state);

			if (ret == -1)
			{
				ShowError(ErrorCode::FunctionFailed, L"Conversion from char8_t failed");
			}
			else
			{
				string_buff << std::string_view{ char_buff, ret };
			}
		}

		string_buff << '\0';

		return StringCast(string_buff.str());
	}
	#endif

	std::wstring StringCast(const std::u16string& param)
	{
		char char_buff[MB_LEN_MAX]{};

		std::mbstate_t state{};
		std::stringstream string_buff{};

		for (const char16_t c16 : param)
		{
			const std::size_t ret = std::c16rtomb(char_buff, c16, &state);

			if (ret == -1)
			{
				ShowError(ErrorCode::FunctionFailed, "Conversion from char16_t failed");
			}
			else
			{
				string_buff << std::string_view{ char_buff, ret };
			}
		}

		string_buff << '\0';

		return StringCast(string_buff.str());
	}

	std::wstring StringCast(const std::u32string& param)
	{
		char char_buff[MB_LEN_MAX]{};

		std::mbstate_t state{};
		std::stringstream string_buff{};

		for (const char32_t c32 : param)
		{
			const std::size_t ret = std::c32rtomb(char_buff, c32, &state);

			if (ret == -1)
			{
				ShowError(ErrorCode::FunctionFailed, "Conversion from char32_t failed");
			}
			else
			{
				string_buff << std::string_view{ char_buff, ret };
			}
		}

		string_buff << '\0';

		return StringCast(string_buff.str());
	}
}
