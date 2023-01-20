
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
*/

/**
 * @file asmformat\SourceFile.cpp
 *
 * File read\write function definitions
 * TODO: Testing needed with better samples
 * TODO: CRT error handling is not well implemented, ex. override invalid parameter handler,
 * see docs for function used in this source
 *
*/

#include "pch.hpp"
#include "SourceFile.hpp"
#include "error.hpp"
#include "ErrorCode.hpp"
using namespace wsl;


std::string GetBOM(const std::filesystem::path& filepath, BOM& bom)
{
	std::string buffer = LoadFileBytes(filepath, 4);
	buffer.shrink_to_fit();
	return GetBOM(buffer, bom);
}

std::string GetBOM(const std::string& buffer, BOM& bom)
{
	std::string result;

	if (!buffer.empty())
	{
		const char ch1 = buffer.at(0);
		const char ch2 = buffer.at(1);
		const char ch3 = buffer.size() > 2 ? buffer.at(2) : '\0';
		const char ch4 = buffer.size() > 3 ? buffer.at(3) : '\0';

		if ((ch1 == '\xEF') && (ch2 == '\xBB') && (ch3 == '\xBF'))
		{
			// UTF-8
			bom = BOM::utf8;
			result = { '\xEF', '\xBB', '\xBF' };
		}
		else if ((ch1 == '\xFF') && (ch2 == '\xFE'))
		{
			// UTF-16, little endian
			bom = BOM::utf16le;
			result = { '\xFF', '\xFE' };
		}
		else if ((ch1 == '\xFE') && (ch2 == '\xFF'))
		{
			// UTF-16, big endian
			bom = BOM::utf16be;
			result = { '\xFE', '\xFF' };
		}
		else if ((ch1 == '\xFF') && (ch2 == '\xFE') && (ch3 == '\x00') && (ch4 == '\x00'))
		{
			// UTF-32, little endian
			bom = BOM::utf32le;
			result = { '\xFF', '\xFE', '\x00', '\x00' };
		}
		else if ((ch1 == '\x00') && (ch2 == '\x00') && (ch3 == '\xFE') && (ch4 == '\xFF'))
		{
			// UTF-32, big endian
			bom = BOM::utf32be;
			result = { '\x00', '\x00', '\xFE', '\xFF' };
		}
		else
		{
			bom = BOM::none;
		}

		result.shrink_to_fit();
	}

	return result;
}

std::size_t GetFileByteCount(const std::filesystem::path& filepath)
{
	struct _stat fileinfo{ 0 };
	// MSDN: Returns 0 if the file-status information is obtained
	int status = _stat(filepath.string().c_str(), &fileinfo);

	if (status != 0)
	{
		ShowError(ErrorCode::FunctionFailed, "Failed to get file size of " + filepath.string());
	}

	// MSDN: Size of the file in bytes
	// a 64-bit integer for variations with the i64 suffix
	return fileinfo.st_size;
}

