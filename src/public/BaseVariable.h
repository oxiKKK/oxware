/*
*	OXWARE developed by oxiKKK
*	Copyright (c) 2023
* 
*	This program is licensed under the MIT license. By downloading, copying, 
*	installing or using this software you agree to this license.
*
*	License Agreement
*
*	Permission is hereby granted, free of charge, to any person obtaining a 
*	copy of this software and associated documentation files (the "Software"), 
*	to deal in the Software without restriction, including without limitation 
*	the rights to use, copy, modify, merge, publish, distribute, sublicense, 
*	and/or sell copies of the Software, and to permit persons to whom the 
*	Software is furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included 
*	in all copies or substantial portions of the Software. 
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
*	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
*	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
*	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
*	IN THE SOFTWARE.
*/

#ifndef BASEVARIABLE_H
#define BASEVARIABLE_H
#pragma once

#include "commondefs.h"
#include "interface/IVariableManager.h"
#include "interface/IBindManager.h"

#include <map>
#include <string>
#include <vector>
#include <format>
#include <algorithm>

#include <assert.h>

template<typename T>
class VariableValue
{
public:
	virtual void set_value(T value) = 0;

	T get_value() const { return m_value; }
	T get_default() const { return m_default; }
	T get_min() const { return m_min; }
	T get_max() const { return m_max; }

	virtual std::string T_to_string(T v) const = 0;
	virtual T string_to_T(const std::string& v, bool& ok) const = 0;

	virtual void clamp_value_if_needed() {}

protected:
	T m_value, m_default, m_min, m_max;
};

// use with bigger objects
template<typename T>
class VariableValueRef
{
public:
	virtual void set_value(const T& value) = 0;

	const T& get_value() const { return m_value; }
	const T& get_default() const { return m_default; }
	const T& get_min() const { return m_min; }
	const T& get_max() const { return m_max; }

	virtual std::string T_to_string(const T& v) const = 0;
	virtual T string_to_T(const std::string& v, bool& ok) const = 0;

	virtual void clamp_value_if_needed() {}

protected:
	T m_value, m_default, m_min, m_max;
};

class BaseVariable
{
public:
	BaseVariable(const char* name, const char* description, const char* value)
	{
		m_name = name;
		m_desc = description;
		m_value_string = m_default_string = value;

		m_has_bounds = false;
		m_min_string = m_max_string = "";

		m_module_owner = CURRENT_MODULE;

		add_to_global_list();
	}
	
	BaseVariable(const char* name, const char* description, const char* value, const char* min, const char* max)
	{
		m_name = name;
		m_desc = description;
		m_value_string = m_default_string = value;

		m_has_bounds = true;
		m_min_string = min;
		m_max_string = max;

		m_module_owner = CURRENT_MODULE;

		add_to_global_list();
	}

	// getters
	virtual const char* get_name() const { return m_name.c_str(); }
	virtual const char* get_description() const { return m_desc.c_str(); }

	virtual const char* get_value_string() const { return m_value_string.c_str(); }
	virtual const char* get_default_value_string() const { return m_default_string.c_str(); }
	virtual const char* get_min_string() const { return m_min_string.c_str(); }
	virtual const char* get_max_string() const { return m_max_string.c_str(); }

	inline const char* get_current_module_name() const { return m_module_owner; }

	virtual bool has_bounds() const { return m_has_bounds; }

	// class that derives from this has to override these.
	virtual void set_from_string(const std::string& new_value)
	{
		assert(0 && "You have to override this function from your class! (set_from_string)");
	}
	virtual void set_from_string(const std::string& new_value, bool& ok)
	{
		assert(0 && "You have to override this function from your class! (set_from_string)(2)");
	}

	struct less
	{
		inline bool operator()(BaseVariable* a, BaseVariable* b) const { return a->m_name < b->m_name; }
	};

private:
	void add_to_global_list();

protected:
	std::string m_name, m_desc;

	std::string m_value_string, m_default_string, m_min_string, m_max_string;
	bool m_has_bounds = false;

	// set automatically
	const char* m_module_owner;
};

