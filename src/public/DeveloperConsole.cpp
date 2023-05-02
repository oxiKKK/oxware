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

//
// DeveloperConsole.cpp -- Implementation of the IDeveloperConsole interface.
// 
//	Note that this is just implemenation of the interface. Each module then idependently
//	includes a cpp file that communicates with this class. This is needed in order
//	for the console code to distinguish between each module (i.e. who's printing which msg)
// 
//	THIS INTERFACE SHOULDN'T BE USED DIRECTLY, RATHER USE CONSOLE.CPP SHARED FILE.
//

#include "interface/IDeveloperConsole.h"
#include "interface/gui/IGUIWidgets.h"
#include <interface/gui/IGUIFontManager.h>
#include <interface/IAppdataManager.h>
#include <interface/IVariableManager.h>

#include "tier/StringTools.h"
#include "tier/MessageBox.h"

#include "BaseVariable.h"

#include <unordered_set>
#include <deque>
#include <map>

VarInteger num_logfiles_to_keep("num_logfiles_to_keep", "Amount of logfiles to keep. Older logfiles will be automatically removed.", 10);

IDeveloperConsole* g_devconsole_i = nullptr;

class CDeveloperConsole : public IDeveloperConsole
{
public: 
	CDeveloperConsole();
	~CDeveloperConsole();

	void initialize(const std::string& module_name);
	void shutdown();
	void render();

	void print(EOutputModule module, EOutputCategory category, const std::string& what, bool printed_already = false, std::chrono::system_clock::time_point time = std::chrono::system_clock::now());

	void flush_screen();

	void register_module(EOutputModule which);
	void unregister_module(EOutputModule which);

	FilePath_t get_logfile_path();

	void provide_hl_execute_cmd_pfn(m_hl_execute_cmd_pfn_t pfn)
	{
		m_hl_execute_cmd_pfn = pfn;
	}

private:
	std::string generate_logfilename();

	void prepare_log_directory();

	static int text_edit_callback_stub(ImGuiInputTextCallbackData* data)
	{
		CDeveloperConsole* _this = (CDeveloperConsole*)data->UserData;
		return _this->text_edit_callback(data);
	}

	int text_edit_callback(ImGuiInputTextCallbackData* data);

	int execute_hl_command(const char* cmd)
	{
		if (m_hl_execute_cmd_pfn)
		{
			return m_hl_execute_cmd_pfn(cmd);
		}
		else
		{
			return 0;
		}
	}

	m_hl_execute_cmd_pfn_t m_hl_execute_cmd_pfn;

private:
	// used only within this file as a convenience
	inline void print_locally(EOutputCategory category, const std::string& what)
	{
		print(EOutputModule::GUI, category, what);
	}

	inline void print_console(EOutputCategory category, const std::string& what)
	{
		print(EOutputModule::CONSOLE, category, what);
	}

	struct output_line_t
	{
		EOutputCategory		category;
		EOutputModule		module;
		std::string			contents;
		std::chrono::system_clock::time_point time;
	};
	std::string line_preamble(const output_line_t& line);

	void log(const output_line_t& line);

	std::vector<output_line_t> m_contents;
	std::mutex m_output_lock;

	// List of modules that are registered to this class
	std::unordered_set<EOutputModule> m_module_list;

	bool m_need_to_scroll_to_the_bottom = false;

	std::ofstream m_logfile_output_stream;

	// the module that runs this code
	std::string m_module_name;

	std::string m_logfile_path_current;

	// command history
	std::deque<std::string> m_entered_commands;
};

BaseCommand clear(
	"clear",
	[&]()
	{
		g_devconsole_i->flush_screen();
	}
);

CDeveloperConsole g_dev_console;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CDeveloperConsole, IDeveloperConsole, IDEVELOPERCONSOLE_INTERFACEID, g_dev_console);

//-------------------------------------------------------------------------------
//
// CDeveloperConsole implementation
// 
//-------------------------------------------------------------------------------

CDeveloperConsole::CDeveloperConsole()
{
	g_devconsole_i = this;

	// FIXME: This check is kinda dumb -- we need a better way of checking for this, maybe fix this?
#ifdef M_CHEATER
	initialize(MODULE_CHEAT);
#endif
}

CDeveloperConsole::~CDeveloperConsole()
{
	g_devconsole_i = nullptr;

	shutdown();
}

static std::string remove_extension(const std::string& filename) {
	size_t lastdot = filename.find_last_of(".");
	if (lastdot == std::string::npos) return filename;
	return filename.substr(0, lastdot);
}

