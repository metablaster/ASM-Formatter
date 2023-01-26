
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
*/

/**
 * @file asmformat\ErrorCode.hpp
 *
 * Declaration of custom error_code
 * For information about std error handling see comment in soruce file
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

		// Code error
		InvalidArgument = 100,	// Invalid argument passed to function
		InvalidPointer,		// Pointer is invalid
		NotImplemented,		// Not implemented
		OutOfRange,			// Out of range
		AlocationFailed,	// Failed to allocate memory
		MemoryReadFailed,	// Failed to read memory
		Unexpected,			// Unexpected execution flow
		PointerNotNULL,		// Attempt to re-initialize valid pointer
		CaseNotHandled,		// Case label not explicitly handled
		InvalidHandle,		// The handle is NULL or invalid
		FunctionFailed,		// Result of function call was not sucess
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
		ConversionFailed,	// Data conversion failed

		// User errors
		UnknownOption = 200,// Unrecognized option passed to program
		InvalidOptionArgument,	// Invalid program option argument specified
		InvalidCommand,		// Invalid command passed to program

		UnspecifiedError = 300	// The type of error is undefined or unknown
	};

	/** Exit status code from custom error enum */
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
	/**
	 * This function is called by error_code constructor for error code enum types,
	 * and should be overloaded for all custom error code enum types defined in order to
	 * provide a mechanism to generate the appropriate error_code objects from them.
	 *
	 * @param code	Custom error code
	 * @return		error_code object
	*/
	[[nodiscard]] std::error_code make_error_code(ErrorCode code) noexcept;
}
