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

#ifndef OXWARE_H
#define OXWARE_H
#pragma once

extern VarBoolean debug;

extern uintptr_t g_cheat_dll_base;

class COxWare
{
public:
	DECL_BASIC_CLASS(COxWare);

public:
	bool run(injector_information_package_t* ifp);
	void shutdown();

	bool initialize_phase2(); // called after the engine is fully initialized. (host_initialized set to 1)

	// if called, we exit in main loop next frame.
	void end_cheat_execution(bool dont_write_unloading_code = false, bool game_exiting_or_restarting = false)
	{
		m_requested_to_exit_cheat = true; 
		m_dont_write_unloading_code = dont_write_unloading_code;
		m_game_exiting_or_restarting = game_exiting_or_restarting;
	}

	inline bool is_fully_initialized() const { return m_fully_initialized; }

	inline uintptr_t get_cheat_dll_base_address() const { return g_cheat_dll_base; }

	auto get_injection_technique() const { return m_ifp->m_ipc_block_ptr->m_technique; }

	inline session_id_t get_current_session_id() const { assert(m_ifp); return m_ifp->m_current_session_id; }

	inline bool is_exit_requested() const { return m_requested_to_exit_cheat; };
	inline bool is_cheat_exiting() { return is_exit_requested() || m_unloading_cheat; }
	inline bool is_shutted_down() { return m_shutted_down; }

	// this is set when the cheat is shutting down and if the game is exiting/restarting.
	inline bool is_game_exiting_or_restarting() const { return m_game_exiting_or_restarting; }

	int get_build_number() const { return m_gs_build_number; }

	bool at_least_once_focused() const { return m_at_least_once_focused; }
	void set_at_least_once_focused() { m_at_least_once_focused = true; }

	// the name varies from build to build
	const wchar_t* get_engine_fs_module_name() const;

private:
	bool initialize();
	bool run_frame();

	bool load_and_initialize_dependencies();
	void unload_dependencies();

	bool initialize_hook_managers();
	void shutdown_hook_managers();

	void remove_inverted_function_table();

	void check_for_clientside_protectors();
	bool is_valid_game();
	bool is_hardware();
	bool validate_engine_build();

	// check for valve blob encryption
	bool check_for_encrypted_modules();

private:
	injector_information_package_t* m_ifp;

	bool m_requested_to_exit_cheat = false;
	bool m_unloading_cheat = false;

	bool m_shutted_down = false;

	bool m_fully_initialized = false;

	// this is set when the cheat is unloaded from another thread, i.e. remotely from the detoured function, for example.
	// this allows the remote function itself to tell the loader that we're unloading, instead of it being told after the
	// shutdown.
	bool m_dont_write_unloading_code = false;

	int m_gs_build_number = 0;

	bool m_at_least_once_focused = false;

	bool m_game_exiting_or_restarting = false;

	// true if running version of GoldSrc that has encrypted modules.
	bool m_is_running_encrypted_game = false;
};

// helper class to sleep the engine whenever we want
// sometimes, when unloading a cheat, engine can call a function that we hook in a middle of
// that function being unloaded, this is totally random and results in a crash.
// this class assures that we first wait for the engine to hang, hence not call any functions
// at all, and after that we unload the cheat.
//
// this also requires the function where the engine hangs to be unloaded as last function.
class CEngineSynchronization
{
public:
	DECL_BASIC_CLASS(CEngineSynchronization);

public:
	// hang our code until engine acknowledges
	void put_engine_in_sleep();

	void resume_engine();

	// run inside engine frame function
	bool engine_frame();

private:
	void hang_till_engine_sleeps();
	void hang_till_engine_resumes_from_sleep();

	inline static constexpr uint32_t k_time_to_acknowledge_sleep = 20 * 1000;

private:
	bool m_engine_is_sleeping = false;

	bool m_engine_should_sleep = false;
};

#endif // OXWARE_H