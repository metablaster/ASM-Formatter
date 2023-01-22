
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
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
 * Line breaks used in file
*/
enum class LineBreak
{
	LF,
	CRLF,
	CR,
	Preserve	// Use existing line breaks
};

/**
 * @brief				Format asm source file encoded as UTF-8, UTF-16 or UTF-16LE
 * @param filedata		file contents loaded into memory
 * @Param tab_width		Count of spaces ocupying a tab character
 * @param spaces		Use spaces instead of tabs?
 * @param compact		Replace all surplus blank lines with single blank line
 * @param line_break	Specify line breaks kind
*/
void FormatFileW(std::wstringstream& filedata, std::size_t tab_width, bool spaces, bool compact, LineBreak line_break = LineBreak::Preserve);

/**
 * @brief				Format asm source file encoded as ANSI
 * @param filedata		file contents loaded into memory
 * @Param tab_width		Count of spaces ocupying a tab character
 * @param spaces		Use spaces instead of tabs?
 * @param compact		Replace all surplus blank lines with single blank line
 * @param line_break	Specify line breaks kind
*/
void FormatFileA(std::stringstream& filedata, std::size_t tab_width, bool spaces, bool compact, LineBreak line_break = LineBreak::Preserve);
