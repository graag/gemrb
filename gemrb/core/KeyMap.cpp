/* GemRB - Infinity Engine Emulator
 * Copyright (C) 2009 The GemRB Project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 */

#include "KeyMap.h"
#include "Interface.h"
#include "Logging/Logging.h"
#include "TableMgr.h"
#include "ScriptEngine.h"
#include "Streams/FileStream.h"

namespace GemRB {

Function::Function(const ieVariable& m, const ieVariable& f, int g, int k)
{
	// make sure the module and function names are no longer than 32 characters, or they will be truncated
	moduleName = m;
	function = f;
	group = g;
	key = k;
}

KeyMap::KeyMap()
{
	keymap.SetType(GEM_VARIABLES_POINTER);
}

static void ReleaseFunction(void *fun)
{
	delete (Function *) fun;
}

KeyMap::~KeyMap()
{
	keymap.RemoveAll(ReleaseFunction);
}

bool KeyMap::InitializeKeyMap(const char* inifile, const ResRef& tablefile)
{
	AutoTable kmtable = gamedata->LoadTable(tablefile);

	if (!kmtable) {
		return false;
	}

	char tINIkeymap[_MAX_PATH];
	PathJoin(tINIkeymap, core->config.GamePath, inifile, nullptr);
	FileStream* config = FileStream::OpenFile( tINIkeymap );

	if (config == NULL) {
		Log(WARNING, "KeyMap", "There is no '{}' file...", inifile);
		return false;
	}

	const ieVariable defaultModuleName = kmtable->QueryField("Default", "MODULE");
	const ieVariable defaultFunction = kmtable->QueryField("Default", "FUNCTION");
	int defaultGroup = kmtable->QueryFieldSigned<int>("Default", "GROUP");
	std::string line;
	while (config->ReadLine(line) != DataStream::Error) {
		if (line.length() == 0 ||
			(line[0] == '#') ||
			(line[0] == '[') ||
			(line[0] == '\r') ||
			(line[0] == '\n') ||
			(line[0] == ';')) {
			continue;
		}
		
		StringToLower(line);
		auto parts = Explode<std::string, std::string>(line, '=', 1);
		if (parts.size() < 2) {
			parts.emplace_back();
		}
		
		auto& val = parts[1];
		if (val.length() == 0) continue;
		LTrim(val);

		if (val.length() > 1 || keymap.HasKey(val)) {
			Log(WARNING, "KeyMap", "Ignoring key {}", val);
			continue;
		}
		
		auto& key = parts[0];
		RTrim(key);
		//change internal spaces to underscore
		std::replace(key.begin(), key.end(), ' ', '_');

		ieVariable moduleName;
		ieVariable function;
		int group;

		if (kmtable->GetRowIndex(key) != TableMgr::npos) {
			moduleName = kmtable->QueryField(key, "MODULE");
			function = kmtable->QueryField(key, "FUNCTION");
			group = kmtable->QueryFieldSigned<int>(key, "GROUP");
		} else {
			moduleName = defaultModuleName;
			function = defaultFunction;
			group = defaultGroup;
		}
		Function *fun = new Function(moduleName, function, group, val[0]);

		// lookup by either key or name
		keymap.SetAt(val, fun);
		keymap.SetAt(key, new Function(*fun));
	}
	delete config;
	return true;
}

//group can be:
//main gamecontrol
bool KeyMap::ResolveKey(unsigned short key, int group) const
{
	// FIXME: key is 2 bytes, but we ignore one. Some non english keyboards won't like this.
	char keystr[2] = {(char)key, 0};
	Log(MESSAGE, "KeyMap", "Looking up key: {}({}) ", key, keystr);

	return ResolveName(keystr, group);
}

bool KeyMap::ResolveName(const char* name, int group) const
{
	void *tmp;
	if (!keymap.Lookup(Variables::key_t(name), tmp)) {
		return false;
	}

	Function* fun = (Function *)tmp;

	if (fun->group!=group) {
		return false;
	}

	Log(MESSAGE, "KeyMap", "RunFunction({}::{})", fun->moduleName, fun->function);
	core->GetGUIScriptEngine()->RunFunction(fun->moduleName.c_str(), fun->function.c_str());
	return true;
}

Function* KeyMap::LookupFunction(std::string key)
{
	// FIXME: Variables::MyCompareKey is already case insensitive AFICT
	StringToLower(key);

	void *tmp;
	if (!keymap.Lookup(key, tmp) ) {
		return nullptr;
	}

	return (Function *)tmp;
}

}
