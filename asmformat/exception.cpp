
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
*/

/**
 * @file asmformat\exception.cpp
 *
 * Definition of custom exception class
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

	void Exception::AddInfo(std::string info)
	{
		if (!mInfo.empty())
			mInfo.append(", ");

		mInfo.append(info);
	}

	std::string Exception::ErrorMessage() const
	{
		// Calls error_category::message
		// Where error_category is implementation detail of error_code
		return mCode.message();
	}

	std::string Exception::ConditionMessage() const
	{
		// Calls error_category::message
		// Where error_category is implementation detail of error_condition
		return mCode.default_error_condition().message();
	}

	std::string Exception::ErrorCategoryName() const
	{
		// Calls error_category::name
		// Where error_category is implementation detail of error_code
		return mCode.category().name();
	}

	std::string Exception::ConditionCategoryName() const
	{
		// Calls error_category::name
		// Where error_category is implementation detail of error_condition
		return mCode.default_error_condition().category().name();
	}
}
