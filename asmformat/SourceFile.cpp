
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

std::string LoadFileBytes(const std::filesystem::path& filepath, std::size_t bytes)
{
	const std::size_t filesize = GetFileByteCount(filepath);
	const std::size_t file_bytes = bytes == 0 ? filesize : std::min(bytes, filesize);

	if (file_bytes == 0)
		return std::string();

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

	std::string buffer;
	buffer.resize(file_bytes);

	char* data = buffer.data();
	std::size_t size = buffer.size();
	std::size_t total_bytes_read = 0;

	while (size)
	{
		DWORD bytes_read = 0;
		// To prevent data overflow read up to size of DWORD
		const DWORD bytes_to_read = static_cast<DWORD>(std::min<std::size_t>(size, std::numeric_limits<DWORD>::max()));

		const BOOL status = ReadFile(
			hFile, data,
			// The maximum number of bytes to be read
			bytes_to_read,
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

			if (CloseHandle(hFile) == FALSE)
			{
				ShowError(ERROR_INFO_HR, ("Failed to close file " + filepath.string()).c_str());
			}

			SetLastError(error);
			ShowError(ERROR_INFO_HR, ("Failed to read file " + filepath.string()).c_str());
			return std::string();
		}

		// If no bytes are read then infinite loop
		if (bytes_read == 0)
			break;

		size -= bytes_read;
		data += bytes_read;
		total_bytes_read += bytes_read;
	}

	if (CloseHandle(hFile) == FALSE)
	{
		ShowError(ERROR_INFO_HR, ("Failed to close file" + filepath.string()).c_str());
	}

	assert(total_bytes_read == file_bytes);
	return buffer;
}
