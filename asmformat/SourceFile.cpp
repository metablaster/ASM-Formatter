
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
 *
*/

#include "pch.hpp"
#include "SourceFile.hpp"
using namespace wsl;


BOM GetBOM(const std::filesystem::path& filepath, std::vector<unsigned char>& bom)
{
	std::string buffer = LoadFileBytes(filepath, 4);
	buffer.shrink_to_fit();
	return GetBOM(buffer, bom);
}

std::vector<unsigned char> GetBOM(BOM bom)
{
	std::vector<unsigned char> result;

	switch (bom)
	{
	case BOM::utf8:
		result.push_back(0xEF);
		result.push_back(0xBB);
		result.push_back(0xBF);
		break;
	case BOM::utf16le:
		result.push_back(0xFF);
		result.push_back(0xFE);
		break;
	case BOM::utf16be:
		result.push_back(0xFE);
		result.push_back(0xFF);
		break;
	case BOM::utf32le:
		result.push_back(0xFF);
		result.push_back(0xFE);
		result.push_back(0x00);
		result.push_back(0x00);
		break;
	case BOM::utf32be:
		result.push_back(0x00);
		result.push_back(0x00);
		result.push_back(0xFE);
		result.push_back(0xFF);
		break;
	case BOM::none:
	default:
		break;
	}

	return result;
}

BOM GetBOM(const std::string& buffer, std::vector<unsigned char>& bom)
{
	BOM ebom = BOM::none;

	if (!buffer.empty())
	{
		const char ch1 = buffer.at(0);
		const char ch2 = buffer.at(1);
		const char ch3 = buffer.size() > 2 ? buffer.at(2) : '\0';
		const char ch4 = buffer.size() > 3 ? buffer.at(3) : '\0';

		if ((ch1 == '\xEF') && (ch2 == '\xBB') && (ch3 == '\xBF'))
		{
			ebom = BOM::utf8;
		}
		else if ((ch1 == '\xFF') && (ch2 == '\xFE'))
		{
			ebom = BOM::utf16le;
		}
		else if ((ch1 == '\xFE') && (ch2 == '\xFF'))
		{
			ebom = BOM::utf16be;
		}
		else if ((ch1 == '\xFF') && (ch2 == '\xFE') && (ch3 == '\x00') && (ch4 == '\x00'))
		{
			ebom = BOM::utf32le;
		}
		else if ((ch1 == '\x00') && (ch2 == '\x00') && (ch3 == '\xFE') && (ch4 == '\xFF'))
		{
			ebom = BOM::utf32be;
		}
		else
		{
			ebom = BOM::none;
		}
	}

	bom.clear();
	bom = GetBOM(ebom);
	bom.shrink_to_fit();
	return ebom;
}

std::string BomToString(BOM bom)
{
	switch (bom)
	{
	case BOM::utf8:
		return "UTF-8";
	case BOM::utf16le:
		return "UTF-16LE";
	case BOM::utf16be:
		return "UTF-16BE";
	case BOM::utf32le:
		return "UTF-32LE";
	case BOM::utf32be:
		return "UTF-32BE";
	case BOM::none:
	default:
		return "ANSI";
	}
}

std::string EncodingToString(Encoding encoding)
{
	switch (encoding)
	{
	case Encoding::UTF8:
		return "UTF-8";
	case Encoding::UTF16LE:
		return "UTF-16LE";
	case Encoding::ANSI:
	default:
		return "ANSI";
	}
}

Encoding BomToEncoding(BOM bom) noexcept
{
	switch (bom)
	{
	case BOM::utf8:
		return Encoding::UTF8;
	case BOM::utf16le:
		return Encoding::UTF16LE;
	case BOM::utf16be:
	case BOM::utf32le:
	case BOM::utf32be:
		return Encoding::Unsupported;
	case BOM::none:
	default:
		return Encoding::Unknown;
	}
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
	return static_cast<std::size_t>(fileinfo.st_size);
}

std::string LoadFileBytes(const std::filesystem::path& filepath, DWORD bytes)
{
	const std::size_t filesize = bytes == 0 ? GetFileByteCount(filepath) : bytes;
	assert(std::numeric_limits<DWORD>::max() >= filesize);

	if (filesize == 0)
		return std::string();

	// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew
	HANDLE hFile = CreateFileW(
		filepath.c_str(),
		// Read access
		GENERIC_READ,
		// Enables subsequent open operations on a file or device to request read access
		FILE_SHARE_READ,
		// Default security
		nullptr,
		// Opens a file or device, only if it exists
		OPEN_EXISTING,
		// The file does not have other attributes set
		FILE_ATTRIBUTE_NORMAL,
		// No attributes template
		nullptr);

	// MSDN: If the function fails, the return value is INVALID_HANDLE_VALUE
	if (hFile == INVALID_HANDLE_VALUE)
	{
		ShowError(ERROR_INFO_HR, ("Failed to open file " + filepath.string()).c_str());
		return std::string();
	}


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
		nullptr
	);

	// MSDN: If the function fails, or is completing asynchronously, the return value is zero (FALSE)
	// To get extended error information, call the GetLastError function
	if (status == FALSE)
	{
		const DWORD error = GetLastError();

		// MSDN: If the function fails, the return value is zero.
		if (CloseHandle(hFile) == FALSE)
		{
			ShowError(ERROR_INFO_HR, ("Failed to close file " + filepath.string()).c_str());
		}

		SetLastError(error);
		ShowError(ERROR_INFO_HR, ("Failed to read file " + filepath.string()).c_str());
		return std::string();
	}

	if (CloseHandle(hFile) == FALSE)
	{
		ShowError(ERROR_INFO_HR, ("Failed to close file" + filepath.string()).c_str());
	}

	// TODO: A solution needed to detect blank files,
	// GetFileByteCount may return size 4 when a file is blank
	//assert(bytes_read == filesize);
	return buffer;
}
