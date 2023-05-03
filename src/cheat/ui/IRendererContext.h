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

#ifndef IRENDERERCONTEXT_H
#define IRENDERERCONTEXT_H
#pragma once

// Every class that wants to render imgui must derive from this base class and implement the "on_render" method.
class IRenderingContext
{
public:
	IRenderingContext()
	{
	}

	virtual ~IRenderingContext()
	{
	}

	explicit IRenderingContext(const std::string& id) :
		m_identifier(id)
	{
		// it is true by default, and it can be turned off explicitly.
		m_is_interactible = true;
	}

	virtual bool should_render() 
	{
		assert(0 && "You need to override this function! " __FUNCTION__);
		return false;
	}

	virtual void on_initialize() = 0;
	virtual void on_render() = 0;
	virtual void on_destroy() = 0;

	bool is_interactible() const { return m_is_interactible; }
	void set_not_interactible() { m_is_interactible = false; }

	inline const std::string& get_id() const { return m_identifier; }

protected:
	std::string m_identifier = "uninitialized";

	// if is, it affects ingame mouse events when closed/opened
	bool m_is_interactible = false;
};

// rendering context activated by a key press.
class IRenderingContext_KeyActivate : public IRenderingContext
{
public:
	IRenderingContext_KeyActivate(const std::string& id, int virtual_key) :
		IRenderingContext(id), m_virtual_key(virtual_key)
	{
		add_key_callback();
	}

	virtual bool should_render()
	{
		return m_should_render;
	}

private:
	void add_key_callback()
	{
		assert(m_virtual_key != NULL);

		g_user_input_i->add_key_press_callback(
			m_virtual_key,
			[this](UserKey_t* key)
			{
				m_should_render ^= 1;
			});
	}

private:
	int m_virtual_key = NULL;

	bool m_should_render = false;
};

// rendering context that is activated by a key, but only if the context that it relies on is activated.
// 
// Note: We have to also provide key callback to this one, otherwise we could toggle it even when the reliant context wouldn't be being rendered at all.
class IRenderingContext_KeyActivateReliantOther : public IRenderingContext
{
public:
	IRenderingContext_KeyActivateReliantOther(const std::string& id, int virtual_key, IRenderingContext* reliant_ctx) :
		IRenderingContext(id), m_virtual_key(virtual_key), m_reliant_ctx(reliant_ctx)
	{
		add_key_callback();
	}

	virtual bool should_render()
	{
		return m_reliant_ctx->should_render() && m_should_render;
	}

private:
	void add_key_callback()
	{
		assert(m_virtual_key != NULL);

		g_user_input_i->add_key_press_callback(
			m_virtual_key,
			[this](UserKey_t* key)
			{
				if (m_reliant_ctx)
				{
					// only if the reliant context is rendering, e.g. we cannot show up the console if the menu isn't up.
					if (m_reliant_ctx->should_render())
					{
						m_should_render ^= 1;
					}
				}
				else
				{
					m_should_render ^= 1; // no reliant context, just switch normally
				}
			});
	}

private:
	int m_virtual_key = NULL;

	bool m_should_render = false;

	IRenderingContext* m_reliant_ctx = nullptr;
};

// rendering context activated together with its parent context.
class IRenderingContext_ParentActivate : public IRenderingContext
{
public:
	IRenderingContext_ParentActivate(const std::string& id, IRenderingContext* parent_ctx) :
		IRenderingContext(id), m_parent_ctx(parent_ctx)
	{
	}

	virtual bool should_render()
	{
		return m_parent_ctx->should_render();
	}

private:
	IRenderingContext* m_parent_ctx = nullptr;
};

// rendering context activated through external code, only when it gets requested to be updated.
class IRenderingContext_IndependentlyActivate : public IRenderingContext
{
public:
	IRenderingContext_IndependentlyActivate(const std::string& id) :
		IRenderingContext(id)
	{
	}

	virtual bool should_render()
	{
		// only one has to be active in order to fully force the render.
		for (const auto& c : m_render_constain_callbacks)
		{
			if (c())
				return true;
		}

		return false;
	}

	void add_render_constain(const std::function<bool()>& callback)
	{
		m_render_constain_callbacks.push_back(callback);
	}

	void add_render_code(const std::function<void()>& callback)
	{
		m_render_code_callbacks.push_back(callback);
	}

protected:
	// set of constrains that enable to execute the rendering code
	std::vector<std::function<bool()>> m_render_constain_callbacks;

	// rendering code that will be executed when any of these constrains are met
	std::vector<std::function<void()>> m_render_code_callbacks;
};

#endif // IRENDERERCONTEXT_H