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

#ifndef UIWINDOWPOPUPS_H
#define UIWINDOWPOPUPS_H
#pragma once

class UIBasePopupContext
{
public:
	// default values
	UIBasePopupContext()
	{
		m_window_size = { -1.0f, -1.0f };
		m_contents_fn = NULL;
		m_on_cancel_fn = NULL;
		m_on_okay_fn = NULL;
		m_window_flags = ImGuiWindowFlags_None;
		m_popup_custom_contents_fn = NULL;
	}

	virtual ~UIBasePopupContext() {}

	// THIS MUST BE PROVIDED EVERY TIME!!!!!!!!!!!!
	void provide_uid(const std::string& uid)
	{
		m_uid = uid;
	}

	// if not provided, {-1, -1} is the default value
	virtual void provide_window_size(const Vector2D& window_size)
	{
		m_window_size = window_size;
	}

	// return true will close the popup
	virtual void provide_contents_fn(const std::function<bool()>& fn)
	{
		m_contents_fn = fn;
	}

	//
	// okay or cancel button. a popup must have at least one provided.
	//

	virtual void provide_on_cancel_fn(const std::function<void()>& fn)
	{
		m_on_cancel_fn = fn;
	}

	virtual void provide_on_okay_fn(const std::function<void()>& fn)
	{
		m_on_okay_fn = fn;
	}

	virtual void provide_window_flags(ImGuiWindowFlags flags)
	{
		m_window_flags = flags;
	}

	//
	// getters
	//

	inline const std::string& get_uid() const { return m_uid; }

	//
	// rendering
	//

	void render();

private:
	void render_background();

protected:
	// unique identifier for this popup. this is used for deallocation when closing.
	std::string m_uid;
	
	Vector2D m_window_size;

	// callbacks
	std::function<bool()> m_contents_fn; // on return true the popup closes
	std::function<void()> m_on_cancel_fn;
	std::function<void()> m_on_okay_fn;

	ImGuiWindowFlags m_window_flags;

	// data shared only between this class and the class that inherits.
	// this can be set by the derived class. if returns true, popup will close.
	std::function<bool()> m_popup_custom_contents_fn;
};

// popup with child window
class UIDecoratedPopup : public virtual UIBasePopupContext
{
public:
	UIDecoratedPopup() : UIBasePopupContext()
	{
		m_popup_custom_contents_fn = [this]() { return popup_window_contents(); };
	}

protected:
	virtual bool popup_window_contents();
};

// popup with child window and a title
class UIDecoratedWithTitlePopup : public virtual UIDecoratedPopup
{
public:
	UIDecoratedWithTitlePopup() : UIBasePopupContext()
	{
		m_title = "None";

		m_popup_custom_contents_fn = [this]() { return popup_window_contents(); };
	}

	virtual void provide_window_title(const std::string& title)
	{
		m_title = title;
	}

protected:
	virtual bool popup_window_contents();

private:
	std::string m_title;
};

//-----------------------------------------------------------------------------------------------

class CUIWindowPopups
{
public:
	DECL_BASIC_CLASS(CUIWindowPopups);

public:
	void pre_render();
	void render();

	// always use this when creating popup windows!
	template<typename T>
	T* create_popup_context(const std::string& uid)
	{
		auto p = new T();
		p->provide_uid(uid); // this is necessary
		return p;
	}

	void schedule_popup(UIBasePopupContext* popup);

	void close_current_popup()
	{
		m_popup_close_requested = true;
	}

	// aka the "about" dialog
	void create_welcome_popup();

	// returns true if there is currently any popup rendering
	bool is_in_popup_dialog() const
	{
		return m_popup_stack.empty() == false;
	}

	void signal_popup_close(const std::string& uid)
	{
		assert(m_popup_to_be_deleted.empty() && "tried to signal popup close but didn't close previous popup properly!");
		m_popup_to_be_deleted = uid;
	}

private:
	std::vector<UIBasePopupContext*> m_popup_stack;

	// popup to be deleted' uid
	std::string m_popup_to_be_deleted;

	bool m_popup_close_requested = false;
	void handle_popup_close();
};

#endif // UIWINDOWPOPUPS_H