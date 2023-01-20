
/*
 *	Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file asmformat\SourceFile.hpp
 *
 * File read\write function declarations
 *
*/

#pragma once
#include <filesystem>
#include <sstream>


/**
 * Source file encoding enum
*/
enum class Encoding
{
	ANSI,
	UTF8,
	UTF16,
	UTF16LE
};

/**
 * Get size of the file in bytes
 * @param file_path file path for which to get byte count
*/
[[nodiscard]] std::size_t GetFileBytesCount(const std::filesystem::path& file_path);

/**
 * Read MASM source file into memory encoded as UTF-8, UTF-16 or UTF-16LE
 *
 * @param file_path	Full path and file name of MASM source file
 * @param encoding	Specify file encoding
*/
[[nodiscard]] std::wstringstream LoadFileW(const std::filesystem::path& file_path, const Encoding& encoding);

/**
 * Read MASM source file into memory encoded as ANSI
 *
 * @param file_path	Full path and file name of MASM source file
*/
[[nodiscard]] std::stringstream LoadFileA(const std::filesystem::path& file_path);

/**
 * Write formatted MASM source file contents back to file encoded as UTF-8, UTF-16 or UTF-16LE
 *
 * @param file_path	Full path and file name of MASM source file
 * @param encoding	Specify file encoding
*/
void WriteFileW(const std::filesystem::path& file_path, const std::wstringstream& filedata, const Encoding& encoding);

/**
 * Write formatted MASM source file contents back to file encoded as ANSI
 *
 * @param file_path	Full path and file name of MASM source file
*/
void WriteFileA(const std::filesystem::path& file_path, const std::stringstream& filedata);
