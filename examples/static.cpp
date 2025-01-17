#include "../LuaFunction.hpp" // » needs -fconcepts

/**
 * $ luac -s test.lua
 * $ xxd -i luac.out > test.h
 * $ g++-8 static.cpp /path/to/lua-5.3.5/src/liblua.a -Wall -ldl -fconcepts -std=c++17
 */
#include "test.h" // « generated via xxd

int main() {
  LuaAdapter lua{};
  LuaFunction f{lua};

  lua.Load((const char *)luac_out, luac_out_len);

  f.Call("Print", (const std::string)"hello");

  return 0;
}