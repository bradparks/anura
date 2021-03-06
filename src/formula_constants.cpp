/*
	Copyright (C) 2003-2013 by David White <davewx7@gmail.com>
	
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <ctype.h>

#include "asserts.hpp"
#include "controls.hpp"
#include "decimal.hpp"
#include "foreach.hpp"
#include "formula_constants.hpp"
#include "i18n.hpp"
#include "key_button.hpp"
#include "preferences.hpp"
#include "raster.hpp"
#include "string_utils.hpp"
#include "variant_utils.hpp"

namespace game_logic
{

namespace {
typedef std::map<std::string, variant> constants_map;
std::vector<constants_map> constants_stack;
}

variant get_constant(const std::string& id)
{
	if(id == "DOUBLE_SCALE") {
		return variant(preferences::double_scale());
	} else if(id == "SCREEN_WIDTH") {
		return variant(graphics::screen_width());
	} else if(id == "SCREEN_HEIGHT") {
		return variant(graphics::screen_height());
	} else if(id == "LOW_END_SYSTEM") {
#if TARGET_OS_HARMATTAN || TARGET_OS_IPHONE || TARGET_BLACKBERRY || defined(__ANDROID__)
		return variant(1);
#else
		return variant(0);
#endif
	} else if(id == "IPHONE_SYSTEM") {
#if TARGET_OS_HARMATTAN || TARGET_OS_IPHONE || TARGET_BLACKBERRY || defined(__ANDROID__)
		return variant(1);
#else
		return variant(preferences::sim_iphone() ? 1 : 0);
#endif
	} else if(id == "HIGH_END_SYSTEM") {
		return variant(!get_constant("LOW_END_SYSTEM").as_bool());
	} else if(id == "TBS_SERVER_ADDRESS") {
		return variant(preferences::get_tbs_uri().host());
	} else if(id == "TBS_SERVER_PORT") {
		return variant(atoi(preferences::get_tbs_uri().port().c_str()));
	} else if(id == "USERNAME") {
		return variant(preferences::get_username());
	} else if(id == "PASSWORD") {
		return variant(preferences::get_password());
	} else if(id == "UP_KEY") {
		return variant(gui::get_key_name(controls::get_keycode(controls::CONTROL_UP)));
	} else if(id == "DOWN_KEY") {
		return variant(gui::get_key_name(controls::get_keycode(controls::CONTROL_DOWN)));
	} else if(id == "LEFT_KEY") {
		return variant(gui::get_key_name(controls::get_keycode(controls::CONTROL_LEFT)));
	} else if(id == "RIGHT_KEY") {
		return variant(gui::get_key_name(controls::get_keycode(controls::CONTROL_RIGHT)));
	} else if(id == "JUMP_KEY") {
		return variant(gui::get_key_name(controls::get_keycode(controls::CONTROL_JUMP)));
	} else if(id == "TONGUE_KEY") {
		return variant(gui::get_key_name(controls::get_keycode(controls::CONTROL_TONGUE)));
	} else if(id == "ATTACK_KEY") {
		return variant(gui::get_key_name(controls::get_keycode(controls::CONTROL_ATTACK)));
	} else if(id == "LOCALE") {
		return variant(i18n::get_locale());
	} else if(id == "EPSILON") {
		return variant(decimal::epsilon());
	} else if(id == "HEX_DIRECTIONS") {
		std::vector<variant> v;
		v.push_back(variant("n"));
		v.push_back(variant("ne"));
		v.push_back(variant("se"));
		v.push_back(variant("s"));
		v.push_back(variant("sw"));
		v.push_back(variant("nw"));
		return variant(&v);
	} else if(id == "BUILD_OPTIONS") {
		std::vector<variant> v;
		for(auto bo : preferences::get_build_options()) {
			v.push_back(variant(bo));
		}
		return variant(&v);
	}

	for(auto i = constants_stack.rbegin(); i != constants_stack.rend(); ++i) {
		constants_map& m = *i;
		constants_map::const_iterator itor = m.find(id);
		if(itor != m.end()) {
			return itor->second;
		}
	}

	return variant();
}

constants_loader::constants_loader(variant node) : same_as_base_(false)
{
	constants_map m;
	if(node.is_null() == false) {
		foreach(variant key, node.get_keys().as_list()) {
			const std::string& attr = key.as_string();
			if(std::find_if(attr.begin(), attr.end(), util::c_islower) != attr.end()) {
				//only all upper case are loaded as consts
				continue;
			}

			m[attr] = node[key];
		}
	}

	if(constants_stack.empty() == false && constants_stack.back() == m) {
		same_as_base_ = true;
	}

	constants_stack.push_back(m);
}

constants_loader::~constants_loader()
{
	ASSERT_EQ(constants_stack.empty(), false);
	constants_stack.pop_back();
	//std::cerr << "REMOVE CONSTANTS_STACK\n";
}

}