//--------------------------------------------------------------------------------------------------------------------

class VarFloat final : public BaseVariable, public VariableValue<float>
{
public:
	VarFloat(const char* name, const char* description, float value)
		: BaseVariable(name, description, T_to_string(value).c_str())
	{
		m_default = value;
		m_min = m_max = 0.0f;
		set_value(value);
	}

	VarFloat(const char* name, const char* description, float value, float min, float max)
		: BaseVariable(name, description, T_to_string(value).c_str(), T_to_string(min).c_str(), T_to_string(max).c_str())
	{
		m_default = value;
		m_min = min;
		m_max = max;
		set_value(value);
	}

	void set_value(float value) override
	{
		if (m_value != value)
		{
			m_value = value;
			m_value_string = T_to_string(value);

			clamp_value_if_needed();
		}
	}

	void set_from_string(const std::string& new_value) override
	{
		bool dummy;
		set_from_string(new_value, dummy);
	}

	void set_from_string(const std::string& new_value, bool& ok) override
	{
		if (m_value_string != new_value)
		{
			bool ok;
			float v = string_to_T(new_value, ok);
			if (!ok)
			{
				return;
			}

			m_value = v;
			m_value_string = new_value;
			clamp_value_if_needed();
		}
	}

	void clamp_value_if_needed() override
	{
		if (!m_has_bounds)
		{
			return;
		}

		if (m_value < m_min)
		{
			m_value = m_min;
			m_value_string = T_to_string(m_min);
		}
		else if (m_value > m_max)
		{
			m_value = m_max;
			m_value_string = T_to_string(m_max);
		}
	}

private:
	std::string T_to_string(float v) const override
	{
		return std::to_string(m_value);
	}
	
	float string_to_T(const std::string& v, bool& ok) const override
	{
		ok = true;
		try
		{
			return std::stof(v);
		}
		catch (...)
		{
			ok = false;
			return 0;
		}
	}
};

class VarInteger final : public BaseVariable, public VariableValue<int>
{
public:
	VarInteger(const char* name, const char* description, int value)
		: BaseVariable(name, description, T_to_string(value).c_str())
	{
		m_default = value;
		m_min = m_max = 0;
		set_value(value);
	}

	VarInteger(const char* name, const char* description, int value, int min, int max)
		: BaseVariable(name, description, T_to_string(value).c_str(), T_to_string(min).c_str(), T_to_string(max).c_str())
	{
		m_default = value;
		m_min = min;
		m_max = max;
		set_value(value);
	}

	void set_value(int value) override
	{
		if (m_value != value)
		{
			m_value = value;
			m_value_string = T_to_string(value);

			clamp_value_if_needed();
		}
	}

	void set_from_string(const std::string& new_value) override
	{
		bool dummy;
		set_from_string(new_value, dummy);
	}

	void set_from_string(const std::string& new_value, bool& ok) override
	{
		if (m_value_string != new_value)
		{
			bool ok;
			int v = string_to_T(new_value, ok);
			if (!ok)
			{
				return;
			}

			m_value = v;
			m_value_string = new_value;
			clamp_value_if_needed();
		}
	}

	void clamp_value_if_needed() override
	{
		if (!m_has_bounds)
		{
			return;
		}

		if (m_value < m_min)
		{
			m_value = m_min;
			m_value_string = T_to_string(m_min);
		}
		else if (m_value > m_max)
		{
			m_value = m_max;
			m_value_string = T_to_string(m_max);
		}
	}

private:
	std::string T_to_string(int v) const override
	{
		return std::to_string(v);
	}
	
	int string_to_T(const std::string& v, bool& ok) const override
	{
		ok = true;
		try
		{
			return std::stoi(v);
		}
		catch (...)
		{
			ok = false;
			return 0;
		}
	}
};

// accepts only "yes" and "no" format.
class VarBoolean final : public BaseVariable, public VariableValue<bool>
{
public:
	VarBoolean(const char* name, const char* description, bool value)
		: BaseVariable(name, description, T_to_string(value).c_str())
	{
		m_default = value;
		set_value(value);
	}

