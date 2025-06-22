#pragma once

extern "C" {
	#include "lua/lua.h"
	#include "lua/lualib.h"
	#include "lua/lauxlib.h"
};

#include <string>
#include <optional>
#include <type_traits>
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include <tuple>

#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace Helper
{

namespace LuaImpl
{
struct is_vector_impl
{
	template<typename... T> std::false_type operator() (T... t);
	template<typename T> std::true_type operator() (std::vector<T>&& value);
};
template<typename T> using is_vector = decltype(is_vector_impl{} (std::declval<T>()));
template<typename T> constexpr bool is_vector_v = is_vector<T>::value;

struct is_map_impl
{
	template<typename... T> std::false_type operator() (T... t);
	template<typename K, typename T> std::true_type operator() (std::map<K, T>&& value);
};
template<typename T> using is_map = decltype(is_map_impl{} (std::declval<T>()));
template<typename T> constexpr bool is_map_v = is_map<T>::value;

template<typename T> using wrap_optional = std::conditional<std::is_void_v<T>, void, std::optional<T>>;
template<typename T> using wrap_optional_t = typename wrap_optional<T>::type;
template<typename T> using dummy_if_void = std::conditional<std::is_void_v<T>, int, std::optional<T>>;
template<typename T> using dummy_if_void_t = typename dummy_if_void<T>::type;

template<typename Value> bool LuaPush(lua_State* luaState, const Value& argument)
{
	if (!lua_checkstack(luaState, 1u)) return false;

	if constexpr (std::is_floating_point_v<Value>)
	{
		// Pushes single float value onto the stack
		lua_pushnumber(luaState, argument);
	}
	else if constexpr (std::is_integral_v<Value>)
	{
		// Pushes single integer value onto the stack
		lua_pushinteger(luaState, argument);
	}
	else if constexpr (std::is_same_v<Value, std::string>)
	{
		// Pushes single value onto the stack (the string is copied)
		lua_pushstring(luaState, argument.c_str());
	}
	else if constexpr (std::is_same_v<std::decay_t<Value>, char*>)
	{
		// Pushes single value onto the stack (the string is copied)
		lua_pushstring(luaState, argument);
	}
	else if constexpr (is_vector_v<Value>)
	{
		if (!lua_checkstack(luaState, 2u)) return false;

		// Creates a new table (array) on the stack
		// and then adds elements with integer indices.
		lua_createtable(luaState, argument.size(), 0u);
		for (size_t index { 0u }; index < argument.size(); index++)
		{
			if (!LuaPush(luaState, argument[index])) return false;
			lua_seti(luaState, -2, (index + 1u));
		}
	}
	else if constexpr (is_map_v<Value>)
	{
		if (!lua_checkstack(luaState, 3u)) return false;

		// Creates a new table (dictionary) on the stack
		// and then adds key-value pairs.
		lua_createtable(luaState, 0u, argument.size());
		for (auto &[key, value] : argument)
		{
			if (!LuaPush(luaState, key)) return false;
			if (!LuaPush(luaState, value)) return false;
			lua_settable(luaState, -3);
		}
	}
	else
	{
		static_assert(std::is_same_v<Value, std::string>, "Unsupported type");
	}

	return true;
}

template<typename Value> std::optional<Value> LuaPop(lua_State* luaState)
{
	std::optional<Value> result { std::nullopt };
	int succeed = 0;

	if constexpr (std::is_floating_point_v<Value>)
	{
		// Tries to convert the topmost element to a float value.
		auto ret = lua_tonumberx(luaState, -1, &succeed);
		if (succeed) result = { ret };
		LOG_IF(!succeed, ERROR) << "Lua error: Could not pop float value";
		goto exit_pop;
	}
	else if constexpr (std::is_integral_v<Value>)
	{
		// Tries to convert the topmost element to an integer value.
		auto ret = lua_tointegerx(luaState, -1, &succeed);
		if (succeed) result = { ret };
		LOG_IF(!succeed, ERROR) << "Lua error: Could not pop integer value";
		goto exit_pop;
	}
	else if constexpr (std::is_same_v<Value, std::string>)
	{
		// Tries to convert the topmost element to an string value.
		auto ret = lua_tostring(luaState, -1);
		if (ret) result = { std::string(ret) };
		LOG_IF(!ret, ERROR) << "Lua error: Could not pop string value";
		goto exit_pop;
	}
	else if constexpr (is_vector_v<Value>)
	{
		if (!lua_istable(luaState, -1)) goto exit_pop;

		// We need one space on the stack for iteration of arrays.
		if (!lua_checkstack(luaState, 1u)) goto exit_pop;

		// Tries to query the length of the table (array).
		// The length will be pushed to the stack.
		lua_len(luaState, -1);

		{
			Value vector;

			// The length is the top element on the stack, so pop it.
			auto length = LuaPop<size_t>(luaState);
			if (!length.has_value()) goto exit_pop;

			vector.reserve(length.value());
			for (size_t index { 0u }; index < length.value(); index++)
			{
				// Push the next value of the table (array) onto the stack.
				lua_geti(luaState, -1, (index + 1u));

				// The value is the top element on the stack, so pop it.
				auto value = LuaPop<typename Value::value_type>(luaState);
				if (!value.has_value()) goto exit_pop;
				vector.push_back(value.value());
			}

			result = { vector };
		}
	}
	else if constexpr (is_map_v<Value>)
	{
		if (!lua_istable(luaState, -1)) goto exit_pop;

		// We need three spaces on the stack for iteration of dictionaries.
		// One for key and value each, and another one for a copy of the key
		// for conversion operations to not break the iteration function lua_next.
		if (!lua_checkstack(luaState, 3u)) goto exit_pop;

		// Push the start key (nil) for the iteration onto the stack.
		lua_pushnil(luaState);

		{
			Value map;

			// As long as there are more key-value pairs it returns true.
			// If there is another key-value pair it pushes the key-value pair
			// as two values onto the stack.
			while (lua_next(luaState, -2) != 0)
			{
				// Clone the key because lua_tostring might modify it.
				// And we don't want to break the lua_next function.
				// Stack layout before: key (-2) value (-1)
				// Stack layout after: key (-3) value (-2) key (-1)
				lua_pushvalue(luaState, -2);

				// The key is the top element on the stack, so pop it.
				// Stack layout before: key (-3) value (-2) key (-1)
				// Stack layout after: key (-2) value (-1)
				auto key = LuaPop<typename Value::key_type>(luaState);
				if (!key.has_value())
				{
					lua_pop(luaState, 2u); // Pop the remaining key and value pair
					goto exit_pop;
				}

				// The value is the top element on the stack, so pop it.
				// Stack layout before: key (-2) value (-1)
				// Stack layout after: key (-1) (for iteration)
				auto value = LuaPop<typename Value::mapped_type>(luaState);
				if (!value.has_value())
				{
					lua_pop(luaState, 1u); // Pop the remaining key
					goto exit_pop;
				}

				map[key.value()] = value.value();
			}

			result = { map };
		}
	}
	else if (std::is_same_v<Value, void>)
	{
		return std::nullopt;
	}
	else
	{
		static_assert(std::is_same_v<Value, std::string>, "Unsupported type");
	}

exit_pop:
	lua_pop(luaState, 1u);
	return result;
};

template<typename Value> bool LuaPop(lua_State* luaState, std::optional<Value>& result)
{
	result = { LuaImpl::LuaPop<Value>(luaState) };
	return result.has_value();
}

template<typename... Values> bool LuaPushAll([[maybe_unused]] lua_State* luaState, const Values&... values)
{
	return (LuaImpl::LuaPush<Values>(luaState, values) && ...);
}

template<typename... Values> bool LuaPopAll(lua_State* luaState, std::optional<std::tuple<Values...>>& result)
{
	// Temporary storage for all the results.
	std::tuple<std::optional<Values>...> results;

	// Pop each element until either all arguments have been
	// popped or a pop operation fails.
	if (!std::apply([&luaState](auto&... values) {
			return (LuaImpl::LuaPop<Values>(luaState, values) && ...);
		}, results))
	{
		return false;
	}

	// Combine all the popped results into a single tuple.
	result = std::apply([](auto&... values) -> std::tuple<Values...> {
		return { values.value()... };
	}, results);
	return true;
}

};

class Lua
{
public:
	Lua(void):
		luaState(),
		luaCallbacks(),
		mutex()
	{
		luaState = { luaL_newstate(), [](lua_State* state) { lua_close(state); } };
		if (!luaState)
		{
			LOG(ERROR) << "Lua error: Can not create new state";
			return;
		}

		// Initialize all the libraries
		luaL_openlibs(luaState.get());
	}

	virtual ~Lua(void) = default;

	bool LoadCode(const std::string& script, const std::string& name = "main")
	{
		if (!luaState)
		{
			LOG(ERROR) << "Lua error: Was not correctly initialized";
			return false;
		}

		std::lock_guard guard { mutex };
		// Load the lua code into the current lua state.
		// If the code defines functions then they will be available
		// in the globals table of lua afterwards.
		if (luaL_dostring(luaState.get(), script.c_str()) != LUA_OK)
		{
			PrintError(luaState.get());
			return false;
		}

		return true;
	}

	template<typename Argument>
	void SetGlobal(const std::string& name, const Argument& value)
	{
		if (!luaState)
		{
			LOG(ERROR) << "Lua error: Was not correctly initialized";
			return;
		}

		std::lock_guard guard { mutex };
		if (!lua_checkstack(luaState.get(), 1u)) return;
		if (!LuaImpl::LuaPush(luaState.get(), value)) return;
		lua_setglobal(luaState.get(), name.c_str());
	}

	template<typename Return, typename... Arguments>
	void SetCallback(const std::string& name, std::function<Return(Arguments...)> callback)
	{
		std::lock_guard guard { mutex };
		if (!lua_checkstack(luaState.get(), 3u)) return;
		lua_pushlightuserdata(luaState.get(), this);
		lua_pushstring(luaState.get(), name.c_str());
		lua_pushcclosure(luaState.get(), Lua::InvokeCallback, 2u);
		lua_setglobal(luaState.get(), name.c_str());

		luaCallbacks[name] = [name, callback](lua_State* state) -> int {
			// The stack holds the arguments right now.
			if (lua_gettop(state) < static_cast<int>(sizeof...(Arguments)))
			{
				luaL_error(state,
					"Callback %s has not recevied enough arguments. Got %d but expected %d.",
					name.c_str(), lua_gettop(state), sizeof...(Arguments));
				__builtin_unreachable();
			}

			// Pop the function arguments from the stack.
			std::optional<std::tuple<Arguments...>> arguments;
			if (!LuaImpl::LuaPopAll(state, arguments))
			{
				luaL_error(state, "Callback %s has not recevied expected argument types.", name.c_str());
				__builtin_unreachable();
			}

			if constexpr (std::is_void_v<Return>)
			{
				// Invoke callback and return number of arguments.
				std::apply(callback, arguments.value());
				return 0;
			}
			else
			{
				// Invoke callback and push result and return number of arguments.
				auto result = std::apply(callback, arguments.value());
				LuaImpl::LuaPush<Return>(state, result);
				return 1;
			}
		};
	}

	template<typename Return, typename... Arguments>
	LuaImpl::wrap_optional_t<Return> Invoke(const std::string& function, const Arguments&... arguments)
	{
		LuaImpl::dummy_if_void_t<Return> result { };
		lua_State* threadState { nullptr };

		std::lock_guard guard { mutex };
		if (!luaState)
		{
			LOG(ERROR) << "Lua error: Was not correctly initialized";
			goto exit_return;
		}

		if (!lua_checkstack(luaState.get(), 1u))
		{
			LOG(ERROR) << "Lua error: Can not push new thread to stack";
			goto exit_return;
		}

		// Lua threads are garbage collected and don't need to be cleaned up manually.
		threadState = lua_newthread(luaState.get());
		if (!threadState)
		{
			LOG(ERROR) << "Lua error: Was not correctly initialized";
			goto exit_return;
		}

		// Load the function from the global variables onto the stack
		if (!lua_checkstack(threadState, 1u))
		{
			LOG(ERROR) << "Lua error: Can not push function to stack";
			goto exit_cleanup;
		}
		lua_getglobal(threadState, function.c_str());

		// Push function arguments onto the stack and check for errors.
		if(!LuaImpl::LuaPushAll<Arguments...>(threadState, arguments...))
		{
			LOG(ERROR) << "Lua error: Can not push arguments to stack";
			goto exit_cleanup;
		}

		// Call the function on the stack
		switch (lua_pcall(threadState, sizeof...(arguments), 1, 0))
		{
			case LUA_OK:
				if constexpr (!std::is_void_v<Return>)
				{
					if (lua_gettop(threadState) < 1)
					{
						LOG(ERROR) << "Lua error: Could not retrieve return value";
						goto exit_cleanup;
					}

					// Get the return value from the stack
					result = LuaImpl::LuaPop<Return>(threadState);
					if (!result.has_value())
					{
						LOG(ERROR) << "Lua error: Could not retrieve return value";
					}
				}
				goto exit_cleanup;

			case LUA_ERRSYNTAX:
			case LUA_ERRRUN:
			case LUA_ERRMEM:
			case LUA_ERRERR:
			default:
				PrintError(threadState);
				goto exit_cleanup;
		}

	exit_cleanup:
		// Remove the thread from the stack.
		lua_pop(luaState.get(), 1u);

	exit_return:
		if constexpr (!std::is_void_v<Return>)
		{
			return result;
		}
	}

private:
	std::unique_ptr<lua_State, std::function<void(lua_State*)>> luaState;
	std::map<std::string, std::function<int(lua_State*)>> luaCallbacks;
	std::mutex mutex;

	void PrintError(lua_State* luaState)
	{
		if (lua_gettop(luaState) >= 1)
		{
			auto message = lua_tostring(luaState, -1);
			LOG(ERROR) << "Lua error: " << (message ? message : "Could not convert error message");
		}
		else
		{
			LOG(ERROR) << "Lua error: Could not retrieve error message";
		}
	}

	static int InvokeCallback(lua_State* luaState)
	{
		auto luaClass = reinterpret_cast<const Lua*>(lua_topointer(luaState, lua_upvalueindex(1)));
		auto callback = std::string(lua_tostring(luaState, lua_upvalueindex(2)));
		if (auto it = luaClass->luaCallbacks.find(callback); it != luaClass->luaCallbacks.end())
		{
			return it->second(luaState);
		}

		luaL_error(luaState, "Callback %s was not properly registered", callback.c_str());
		__builtin_unreachable();
	}
};

};
};
