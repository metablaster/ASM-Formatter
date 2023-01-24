
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
 * NOTE: Portions of code in this file are not MIT but licensed under their own terms,
 * for more information see individual comments per function
 *
*/

/**
 * @file asmformat\utils.hpp
 *
 * Utility function declarations
 *
*/

#pragma once
#include <bit>
#include <utility>
#include <string>
#include <type_traits>


namespace wsl
{
	/**
	 * @brief Returns BOM (Byte Order Mark) for given char type
	 * Takes endianess into account
	 *
	 * @tparam CharType	One of the built in character types
	 * @return Byte Order Mark
	*/
	template<typename CharType>
	[[nodiscard]] CharType GetBom() noexcept
	{
		/*
		 * Bytes		Encoding Form
		 * 00 00 FE FF	UTF - 32, big - endian
		 * FF FE 00 00	UTF - 32, little - endian
		 * FE FF		UTF - 16, big - endian
		 * FF FE		UTF - 16, little - endian
		 * EF BB BF		UTF - 8
		*/

		// Byte order mark depends on endianess
		if constexpr (sizeof(CharType) == 16)
		{
			if constexpr (std::endian::native == std::endian::little)
				return static_cast<CharType>(0xFFFE);

			else return static_cast<CharType>(0xFEFF);
		}
		else if constexpr (sizeof(CharType) == 32)
		{
			if constexpr (std::endian::native == std::endian::little)
				return static_cast<CharType>(0xFFFE0000);

			else return static_cast<CharType>(0x0000FEFF);
		}
		else
		{
			return static_cast<CharType>(0xEFBBBF);
		}
	}

	/** Helper function to determine if client code is console or windows program */
	[[nodiscard]] bool IsConsole() noexcept;

	/**
	 * Used for template expansion by HasCodeMethod template
	 *
	 * AUTHOR:
	 * https://stackoverflow.com/a/16824239/12091999
	 * LICENSE:
	 * https://creativecommons.org/licenses/by-sa/3.0
	 * CHANGES:
	 * rename of variables and types
	*/
	template<typename, typename T>
	struct HasCodeMethod
	{
		static_assert(std::integral_constant<T, false>::value,
			"Second template parameter needs to be member function");
	};

	/**
	 * Type trait to check if a class has "code" method with given signature.
	 * Used to check if exception class has code method
	 *
	 * AUTHOR:
	 * https://stackoverflow.com/a/16824239/12091999
	 * LICENSE:
	 * https://creativecommons.org/licenses/by-sa/3.0
	 * CHANGES:
	 * rename of variables and types
	*/
	template<typename ClassType, typename FuncType, typename... FuncArgs>
	struct HasCodeMethod<ClassType, FuncType(FuncArgs...)>
	{
	private:
		template<typename T>
		static constexpr auto check(T*) -> typename std::is_same<
			decltype(std::declval<T>().code(std::declval<FuncArgs>() ...)), FuncType>::type;

		template<typename>
		static constexpr std::false_type check(...);

	public:
		using type = decltype(check<ClassType>(0));

		static constexpr bool value = type::value;
	};

	/**
	 * @brief			ANSI string replacement function
	 * @param source	String which process
	 * @param from		Replace what?
	 * @param to		Replacement
	*/
	void ReplaceAll(std::string& source, const std::string& from, const std::string& to);

	/**
	 * @brief			Wide string replacement function
	 * @param source	String which process
	 * @param from		Replace what?
	 * @param to		Replacement
	*/
	void ReplaceAll(std::wstring& source, const std::wstring& from, const std::wstring& to);
}
