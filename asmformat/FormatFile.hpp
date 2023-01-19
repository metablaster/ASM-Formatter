
/*
 *	Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file asmformat\FormatFile.hpp
 *
 * File formatting function declarations
 *
*/

#pragma once
#include <sstream>


/**
* Format asm source file encoded as UTF-8, UTF-16 or UTF-16LE
*
* @param filedata	file contents loaded into memory
*/
void FormatFileW(std::wstringstream& filedata, unsigned tab_width, bool spaces);

/**
* Format asm source file encoded as ANSI
*
* @param filedata	file contents loaded into memory
*/
void FormatFileA(std::stringstream& filedata, unsigned tab_width, bool spaces);
