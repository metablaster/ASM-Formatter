
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
*/

/**
 * @file asmformat\StringCast.hpp
 *
 * String conversion function declarations
 *
 * The following quotes and links explain why std::setlocate() is significat for conversion:
 * https://learn.microsoft.com/en-us/cpp/c-runtime-library/interpretation-of-multibyte-character-sequences
 * Most multibyte-character routines in the Microsoft run-time library recognize multibyte-character sequences relating to a multibyte code page.
 * The output value is affected by the setting of the LC_CTYPE category setting of the locale
 * https://stackoverflow.com/questions/13045682/what-encoding-does-c32rtomb-convert-to
 * The multibyte encoding used by c32rtomb() is the multibyte encoding from the default "C" locale.
 *
 * See also code page values:
 * https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
 *
*/

#pragma once
#include <string>
#include <sstream>
#include <cuchar>
#include <type_traits>
#include <clocale>
#include <Windows.h>
#include "error.hpp"
#include "ErrorCode.hpp"


namespace wsl
{
	/**
	 * @brief			Function to convert from string to wstring
	 * @param param		String to convert
	 * @param codepage	Code page identifier for the code page to check
	*/
	[[nodiscard]] std::wstring StringCast(const std::string& param, UINT codepage = CP_UTF8);

	/**
	 * @brief			Function to convert from wstring to string
	 * @param param		string to convert
	 * @param codepage	Code page identifier for the code page to check
	*/
	[[nodiscard]] std::string StringCast(const std::wstring& param, UINT codepage = CP_UTF8);

	// 'setlocale(0, locale.c_str())' could be '0', and is a copy of the value found in 'setlocale()`252'
	PUSH DISABLE(28183)

	/**
	 * @brief				Converts a UTF string to its narrow multibyte representation
	 * @tparam StringType	One of the UTF versions of std::string types
	 * @param param			u32string, u16string string or char32_t, char16_t character
	 * @param locale		The multibyte encoding used is dictated by the locale
	 * @return				Converted string
	*/
	template<typename StringType>
	requires std::is_same_v<typename StringType::value_type, char8_t> ||
	std::is_same_v<typename StringType::value_type, char16_t> ||
	std::is_same_v<typename StringType::value_type, char32_t>
	[[nodiscard]] std::string StringCast(const StringType& param, const std::string locale = "en_US.utf8")
	{
		// null pointer on failure
		const char* const old_locale = std::setlocale(LC_CTYPE, locale.c_str());
		if (old_locale == nullptr)
			ShowError(ErrorCode::FunctionFailed, "Setting locale to " + locale + " failed");

		using char_type = typename StringType::value_type;
		std::size_t(*function)(char*, char_type, std::mbstate_t*) = nullptr;

		if constexpr (std::is_same_v<char_type, char16_t>)
			function = std::c16rtomb;
		else if constexpr (std::is_same_v<char_type, char32_t>)
			function = std::c32rtomb;

		static_assert(!std::is_same_v<char_type, char8_t>, "MSVC does not yet support std::c8rtomb");
		#if FALSE
		else if constexpr (std::is_same_v<char_type, char8_t>)
			func = std::c8rtomb;
		#endif

		std::mbstate_t state{ };
		std::stringstream string_buff{ };
		char char_buff[MB_LEN_MAX]{ };

		for (const char_type char_value : param)
		{
			// MSDN: The number of bytes stored in array object mbchar, including any shift sequences.
			// If char_value isn't a valid wide character, the value (size_t)(-1) is returned,
			// errno is set to EILSEQ, and the value of state is unspecified
			const std::size_t ret = function(char_buff, char_value, &state);

			if (ret == static_cast<std::size_t>(-1))
			{
				ShowError(ErrorCode::ConversionFailed, std::string("Conversion from ") + typeid(char_type).name() + " failed - Illegal byte sequence");
				return std::string();
			}
			else
			{
				string_buff << std::string_view{ char_buff, ret };
			}
		}

		string_buff << '\0';

		if (old_locale != nullptr)
			if (std::setlocale(LC_CTYPE, old_locale) == nullptr)
				ShowError(ErrorCode::FunctionFailed, std::string("Restoring locale to ") + old_locale + " failed");

		return string_buff.str();
	}

	POP

	/**
	 * @brief			Converts a narrow multibyte character to UTF-16 character representation
	 * @param param		string or char
	 * @param locale	The multibyte encoding used
	 * @return			Converted string
	*/
	[[nodiscard]] std::u16string StringCast16(const std::string& param, const std::string locale = "en_US.utf8");

	/**
	 * @brief			Converts a narrow multibyte character to its UTF-32 character representation
	 * @param param		string or char
	 * @param locale	The multibyte encoding used
	 * @return			Converted string
	*/
	[[nodiscard]] std::u32string StringCast32(const std::string& param, const std::string locale = "en_US.utf8");
}