std::wstring LoadFileW(const std::filesystem::path& filepath, const Encoding& encoding)
{
	FILE* file = NULL;
	std::wstring buffer;

	// MSDN: Opens for reading. If the file doesn't exist or can't be found, the fopen_s call fails.
	std::string mode = "r";

	switch (encoding)
	{
	case Encoding::UTF8:
		mode += ", ccs=UTF-8";
		break;
	case Encoding::UTF16LE:
		mode += ", ccs=UTF-16LE";
		break;
	default:
		ShowError(ErrorCode::UnsuportedOperation, "Encoding not supported by LoadFileW");
		return std::wstring();
	}

	_set_errno(0);
	// MSDN: The fopen_s doesn't open a file for sharing
	// the byte order mark (BOM), if present in the file, determines the encoding.
	// The BOM encoding takes precedence over the encoding that's specified by the ccs flag.
	// The ccs encoding is only used when no BOM is present or if the file is a new file
	// Files that are opened for writing in Unicode mode have a BOM written to them automatically
	const errno_t status = fopen_s(&file, filepath.string().c_str(), mode.c_str());

	if (status == 0)
	{
		const std::size_t filesize = GetFileByteCount(filepath);

		if (filesize > 0)
		{
			buffer.resize(filesize);

			// MSDN: The fread function reads up to count items of size bytes from the input stream
			// fread returns the number of full items the function read, which may be less than count if an error occurs,
			// or if it encounters the end of the file before reaching count
			// TODO: Use of sizeof(wchar_t) works, but is it correct?
			const std::size_t wchars_read = fread(&(buffer.front()), sizeof(wchar_t), filesize, file);
			buffer.resize(wchars_read);
			buffer.shrink_to_fit();
		}

		// MSDN: fclose returns 0 if the stream is successfully closed.
		if (fclose(file) != 0)
		{
			ShowError(ErrorCode::FunctionFailed, "Failed to close file" + filepath.string());
		}
	}
	else
	{
		ShowCrtError(Exception(ErrorCode::FunctionFailed, std::string("Failed to open file ") + filepath.string()), ERROR_INFO);
	}

	return buffer;
}

std::string LoadFileBytes(const std::filesystem::path& filepath, DWORD bytes)
{
	// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew
	HANDLE hFile = CreateFileW(
		filepath.c_str(),
		// Read access
		GENERIC_READ,
		// Enables subsequent open operations on a file or device to request read access
		FILE_SHARE_READ,
		// Default security
		NULL,
		// Opens a file or device, only if it exists
		OPEN_EXISTING,
		// The file does not have other attributes set
		FILE_ATTRIBUTE_NORMAL,
		// No attributes template
		NULL);

	// MSDN: If the function fails, the return value is INVALID_HANDLE_VALUE
	if (hFile == INVALID_HANDLE_VALUE)
	{
		ShowError(ERROR_INFO_HR, (std::string("Failed to open file ") + filepath.string()).c_str());
		return std::string();
	}

	const std::size_t filesize = bytes == 0 ? GetFileByteCount(filepath) : bytes;
	assert(std::numeric_limits<DWORD>::max() >= filesize);

	DWORD bytes_read = 0;
	std::string buffer;
	buffer.resize(filesize);

	// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile
	const BOOL status = ReadFile(
		hFile,
		&(buffer.front()),
		// The maximum number of bytes to be read
		static_cast<DWORD>(filesize),
		// A pointer to the variable that receives the number of bytes read when using a synchronous hFile parameter
		// ReadFile sets this value to zero before doing any work or error checking
		&bytes_read,
		// A pointer to an OVERLAPPED structure is required if the hFile parameter was opened with FILE_FLAG_OVERLAPPED
		NULL
	);

	// MSDN: If the function fails, or is completing asynchronously, the return value is zero (FALSE)
	// To get extended error information, call the GetLastError function
	if (status == FALSE)
	{
		const DWORD error = GetLastError();

		// MSDN: If the function fails, the return value is zero.
		if (CloseHandle(hFile) == 0)
		{
			ShowError(ERROR_INFO_HR, "Failed to close file");
		}

		SetLastError(error);
		ShowError(ERROR_INFO_HR, (std::string("Failed to read file ") + filepath.string()).c_str());
		return std::string();
	}

	if (CloseHandle(hFile) == 0)
	{
		ShowError(ERROR_INFO_HR, "Failed to close file");
	}

	assert(bytes_read == filesize);
	return buffer;
}

