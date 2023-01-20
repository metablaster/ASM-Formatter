
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

		// TODO: flags for string conversion
		DWORD flags = MB_ERR_INVALID_CHARS;
		//flags |= MB_USEGLYPHCHARS;
		//flags |= MB_PRECOMPOSED;

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
			flags = 0;
			break;
		case 54936:
		case CP_UTF8:
			flags = MB_ERR_INVALID_CHARS; // or 0
			break;
		default:
			if ((code_page >= 57002) && (code_page <= 57011))
				flags = 0;
			break;
		}

		const int source_char_size = static_cast<int>(param.size());
		const int wchar_size_needed = MultiByteToWideChar(code_page, flags, param.c_str(), source_char_size, nullptr, 0);

		if (wchar_size_needed == 0)
		{
			ShowError(ERROR_INFO);
			return std::wstring();
		}

		std::wstring return_string(static_cast<const unsigned int>(wchar_size_needed), 0);

		if (!MultiByteToWideChar(code_page, flags, param.c_str(), source_char_size, &return_string[0], wchar_size_needed))
		{
			ShowError(ERROR_INFO);
			return std::wstring();
		}

		return return_string;
	}

	std::string StringCast(const std::wstring& param, UINT code_page)
	{
		if (param.empty())
			return std::string();

		DWORD flags = WC_ERR_INVALID_CHARS;
		//flags |= WC_COMPOSITECHECK;
		flags |= WC_NO_BEST_FIT_CHARS;

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
			flags = 0;
			break;
		case 54936:
		case CP_UTF8:
			flags = WC_ERR_INVALID_CHARS; // or 0
			break;
		default:
			if ((code_page >= 57002) && (code_page <= 57011))
				flags = 0;
			break;
		}

		const int source_wchar_size = static_cast<int>(param.size());
		const int char_size_needed = WideCharToMultiByte(code_page, flags, param.c_str(), source_wchar_size, nullptr, 0, nullptr, nullptr);

		if (char_size_needed == 0)
		{
			ShowError(ERROR_INFO);
			return std::string();
		}

		std::string return_string(static_cast<const unsigned int>(char_size_needed), 0);

		if (!WideCharToMultiByte(code_page, flags, param.c_str(), source_wchar_size, &return_string[0], char_size_needed, nullptr, nullptr))
		{
			ShowError(ERROR_INFO);
			return std::string();
		}

		return return_string;
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
