
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
#include "ErrorCode.hpp"


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
 * @param filepath	string buffer containing file data
 * @param bom		receives BOM bytes
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
 * @tparam StringType	std::string or std::wstring depending on file encoding
 * @param filepath		Full path and file name of a source file
 * @param encoding		Specify predefined file encoding
 * @return				Source file contents as wide or ANSI string
*/
template<typename StringType>
requires std::is_same_v<std::string, StringType> || std::is_same_v<std::wstring, StringType>
[[nodiscard]] StringType LoadFile(const std::filesystem::path& filepath, const Encoding& encoding)
{
	// TODO: This function does not read \r in ANSI mode, question is what else does it not read?
	FILE* file = nullptr;
	StringType buffer{ };

	using namespace wsl;
	constexpr bool iswide = std::is_same_v<std::wstring, StringType>;

	// MSDN: Opens for reading.
	// If the file doesn't exist or can't be found, the fopen_s call fails.
	std::string mode = "r";

	switch (encoding)
	{
	case Encoding::UTF8:
		if (!iswide)
			goto bad_argument;
		mode += ", ccs=UTF-8";
		break;
	case Encoding::UTF16LE:
		if (!iswide)
			goto bad_argument;
		mode += ", ccs=UTF-16LE";
		break;
	case Encoding::ANSI:
		if (iswide)
			goto bad_argument;
		break;
	case Encoding::Unknown:
	case Encoding::Unsupported:
	default:
		ShowError(ErrorCode::UnsuportedOperation, "Encoding not supported by LoadFile");
		return StringType();
	}

	_set_errno(0);
	// MSDN: The fopen_s doesn't open a file for sharing
	// the byte order mark (BOM), if present in the file, determines the encoding.
	// The BOM encoding takes precedence over the encoding that's specified by the ccs flag.
	// The ccs encoding is only used when no BOM is present or if the file is a new file
	// Files that are opened for writing in Unicode mode have a BOM written to them automatically
	// Zero if successful, If an error occurs, the error code is returned, and the global variable errno is set
	if (fopen_s(&file, filepath.string().c_str(), mode.c_str()) == 0)
	{
		SUPPRESS(26496)	// mark it as const
		std::size_t filesize = GetFileByteCount(filepath);

		if (filesize > 0)
		{
			if constexpr (iswide)
				filesize /= 2;

			buffer.resize(filesize);

			// MSDN: The fread function reads up to count items of size bytes from the input stream
			// fread returns the number of full items the function read, which may be less than count if an error occurs,
			// or if it encounters the end of the file before reaching count
			const std::size_t wchars_read = fread(&(buffer.front()), sizeof(typename StringType::value_type), filesize, file);

			// MSDN: If no error has occurred on stream, ferror returns 0
			if (ferror(file) != 0)
			{
				ShowError(ErrorCode::BadResult, "Failed to read file " + filepath.string());
				return StringType();
			}

			buffer.resize(wchars_read);
			buffer.shrink_to_fit();
		}

		// MSDN: fclose returns 0 if the stream is successfully closed.
		if (fclose(file) != 0)
		{
			ShowError(ErrorCode::FunctionFailed, "Failed to close file " + filepath.string());
		}
	}
	else
	{
		ShowCrtError(Exception(ErrorCode::FunctionFailed, "Failed to open file " + filepath.string()), ERROR_INFO);
	}

	return buffer;

bad_argument:
	ShowError(ErrorCode::InvalidArgument, (std::string("Invalid combination of arguments: ") + typeid(typename StringType::value_type).name() + " and " + EncodingToString(encoding)).c_str());
	return StringType();
}

/**
 * @brief			Read source file into memory as byte stream
 * @param filepath	Full path and file name of a source file
 * @param bytes		Specify maximum bytes to read, if 0 all bytes are read
 * @return			Source file contents as ANSI string
*/
[[nodiscard]] std::string LoadFileBytes(const std::filesystem::path& filepath, std::size_t bytes = 0);