void CDeveloperConsole::initialize(const std::string& module_name)
{
	// assuming that 'module_name' has an extension
	m_module_name = remove_extension(module_name);

	prepare_log_directory();

	m_logfile_output_stream = std::ofstream(get_logfile_path(), std::ios_base::app);
	if (m_logfile_output_stream.bad())
	{
		print_locally(EOutputCategory::ERROR, std::format("Failed to open logfile. The path is '{}'", get_logfile_path()));
	}
	else
	{
		m_logfile_output_stream << "// oxWARE console log file\n";
		m_logfile_output_stream << "// \n";
		m_logfile_output_stream << "// version     : " OXVER_STRING "\n";
		m_logfile_output_stream << "// build       : " OX_BUILD "\n";
		m_logfile_output_stream << "// compiled at : " OX_COMPILE_TIMESTAMP "\n";
		m_logfile_output_stream << "// generated by: " << module_name << "\n";
		m_logfile_output_stream << "// \n";
		m_logfile_output_stream << "\n";
		std::flush(m_logfile_output_stream);

		print_locally(EOutputCategory::INFO, std::format("Logging to {}", get_logfile_path()));
	}

	print_locally(EOutputCategory::INFO, "Console Initialized");
}

void CDeveloperConsole::shutdown()
{
	print_locally(EOutputCategory::INFO, "Shutting down console...");

	if (m_logfile_output_stream.good())
	{
		m_logfile_output_stream.close();
	}
}

void CDeveloperConsole::render()
{
	const float footer_height_to_reserve = -32.0f;

	g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, { 4.0f, 4.0f });
	g_gui_widgets_i->add_child(
		"console", { 0.0f, footer_height_to_reserve }, true, ImGuiWindowFlags_HorizontalScrollbar, [this]()
		{
			g_gui_widgets_i->render_clipped_contents(
				m_contents.size(),
				[this](int line_no)
				{
					auto& line = m_contents.at(line_no);

					// shrink a little..
					g_gui_widgets_i->push_stylevar(ImGuiStyleVar_ItemSpacing, { 0.0f, 1.0f });
					g_gui_widgets_i->push_stylevar(ImGuiStyleVar_IndentSpacing, 0.0f);
					g_gui_widgets_i->push_font(g_gui_fontmgr_i->get_imgui_font("proggyclean", FONT_SMALL, FONTDEC_Regular));

					g_gui_widgets_i->add_text(line_preamble(line), TEXTPROP_ColorBlack);
					g_gui_widgets_i->sameline();
					g_gui_widgets_i->add_text("[", TEXTPROP_ColorBlack);
					g_gui_widgets_i->sameline();
					g_gui_widgets_i->add_colored_text(s_module_color_table[(int)line.module].color, outputmodule_as_string(line.module));
					g_gui_widgets_i->sameline();
					g_gui_widgets_i->add_text("] ", TEXTPROP_ColorBlack);
					g_gui_widgets_i->sameline();

					const char* which[] = { "", "error: ", "warning: " };

					if (line.category != EOutputCategory::INFO)
					{
						g_gui_widgets_i->add_colored_text(s_category_color_table[(int)line.category].color, which[(int)line.category]);
						g_gui_widgets_i->sameline();
					}
					g_gui_widgets_i->add_colored_text(s_category_color_table[(int)line.category].color, line.contents);

					g_gui_widgets_i->pop_font();

					g_gui_widgets_i->pop_stylevar(2);
				});

			// after new log is added, scroll to the bottom every time
			if (m_need_to_scroll_to_the_bottom)
			{
				g_gui_widgets_i->set_scroll_here_y(1.0f);
				m_need_to_scroll_to_the_bottom = false;
			}
		});

	static char buffer[256];
	ImGuiInputTextFlags input_text_flags = 
		ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | 
		ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
	bool reclaim_focus = false;
	if (g_gui_widgets_i->add_text_input("Enter commands", buffer, sizeof(buffer), input_text_flags, true, &text_edit_callback_stub, (void*)this))
	{
		if (buffer[0])
		{
			bool add_to_history = false;
			bool cmd_buffer_tokenized = false;
			if (!strnicmp(buffer, "hl ", 3))
			{
				const char* s = buffer + 3;

				if (s[0])
				{
					// hl command
					if (!execute_hl_command(s))
					{
						print_console(EOutputCategory::ERROR, std::format("Bad HL command: {}", s));
					}
					else
					{
						print_console(EOutputCategory::INFO, std::format("HL command: {}", s));
					}

					add_to_history = true;
				}
			}
			else
			{
				// feed the parser with new buffer
				g_variablemgr_i->update_cmd_buffer(buffer);
				cmd_buffer_tokenized = true;

				// our command - only the name before first space.
				std::string str_buffer = buffer;
				std::string command_name = buffer;

				auto space = str_buffer.find(' ');
				if (space != std::string::npos)
				{
					command_name = str_buffer.substr(0, space);
				}

				auto cmd = g_variablemgr_i->query_command(command_name.c_str());

				if (cmd)
				{
					cmd->execute();
					print_console(EOutputCategory::INFO, std::format("{}", str_buffer));
				}
				else
				{
					print_console(EOutputCategory::INFO, std::format("Unknown command '{}'", command_name));
				}

				add_to_history = true;
			}

			if (add_to_history)
			{
				if (m_entered_commands.size() > 20)
				{
					m_entered_commands.pop_front();
				}

				if (!cmd_buffer_tokenized)
				{
					// feed the parser with new buffer
					g_variablemgr_i->update_cmd_buffer(buffer);
				}

				m_entered_commands.push_back(buffer);
			}

			strcpy(buffer, "");
		}

		reclaim_focus = true;
	}

	// Auto-focus on window apparition
	g_gui_widgets_i->set_item_default_focus();
	if (reclaim_focus)
	{
		g_gui_widgets_i->set_keyboard_focus_here(-1); // Auto focus previous widget
	}

	g_gui_widgets_i->pop_stylevar(1);
}

