
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

	// 'setlocale(0, locale.c_str())' could be '0', and is a copy of the value found in 'setlocale()`252'
	// Functions handle it's not null but not if same
	#pragma warning (disable: 28183)

	std::u16string StringCast16(const std::string& param, const std::string locale)
	{
		if (locale.empty())
		{
			ShowError(ErrorCode::BadArgument, "locale cannot be empty string");
			return std::u16string();
		}

		// null pointer on failure
		std::string old_locale = std::setlocale(LC_ALL, locale.c_str());
		if (old_locale.empty())
			ShowError(ErrorCode::FunctionFailed, "Setting locale to " + locale + " failed");

		char16_t char_buff{};
		std::mbstate_t state{};
		std::u16string string_buff{};

		const char* ptr = param.c_str();
		const char* end = param.c_str() + param.size() + 1; // + null character
		std::ptrdiff_t max_bytes = end - ptr;

		#if _DEBUG
		std::cout << "Processing " << param.size() << " bytes: [ " << std::showbase;
		for (const unsigned char ch : param)
			std::cout << std::hex << +ch << ' ';
		std::cout << "]" << std::endl;
		#endif

		// args: destination, source, max_bytes, state
		while (const std::size_t ret = std::mbrtoc16(&char_buff, ptr, end - ptr, &state))
		{
			#ifdef _DEBUG
			std::cout << "Next UTF-16 char: " << std::hex << static_cast<int>(char_buff) << " obtained from ";
			#endif

			switch (ret)
			{
			case static_cast<size_t>(-1):
				// MSDN: An encoding error has occurred
				// The next max_bytes or fewer bytes don't contribute to a complete and valid UTF-8 multibyte character
				// No value is stored in destination
				// EILSEQ is stored in errno and the conversion state value state is unspecified
				ShowError(ErrorCode::ConversionFailed, "Conversion to char32_t failed - Illegal byte sequence");
				return std::u16string();
			case static_cast<size_t>(-2):
				// MSDN: The next max_bytes bytes represent an incomplete, but potentially valid, UTF-8 multibyte character.
				// No value is stored in destination
				// This result can occur if max_bytes is zero.
				max_bytes = end - ptr;
				continue;
			case static_cast<size_t>(-3):
				// MSDN: The next wide character resulting from a previous call to the function has been stored in destination
				// No bytes from source are consumed by this call to the function
				#if _DEBUG
				// https://unicode.org/faq/utf_bom.html#utf16-2
				std::cout << "earlier surrogate pair" << std::endl;
				#endif

				max_bytes = end - ptr;
				string_buff.push_back(char_buff);
				continue;
			case 0:
				// The character converted from source (and stored in destination if non-null) was the null character
			default:
				// The number of bytes of the multibyte character converted from source
				#if _DEBUG
				std::cout << std::dec << ret << " bytes [ ";
				for (std::size_t bytes = 0; bytes < ret; ++bytes)
					std::cout << std::hex << +static_cast<unsigned char>(ptr[bytes]) << ' ';
				std::cout << "]" << std::endl;
				#endif

				ptr += ret;
				max_bytes = end - ptr;
				// Which also puts the null character
				string_buff.push_back(char_buff);
				continue;
			}
		}

		if (!old_locale.empty())
			if (std::setlocale(LC_ALL, old_locale.c_str()) == nullptr)
				ShowError(ErrorCode::FunctionFailed, "Restoring locale to " + old_locale + " failed");

		return string_buff;
	}

	std::u32string StringCast32(const std::string& param, const std::string locale)
	{
		if (locale.empty())
		{
			ShowError(ErrorCode::BadArgument, "locale cannot be empty string");
			return std::u32string();
		}

		// null pointer on failure
		std::string old_locale = std::setlocale(LC_ALL, locale.c_str());
		if (old_locale.empty())
			ShowError(ErrorCode::FunctionFailed, "Setting locale to " + locale + " failed");

		char32_t char_buff{};
		std::mbstate_t state{};
		std::u32string string_buff{};

		const char* ptr = param.c_str();
		const char* end = param.c_str() + param.size() + 1; // + null character
		std::ptrdiff_t max_bytes = end - ptr;

		#if _DEBUG
		std::cout << "Processing " << param.size() << " bytes: [ " << std::showbase;
		for (const unsigned char ch : param)
			std::cout << std::hex << +ch << ' ';
		std::cout << "]" << std::endl;
		#endif
		
		// args: destination, source, max_bytes, state
		while (const std::size_t ret = std::mbrtoc32(&char_buff, ptr, max_bytes, &state))
		{
			#ifdef _DEBUG
			std::cout << "Next UTF-32 char: " << std::hex << static_cast<int>(char_buff) << " obtained from ";
			#endif

			switch (ret)
			{
			case static_cast<size_t>(-1):
				// MSDN: An encoding error has occurred
				// The next max_bytes or fewer bytes don't contribute to a complete and valid UTF-8 multibyte character
				// No value is stored in destination
				// EILSEQ is stored in errno and the conversion state value state is unspecified
				ShowError(ErrorCode::ConversionFailed, "Conversion to char32_t failed - Illegal byte sequence");
				return std::u32string();
			case static_cast<size_t>(-2):
				// MSDN: The next max_bytes bytes represent an incomplete, but potentially valid, UTF-8 multibyte character.
				// No value is stored in destination
				// This result can occur if max_bytes is zero.
				max_bytes = end - ptr;
				continue;
			case static_cast<size_t>(-3):
				// MSDN: The next wide character resulting from a previous call to the function has been stored in destination
				// No bytes from source are consumed by this call to the function
				ShowError(ErrorCode::Unexpected, "Conversion to char32_t failed - Surrogates don't exist in UTF-32");
				return std::u32string();
			case 0:
				// The character converted from source (and stored in destination if non-null) was the null character
			default:
				// The number of bytes of the multibyte character converted from source
				#if _DEBUG
				std::cout << std::dec << ret << " bytes [ ";
				for (std::size_t bytes = 0; bytes < ret; ++bytes)
					std::cout << std::hex << +static_cast<unsigned char>(ptr[bytes]) << ' ';
				std::cout << "]" << std::endl;
				#endif

				ptr += ret;
				max_bytes = end - ptr;
				// Which also puts the null character
				string_buff.push_back(char_buff);
				continue;
			}
		}

		if (!old_locale.empty())
			if (std::setlocale(LC_ALL, old_locale.c_str()) == nullptr)
				ShowError(ErrorCode::FunctionFailed, "Restoring locale to " + old_locale + " failed");

		return string_buff;
	}

	#pragma warning (default: 28183)
}
