#pragma once

#define USE_API(NAME, API) static auto proc_##API = (decltype(&API))(GetProcAddress(GetModuleHandle(L#NAME), #API))