std::string LoadFileA(const std::filesystem::path& filepath)
{
	FILE* file = NULL;
	std::string buffer;

	_set_errno(0);
	const errno_t status = fopen_s(&file, filepath.string().c_str(), "r");

	if (status == 0)
	{
		const std::size_t filesize = GetFileByteCount(filepath);

		if (filesize > 0)
		{
			buffer.resize(filesize);
			const std::size_t chars_read = fread(&(buffer.front()), sizeof(char), filesize, file);

			buffer.resize(chars_read);
			buffer.shrink_to_fit();
		}

		if (fclose(file) != 0)
		{
			ShowError(ErrorCode::FunctionFailed, "Failed to close file" + filepath.string());
		}
	}
	else
	{
		ShowCrtError(Exception(ErrorCode::FunctionFailed, std::string("Failed to open file ") + filepath.string()), ERROR_INFO);
	}

	return buffer;
}

void WriteFileW(const std::filesystem::path& filepath, const std::wstring& filedata, const Encoding& encoding)
{
	FILE* file = NULL;

	// MSDN: Opens an empty file for writing. If the given file exists, its contents are destroyed.
	std::string mode = "w";

	switch (encoding)
	{
	case Encoding::UTF8:
		mode += ", ccs=UTF-8";
		break;
	case Encoding::UTF16LE:
		mode += ", ccs=UTF-16LE";
		break;
	default:
		ShowError(ErrorCode::UnsuportedOperation, "Encoding not supported by WriteFileW");
		return;
	}

	_set_errno(0);
	const errno_t status = fopen_s(&file, filepath.string().c_str(), mode.c_str());

	if (status == 0)
	{
		// MSDN: fwrite returns the number of full items the function writes, which may be less than count if an error occurs
		fwrite(filedata.c_str(), sizeof(wchar_t), filedata.size(), file);

		if (fclose(file) != 0)
		{
			ShowError(ErrorCode::FunctionFailed, "Failed to close file" + filepath.string());
		}
	}
	else
	{
		ShowCrtError(Exception(ErrorCode::FunctionFailed, std::string("Failed to write file ") + filepath.string()), ERROR_INFO);
	}
}

void WriteFileBytes(const std::filesystem::path& filepath, const std::string& filedata)
{
	HANDLE hFile = CreateFileW(
		filepath.c_str(),
		// Write access
		GENERIC_WRITE,
		// Prevents subsequent open operations on a file or device if they request delete, read, or write access
		0,
		// Default security
		NULL,
		// If the specified file exists and is writable, the function overwrites the file,
		// the function succeeds, and last-error code is set to ERROR_ALREADY_EXISTS
		CREATE_ALWAYS,
		// The file does not have other attributes set
		FILE_ATTRIBUTE_NORMAL,
		// No attributes template
		NULL
	);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		ShowError(ERROR_INFO_HR, (std::string("Failed to open file ") + filepath.string()).c_str());
		return;
	}

	// Remove ERROR_ALREADY_EXISTS
	SetLastError(0);
	DWORD bytes_written;

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

		// MSDN: If the function fails, the return value is zero.
		if (CloseHandle(hFile) == 0)
		{
			ShowError(ERROR_INFO_HR, "Failed to close file");
		}

		SetLastError(error);
		ShowError(ERROR_INFO_HR, (std::string("Failed to read file ") + filepath.string()).c_str());
	}

	if (CloseHandle(hFile) == 0)
	{
		ShowError(ERROR_INFO_HR, "Failed to close file");
	}

	assert(bytes_written == static_cast<DWORD>(filedata.size()));
}

void WriteFileA(const std::filesystem::path& filepath, const std::string& filedata)
{
	FILE* file = NULL;

	_set_errno(0);
	const errno_t status = fopen_s(&file, filepath.string().c_str(), "w");

	if (status == 0)
	{
		fwrite(filedata.c_str(), sizeof(char), filedata.size(), file);

		if (fclose(file) != 0)
		{
			ShowError(ErrorCode::FunctionFailed, "Failed to close file" + filepath.string());
		}
	}
	else
	{
		ShowCrtError(Exception(ErrorCode::FunctionFailed, std::string("Failed to write file ") + filepath.string()), ERROR_INFO);
	}
}