void CDeveloperConsole::print(EOutputModule module, EOutputCategory category, const std::string& what, bool printed_already, std::chrono::system_clock::time_point time)
{
	m_output_lock.lock();

	std::string fmt = what;

	std::string fmt_final;
	for (size_t i = 0; i < fmt.length(); i++)
	{
		unsigned char c = fmt[i];
		if (!std::isprint(c))
		{
			fmt_final += std::format("\\x{:02X}", c);
		}
		else
		{
			fmt_final.push_back(c);
		}
	}

	fmt_final.push_back('\n');

	output_line_t new_line =
	{
		category,
		module,
		fmt_final,
		time
	};

	if (!printed_already)
	{
		OutputDebugStringA(("[ox]" + line_preamble(new_line) + fmt_final).c_str());
	}

	m_contents.push_back(new_line);

	log(new_line);

	m_need_to_scroll_to_the_bottom = true;

	m_output_lock.unlock();
}

void CDeveloperConsole::flush_screen()
{
	size_t size = m_contents.size();
	m_contents.clear();

	print_console(EOutputCategory::INFO, std::format("Cleared {} lines", size));
}

void CDeveloperConsole::register_module(EOutputModule which)
{
	auto [iter, did_insert] = m_module_list.insert(which);
	assert(did_insert);

	print_locally(EOutputCategory::INFO, std::format("DeveloperConsole: Registered '{}' module", outputmodule_as_string(which)));
}

void CDeveloperConsole::unregister_module(EOutputModule which)
{
	auto num_erased = m_module_list.erase(which);
	assert(num_erased == 1);

	print_locally(EOutputCategory::INFO, std::format("DeveloperConsole: Unregistered '{}' module", outputmodule_as_string(which)));
}

#include <shlobj_core.h>

// just a little hack over the fact that this code already exists inside the FileSystem code.. since we
// want to have the console ready right as soon as the application starts, we cannot just use the appdata manager
// or the filesystem, so yeah.. this kinda sucks but whatever, its worth it so that we can log as soon as we start..
static FilePath_t get_appdata_dir()
{
	PWSTR pwstr_appdata_directory;
	HRESULT result = SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &pwstr_appdata_directory);
	assert(SUCCEEDED(result));
	FilePath_t ret = pwstr_appdata_directory;
	CoTaskMemFree(pwstr_appdata_directory);
	return ret / "oxware";
}

FilePath_t CDeveloperConsole::get_logfile_path()
{
	auto logdir = get_appdata_dir() / m_logfile_path_current;
	auto logfile = logdir / generate_logfilename();
	return logfile;
}

std::string CDeveloperConsole::generate_logfilename()
{
	static std::string generated_filename = "";
	if (!generated_filename.empty())
	{
		// generate only once
		return generated_filename;
	}

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, std::format("{}_%d%m%Y_%H%M%S.log", m_module_name).c_str());
	generated_filename = oss.str();
	return generated_filename;
}

