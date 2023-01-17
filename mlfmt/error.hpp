
/*
 *	Project: "masm-formatter" https://github.com/metablaster/masm-formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file mlfmt\error.hpp
 *
 * Error handling function declarations
 *
*/

#pragma once
#include <memory>
#include <array>
#include <Windows.h>


/* error message buffer size used by ForamtMessage API **/
constexpr short msg_buff_size = 512;

/**
 * Returns pointer to formated wchar_t array error code
 * 
 * @param error_code Error code which to format
 * @param dwChars Number of TCHARs returned, excluding the terminating null character, 0 if failed.
*/
[[nodiscard]] const std::shared_ptr<std::array<char, msg_buff_size>>
FormatErrorMessage(const DWORD& error_code, DWORD& dwChars);
