
/*
 *	Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file asmformat\StringCast.hpp
 *
 * String conversion function declarations
 *
*/

#pragma once
#include <string>
#include <Windows.h>


namespace wsl
{
	/**
	* Function to convert from string to wstring
	*
	* @param param	string to convert
	*/
	[[nodiscard]] std::wstring StringCast(const std::string& param, UINT code_page = CP_UTF8);

	/**
	* Function to convert from wstring to string
	*
	* @param param	string to convert
	*/
	[[nodiscard]] std::string StringCast(const std::wstring& param, UINT code_page = CP_UTF8);

	// TODO: waiting for C++ 20
	#if 0
	/**
	 * Convert u8string or char32_t to wstring
	 *
	 * @param param u8string or char8_t string
	 * @return String converted to wchar_t
	*/
	[[nodiscard]] std::wstring StringCast(const std::u8string& param);
	#endif

	/**
	 * Convert u16string or char16_t to wstring
	 *
	 * @param param u16string or char16_t string
	 * @return String converted to wchar_t
	*/
	[[nodiscard]] std::wstring StringCast(const std::u16string& param);

	/**
	 * Convert u32string or char32_t to wstring
	 *
	 * @param param u32string or char32_t string
	 * @return String converted to wchar_t
	*/
	[[nodiscard]] std::wstring StringCast(const std::u32string& param);
}
