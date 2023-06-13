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

#ifndef FEATUREMANAGER_H
#define FEATUREMANAGER_H
#pragma once

enum EFeatureType
{
	FEATURE_Unset,

	FEATURE_Bool,
	FEATURE_Integer,
	FEATURE_Float,
};

class BaseFeature
{
public:
	BaseFeature(VarBoolean* var, const std::string& name)
	{
		m_type = FEATURE_Bool;
		m_base_variable = m_variable_bool = var;
		m_name = name;
	}

	BaseFeature(VarInteger* var, int off_value, const std::string& name)
	{
		m_type = FEATURE_Integer;
		m_base_variable = m_variable_integer = var;
		m_off_value_int = off_value;
		m_name = name;
	}

	BaseFeature(VarFloat* var, float off_value, const std::string& name)
	{
		m_type = FEATURE_Float;
		m_base_variable = m_variable_float = var;
		m_off_value_float = off_value;
		m_name = name;
	}

	const std::string& get_name() const { return m_name; }
	BaseVariable* get_variable() const { assert(m_base_variable); return m_base_variable; }

	bool is_enabled() const
	{
		switch (m_type)
		{
			case FEATURE_Bool: return m_variable_bool->get_value() != false;
			case FEATURE_Integer: return m_variable_integer->get_value() != m_off_value_int;
			case FEATURE_Float: return m_variable_float->get_value() != m_off_value_float;
		}

		assert(0 && "Unknown feature type.");
		return false;
	};

private:
	std::string m_name = "unset";

	VarBoolean* m_variable_bool = nullptr;
	VarInteger* m_variable_integer = nullptr;
	int m_off_value_int = 0;
	VarFloat* m_variable_float = nullptr;
	float m_off_value_float = 0;

	BaseVariable* m_base_variable = nullptr;

	std::function<bool(VarBoolean*)> m_custom_is_enabled;

	EFeatureType m_type = FEATURE_Unset;
};

class CFeatureManager
{
public:
	DECL_BASIC_CLASS(CFeatureManager);

	void initialize();
	void shutdown();

	void add_new_feature(const BaseFeature& feature);

	void for_each_feature(const std::function<void(BaseFeature* f)>& callback)
	{
		if (!m_initialized)
		{
			return;
		}

		for (auto& f : m_features)
		{
			callback(&f);
		}
	}

private:
	void precache_features();

private:
	std::vector<BaseFeature> m_features;

	bool m_initialized = false;
};

#endif // FEATUREMANAGER_H