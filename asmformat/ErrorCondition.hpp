
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
*/

/**
 * @file asmformat\ErrorCondition.hpp
 *
 * Declaration of custom error_condition
 *
*/

#pragma once
#include <cstdint>
#include <system_error>


namespace wsl
{
	/**
	 * Error condition codes enum
	*/
	enum class ErrorCondition : std::uint16_t
	{
		no_error,		// No error
		code_error,		// An error in code
		user_error,		// An error mady by user
		unspecified_error	// Unspecified error condition
	};
}

namespace std
{
	//
	// Register ErrorCondition as valid condition enum
	//
	template<>
	struct is_error_condition_enum<wsl::ErrorCondition> :
		public true_type
	{
	};
}

namespace wsl
{
	/**
	 * This function is called by error_condition constructor for error condition enum types,
	 * and should be overloaded for all custom error condition enum types in order to
	 * provide a mechanism to generate the appropriate error_condition objects from them.
	 * 
	 * @param condition	Custom error condition
	 * @return			error_condition object
	*/
	[[nodiscard]] std::error_condition make_error_condition(ErrorCondition condition) noexcept;
}
