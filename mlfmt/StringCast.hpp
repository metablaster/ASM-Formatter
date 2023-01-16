
/*
 *	Project: "masm-formatter" https://github.com/metablaster/masm-formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file mlfmt\StringCast.hpp
 *
 * String conversion function declarations
 *
*/

#pragma once
#include <string>
#include <Windows.h>


/**
* Helper function to convert from string to wstring
*
* @param param	string to convert
*/
[[nodiscard]] std::wstring StringCast(const std::string& param, UINT code_page = CP_UTF8);

/**
* Helper function to convert from wstring to string
*
* @param param	string to convert
*/
[[nodiscard]] std::string StringCast(const std::wstring& param, UINT code_page = CP_UTF8);

#if 0	// UNDONE: waiting for C++ 20
/**
 * @brief Convert u8string or char32_t to wstring
 * @param param u8string or char8_t string
 * @return String converted to wchar_t
*/
[[nodiscard]] std::wstring StringCast(const std::u8string& param);
#endif

/**
 * @brief Convert u16string or char16_t to wstring
 * @param param u16string or char16_t string
 * @return String converted to wchar_t
*/
[[nodiscard]] std::wstring StringCast(const std::u16string& param);

/**
 * @brief Convert u32string or char32_t to wstring
 * @param param u32string or char32_t string
 * @return String converted to wchar_t
*/
[[nodiscard]] std::wstring StringCast(const std::u32string& param);
