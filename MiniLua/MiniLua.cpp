/*
* Copyright (c) 2015-2019 JlnWntr (jlnwntr@gmail.com)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#ifndef MINI_LUA_H
#include "MiniLua.hpp"
#endif
#ifdef LUA_ADAPTER_DEBUG
#include <iostream>
#define LUA_ADAPTER_PREFIX "Lua > "
#endif

MiniLua::MiniLua(const std::string &filename) : Lua{nullptr} {
  this->Lua = luaL_newstate();
  luaL_openlibs(this->Lua);
  this->Load(filename);
}

MiniLua::MiniLua() : Lua{nullptr} {
  this->Lua = luaL_newstate();
  luaL_openlibs(this->Lua);
}

bool MiniLua::Load(const std::string &filename){
  if (this->Lua && (filename.length() > 0)) {
    luaL_loadfile(this->Lua, filename.c_str());
    if (lua_pcall(this->Lua, 0, 0, 0) == LUA_OK)
      return true;
  }
  return false;
}

bool MiniLua::GetGlobal(const char *name) {
  if ((!this->Lua) || (!name))
    return false;
  lua_getglobal(this->Lua, name);
  return true;
}

bool MiniLua::Get(const char *name, int &result) {
  if (this->GetGlobal(name) == false)
    return false;
  if (lua_isnumber(this->Lua, -1) == false) {
    lua_pop(this->Lua, 1);
    return false;
  }
  result = lua_tointeger(this->Lua, -1);
  lua_pop(this->Lua, 1);
  return true;
}

bool MiniLua::Get(const char *name, std::string &result) {
  if (this->GetGlobal(name) == false)
    return false;
  if (lua_type(this->Lua, -1) != LUA_TSTRING) {
    lua_pop(this->Lua, 1);
    return false;
  }
  result = lua_tostring(this->Lua, -1);
  lua_pop(this->Lua, 1);
  return true;
}

bool MiniLua::Get(const char *name, double &result) {
  if (this->GetGlobal(name) == false)
    return false;
  if (lua_isnumber(this->Lua, -1) == false) {
    lua_pop(this->Lua, 1);
    return false;
  }
  result = lua_tonumber(this->Lua, -1);
  lua_pop(this->Lua, 1);
  return true;
}

bool MiniLua::Get(const char *name, float &result) {
  double temp {};
  if (this->Get(name, temp) == false)
    return false;
  result = (float){(float)temp};
  return true;
}

bool MiniLua::Get(const char *name, bool &result) {
  if (this->GetGlobal(name) == false)
    return false;
  if (lua_isboolean(this->Lua, -1) == false) {
    lua_pop(this->Lua, 1);
    return false;
  }
  result = lua_toboolean(this->Lua, -1);
  lua_pop(this->Lua, 1);
  return true;
}

bool MiniLua::Call(const char *f, const int c, const int *a, int &r) {
    if (not this->Lua or not f or not(lua_getglobal(this->Lua, f) == LUA_TFUNCTION))
      return false;
    for (auto i = 0; i < c; i++)
      if (a + i)
        lua_pushinteger(this->Lua, a[i]);

    if (lua_pcall(this->Lua, c, 1, 0) != LUA_OK) {
#ifdef LUA_ADAPTER_DEBUG
      std::cerr << LUA_ADAPTER_PREFIX << "Error: pcall failed. ";
      std::cerr << lua_tostring(this->Lua, -1) << "'\n";
#endif
      lua_pop(this->Lua, 1);
      return false;
    }
    if (lua_isinteger(this->Lua, -1))
      r = lua_tointeger(this->Lua, -1);
    else if ((lua_isboolean(this->Lua, -1))
    and (lua_toboolean(this->Lua, -1)))
      r = 1;
    lua_pop(this->Lua, 1);
    return true;
}

bool MiniLua::Call(const char *f, const int a) {
    if( not this->Lua or not f or not a
    or not(lua_getglobal(this->Lua, f) == LUA_TFUNCTION))
      return false;
    lua_pushinteger(this->Lua, a);
    if(lua_pcall(this->Lua, 1, 0, 0) == LUA_OK)
      return true;
#ifdef LUA_ADAPTER_DEBUG
    std::cerr << LUA_ADAPTER_PREFIX << "Error: pcall failed. ";
    std::cerr << lua_tostring(this->Lua, -1) << "'\n";
#endif
    lua_pop(this->Lua, 1);
    return false;
}

bool MiniLua::Call(const char *f) {
    if (not this->Lua or not f or not(lua_getglobal(this->Lua, f) == LUA_TFUNCTION))
      return false;
    if (lua_pcall(this->Lua, 0, 0, 0) == LUA_OK)
      return true;
#ifdef LUA_ADAPTER_DEBUG
    std::cerr << LUA_ADAPTER_PREFIX << "Error: pcall failed. ";
    std::cerr << lua_tostring(this->Lua, -1) << "'\n";
#endif
    lua_pop(this->Lua, 1);
    return false;
}

bool MiniLua::Push(Lua_callback_function function, const char *name) {
    if (not this->Lua or not name)
      return false;
    lua_pushcfunction(this->Lua, function);
    lua_setglobal(this->Lua, name);
    return true;
  }

void MiniLua::Close() {
  if (!this->Lua)
    return;
  lua_close(this->Lua);
  this->Lua = nullptr;
}

MiniLua::~MiniLua() { this->Close(); }
