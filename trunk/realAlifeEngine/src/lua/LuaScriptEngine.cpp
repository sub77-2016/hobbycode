#include "LuaScriptEngine.h"
/*#include "modules_wrap.cxx"*/

#ifndef LUA_EXTRALIBS
#define LUA_EXTRALIBS	/* empty */
#endif

extern int luaopen_ALifeSim(lua_State* L);

static const luaL_reg lualibs[] = {
  {"base", luaopen_base},
  {"table", luaopen_table},
  {"io", luaopen_io},
  {"string", luaopen_string},
  {"math", luaopen_math},
  {"debug", luaopen_debug},
  /*{"loadlib", luaopen_loadlib},*/
  /* add your libraries here */
  {"ALifeSim", luaopen_ALifeSim},
  LUA_EXTRALIBS
  {NULL, NULL}
};

template<class LuaScriptEngine> LuaScriptEngine* Singleton<LuaScriptEngine>::ms_Singleton = 0;
/**
 * Run the given script.
 */
int LuaScriptEngine::runFile(const char *fileName) 
{
	/* run the script */
    return luaL_dofile( m_pLuaVM, fileName );
}

/**
 * Run the given string.
 */
int LuaScriptEngine::runString(const char *strLuaInput) 
{
	/* run the string */
	return luaL_dostring(m_pLuaVM, strLuaInput);
}

int LuaScriptEngine::loadFile(const char *fileName) 
{
	int error = luaL_loadfile(m_pLuaVM, fileName);
  	if (!error)
    	lua_pcall(m_pLuaVM, 0, 0, 0);

  	return error;
}

int LuaScriptEngine::loadBuffer(const char *buff) 
{
  	int error = luaL_loadbuffer(m_pLuaVM, buff, strlen(buff), "line");
  	if (!error)
    	lua_pcall(m_pLuaVM, 0, 0, 0);

  	return error;
}

char *LuaScriptEngine::report() 
{
  	char *msg = (char *)malloc(1024*sizeof(char));
  	//sprintf(msg, "%s", lua_tostring(m_pLuaVM, -1));
  	strcpy(msg, lua_tostring(m_pLuaVM, -1));
  	lua_pop(m_pLuaVM, 1);  /* pop error message from the stack */

  	return msg;
}

LuaScriptEngine::LuaScriptEngine() 
{
	m_pLuaVM = NULL;
}

LuaScriptEngine::~LuaScriptEngine() 
{
	unloadEngine();
}

bool LuaScriptEngine::loadEngine() 
{
	m_pLuaVM = lua_open();

	if(NULL == m_pLuaVM) {
		//serious problem, exit program
		return false;
	}

	/**
	 * Load all modules (EXPERIMENT)
	 * */
  	const luaL_reg *lib = lualibs;
  	for (; lib->func; lib++) {
    	lib->func(m_pLuaVM);  /* open library */
    	lua_settop(m_pLuaVM, 0);  /* discard any results */
  	}
  	
	return true;
}

void LuaScriptEngine::unloadEngine() 
{
	//do our script processing here.
	//now we are finished, close off Lua
		
	// The latest version of Lua seems to give me nasty exceptions when
	// explicitly calling this function directly. I've looked through
	// the documentation, and all I can see about it is that it's
	// automatically internally called when performing cleanup. This
	// seems to fix my generated exceptions, so there you go.
	// lua_close( m_pLuaVM );
}
