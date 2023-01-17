
/*
 *	Project: "masm-formatter" https://github.com/metablaster/masm-formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file mlfmt\FormatFile.hpp
 *
 * Source file function declarations
 *
*/

#pragma once
#include <regex>
#include <sstream>

/**
* Tab width expressed in spaces
*/
constexpr unsigned tab_width = 4;

/**
* Format nasm source file encoded as UTF-8, UTF-16 or UTF-16LE
*
* @param filedata	file contents loaded into memory
*/
void FormatFileW(std::wstringstream& filedata);

/**
* Format nasm source file encoded as ANSI
*
* @param filedata	file contents loaded into memory
*/
void FormatFileA(std::stringstream& filedata);
