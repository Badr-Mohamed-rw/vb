// ============================================================
//  Single-file SA-MP plugin — никаких внешних SDK не требуется.
//  Компиляция:
//    Windows (MSVC):  cl /LD /O2 /EHsc my_plugin.cpp
//    Linux   (mingw): i686-w64-mingw32-g++ -shared -O2 -o my_plugin.so my_plugin.cpp
// ============================================================

#include <cstdint>

// ---------- Экспорт и соглашение о вызовах ----------
#ifdef _WIN32
    #define PLUGIN_EXPORT extern "C" __declspec(dllexport)
    #define PLUGIN_CALL   __stdcall
#else
    #define PLUGIN_EXPORT extern "C" __attribute__((visibility("default")))
    #define PLUGIN_CALL
#endif

// ---------- Минимальный AMX ABI ----------
typedef int32_t cell;
struct AMX;   // неполный тип — содержимое AMX нам не нужно

typedef cell (PLUGIN_CALL *AMX_NATIVE)(AMX* amx, cell* params);

typedef struct _AMX_NATIVE_INFO {
    const char* name;
    AMX_NATIVE  func;
} AMX_NATIVE_INFO;

// ---------- Флаги из plugincommon.h ----------
#define SUPPORTS_VERSION      (1 << 2)
#define SUPPORTS_AMX_NATIVES  (1 << 1)

// ---------- Реализация нативов ----------
// Все параметры приходят в params[1], params[2], ...
// (params[0] — размер списка аргументов в байтах)

static cell PLUGIN_CALL n_AddNumbers(AMX* /*amx*/, cell* params)
{
    return params[1] + params[2];
}

static cell PLUGIN_CALL n_Multiply(AMX* /*amx*/, cell* params)
{
    return params[1] * params[2];
}

// ---------- Обязательные экспорты плагина ----------

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT int PLUGIN_CALL Load(void** /*ppData*/)
{
    return 1;   // 1 = успех
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
    // нечего освобождать
}

PLUGIN_EXPORT const AMX_NATIVE_INFO* PLUGIN_CALL GetNatives()
{
    static const AMX_NATIVE_INFO list[] = {
        { "AddNumbers", n_AddNumbers },
        { "Multiply",   n_Multiply   },
        { nullptr, nullptr }
    };
    return list;
}