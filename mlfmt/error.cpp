
/*
 *	Project: "masm-formatter" https://github.com/metablaster/masm-formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file mlfmt\error.cpp
 *
 * Error handling function definitions
 *
*/

#include "pch.hpp"
#include "error.hpp"


const std::shared_ptr<std::array<char, msg_buff_size>>
FormatErrorMessage(const DWORD& error_code, DWORD& dwChars)
{
	const std::shared_ptr<std::array<char, msg_buff_size>> message =
		std::make_shared<std::array<char, msg_buff_size>>();

	dwChars = FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,	// The location of the message definition.
		error_code,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		message.get()->data(),
		msg_buff_size,
		nullptr);

	return message;
}