	void set_value(bool value) override
	{
		if (m_value != value)
		{
			m_value = value;
			m_value_string = T_to_string(value);
		}
	}

	void set_from_string(const std::string& new_value) override
	{
		bool dummy;
		set_from_string(new_value, dummy);
	}

	void set_from_string(const std::string& new_value, bool& ok) override
	{
		if (m_value_string != new_value)
		{
			bool ok;
			bool v = string_to_T(new_value, ok);
			if (!ok)
			{
				return;
			}

			m_value = v;
			m_value_string = new_value;
			clamp_value_if_needed();
		}
	}

private:
	std::string T_to_string(bool v) const override
	{
		return v == true ? "yes" : "no";
	}
	
	bool string_to_T(const std::string& v, bool& ok) const override
	{
		ok = true;
		if (v == "yes" || v == "1" || v == "true" || v == "on")
		{
			return true;
		}
		else if (v == "no" || v == "0" || v == "false" || v == "off")
		{
			return false;
		}
		ok = false;
		return false;
	}
};

class VarColor final : public BaseVariable, public VariableValue<CColor>
{
public:	
	VarColor(const char* name, const char* description, const CColor& value)
		: BaseVariable(name, description, T_to_string(value).c_str())
	{
		m_default = value;
		set_value(value);
	}

	void set_value(CColor value) override
	{
		if (m_value != value)
		{
			m_value = value;
			m_value_string = T_to_string(value);
		}
	}

	void set_from_string(const std::string& new_value) override
	{
		bool dummy;
		set_from_string(new_value, dummy);
	}

	void set_from_string(const std::string& new_value, bool& ok) override
	{
		if (m_value_string != new_value)
		{
			bool ok;
			CColor v = string_to_T(new_value, ok);
			if (!ok)
			{
				return;
			}

			m_value = v;
			m_value_string = new_value;
			clamp_value_if_needed();
		}
	}

private:
	std::string T_to_string(CColor v) const override
	{
		return std::format("{}", v);
	}
	
	CColor string_to_T(const std::string& v, bool& ok) const override
	{
		return CColor::parse_color_out_of_string(v, ok);
	}
};

class VarKeyValue final : public BaseVariable, public VariableValueRef<KeyValue>
{
public:	
	VarKeyValue(const char* name, const char* description, const KeyValue& value)
		: BaseVariable(name, description, T_to_string(value).c_str())
	{
		m_default = value;
		set_value(value);
	}

	void set_value(const KeyValue& value) override
	{
		if (m_value != value)
		{
			m_value = value;
			m_value_string = T_to_string(value);
		}
	}

	void set_from_string(const std::string& new_value) override
	{
		bool dummy;
		set_from_string(new_value, dummy);
	}

	void set_from_string(const std::string& new_value, bool& ok) override
	{
		if (m_value_string != new_value)
		{
			bool ok;
			KeyValue v = string_to_T(new_value, ok);
			if (!ok)
			{
				return;
			}

			m_value = v;
			m_value_string = new_value;
			clamp_value_if_needed();
		}
	}

private:
	std::string T_to_string(const KeyValue& v) const override
	{
		return std::format("{}", v);
	}
	
	KeyValue string_to_T(const std::string& v, bool& ok) const override
	{
		return KeyValue::parse_kv_out_of_string(v, ok);
	}
};

class VarString final : public BaseVariable // don't need variable value. BaseVariable itself contains strings.
{
public:
	VarString(const char* name, const char* description, const char* value)
		: BaseVariable(name, description, value)
	{
		set_from_string(value);
	}

	void set_from_string(const std::string& new_value) override
	{
		m_value_string = new_value;
	}

	void set_value(const char* value)
	{
		if (strcmp(m_value_string.c_str(), value))
		{
			m_value_string = value;
		}
	}
};

//--------------------------------------------------------------------------------------------------------------------

// maximum amount of variables that our list can hold
static constexpr size_t k_max_variables_per_module = 1024; // 5th June, increased from 128 to 256
														   // 25th July, increased from 256 to 1024 so that we don't have to deal with this no more.
static constexpr size_t k_max_variables_absolute = 1024; // used in VariableManager

