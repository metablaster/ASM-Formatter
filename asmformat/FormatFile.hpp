
/*
 *	Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file asmformat\FormatFile.hpp
 *
 * ASM source file formatting function declarations
 *
*/

#pragma once
#include <sstream>


/**
 * Format asm source file encoded as UTF-8, UTF-16 or UTF-16LE
 *
 * @param filedata	file contents loaded into memory
 * @Param tab_width	Count of spaces ocupying a tab character
 * @param spaces	Use spaces instead of tabs?
 * 
*/
void FormatFileW(std::wstringstream& filedata, unsigned tab_width, bool spaces);

/**
 * Format asm source file encoded as ANSI
 *
 * @param filedata	file contents loaded into memory
 * @Param tab_width	Count of spaces ocupying a tab character
 * @param spaces	Use spaces instead of tabs?
 *
*/
void FormatFileA(std::stringstream& filedata, unsigned tab_width, bool spaces);
