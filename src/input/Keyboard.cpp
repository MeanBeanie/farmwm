#include <LKeyboardKeyEvent.h>
#include <LSessionLockManager.h>
#include <LLauncher.h>
#include <linux/input-event-codes.h>
#include "Keyboard.h"
#include "../utils/Global.h"
#include "../scene/Scene.h"
#include "../utils/Settings.h"
#include "LClient.h"
#include "LCompositor.h"
#include "LCursor.h"
#include "LNamespaces.h"
#include "LOutput.h"
#include "src/Config.hpp"

Keyboard::Keyboard(const void *params) noexcept : LKeyboard(params)
{
    /* Key press repeat rate */
    setRepeatInfo(32, 500);

    /* Keymap sent to clients and used by the compositor, check the LKeyboard class or XKB doc */
    setKeymap(
        nullptr,  // Rules
        nullptr,  // Model
        nullptr,  // Layout
        nullptr); // Options
}

void Keyboard::keyEvent(const LKeyboardKeyEvent &event)
{
    G::scene().handleKeyboardKeyEvent(event, SETTINGS_SCENE_EVENT_OPTIONS);

		LOutput* currentOutput = compositor()->mostIntersectedOutput(compositor()->cursor()->rect());

		for(BoundKey boundKey : conf.boundKeys){
			if(event.keyCode() == boundKey.keybind.keycode
				 && (isKeyCodePressed(KEY_LEFTSHIFT) || isKeyCodePressed(KEY_RIGHTSHIFT)) == boundKey.keybind.modifiers[0]
				 && (isKeyCodePressed(KEY_LEFTALT) || isKeyCodePressed(KEY_RIGHTALT)) == boundKey.keybind.modifiers[1]
				 && (isKeyCodePressed(KEY_LEFTCTRL) || isKeyCodePressed(KEY_RIGHTCTRL)) == boundKey.keybind.modifiers[2]
				 && (isKeyCodePressed(KEY_LEFTMETA) || isKeyCodePressed(KEY_RIGHTMETA)) == boundKey.keybind.modifiers[3]
				 && event.state() == LKeyboardKeyEvent::Released){
				if(boundKey.action.isSystem){
					if(boundKey.action.act == "exit"){
						compositor()->finish();
					}
					else if(boundKey.action.act == "quit"){
						if(focus()){
							focus()->client()->destroyLater();
						}
					}
					else if(focus() && boundKey.action.act == "tile left"){
						focus()->setPos(conf.edgeOffsets[3], conf.edgeOffsets[0]);
						focus()->toplevel()->configureSize((currentOutput->size().width()/2)-conf.edgeOffsets[3], currentOutput->size().height()-20-conf.edgeOffsets[2]);
					}
					else if(focus() && boundKey.action.act == "tile right"){
						focus()->setPos((currentOutput->size().width()/2)-conf.edgeOffsets[1], conf.edgeOffsets[0]);
						focus()->toplevel()->configureSize((currentOutput->size().width()/2)-conf.edgeOffsets[1], currentOutput->size().height()-20-conf.edgeOffsets[2]);
					}
					else if(focus() && boundKey.action.act == "tile up"){
						focus()->setPos(conf.edgeOffsets[3], conf.edgeOffsets[0]);
						focus()->toplevel()->configureSize(currentOutput->size().width()-conf.edgeOffsets[1], (currentOutput->size().height()/2)-conf.edgeOffsets[0]); 
					}
					else if(focus() && boundKey.action.act == "tile down"){
						focus()->setPos(conf.edgeOffsets[3], (currentOutput->size().height()/2));
						focus()->toplevel()->configureSize(currentOutput->size().width()-conf.edgeOffsets[1], (currentOutput->size().height()/2)-conf.edgeOffsets[2]);
					}
				}
				else{
					Louvre::LLauncher::launch(boundKey.action.act);
				}
			}
		}

		// panic escape
		if(event.keyCode() == KEY_ESC
							&& isKeyCodePressed(KEY_LEFTSHIFT)
							&& isKeyCodePressed(KEY_LEFTALT)
							&& event.state() == LKeyboardKeyEvent::Released){
			compositor()->finish();
		}
}
