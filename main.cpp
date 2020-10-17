/*
* Hex-Rays MicroCode Explorer
* Ported from https://github.com/RolfRolles/HexRaysDeob
*
* Pedro Vilaça - reverser@put.as - https://reverse.put.as
*
* Implements a menu item on pseudo code view
*
* It generates microcode for selection and dumps it to the output window.
* Reference: https://www.hex-rays.com/blog/hex-rays-microcode-api-vs-obfuscating-compiler/#appendix
*
* Alternative microcode explorer with different features is Lucid
* https://github.com/gaasedelen/lucid
* https://blog.ret2.io/2020/09/11/lucid-hexrays-microcode-explorer/
*
* This uses the new C++ plugin API so it's only >= IDA 7.5 compatible
*
* Based on IDA SDK ht_view sample plugin
*
*/

#include <set>
#define USE_DANGEROUS_FUNCTIONS 
#include <hexrays.hpp>
#include <kernwin.hpp>
#include <idp.hpp>
#include "HexRaysUtil.hpp"
#include "MicrocodeExplorer.hpp"
#include "Config.hpp"

#if IDA_SDK_VERSION < 750
#error This code requires IDA SDK 7.5 or higher
#endif

#define VERSION "1.0"

/// Pointer to Hex-Rays decompiler dispatcher.
/// This variable must be instantiated by the plugin. It is initialized by init_hexrays_plugin().
hexdsp_t *hexdsp = NULL;

#define ACTION_NAME "mce:ViewMicrocode"
#define ACTION_LABEL "Start MicrocodeExplorer"

struct microcode_ah_t : public action_handler_t
{
    // the menu item was selected so we can launch the explorer itself
    virtual int idaapi activate(action_activation_ctx_t *) override
    {
        ShowMicrocodeExplorer();
        return true;
    }

    virtual action_state_t idaapi update(action_update_ctx_t *) override
    {
        return AST_ENABLE_ALWAYS;
    }
};

struct microcode_ctx_t : public plugmod_t
{
    bool hooked = false;
    
    microcode_ah_t ah;
    const action_desc_t main_action = ACTION_DESC_LITERAL_PLUGMOD(
        ACTION_NAME,
        ACTION_LABEL,
        &ah,
        this,
        "Ctrl+Shift+X",
        "Open the MicrocodeExplorer",
        -1);

    microcode_ctx_t();
    ~microcode_ctx_t();
    
    virtual bool idaapi run(size_t arg) override;
};

//---------------------------------------------------------------------------
// Callback for ui notifications
static ssize_t idaapi ui_callback(void *ud, int notification_code, va_list va)
{
    switch (notification_code)
    {
        // called when IDA is preparing a context menu for a view
        // Here dynamic context-depending user menu items can be added.
        case ui_finish_populating_widget_popup:
        {
            TWidget *view = va_arg(va, TWidget *);
            // BWN_PSEUDOCODE is for hex-rays output view
            // check include/kernwin.hpp for available window types
            if ( get_widget_type(view) == BWN_PSEUDOCODE )
            {
                TPopupMenu *p = va_arg(va, TPopupMenu *);
                microcode_ctx_t *plgmod = (microcode_ctx_t *) ud;
                attach_action_to_popup(view, p, ACTION_NAME);
            }
            break;
        }
    }
    return 0;
}

//--------------------------------------------------------------------------
plugmod_t * idaapi init(void)
{
    if (is_idaq() == false) {
        return nullptr;
    }
	if (!init_hexrays_plugin()) {
        msg("--------------------------------------------\n");
        msg("Microcode Explorer %s\n", VERSION);
        msg("No Hex-Rays decompiler found, plugin not loaded.\n");
        msg("--------------------------------------------\n");
        return nullptr;
    }

    const char *hxver = get_hexrays_version();
    msg("--------------------------------------------\n");
    msg("Microcode Explorer %s\n", VERSION);
    msg("Hex-Rays version %s has been detected, plugin is ready to use\n", hxver);
    msg("--------------------------------------------\n");

	return new microcode_ctx_t;
}

//--------------------------------------------------------------------------
microcode_ctx_t::microcode_ctx_t() 
{
    register_action(main_action);
    /* set callback for view notifications */
    if (!hooked) {
        hook_to_notification_point(HT_UI, ui_callback, this);
        hooked = true;
    }
}

microcode_ctx_t::~microcode_ctx_t()
{
    unhook_from_notification_point(HT_UI, ui_callback, this);
    if (hexdsp != NULL) {
        term_hexrays_plugin();
    }
}

//--------------------------------------------------------------------------
bool idaapi microcode_ctx_t::run(size_t)
{
	return true;
}

//--------------------------------------------------------------------------
static const char comment[] = "Explore Hex-Rays microcode";

//--------------------------------------------------------------------------
//
//      PLUGIN DESCRIPTION BLOCK
//
//--------------------------------------------------------------------------
plugin_t PLUGIN =
{
	IDP_INTERFACE_VERSION,
	PLUGIN_UNL | PLUGIN_MULTI, // plugin flags
	init,                      // initialize
	nullptr,                   // terminate. this pointer may be NULL.
	nullptr,                   // invoke plugin
	comment,                   // long comment about the plugin
						       // it could appear in the status line
						       // or as a hint
	"",                        // multiline help about the plugin
	"Microcode Explorer",      // the preferred short name of the plugin
	""                         // the preferred hotkey to run the plugin
};
