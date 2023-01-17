
/*
 *	Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file asmformat\exception.cpp
 *
 * Definition of Exception class
 *
*/

#include "pch.hpp"
#include "exception.hpp"


namespace wsl
{
	Exception::~Exception() noexcept = default;

	Exception::Exception(const Exception& other) :
		runtime_error(other.what()),
		mCode(other.mCode),
		mInfo(other.mInfo)
	{
	}

	Exception::Exception(Exception&& other) noexcept(false) :
		runtime_error(other.what()),
		mCode(std::move(other.mCode)),
		mInfo(other.mInfo)
	{
	}

	std::string Exception::CategoryName() const
	{
		return mCode.category().name();
	}

	std::string Exception::ConditionName() const
	{
		return mCode.default_error_condition().message();
	}
}
