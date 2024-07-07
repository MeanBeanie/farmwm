#include "Config.hpp"
#include "LCompositor.h"
#include "LNamespaces.h"
#include "LOutput.h"
#include <LLog.h>
#include <cstdlib>

void Config::initialize(){
	std::string home = getenv("HOME");
	auto data = toml::parse(home+"/.config/farmwm/config.toml");

	Louvre::LLog::log("Found and loaded config file located at %s/.config/farmwm/config.toml", home.c_str());

	onStart = toml::find<std::vector<std::string>>(data, "on-start");
	edgeOffsets = toml::find<std::vector<int>>(data, "offset");
	outputSettings = toml::find<toml::value>(data, "output");

	bool hasGlobalOutputSettings = outputSettings.as_table().contains("all");

	for(Louvre::LOutput* output : Louvre::compositor()->outputs()){
		const char* name = output->name();
		if(outputSettings.as_table().contains(name)){
			if(outputSettings[name].contains("scale")){
				output->setScale(outputSettings[name]["scale"].as_floating());
			}
		}
		else if(hasGlobalOutputSettings){
			if(outputSettings["all"].contains("scale")){
				output->setScale(outputSettings["all"]["scale"].as_floating());
			}
		}
	}

	toml::value keybindings = toml::find<toml::value>(data, "keybinds");
	if(keybindings.is_table()){
		for(auto & [ key, value ] : keybindings.as_table()) {
			Keybind keybind;
			std::string keySection;
			for(std::string::size_type i = 0; i < key.size(); i++){
				if(i+1 == key.size()){
					keySection.push_back(key.at(i));
				}
				if(key[i] == '+' || i+1 == key.size()){	
					if(i+1 != key.size()){
						if(keySection == "Shift"){
							keybind.modifiers[0] = true;
						}
						else if(keySection == "Alt"){
							keybind.modifiers[1] = true;
						}
						else if(keySection == "Ctrl"){
							keybind.modifiers[2] = true;
						}
						else if(keySection == "Super"){
							keybind.modifiers[3] = true;
						}
						else{
							Louvre::LLog::error("Failed to recognize modifer key(s): %s", keySection.c_str());
							exit(EXIT_FAILURE);
						}
					}
					else{
						if(keycodeMap.find(keySection) != keycodeMap.end()){
							keybind.keycode = keycodeMap[keySection];
						}
						else{
							Louvre::LLog::error("Failed to recognize key: %s", keySection.c_str());
							exit(EXIT_FAILURE);
						}
					}
					keySection.clear();
				}
				else{
					keySection.push_back(key.at(i));
				}
			}
			Action action;
			if(value.as_string().size() >= 6 && value.as_string().substr(0, 5) == "exec "){
				action.isSystem = false;
				action.act = value.as_string().substr(5);
				if(action.act.empty()){
					Louvre::LLog::error("Did not find shell command after exec: %s", value.as_string().c_str());
					exit(EXIT_FAILURE);
				}
			}
			else{
				action.isSystem = true;
				action.act = value.as_string();
			}

			BoundKey bk;
			bk.action = action;
			bk.keybind = keybind;
			boundKeys.push_back(bk);
			Louvre::LLog::log("Bound key %d with modifiers {SHIFT: %d, ALT: %d, CTRL: %d, SUPER: %d} with action %s", bk.keybind.keycode, bk.keybind.modifiers[0], bk.keybind.modifiers[1], bk.keybind.modifiers[2], bk.keybind.modifiers[3], bk.action.act.c_str());
		} // for key, value in keybindings
	}
}
