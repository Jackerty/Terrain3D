// UtilityFunctions does not exist in the engine. Fix this situation is variant utility
// is used.
#ifndef UTILITY_FUNCTIONS
#define UTILITY_FUNCTIONS

#include <array>
#include "core/variant/variant.h"
#include "core/variant/variant_utility.h"

namespace UtilityFunctions
{
	// Packs the Variants to std::array for automatic memory clear.
	template <typename... Args>
	static std::array<const Variant *, 1 + sizeof...(Args)> _pack_variant(const Variant &p_arg1, const Args &...p_args){
		std::array<Variant, sizeof...(Args)> arg_alias {Variant(p_args)... };
		std::array<const Variant *, 1 + sizeof...(Args)> args;
		args[0] = &p_arg1;
		for(size_t i = 0; i < arg_alias.size(); i++){
			args[i + 1] = &arg_alias[i];
		}

		return args;
	}

	template <typename... Args>
	static void push_error(const Variant &p_arg1, const Args &...p_args) {
		Callable::CallError error;
		std::array arr = _pack_variant(p_arg1, p_args...);
		VariantUtilityFunctions::push_error(arr.data(), arr.size(), error);
		DEV_ASSERT(error.error == Callable::CallError::Error::CALL_OK);
	}

	template <typename... Args>
	static void push_warning(const Variant &p_arg1, const Args &...p_args) {
		Callable::CallError error;
		std::array arr = _pack_variant(p_arg1, p_args...);
		VariantUtilityFunctions::push_warning(arr.data(), arr.size(), error);
		DEV_ASSERT(error.error == Callable::CallError::Error::CALL_OK);
	}

	template <typename... Args>
	static void print(const Variant &p_arg1, const Args &...p_args) {
		Callable::CallError error;
		std::array arr = _pack_variant(p_arg1, p_args...);
		VariantUtilityFunctions::print(arr.data(), arr.size(), error);
		DEV_ASSERT(error.error == Callable::CallError::Error::CALL_OK);
	}

	template <typename... Args>
	static String error_string(Error error) {
		return VariantUtilityFunctions::error_string(error);
	}

	static inline int64_t floori(real_t x){
		return VariantUtilityFunctions::floori(x);
	}

	static inline real_t randf(){
		return VariantUtilityFunctions::randf();
	}

	static inline real_t posmod(int64_t x, int64_t y){
		return VariantUtilityFunctions::posmod(x, y);
	}
}

#endif /* UTILITY_FUNCTIONS */