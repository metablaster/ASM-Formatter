
/*
 *	Project: "masm-formatter" https://github.com/metablaster/ASM-Formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file mlfmt\ErrorCondition.hpp
 *
 * Declaration of Error condition for error codes
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
		no_error,
		code_error,
		unknown_error
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
	//
	// This function is called by error_condition's constructor for error condition enum types,
	// and should be overloaded for all custom error condition enum types in order to
	// provide a mechanism to generate the appropriate error_condition objects from them.
	//
	[[nodiscard]] std::error_condition make_error_condition(ErrorCondition cond_err_value) noexcept;
}
