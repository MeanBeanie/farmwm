#pragma once
#include <linux/input-event-codes.h>
#include <string>
#include <map>
#include <vector>
#include "external/toml.hpp"

struct Keybind {
	Keybind(){}

	unsigned int keycode;
	/* Does not consider right or left
		 Goes in order {Shift, Alt, Control, Meta/Super} */
	bool modifiers[4] = {0};
};

struct Action {
	Action(){}

	/* Determines whether the given act is a compositor command(e.g exit)
		 or a shell command. Shell command is most used to launch programs */
	bool isSystem;
	std::string act;
};

struct BoundKey {
	BoundKey(){}

	Keybind keybind;
	Action action;
};

class Config {
	public:
		Config(){};
		void initialize();

		// stores all listed keybinds in the config file
		std::vector<BoundKey> boundKeys;
		// stores all shell commands to be launched on load
		std::vector<std::string> onStart;
		// stores all settings for outputs(screens)
		toml::value outputSettings;
		// stores screen edge offsets
		std::vector<int> edgeOffsets = {0, 0, 0, 0};
	
	private:
		std::map<std::string, int> keycodeMap = {
        {"0", KEY_0}, {"1", KEY_1}, {"2", KEY_2}, {"3", KEY_3}, {"4", KEY_4},
        {"5", KEY_5}, {"6", KEY_6}, {"7", KEY_7}, {"8", KEY_8}, {"9", KEY_9},
        {"A", KEY_A}, {"B", KEY_B}, {"C", KEY_C}, {"D", KEY_D}, {"E", KEY_E},
        {"F", KEY_F}, {"G", KEY_G}, {"H", KEY_H}, {"I", KEY_I}, {"J", KEY_J},
        {"K", KEY_K}, {"L", KEY_L}, {"M", KEY_M}, {"N", KEY_N}, {"O", KEY_O},
        {"P", KEY_P}, {"Q", KEY_Q}, {"R", KEY_R}, {"S", KEY_S}, {"T", KEY_T},
        {"U", KEY_U}, {"V", KEY_V}, {"W", KEY_W}, {"X", KEY_X}, {"Y", KEY_Y},
        {"Z", KEY_Z}, {"Enter", KEY_ENTER}, {"Backspace", KEY_BACKSPACE}, 
				{"Tab", KEY_TAB}, {"F1", KEY_F1}, {"F2", KEY_F2}, {"F3", KEY_F3},
				{"F4", KEY_F4}, {"F5", KEY_F5}, {"F6", KEY_F6}, {"F7", KEY_F7},
				{"F8", KEY_F8}, {"F9", KEY_F9}, {"F10", KEY_F10}, {"F11", KEY_F11},
				{"F12", KEY_F12}, {"Right", KEY_RIGHT}, {"Left", KEY_LEFT}, {"Up", KEY_UP},
				{"Down", KEY_DOWN}
    };
};

inline Config conf;
