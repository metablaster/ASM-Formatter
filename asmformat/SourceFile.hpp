
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
#include <vector>
#include <filesystem>
#include <type_traits>
#include "error.hpp"


/**
 * Source file encoding enum
 * Note that ccs=UNICODE is same as UTF-8, thus not used, see:
 * https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-s-wfopen-s
*/
enum class Encoding
{
	ANSI,
	UTF8,
	UTF16LE,
	Unknown,	// Unknown encoding
	Unsupported	// Unsupported encoding
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
 * @brief			Get Byte Order Mark from file if there is one
 * @param filepath	full path to file
 * @param bom		receives BOM enumeration
 * @return			BOM enumeration which also handles no BOM case
*/
[[nodiscard]] BOM GetBOM(const std::filesystem::path& filepath, std::vector<unsigned char>& bom);

/**
 * @brief			Get Byte Order Mark from string buffer if there is one
 * @param filepath	full path to file
 * @param bom		receives BOM enumeration
 * @return			BOM enumeration which also handles no BOM case
*/
[[nodiscard]] BOM GetBOM(const std::string& buffer, std::vector<unsigned char>& bom);

/**
 * @brief		Create BOM string
 * @param bom	Specify BOM which to create
 * @return		A string which which will receive the BOM string
*/
[[nodiscard]] std::vector<unsigned char> GetBOM(BOM bom);

/**
 * @brief Convert BOM enum to string
*/
[[nodiscard]] std::string BomToString(BOM bom);

/**
 * @brief			Convert encoding enum to string
 * @param encoding	Encoding enum
 * @return			String result
*/
[[nodiscard]] std::string EncodingToString(Encoding encoding);

/**
 * @brief		Associate BOM enum to Encoding enum
 * @param bom	BOM enum
 * @return		Encoding enum
*/
[[nodiscard]] Encoding BomToEncoding(BOM bom) noexcept;

/**
 * @brief	Get size of a file in bytes
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
 * @brief			Read source file into memory as byte stream
 * @param filepath	Full path and file name of a source file
 * @param bytes		Specify how many bytes to read, if 0 all bytes are read
 * @return			Source file contents as ANSI string
*/
[[nodiscard]] std::string LoadFileBytes(const std::filesystem::path& filepath, DWORD bytes = 0);

/**
 * @brief			Read source file into memory encoded as ANSI
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
 * @param append	Set to true to append data to file, by default file contents are replaced
*/
template<typename DataType>
requires std::is_same_v<std::vector<unsigned char>, DataType> || std::is_same_v<std::string, DataType>
void WriteFileBytes(const std::filesystem::path& filepath, const DataType& filedata, bool append)
{
	HANDLE hFile = CreateFileW(
		filepath.c_str(),
		// Append or overwrite
		append ? FILE_APPEND_DATA : GENERIC_WRITE,
		// Prevents subsequent open operations on a file or device if they request delete, read, or write access
		0ul,
		// Default security
		NULL,
		// If the specified file exists and is writable, the function overwrites the file,
		// the function succeeds, and last-error code is set to ERROR_ALREADY_EXISTS
		append ? OPEN_EXISTING :
		// Opens a file or device, only if it exists.
		// Otherwise the function fails and the last - error code is set to ERROR_FILE_NOT_FOUND
		CREATE_ALWAYS,
		// The file does not have other attributes set
		FILE_ATTRIBUTE_NORMAL,
		// No attributes template
		NULL
	);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		ShowError(ERROR_INFO_HR, ("Failed to open file " + filepath.string()).c_str());
		return;
	}

	if (!append)
	{
		// Remove ERROR_ALREADY_EXISTS
		SetLastError(0);
	}
	else if (GetLastError() == ERROR_FILE_NOT_FOUND)
	{
		ShowError(ERROR_INFO_HR, ("Failed to open file " + filepath.string()).c_str());
		return;
	}
	else
	{
		// MSDN: If the function succeeds and lpDistanceToMoveHigh is NULL, the return value is the low-order DWORD of the new file pointer
		// If the function returns a value other than INVALID_SET_FILE_POINTER, the call to SetFilePointer has succeeded
		const DWORD bytes_moved = SetFilePointer(hFile, 0L, nullptr, FILE_END);

		if (bytes_moved == INVALID_SET_FILE_POINTER)
		{
			ShowError(ERROR_INFO_HR, ("Failed to move file pointer to the end of file " + filepath.string()).c_str());
			return;
		}
	}

	DWORD bytes_written = 0;
	assert(std::numeric_limits<DWORD>::max() >= filedata.size());

	const BOOL status = WriteFile(
		hFile,
		filedata.data(),
		// The number of bytes to be written to the file
		static_cast<DWORD>(filedata.size()),
		// A pointer to the variable that receives the number of bytes written when using a synchronous hFile parameter
		&bytes_written,
		// A pointer to an OVERLAPPED structure is required if the hFile parameter was opened with FILE_FLAG_OVERLAPPED
		NULL
	);

	if (status == FALSE)
	{
		const DWORD error = GetLastError();

		if (CloseHandle(hFile) == 0)
		{
			ShowError(ERROR_INFO_HR, "Failed to close file");
		}

		SetLastError(error);
		ShowError(ERROR_INFO_HR, ("Failed to read file " + filepath.string()).c_str());

		return;
	}

	if (CloseHandle(hFile) == 0)
	{
		ShowError(ERROR_INFO_HR, "Failed to close file");
	}

	assert(bytes_written == static_cast<DWORD>(filedata.size()));
}

/**
 * Write formatted source file contents back to file encoded as ANSI
 * This function converts LF to CRLF
 *
 * @param filepath	Full path and file name of a source file
 * @param filedata	ANSI string contents which to write to file
*/
void WriteFileA(const std::filesystem::path& filepath, const std::string& filedata);