void CDeveloperConsole::prepare_log_directory()
{
	print_locally(EOutputCategory::INFO, "Preparing log directories...");

	m_logfile_path_current = "log\\current\\";

	auto base_directory = get_appdata_dir();
	auto current_logdir = get_appdata_dir() / m_logfile_path_current;
	auto logdir = get_appdata_dir() / "log\\";

	std::error_code code;

	// create the oxware/ directory
	if (!std::filesystem::exists(base_directory, code))
	{
		if (!std::filesystem::create_directory(base_directory, code))
		{
			CMessageBox::display_warning("Couldn't create the base cheat directory '{}'.\n\nError message: {}", base_directory, code.message());
			m_logfile_path_current = get_appdata_dir().string();
			return;
		}
	}

	// log/
	if (!std::filesystem::exists(logdir, code))
	{
		if (!std::filesystem::create_directory(logdir, code))
		{
			CMessageBox::display_warning("Couldn't create directory for logs. The directory was '{}'.\n\nError message: {}", logdir, code.message());
			m_logfile_path_current = get_appdata_dir().string();
			return;
		}
	}

	// log/current/
	if (!std::filesystem::exists(current_logdir, code))
	{
		if (!std::filesystem::create_directory(current_logdir, code))
		{
			CMessageBox::display_warning("Couldn't create directory for logs. The directory was '{}'.\n\nError message: {}", current_logdir, code.message());
			m_logfile_path_current = "log\\";
			return;
		}
	}

	// move stuff that is there already to the log\\ directory.
	for (const auto& iter : std::filesystem::directory_iterator(current_logdir, code))
	{
		std::filesystem::rename(iter.path(), logdir / iter.path().filename(), code);
		
		print_locally(EOutputCategory::INFO, std::format("Moved {} to {}.", iter.path(), logdir));
	}

	print_locally(EOutputCategory::INFO, std::format("Maximum amount of logfiles that will be kept: {}", num_logfiles_to_keep.get_value()));

	// remove oldest logs
	// but first, get all the files that need to be removed.
	std::multimap<std::filesystem::file_time_type, std::filesystem::path> files_to_remove;
	int n = 0;
	for (const auto& iter : std::filesystem::directory_iterator(logdir, code))
	{
		if (n >= num_logfiles_to_keep.get_value()
			&& std::filesystem::is_regular_file(iter.path(), code))
		{
			files_to_remove.insert({ std::filesystem::last_write_time(iter.path(), code), iter.path() });
		}

		n++;
	}

	// now remove the files
	for (const auto& [time, log_file] : files_to_remove)
	{
		if (!std::filesystem::remove(log_file, code))
		{
			print_locally(EOutputCategory::ERROR, std::format("Failed to remove old log file! '{}'", log_file));
			print_locally(EOutputCategory::ERROR, std::format("Error message: {}", code.message()));
		}
		else
		{
			print_locally(EOutputCategory::INFO, std::format("Removed old log file '{}'", log_file.filename()));
		}
	}

	print_locally(EOutputCategory::INFO, "Done.");
}

int CDeveloperConsole::text_edit_callback(ImGuiInputTextCallbackData* data)
{
	switch (data->EventFlag)
	{
		case ImGuiInputTextFlags_CallbackCompletion:
		{
			break;
		}
		case ImGuiInputTextFlags_CallbackHistory:
		{
			static int history_pos = 0;
			switch (data->EventKey)
			{
				case ImGuiKey_UpArrow:
				{
					history_pos++;

					if (history_pos > m_entered_commands.size() - 1)
					{
						history_pos = 0;
					}
					break;
				}
				case ImGuiKey_DownArrow:
				{
					history_pos--;

					if (history_pos < 0)
					{
						history_pos = m_entered_commands.size() - 1;
					}
					break;
				}
			}

			auto& item = m_entered_commands.at(history_pos);

			g_gui_widgets_i->delete_chars_on_textinput_buffer(data, 0, data->BufTextLen);
			g_gui_widgets_i->insert_chars_to_textinput_buffer(data, 0, item.c_str());

			break;
		}
	}
	return 0;
}

std::string CDeveloperConsole::line_preamble(const output_line_t& line)
{
	return std::format("[{}]", CStringTools::the().format_timestamp_log(line.time));
}

void CDeveloperConsole::log(const output_line_t& line)
{
	if (m_logfile_output_stream.bad())
	{
		return;
	}

	m_logfile_output_stream << line_preamble(line) << "[" << outputmodule_as_string(line.module) << "]";

	const char* which[] = { "", "error: ", "warning: " };

	if (line.category != EOutputCategory::INFO)
	{
		m_logfile_output_stream << which[(int)line.category];
	}

	m_logfile_output_stream << line.contents;

	std::flush(m_logfile_output_stream);
}
