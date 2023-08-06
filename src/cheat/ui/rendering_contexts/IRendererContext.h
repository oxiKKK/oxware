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
		m_interaction_is_locked = false;
	}

	virtual bool should_render() = 0;

	virtual void toggle_rendering() = 0;

	virtual void on_initialize() = 0;
	virtual void on_render() = 0;
	virtual void on_destroy() = 0;

	bool is_interactible() const { return m_is_interactible; }
	void set_not_interactible() { m_is_interactible = false; }

	inline const std::string& get_id() const { return m_identifier; }

	inline void lock_interaction() { m_interaction_is_locked = true; }
	inline void unlock_interaction() { m_interaction_is_locked = false; }
	inline bool is_interaction_blocked() { return m_interaction_is_locked; }

protected:
	std::string m_identifier = "uninitialized";

	// if is, it affects ingame mouse events when closed/opened
	bool m_is_interactible = false;

	bool m_interaction_is_locked = true;
};

// rendering context activated from elsewhere
class IRenderingContext_Generic : public IRenderingContext
{
public:
	IRenderingContext_Generic(const std::string& id) :
		IRenderingContext(id)
	{
	}

	bool should_render()
	{
		return m_should_render;
	}

	void toggle_rendering()
	{
		if (m_interaction_is_locked)
		{
			return;
		}

		m_should_render ^= 1;
	}

private:
	bool m_should_render = false;
};

// rendering context that is activated only if the context that it relies on is activated.
class IRenderingContext_GenericReliantOther : public IRenderingContext
{
public:
	IRenderingContext_GenericReliantOther(const std::string& id, IRenderingContext* reliant_ctx) :
		IRenderingContext(id), m_reliant_ctx(reliant_ctx)
	{
	}

	bool should_render()
	{
		return m_reliant_ctx->should_render() && m_should_render;
	}

	void toggle_rendering()
	{
		if (m_interaction_is_locked)
		{
			return;
		}

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
	}

private:
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

	bool should_render()
	{
		return m_parent_ctx->should_render();
	}

	void toggle_rendering()
	{
		// not supported
	}

private:
	IRenderingContext* m_parent_ctx = nullptr;
};

// Same as ParentActivate however, this one get's deactivated only after some threshold.
class IRenderingContext_ParentActivateThreshold : public IRenderingContext
{
public:
	IRenderingContext_ParentActivateThreshold(const std::string& id, IRenderingContext* parent_ctx, std::chrono::duration<float, std::milli> threshold) :
		IRenderingContext(id), m_parent_ctx(parent_ctx), m_threshold(threshold)
	{
	}

	bool should_render()
	{
		bool parent_render = m_parent_ctx->should_render();

		if (parent_render)
		{
			m_is_rendering_timeless = true;

			if (!m_just_opened)
			{
				m_opened_time = std::chrono::high_resolution_clock::now();
				m_just_opened = true;
			}

			m_just_closed = false;
			return true;
		}
		else
		{
			m_is_rendering_timeless = false;

			if (!m_just_closed && m_just_opened)
			{
				m_closed_time = std::chrono::high_resolution_clock::now();
				m_just_closed = true;
			}

			if (time_elapsed_since_closed() < m_threshold)
			{
				return true;
			}

			m_just_opened = false;
			m_just_closed = false;
			return false;
		}
	}

	void toggle_rendering()
	{
		// not supported
	}

protected:
	std::chrono::duration<float, std::milli> time_elapsed_since_closed() const
	{
		return std::chrono::high_resolution_clock::now() - m_closed_time;
	}
	std::chrono::duration<float, std::milli> time_elapsed_since_opened() const
	{
		return std::chrono::high_resolution_clock::now() - m_opened_time;
	}

private:
	IRenderingContext* m_parent_ctx = nullptr;

	std::chrono::high_resolution_clock::time_point m_closed_time, m_opened_time;
	bool m_just_closed = false, m_just_opened = false;

protected:
	std::chrono::duration<float, std::milli> m_threshold;
	
	bool m_is_rendering_timeless = false;
};

// rendering context activated through external code, only when it gets requested to be updated.
class IRenderingContext_IndependentlyActivate : public IRenderingContext
{
public:
	IRenderingContext_IndependentlyActivate(const std::string& id) :
		IRenderingContext(id)
	{
	}

	bool should_render()
	{
		// only one has to be active in order to fully force the render.
		for (const auto& c : m_render_constain_callbacks)
		{
			if (c())
				return true;
		}

		return false;
	}

	void toggle_rendering()
	{
		// not supported
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