// This class is used in order to register/unregister all variables that are declared globally. Note that this is only
// a "temporary" storage, and isn't really used for variable lookup. The VaraibleManager then collects all variables stored 
// by this container and creates its own list.
class StaticVariableContainer
{
public:
	void add_variable(BaseVariable* var);

	inline auto& get_variable_list() const { return m_variables; }

private:
	// call only once
	void reserve_limit();

	bool see_for_overflow(BaseVariable* current_var);

private:
	std::unordered_set<BaseVariable*> m_variables;
};

extern StaticVariableContainer g_static_variable_container;

// called once global variable is instantiated.
inline void BaseVariable::add_to_global_list()
{
	g_static_variable_container.add_variable(this);
}

//--------------------------------------------------------------------------------------------------------------------
// COMMANDS
//

struct CmdArgs
{
	// implicit constructor
	CmdArgs(const std::vector<std::string>& _args) : args(_args) {}

	std::string get(size_t pos) const
	{
		std::string token;
		try
		{
			token = args.at(pos);
		}
		catch (...)
		{
			CConsole::the().derror("Failed to get token at position '{}'. Tokenized buffer size is {}.", pos, args.size());
		}
		return token;
	}

	size_t count() const { return args.size(); }

private:
	std::vector<std::string> args;
};

using pfnCommandFunc_t = std::function<void(BaseCommand* cmd, const CmdArgs& args)>;

enum ECommandFlags
{
	CMDFLAGS_None = 0, 

	CMDFLAGS_ExecuteOverUI = BIT(0),
};

class BaseCommand
{
public:
	BaseCommand(const std::string& name, const std::string& description, const pfnCommandFunc_t& func, ECommandFlags flags = CMDFLAGS_None) :
		m_name(name), 
		m_usage(""),
		m_description(description),
		m_function(func), 
		m_flags(flags)
	{
		add_to_global_list();
	}

	BaseCommand(const std::string& name, const std::string& usage, const std::string& description, const pfnCommandFunc_t& func, ECommandFlags flags = CMDFLAGS_None) :
		m_name(name), 
		m_usage(usage), 
		m_description(description),
		m_function(func), 
		m_flags(flags)
	{
		add_to_global_list();
	}

	inline const char* get_name() const { return m_name.c_str(); }
	inline const char* get_description() const { return m_description.c_str(); }
	inline ECommandFlags get_flags() const { return m_flags; }

	// usage
	inline bool has_usage() const { return !m_usage.empty(); }
	inline const char* get_usage() const { return m_usage.c_str(); }

	void execute(BaseCommand* cmd, const CmdArgs& args)
	{
		m_function(cmd, args);
	}

	void print_usage()
	{
		CConsole::the().info("Usage: {} {}", m_name, m_usage);
	}

	struct less
	{
		inline bool operator()(BaseCommand* a, BaseCommand* b) const { return a->m_name < b->m_name; }
	};

private:
	void add_to_global_list();

protected:
	std::string m_name, m_usage, m_description;

	pfnCommandFunc_t m_function;

	ECommandFlags m_flags;
};

//--------------------------------------------------------------------------------------------------------------------

// maximum amount of variables that our list can hold
static constexpr size_t k_max_commands_per_module = 64;
static constexpr size_t k_max_commands_absolute = 64 * 8; // used in VariableManager

// This class is used in order to register/unregister all variables that are declared globally. Note that this is only
// a "temporary" storage, and isn't really used for variable lookup. The VaraibleManager then collects all variables stored 
// by this container and creates its own list.
class StaticCommandContainer
{
public:
	void add_command(BaseCommand* var);

	inline auto& get_command_list() const { return m_commands; }

private:
	// call only once
	void reserve_limit();

	bool see_for_overflow(BaseCommand* current_var);

private:
	std::unordered_set<BaseCommand*> m_commands;
};

extern StaticCommandContainer g_static_command_container;

// called once global variable is instantiated.
inline void BaseCommand::add_to_global_list()
{
	g_static_command_container.add_command(this);
}

#endif // BASEVARIABLE_H
