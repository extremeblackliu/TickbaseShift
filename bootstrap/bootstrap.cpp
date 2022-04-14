#include "bootstrap.h"
#include "../minhook/include/MinHook.h"
#include "../xorstr.h"
#include "../vfunc_hook.h"

#include "../VMPSDK.h"

#define PI 3.1415926535898

HMODULE client, engine;


void* result1 = nullptr;
void* orig = nullptr;
void* orig_cm = nullptr;
void* orig_gea = nullptr;

void* mdlrender;

DWORD* dwEntityList;

static DWORD* lpoffset;

vfunc_hook mdlrender_hook;

bool on = false;
bool on_chams = false;

void thread1()
{
	while (1)
	{
		if (GetAsyncKeyState(VK_F9))
		{
			on = !on;
			Sleep(500);
		}
		if (GetAsyncKeyState(VK_F8))
		{
			on_chams = !on_chams;
			Sleep(500);
		}
		Sleep(100); //cpu stonks
	}
}


namespace Hooks
{

	void Init(HMODULE mod)
	{
		//VMProtectBeginMutation("Init");

		while (!GetModuleHandleA(XString("serverbrowser.dll")))
		{
			Sleep(2000);
		}
		client = GetModuleHandleA(XString("client.dll"));
		engine = GetModuleHandleA(XString("engine.dll"));
		void* studiorender = GetModuleHandleA(XString("studiorender.dll"));
		void* materialsystem = GetModuleHandleA(XString("materialsystem.dll"));
		MH_Initialize();
		const char* sig = "55 8B EC 56 8B F1 33 C0 57 8B 7D 08 8B 8E ? ? ? ? 85 C9 7E";
		void* a = Utils::PatternScan(client, sig);
		void* b = Utils::PatternScan(engine, sig);
		void* c = Utils::PatternScan(studiorender, sig);
		void* d = Utils::PatternScan(materialsystem, sig);
		if (!a || !b || !c || !d)
		{
			MessageBoxA(0, XString("绕过失败"), XString("LauncherSU.net"), 0);
			return;
		}
		void* orig1;
		MH_CreateHook(a, &hk_VerifyReturnAddress, &orig1);
		MH_CreateHook(b, &hk_VerifyReturnAddress, &orig1);
		MH_CreateHook(c, &hk_VerifyReturnAddress, &orig1);
		MH_CreateHook(d, &hk_VerifyReturnAddress, &orig1);


		void* result = Utils::PatternScan(engine, XString("55 8B EC 83 EC 08 56 8B F1 8B 4D 04"));
		result1 = (void*)((uintptr_t)Utils::PatternScan(engine, XString("FF 90 ? ? ? ? 8D 4D A8 E8 ? ? ? ? 5F")) + 0x6);
		void* createmove = (void*)Utils::PatternScan(client, XString("55 8B EC 56 8D 75 04 8B 0E E8 ? ? ? ? 8B"));
		if (!result || !result1)
		{
			MessageBoxA(0, XString("初始化失败"), XString("LauncherSU.net"), 0);
			return;
		}

		using CI = void*(__cdecl*)(const char*,void*);
		CI createinterface_engine = (CI)(GetProcAddress(engine, "CreateInterface"));
		CI createinterface_client = (CI)(GetProcAddress(client,	 "CreateInterface"));
		engineclient = (IVEngineClient*)createinterface_engine("VEngineClient014", nullptr);
		DWORD** vtable_hlclient = (DWORD**)createinterface_client("VClient018", nullptr);

		dwEntityList = *(DWORD**)(Utils::PatternScan(client, XString("BB ? ? ? ? 83 FF 01 0F 8C ? ? ? ? 3B F8")) + 1);
		mdlrender = createinterface_engine("VEngineModel016", nullptr);

		MH_CreateHook(createmove, &hk_CreateMove, &orig_cm);

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread1, 0, 0, 0);
		
		MH_CreateHook(result, &hk_SendNetMsg, &orig);
		MH_EnableHook(MH_ALL_HOOKS);

		MessageBoxA(0, XString("初始化已经成功了\nLauncherSU.net\n使用说明:\n使用F9键打开或者关闭\n必须在进入服务器前打开！\n进入服务器之后无法选择阵营，需要让它自动选择\n如果要关闭的话请再次按F9关闭并且重连\n要秒拆包必须用回合开始的角度对准包\n由于模拟时间不会增加，开暴力自瞄的人不会开枪\n但是他们可以强制/手动开枪，再怎么样你都可以戏耍一下他们\n:)"), XString("LauncherSU.net"), 0);
		MessageBoxA(0, XString("初始化已经成功了\nLauncherSU.net\n使用说明:\n使用F9键打开或者关闭\n必须在进入服务器前打开！\n进入服务器之后无法选择阵营，需要让它自动选择\n如果要关闭的话请再次按F9关闭并且重连\n要秒拆包必须用回合开始的角度对准包\n由于模拟时间不会增加，开暴力自瞄的人不会开枪\n但是他们可以强制/手动开枪，再怎么样你都可以戏耍一下他们\n:)"), XString("LauncherSU.net"), 0);
		MessageBoxA(0, XString("另外的说明:如果你要玩官匹，请在热身阶段差不多结束的时候重连一下，不需要按F9重新打开，只需要重连一下，然后它就可以正常使用"), XString("LauncherSU.net"), 0);
		MessageBoxA(0, XString("另外的说明:如果你要玩官匹，请在热身阶段差不多结束的时候重连一下，不需要按F9重新打开，只需要重连一下，然后它就可以正常使用"), XString("LauncherSU.net"), 0);
		MessageBoxA(0, XString("我每个说明都提示了两遍，请问你看完了/看明白了吗？\n如果没有看完/没有看明白，请重新注入再看一遍\n或者截图提示框可以一直查看"), XString("LauncherSU.net"), 0);
		
		//VMProtectEnd();

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
			//Vector eyeangle = Vector(*(float*)((*lpoffset) + 0x117D0), *(float*)((*lpoffset) + 0x117D0 + 0x4), 0.f);


			//anti self flash
			//*(float*)((*lpoffset) + 0x10470) = 0.f; //m_flFlashDuration
			//*(float*)((*lpoffset) + 0x1046C) = 0.f; //m_flFlashMaxAlpha

			//cmd->viewangles = eyeangle;

			//MovementFix(cmd);
			
			cmd->viewangles.x = clamp(cmd->viewangles.x, -89.f, 89.f);
			cmd->viewangles.y = clamp(cmd->viewangles.y, -180.f, 180.f);


			return false;
		}
		using origin = bool(__stdcall*)(float, void*);
		return ((origin)orig_cm)(a1, cmd);
	}
}