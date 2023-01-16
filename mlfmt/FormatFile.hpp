
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


void FormatFileW(std::wstringstream& filedata);

void FormatFileA(std::stringstream& filedata);
