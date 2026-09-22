#include "amx/amx.h"
#include "plugincommon.h"

#include <cstdio>
#include <cstring>

// ---- Вспомогательная функция: вызов натива SA-MP по имени ----
static cell CallNative(AMX* amx, const char* name, cell* args, int num_args)
{
    int index = 0;
    if (amx_FindNative(amx, name, &index) != AMX_ERR_NONE)
        return 0;
    if (amx->natives == nullptr)
        return 0;

    // В AMX params[0] — размер в байтах, дальше сами аргументы
    cell params[33];
    params[0] = num_args * static_cast<cell>(sizeof(cell));
    for (int i = 0; i < num_args; ++i)
        params[i + 1] = args[i];

    return amx->natives[index](amx, params);
}

// ---- Сам натив: native HelloWorld(playerid); ----
static cell AMX_NATIVE_CALL n_HelloWorld(AMX* amx, cell* params)
{
    int playerid = static_cast<int>(params[1]);

    // --- Получаем имя игрока: GetPlayerName(playerid, name[], len) ---
    cell* name_cell = nullptr;
    if (amx_Allot(amx, 32, &name_cell) != AMX_ERR_NONE)
        return 0;

    cell name_args[3] = { playerid, reinterpret_cast<cell>(name_cell), 32 };
    CallNative(amx, "GetPlayerName", name_args, 3);

    char name[32] = { 0 };
    amx_GetString(name, name_cell, 0, sizeof(name));
    amx_Release(amx, name_cell);

    // --- Формируем сообщение ---
    char message[64];
    std::snprintf(message, sizeof(message), "Hello, %s!", name);

    // --- Отправляем: SendClientMessage(playerid, color, message[]) ---
    cell* msg_cell = nullptr;
    if (amx_Allot(amx, 64, &msg_cell) != AMX_ERR_NONE)
        return 0;

    amx_SetString(msg_cell, message, 0, 0, 64);

    cell msg_args[3] = { playerid, 0xFFFFFFFF, reinterpret_cast<cell>(msg_cell) };
    CallNative(amx, "SendClientMessage", msg_args, 3);

    amx_Release(amx, msg_cell);

    return 1;
}

// ---- Обязательные экспорты плагина ----

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT int PLUGIN_CALL Load(void** /*ppData*/)
{
    return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
    // Нечего освобождать
}

PLUGIN_EXPORT const AMX_NATIVE_INFO* PLUGIN_CALL GetNatives()
{
    static const AMX_NATIVE_INFO natives[] = {
        { "HelloWorld", n_HelloWorld },
        { nullptr, nullptr }
    };
    return natives;
}