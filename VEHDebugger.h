#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>

namespace Plugin
{

	namespace VEHDebugger
	{
		typedef void(*FNVEHCallback)(_EXCEPTION_POINTERS* ExceptionInfo);

		PVOID pHandler = nullptr;
		class CPageGuardBreakpoint
		{
		public:
			CPageGuardBreakpoint(void* HookAddr, FNVEHCallback Callback)
			{
				m_pHookAddr = HookAddr;
				m_Callback = Callback;
				m_Active = false;
			}

			bool Activate()
			{
				if (m_Active)
					return true;

				VirtualProtect(m_pHookAddr, 1, PAGE_READWRITE, &m_OldProtection);
			
				m_Active = true;
			}

			bool Deactivate()
			{
				if (!m_Active)
					return true;

				VirtualProtect(m_pHookAddr, 1, m_OldProtection, &m_OldProtection);
			}


			void*			m_shadowPage;
			DWORD			m_OldProtection;
			bool			m_Active;
			FNVEHCallback	m_Callback;
			void*			m_pHookAddr;
		};
		
		std::vector<CPageGuardBreakpoint> pageGuardBreakpoints;

		LONG WINAPI	VEHandler(struct _EXCEPTION_POINTERS *ExceptionInfo)
		{

			auto SetTrapFlag = [&ExceptionInfo](bool on)
			{
				on ? ExceptionInfo->ContextRecord->EFlags |= 0x100 : ExceptionInfo->ContextRecord->EFlags &= ~0x100;
			};

			if (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION)
			{
				for (auto& bp : pageGuardBreakpoints)
				{
					if (bp.m_pHookAddr == ExceptionInfo->ExceptionRecord->ExceptionAddress)
					{
						bp.m_Callback(ExceptionInfo);
					
						bp.Deactivate();

						SetTrapFlag(true);
					}
				}
			}


			return EXCEPTION_CONTINUE_EXECUTION;
		}

		void AddPageGuardBreakpoint(void* Address, FNVEHCallback Callback)
		{
		}

		bool Initialize()
		{
			pHandler = AddVectoredExceptionHandler(1, VEHandler);
		
			return pHandler != nullptr;
		}

		bool Remove()
		{
			if (pHandler == nullptr)
				return true;

			RemoveVectoredExceptionHandler(pHandler);
		
			pHandler = nullptr;

			return true;
		}


	}
}