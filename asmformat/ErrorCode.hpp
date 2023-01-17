
/*
 *	Project: "masm-formatter" https://github.com/metablaster/ASM-Formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file mlfmt\ErrorCode.hpp
 *
 * Declaration of code error code
 *
*/

#pragma once
#include <cstdint>
#include <system_error>


namespace wsl
{
	/**
	* Generic error codes enum
	*/
	enum class ErrorCode : std::uint16_t
	{
		Success = 0,		// No error
		BadArgument = 100,	// bad argument passed to function
		InvalidPointer,		// Pointer is invalid
		NoImplementation,	// No implementation
		OutOfRange,			// Out of range
		AlocationFailed,	// Failed to allocate memory
		MemoryReadFailed,	// Failed to read memory
		Unexpected,			// Unexpected execution flow
		PointerNotNULL,		// Attempt to re-initialize valid pointer
		CaseNotHandled,		// Case label not explicitly handled
		InvalidHandle,		// The handle is NULL or invalid
		FunctionFailed,		// Function call failed
		MemoryLeak,			// Possible memory leak
		BadFunctionCall,	// Function should not be called
		ObjectNotValid,		// Object is not valid
		InitializationFailed,	// Initialization failed
		InvalidFlags,		// Invalid bitwise flags
		BadResult,			// result is most likely wrong
		RunTimeLibraryError,// CRT Error
		BadDelete,			// attempt to delete invalid memory
		ExceptionInDestructor,	// Destructor threw an exception
		UnhandledException,	// Exception handler is missing
		ResourceLeak,		// resource leaked
		ParseFailure,		// parsing error
		UnsuportedOperation,	// Operation not supported
		BadVariableData,	// unexpected variable data
		BadCast,			// Bad cast
		NotFound,			// Item not found
		UnknownError = 1000	// the type of error is not known
	};

	/** Exit status code from error enum */
	[[nodiscard]] int ExitCode(ErrorCode err_enum) noexcept;		// can't be inlined
}

namespace std
{
	//
	// Register ErrorCode as valid error enum
	//
	template<>
	struct is_error_code_enum<wsl::ErrorCode> :
		public true_type
	{
	};
}

namespace wsl
{
	//
	// This function is called by error_code's constructor for error code enum types,
	// and should be overloaded for all custom error code enum types defined in order to
	// provide a mechanism to generate the appropriate error_code objects from them.
	//
	[[nodiscard]] std::error_code make_error_code(ErrorCode err_code_value) noexcept;
}
