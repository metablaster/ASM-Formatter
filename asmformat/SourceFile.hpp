
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
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


/**
 * Source file encoding enum
 * Note that ccs=UNICODE is same as UTF-8, thus not used, see:
 * https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-s-wfopen-s
*/
enum class Encoding
{
	ANSI,
	UTF8,
	UTF16LE
};

/*
 * Byte Order Mark enum
 * Bytes		Encoding Form
 * 00 00 FE FF	UTF-32, big endian
 * FF FE 00 00	UTF-32, little endian
 * FE FF		UTF-16, big endian
 * FF FE		UTF-16, little endian
 * EF BB BF		UTF-8
*/
enum class BOM
{
	none,
	utf8,
	utf16le,
	utf16be,
	utf32le,
	utf32be
};

/**
 * @brief Get Byte Order Mark from file if there is one
 *
 * @param filepath	full path to file
 * @param bom		receives BOM enumeration
 * @return			BOM enumeration which also handles no BOM case
*/
[[nodiscard]] std::string GetBOM(const std::filesystem::path& filepath, BOM& bom);

/**
 * @brief Get Byte Order Mark from string buffer if there is one
 *
 * @param filepath	full path to file
 * @param bom		receives BOM enumeration
 * @return			BOM enumeration which also handles no BOM case
*/
[[nodiscard]] std::string GetBOM(const std::string& buffer, BOM& bom);

/**
 * @brief	Get size of a file in bytes
 * 
 * @param	filepath file path for which to get byte count
 * @return	Size of the file in bytes
*/
[[nodiscard]] std::size_t GetFileByteCount(const std::filesystem::path& filepath);

/**
 * Read source file into memory encoded as UTF-8, UTF-16 or UTF-16LE
 * If the source file contains BOM then encoding parameter is ignored
 *
 * @param filepath	Full path and file name of a source file
 * @param encoding	Specify predefined file encoding
 * @return			Source file contents as wide string
*/
[[nodiscard]] std::wstring LoadFileW(const std::filesystem::path& filepath, const Encoding& encoding);

/**
 * Read source file into memory as byte stream
 *
 * @param filepath	Full path and file name of a source file
 * @param bytes		Specify how many bytes to read, if 0 all bytes are read
 * @return			Source file contents as ANSI string
*/
[[nodiscard]] std::string LoadFileBytes(const std::filesystem::path& filepath, DWORD bytes = 0);

/**
 * Read source file into memory encoded as ANSI
 *
 * @param filepath	Full path and file name of a source file
 * @return			Source file contents as ANSI string
*/
[[nodiscard]] std::string LoadFileA(const std::filesystem::path& filepath);

/**
 * Write formatted source file contents back to file encoded as UTF-8, UTF-16 or UTF-16LE.
 * This function puts BOM into file if it doesn't exist for the specified encoding.
 * This function converts LF to CRLF
 *
 * @param filepath	Full path and file name of a source file
 * @param filedata	Wide string contents which to write to file
 * @param encoding	Specify predefined file encoding
*/
void WriteFileW(const std::filesystem::path& filepath, const std::wstring& filedata, const Encoding& encoding);

/**
 * Write formatted source file contents back to file as byte stream
 *
 * @param filepath	Full path and file name of a source file
 * @param filedata	ANSI string contents which to write to file
*/
void WriteFileBytes(const std::filesystem::path& filepath, const std::string& filedata);

/**
 * Write formatted source file contents back to file encoded as ANSI
 * This function converts LF to CRLF
 *
 * @param filepath	Full path and file name of a source file
 * @param filedata	ANSI string contents which to write to file
*/
void WriteFileA(const std::filesystem::path& filepath, const std::string& filedata);