/**
 * Write formatted source file contents back to file encoded as ANSI, UTF-8, UTF-16 or UTF-16LE.
 * This function puts BOM into file if it doesn't exist for the specified UTF encoding.
 * This function converts LF to CRLF
 *
 * @tparam StringType	std::string or std::wstring
 * @param filepath		Full path and file name of a source file
 * @param filedata		Wide or ANSI string contents which to write to file
 * @param encoding		Specify predefined file encoding to use
*/
template<typename StringType>
requires std::is_same_v<std::string, StringType> || std::is_same_v<std::wstring, StringType>
void WriteFile(const std::filesystem::path& filepath, const StringType& filedata, const Encoding& encoding)
{
	FILE* file = nullptr;
	using namespace wsl;
	constexpr bool iswide = std::is_same_v<std::wstring, StringType>;

	// MSDN: Opens an empty file for writing.
	// If the given file exists, its contents are destroyed.
	std::string mode = "w";

	switch (encoding)
	{
	case Encoding::UTF8:
		if (!iswide)
			goto bad_argument;
		mode += ", ccs=UTF-8";
		break;
	case Encoding::UTF16LE:
		if (!iswide)
			goto bad_argument;
		mode += ", ccs=UTF-16LE";
		break;
	case Encoding::ANSI:
		if (iswide)
			goto bad_argument;
		break;
	case Encoding::Unknown:
	case Encoding::Unsupported:
	default:
		ShowError(ErrorCode::UnsuportedOperation, "Encoding not supported by WriteFile");
		return;
	}

	_set_errno(0);
	if (fopen_s(&file, filepath.string().c_str(), mode.c_str()) == 0)
	{
		// MSDN: fwrite returns the number of full items the function writes, which may be less than count if an error occurs
		// if an odd number of bytes to be written is specified in Unicode mode, the function invokes the invalid parameter handler
		// If execution is allowed to continue, this function sets errno to EINVAL and returns 0
		if (fwrite(filedata.c_str(), sizeof(typename StringType::value_type), filedata.size(), file) == 0)
		{
			ShowCrtError(Exception(ErrorCode::FunctionFailed, "Failed to write file " + filepath.string()), ERROR_INFO);
		}

		if (fclose(file) != 0)
		{
			ShowError(ErrorCode::FunctionFailed, "Failed to close file " + filepath.string());
		}
	}
	else
	{
		ShowCrtError(Exception(ErrorCode::FunctionFailed, "Failed to open file " + filepath.string()), ERROR_INFO);
	}

	return;

bad_argument:
	ShowError(ErrorCode::InvalidArgument, (std::string("Invalid combination of arguments: ") + typeid(typename StringType::value_type).name() + " and " + EncodingToString(encoding)).c_str());
	return;
}

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
	std::size_t size = filedata.size();
	if (size == 0)
		return;

	HANDLE hFile = CreateFileW(
		filepath.c_str(),
		// Append or overwrite
		append ? FILE_APPEND_DATA : GENERIC_WRITE,
		// Prevents subsequent open operations on a file or device if they request delete, read, or write access
		0ul,
		// Default security
		nullptr,
		// If the specified file exists and is writable, the function overwrites the file,
		// the function succeeds, and last-error code is set to ERROR_ALREADY_EXISTS
		append ? OPEN_EXISTING :
		// Opens a file or device, only if it exists.
		// Otherwise the function fails and the last - error code is set to ERROR_FILE_NOT_FOUND
		CREATE_ALWAYS,
		// The file does not have other attributes set
		FILE_ATTRIBUTE_NORMAL,
		// No attributes template
		nullptr
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
			const DWORD error = GetLastError();
			if (CloseHandle(hFile) == FALSE)
			{
				ShowError(ERROR_INFO_HR, ("Failed to close file " + filepath.string()).c_str());
			}

			SetLastError(error);
			ShowError(ERROR_INFO_HR, ("Failed to move file pointer to the end of file " + filepath.string()).c_str());
			return;
		}
	}

	auto data = filedata.data();
	std::size_t total_bytes_written = 0;

	while (size)
	{
		DWORD bytes_written = 0;
		// To prevent data overflow write up to size of DWORD
		const DWORD bytes_to_write = static_cast<DWORD>(std::min<std::size_t>(size, std::numeric_limits<DWORD>::max()));

		const BOOL status = WriteFile(
			hFile, data,
			// The number of bytes to be written to the file
			bytes_to_write,
			// A pointer to the variable that receives the number of bytes written when using a synchronous hFile parameter
			&bytes_written,
			// A pointer to an OVERLAPPED structure is required if the hFile parameter was opened with FILE_FLAG_OVERLAPPED
			nullptr
		);

		if (status == FALSE)
		{
			const DWORD error = GetLastError();
			if (CloseHandle(hFile) == FALSE)
			{
				ShowError(ERROR_INFO_HR, ("Failed to close file " + filepath.string()).c_str());
			}

			SetLastError(error);
			ShowError(ERROR_INFO_HR, ("Failed to read file " + filepath.string()).c_str());
			return;
		}

		// If no bytes are writen then infinite loop
		if (bytes_written == 0)
			break;

		size -= bytes_written;
		data += bytes_written;
		total_bytes_written += bytes_written;
	}

	if (CloseHandle(hFile) == FALSE)
	{
		ShowError(ERROR_INFO_HR, ("Failed to close file " + filepath.string()).c_str());
	}

	assert(total_bytes_written == filedata.size());
}
