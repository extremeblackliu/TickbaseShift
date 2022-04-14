#include "bootstrap.h"
#include "../minhook/include/MinHook.h"

#define PI 3.1415926535898

HMODULE client, engine;


void* result1 = nullptr;
void* orig = nullptr;
void* orig_cm = nullptr;

static DWORD* lpoffset;

bool on = false;

void thread1()
{
	while (1)
	{
		if (GetAsyncKeyState(VK_F9))
		{
			on = !on;
			Sleep(500);
		}
		Sleep(100); //cpu stonks
	}
}


namespace Hooks
{

	void Init(HMODULE mod)
	{
		while (!GetModuleHandleA(("serverbrowser.dll")))
		{
			Sleep(2000);
		}
		client = GetModuleHandleA(("client.dll"));
		engine = GetModuleHandleA(("engine.dll"));
		void* studiorender = GetModuleHandleA(("studiorender.dll"));
		void* materialsystem = GetModuleHandleA(("materialsystem.dll"));
		MH_Initialize();
		const char* sig = "55 8B EC 56 8B F1 33 C0 57 8B 7D 08 8B 8E ? ? ? ? 85 C9 7E";
		void* a = Utils::PatternScan(client, sig);
		void* b = Utils::PatternScan(engine, sig);
		void* c = Utils::PatternScan(studiorender, sig);
		void* d = Utils::PatternScan(materialsystem, sig);
		if (!a || !b || !c || !d)
		{
			MessageBoxA(0, ("绕过失败"), XString("LauncherSU.net"), 0);
			return;
		}
		void* orig1;
		MH_CreateHook(a, &hk_VerifyReturnAddress, &orig1);
		MH_CreateHook(b, &hk_VerifyReturnAddress, &orig1);
		MH_CreateHook(c, &hk_VerifyReturnAddress, &orig1);
		MH_CreateHook(d, &hk_VerifyReturnAddress, &orig1);


		void* result = Utils::PatternScan(engine, ("55 8B EC 83 EC 08 56 8B F1 8B 4D 04"));
		result1 = (void*)((uintptr_t)Utils::PatternScan(engine, ("FF 90 ? ? ? ? 8D 4D A8 E8 ? ? ? ? 5F")) + 0x6);
		void* createmove = (void*)Utils::PatternScan(client, ("55 8B EC 56 8D 75 04 8B 0E E8 ? ? ? ? 8B"));
		if (!result || !result1)
		{
			MessageBoxA(0, ("初始化失败"), ("LauncherSU.net"), 0);
			return;
		}
		MH_CreateHook(createmove, &hk_CreateMove, &orig_cm);

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread1, 0, 0, 0);
		
		MH_CreateHook(result, &hk_SendNetMsg, &orig);
		MH_EnableHook(MH_ALL_HOOKS);

		VMProtectEnd();

	}
	bool __fastcall hk_VerifyReturnAddress(void* ecx, void* edx, const char* modulename)
	{
		return true;
	}

	bool __fastcall hk_SendNetMsg(void* netchan, void* edx, void* msg, bool a1, bool a2)
	{
		if (on && ((uintptr_t)_ReturnAddress()) == (uintptr_t)result1)
		{
			struct clc_msg_move_t
			{
				char pad[0xc];
				int backup_commands;
				int num_commands;
			};
			clc_msg_move_t* data = (clc_msg_move_t*)msg;
			data->backup_commands = 0;
			data->num_commands = 0;
		}
		using origin = bool(__fastcall*)(void*, void*, void*, bool, bool);
		return ((origin)orig)(netchan, edx, msg, a1, a2);
	}

	bool __stdcall hk_CreateMove(float a1, CUserCmd* cmd)
	{
		if (!lpoffset)
		{
			lpoffset = (DWORD*)((*(DWORD*)(Utils::PatternScan(client, XString("8D 34 85 ? ? ? ? 89 15 ? ? ? ? 8B 41 08 8B 48 04 83 F9 FF")) + 3)) + 4);
		}
		if (on && *lpoffset)
		{
			//dwLocalPlayer + m_EyeAngle[0]
			Vector eyeangle = Vector(*(float*)((*lpoffset) + 0x117D0), *(float*)((*lpoffset) + 0x117D0 + 0x4), 0.f);


			//anti self flash
			*(float*)((*lpoffset) + 0x1046C) = 0.f; //m_flFlashMaxAlpha

			cmd->viewangles = eyeangle;

			MovementFix(cmd);
			
			//clamp angle cuz valve give untrusted ban
			cmd->viewangles.x = clamp(cmd->viewangles.x, -89.f, 89.f);
			cmd->viewangles.y = clamp(cmd->viewangles.y, -180.f, 180.f);


			return false;
		}
		using origin = bool(__stdcall*)(float, void*);
		return ((origin)orig_cm)(a1, cmd);
	}
}
