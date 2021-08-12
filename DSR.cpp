#include "DSR.h"

#include "Textures/scrollTexture.h"
#include "Textures/bloodlineTexture.h"

using namespace Plugin;

namespace DSR
{
	namespace SDK
	{
		namespace Internal
		{
			/*
				FWD
			*/
			class CPlayerIns;

			class CPlayerGameData
			{
			public:
				int& Vitality()
				{
					return *(int*)((uintptr_t)this + 0x40);
				}
				int& Attunement()
				{
					return *(int*)((uintptr_t)this + 0x48);
				}
				int& Eddurance()
				{
					return *(int*)((uintptr_t)this + 0x50);
				}
				int& Strenght()
				{
					return *(int*)((uintptr_t)this + 0x58);
				}
				int& Dexterirty()
				{
					return *(int*)((uintptr_t)this + 0x60);
				}
				int& Resistance()
				{
					return *(int*)((uintptr_t)this + 0x68);
				}
				int& Intelligence()
				{
					return *(int*)((uintptr_t)this + 0x70);
				}
				int& Faith()
				{
					return *(int*)((uintptr_t)this + 0x78);
				}
				int& SoulLevel()
				{
					return *(int*)((uintptr_t)this + 0x90);
				}

				enum Gender_t : unsigned char{
					GENDER_FEMALE = 0x0,
					GENDER_MALE = 0x1
				};

				Gender_t& Gender()
				{
					return *(Gender_t*)((uintptr_t)this + 0xca);
				}
				/*
					Contains the rest of the stats shown in the status menu, yet these are only readable
				*/
			};

			class CPlayerAnim
			{
			public:
				CPlayerIns*& PlayerIns()
				{
					return *(CPlayerIns**)((uintptr_t)this + 0x10);
				}

				float& SpeedModifier()
				{
					return *(float*)((uintptr_t)this + 0xa8);
				}
			};

			class ChkpSimpleShapePhantom
			{
			public:
			};

			class CPhysShapePhantomIns
			{
			public:
				ChkpSimpleShapePhantom * SimpleShapePhantom()
				{
					return *(ChkpSimpleShapePhantom**)((uintptr_t)this + 0x20);
				}

			};

			class CPlayerPhysicsData
			{
			public:
				float& XCoord()
				{
					return *(float*)((uintptr_t)this + 0x10);
				}
				float& YCoord()
				{
					return *(float*)((uintptr_t)this + 0x14);
				}
				float& ZCoord()
				{
					return *(float*)((uintptr_t)this + 0x18);
				}
			};

			class CPlayerPhysics
			{
			public:
				CPlayerPhysicsData *& PhysicsData()
				{
					return *(CPlayerPhysicsData**)((uintptr_t)this + 0x28);
				}

			};

			class CPlayerIns
			{
			public:
				enum TeamType_t : int {
					TEAMTYPE_NONE = 0,
					TEAMTYPE_ALIVE = 1,
					TEAMTYPE_WHITEGHOST = 2,
					TEAMTYPE_BLACKGHOST = 3,
					TEAMTYPE_HOLLOWGHOST = 4,
					TEAMTYPE_WANDERERGHOST = 5,
					TEAMTYPE_ENEMY = 6,
					TEAMTYPE_BOSS = 7,
					TEAMTYPE_FRIEND = 8,
					TEAMTYPE_ANGRYFRIEND = 9,
					TEAMTYPE_DECOYENEMY = 10,
					TEAMTYPE_BLOODCHILD = 11,
					TEAMTYPE_BATTLEFIEND = 12
				};

				enum CharType_t : int
				{
					CHARTYPE_NONE = -1,
					CHARTYPE_ALIVE = 0,
					CHARTYPE_WHITEPHANTOM = 1,
					CHARTYPE_REDPHANTOM = 2,
					CHARTYPE_GREYGHOST = 3,
					CHARTYPE_HOLLOW = 8,
					CHARTYPE_FLATGREYGHOST = 11,
					CHARTYPE_BLACKPHANTOM = 12,
					CHARTYPE_LOWDETAIL = 13,
					CHARTYPE_MAXIDX = 8
				};

				CharType_t& CharType()
				{
					return *(CharType_t*)((uintptr_t)this + 0xb4);
				}
				TeamType_t& TeamType()
				{
					return *(TeamType_t*)((uintptr_t)this + 0xb8);

				}

				CPlayerPhysics* PlayerPhysics()
				{
					return *(CPlayerPhysics**)((uintptr_t)this + 0x18);
				}

				CPlayerGameData*& PlayerGameData() 
				{
					return *(CPlayerGameData**)((uintptr_t)this + 0x568);
				}

				int& PlayerNumber()
				{
					return *(int*)((uintptr_t)this + 0x560);
				}

				int& Health()
				{
					return *(int*)((uintptr_t)this + 0x3d8);
				}
				int& MaxHealth()
				{
					return *(int*)((uintptr_t)this + 0x3dc);
				}
				int& Stamina()
				{
					return *(int*)((uintptr_t)this + 0x3e8);
				}
				int& MaxStamina()
				{
					return *(int*)((uintptr_t)this + 0x3ec);
				}
				unsigned char& GravityEnabled()
				{
					return *(unsigned char*)((uintptr_t)this + 0x285);
				}


				/*
					Also contains writable bleed, poison and curse values
				*/
			};

			class CWorldChrManImp
			{
			public:
				CPlayerIns* PlayerIns()
				{
					if (this == nullptr)
						return nullptr;

					return *(CPlayerIns**)((uintptr_t)this + 0x68);
				}

				static CWorldChrManImp* Get()
				{
					static unsigned char* patloc = 0;
					static void* adr = 0;

					if (!patloc)
					{
						patloc = Utility::FindPattern("DarkSoulsRemastered.exe", "48 8B 0D ? ? ? ? 0F 28 F3 41");
						DWORD relativeOffset = *(DWORD*)(patloc + 0x3);
						adr = (void*)(patloc + Utility::GetInstructionSize(patloc) + relativeOffset);
						msg("CWorldChrManImp: 0x" << adr);
					}
					return *(CWorldChrManImp**)adr;
				}
			private:
				CWorldChrManImp() {

				}
			};

		}

		class CPlayerInstance
			: public Internal::CPlayerIns
		{
		public:
			
		};
		class CLocalPlayer
		{
		public:

			CPlayerInstance * PlayerInstance()
			{
				return (CPlayerInstance*)Internal::CWorldChrManImp::Get()->PlayerIns();
			}

			float* SpeedModifier()
			{
				uintptr_t p = (uintptr_t)Internal::CWorldChrManImp::Get()->PlayerIns();
				if (!p) return nullptr;
				p = *(uintptr_t*)(p + 0x48);
				if (!p) return nullptr;
				p = *(uintptr_t*)(p + 0x18);
				if (!p) return nullptr;
				p = p + 0xa8;
				return (float*)(p);
			}
		};
	}

	//namespace SDK
	//{
	//	namespace Internal
	//	{
	//
	//
	//		class CWorldChrBase;
	//		class CLocalPlayer;
	//		class CStatsBase;
	//
	//		class CWorldChrBase
	//		{
	//		public:
	//			static CStatsBase * StatsBase()
	//			{
	//				auto p = Get();
	//				if (!p) return nullptr;
	//				return *(CStatsBase**)((uintptr_t)p + 0x68);
	//			}
	//
	//		private:
	//
	//			static CWorldChrBase* Get()
	//			{
	//				static CWorldChrBase* adr = 0;
	//				if (!adr)
	//				{
	//					auto address = Utility::FindPattern("DarkSoulsRemastered.exe", "48 8B 0D ? ? ? ? 0F 28 F3 41");
	//					DWORD relativeOffset = *(DWORD*)(address + 0x3);
	//					adr = *(CWorldChrBase**)(address + Utility::GetInstructionSize(address) + relativeOffset);
	//					msg("WorldBase: 0x" << adr);
	//				}
	//				return adr;
	//			}
	//
	//			
	//		};
	//		
	//		class CChrClassBase
	//		{
	//		public:
	//			static CChrClassBase* Get()
	//			{
	//				static CChrClassBase* adr = 0;
	//				if (!adr)
	//				{
	//					auto address = Utility::FindPattern("DarkSoulsRemastered.exe", "48 8B 05 ? ? ? ? 48 85 C0 ? ? F3 0F 58 80 AC 00 00 00");
	//					DWORD relativeOffset = *(DWORD*)(address + 0x3);
	//					adr = *(CChrClassBase**)(address + Utility::GetInstructionSize(address) + relativeOffset);
	//					msg("CChrClassBase: 0x" << adr);
	//				}
	//				return adr;
	//			}
	//		};
	//		
	//		class CChrClassWarp	
	//		{
	//		public:
	//			static CChrClassWarp* Get()
	//			{
	//				static CChrClassWarp* adr = 0;
	//				if (!adr)
	//				{
	//					auto address = Utility::FindPattern("DarkSoulsRemastered.exe", "48 8B 05 ? ? ? ? 66 0F 7F 80 A0 0B 00 00 0F 28 02 66 0F 7F 80 B0 0B 00 00 C6 80");
	//					DWORD relativeOffset = *(DWORD*)(address + 0x3);
	//					adr = *(CChrClassWarp**)(address + Utility::GetInstructionSize(address) + relativeOffset);
	//					msg("CChrClassWarp: 0x" << adr);
	//				}
	//				return adr;
	//			}
	//		};
	//
	//		class CCamManBase
	//		{
	//		public:
	//			static CCamManBase* Get()
	//			{
	//				static CCamManBase* adr = 0;
	//				if (!adr)
	//				{
	//					auto address = Utility::FindPattern("DarkSoulsRemastered.exe", "48 8B 05 ? ? ? ? 48 63 D1 48 8B 44 D0 08 C3");
	//					DWORD relativeOffset = *(DWORD*)(address + 0x3);
	//					adr = *(CCamManBase**)(address + Utility::GetInstructionSize(address) + relativeOffset);
	//					msg("CCamManBase: 0x" << adr);
	//				}
	//				return adr;
	//			}
	//		};
	//
	//		class CLockTGTBase
	//		{
	//		public:
	//			static CLockTGTBase* Get()
	//			{
	//				static CLockTGTBase* adr = 0;
	//				if (!adr)
	//				{
	//					auto address = Utility::FindPattern("DarkSoulsRemastered.exe", "48 8B 0D ? ? ? ? 89 99 ? ? ? ? 4C 89 6D 58");
	//					DWORD relativeOffset = *(DWORD*)(address + 0x3);
	//					adr = *(CLockTGTBase**)(address + Utility::GetInstructionSize(address) + relativeOffset);
	//					msg("CLockTGTBase: 0x" << adr);
	//				}
	//				return adr;
	//			}
	//		};
	//		
	//		class CMenuManBase
	//		{
	//			static CMenuManBase* Get()
	//			{
	//				static CMenuManBase* adr = 0;
	//				if (!adr)
	//				{
	//					auto address = Utility::FindPattern("DarkSoulsRemastered.exe", "48 8B 05 ? ? ? ? 89 88 28 08 00 00 85 C9");
	//					DWORD relativeOffset = *(DWORD*)(address + 0x3);
	//					adr = *(CMenuManBase**)(address + Utility::GetInstructionSize(address) + relativeOffset);
	//					msg("CMenuManBase: 0x" << adr);
	//				}
	//				return adr;
	//			}
	//		};
	//		
	//		class CParam
	//		{
	//			static CParam* Get()
	//			{
	//				static CParam* adr = 0;
	//				if (!adr)
	//				{
	//					auto address = Utility::FindPattern("DarkSoulsRemastered.exe", "4C 8B 05 ? ? ? ? 48 63 C9 48 8D 04 C9 41 3B 54 C0 10");
	//					DWORD relativeOffset = *(DWORD*)(address + 0x3);
	//					adr = *(CParam**)(address + Utility::GetInstructionSize(address) + relativeOffset);
	//					msg("CParam: 0x" << adr);
	//				}
	//				return adr;
	//			}
	//		};
	//		
	//		class CWorldChrDbgBase
	//		{
	//			static CWorldChrDbgBase* Get()
	//			{
	//				static CWorldChrDbgBase* adr = 0;
	//				if (!adr)
	//				{
	//					auto address = Utility::FindPattern("DarkSoulsRemastered.exe", "48 8B 05 ? ? ? ? 48 8B 80 F0 00 00 00 48 85 C0");
	//					DWORD relativeOffset = *(DWORD*)(address + 0x3);
	//					adr = *(CWorldChrDbgBase**)(address + Utility::GetInstructionSize(address) + relativeOffset);
	//					msg("CChrClassWarp: 0x" << adr);
	//				}
	//				return adr;
	//			}
	//		};
	//
	//		class CDbgEventBase
	//		{
	//			static CDbgEventBase* Get()
	//			{
	//				static CDbgEventBase* adr = 0;
	//				if (!adr)
	//				{
	//					auto address = Utility::FindPattern("DarkSoulsRemastered.exe", "48 8B 05 ? ? ? ? 44 38 80 ? ? ? ? ? ? 44 38 41 49 0F 84 ? ? ? ? 66 0F 6E 41 38 48 8B 41 10 0F 28 0D ? ? ? ? 48 89 BC 24 ? ? ? ? 0F 5B C0 4C 89 A4 24");
	//					DWORD relativeOffset = *(DWORD*)(address + 0x3);
	//					adr = *(CDbgEventBase**)(address + Utility::GetInstructionSize(address) + relativeOffset);
	//					msg("CChrClassWarp: 0x" << adr);
	//				}
	//				return adr;
	//			}
	//		};
	//
	//		class CThrowParam
	//		{
	//			static CThrowParam* Get()
	//			{
	//				static CThrowParam* adr = 0;
	//				if (!adr)
	//				{
	//					auto address = Utility::FindPattern("DarkSoulsRemastered.exe", "48 8B 05 ? ? ? ? 48 8B 40 08 48 8B 40 38 0F B7 50 0A 3B CA ? ? 8B C9 48 83 C1 04 48 8D 0C 49 8B 04 88");
	//					DWORD relativeOffset = *(DWORD*)(address + 0x3);
	//					adr = *(CThrowParam**)(address + Utility::GetInstructionSize(address) + relativeOffset);
	//					msg("CThrowParam: 0x" << adr);
	//				}
	//				return adr;
	//			}
	//		};
	//		class CChrFollowCam
	//		{
	//			static CChrFollowCam* Get()
	//			{
	//				static CChrFollowCam* adr = 0;
	//				if (!adr)
	//				{
	//					auto address = Utility::FindPattern("DarkSoulsRemastered.exe", "48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B 4E 68");
	//					DWORD relativeOffset = *(DWORD*)(address + 0x3);
	//					adr = *(CChrFollowCam**)(address + Utility::GetInstructionSize(address) + relativeOffset);
	//					msg("CChrFollowCam: 0x" << adr);
	//				}
	//				return adr;
	//			}
	//		};
	//		class CFRPGNETBase
	//		{
	//			static CFRPGNETBase* Get()
	//			{
	//				static CFRPGNETBase* adr = 0;
	//				if (!adr)
	//				{
	//					auto address = Utility::FindPattern("DarkSoulsRemastered.exe", "48 8B 1D ? ? ? ? 48 8D 94 24 ? ? ? ? 4C 8B F1 0F 29 7C 24 40");
	//					DWORD relativeOffset = *(DWORD*)(address + 0x3);
	//					adr = *(CFRPGNETBase**)(address + Utility::GetInstructionSize(address) + relativeOffset);
	//					msg("CFRPGNETBase: 0x" << adr);
	//				}
	//				return adr;
	//			}
	//		};
	//
	//		class CWorldChrBase_P
	//		{
	//			static CWorldChrBase_P* Get()
	//			{
	//				static CWorldChrBase_P* adr = 0;
	//				if (!adr)
	//				{
	//					auto address = Utility::FindPattern("DarkSoulsRemastered.exe", "48 89 05 ? ? ? ? 48 89 5C 24 38 48 85 DB ? ? 48 8B 03");
	//					DWORD relativeOffset = *(DWORD*)(address + 0x3);
	//					adr = *(CWorldChrBase_P**)(address + Utility::GetInstructionSize(address) + relativeOffset);
	//					msg("CChrClassWarp: 0x" << adr);
	//				}
	//				return adr;
	//			}
	//		};
	//		
	//		class CNetworkProp
	//		{
	//			static CNetworkProp* Get()
	//			{
	//				static CNetworkProp* adr = 0;
	//				if (!adr)
	//				{
	//					auto address = Utility::FindPattern("DarkSoulsRemastered.exe", "48 89 05 ? ? ? ? 48 83 3D ? ? ? ? 00 ? ? 4C 8B 05 ? ? ? ? 4C 89 44 24 48 BA 08 00 00 00 B9 58 60 00 00");
	//					DWORD relativeOffset = *(DWORD*)(address + 0x3);
	//					adr = *(CNetworkProp**)(address + Utility::GetInstructionSize(address) + relativeOffset);
	//					msg("CChrClassWarp: 0x" << adr);
	//				}
	//				return adr;
	//			}
	//		};
	//
	//		class CSessionPlayer
	//		{
	//
	//		};
	//
	//		class CSessionPlayers
	//		{
	//			CSessionPlayer* Player1()
	//			{
	//
	//			}
	//
	//		};
	//
	//		class CAttributes
	//		{
	//
	//		};
	//
	//		class CStats
	//		{
	//		public:
	//			int& Health()
	//			{
	//				return *(int*)((uintptr_t)this + 0x3d8);
	//			}
	//			int& MaxHealth()
	//			{
	//				return *(int*)((uintptr_t)this + 0x3dc);
	//			}
	//			int& Stamina()
	//			{
	//				return *(int*)((uintptr_t)this + 0x3e8);
	//			}
	//			int& MaxStamina()
	//			{
	//				return *(int*)((uintptr_t)this + 0x3ec);
	//			}
	//		};
	//
	//		class CStatsBase
	//		{
	//		public:
	//			CStats * LocalPlayerStats()
	//			{
	//				return (CStats*)this;
	//			}
	//
	//			CStats * SessionPlayer1Stats()
	//			{
	//				uintrptr_t * p = ds
	//			}
	//			
	//		};
	//	};
	//
	//	/*
	//		Fwd
	//	*/
	//	class CEngine;
	//	class CPlayer;
	//
	//	/*
	//		Def
	//	*/
	//	class CPlayerStats
	//		: public Internal::CStats
	//	{
	//
	//	};
	//
	//	class CPlayerAttributes
	//	{
	//
	//	};
	//
	//	class CLocalPlayer
	//	{
	//	public:
	//
	//		CLocalPlayer()
	//		{}
	//
	//		CPlayerStats* Stats()
	//		{
	//			return (CPlayerStats*)Internal::CWorldChrBase::StatsBase();
	//		}
	//		
	//		float* SpeedModifier()
	//		{
	//			uintptr_t p = (uintptr_t)Internal::CWorldChrBase::StatsBase();
	//			if (!p) return nullptr;
	//			p = *(uintptr_t*)(p + 0x48);
	//			if (!p) return nullptr;
	//			p = *(uintptr_t*)(p + 0x18);
	//			if (!p) return nullptr;
	//			p = p + 0xa8;
	//			return (float*)(p);
	//		}
	//
	//	};
	//
	//	class CRemotePlayer
	//	{
	//		CRemotePlayer(const int Index)
	//			: m_Index(Index)
	//		{}
	//
	//		
	//		
	//
	//	private:
	//		int m_Index;
	//	};
	//
	//}

	namespace Addresses 
	{
		
		void* HomewardCall = NULL;

		bool Initialize()
		{

			

			return true;
		}
	}

	namespace Features
	{

		namespace Settings
		{
			bool infiniteHealth = true;
			bool infiniteStamina = false;
			bool infiniteSpells = false;
			int characterType = 0;
			int speedModifier = 1;
			int giveSoulsModifier = 0;
			bool antiCheat = false;
			bool connectionInfo = false;
			int kickPlayerIndex = 0;
			bool flyMode = false;
			int equipSpellIndex = 0;
		}
		/*
		Gives the user the ability to teleport
		*/
		namespace Teleportation {

			float storedX = 0, storedY = 0, storedZ = 0;
			bool wayPointStored = false;

			void StoreWaypoint()
			{
				SDK::CLocalPlayer localPlayer;

				auto playerInstance = localPlayer.PlayerInstance();

				if (!playerInstance)
					return;

				auto playerPhysics = playerInstance->PlayerPhysics();

				if (!playerPhysics)
					return;

				auto playerPhysicsData = playerPhysics->PhysicsData();

				if (!playerPhysicsData)
					return;

				storedX = playerPhysicsData->XCoord();
				storedY = playerPhysicsData->YCoord();
				storedZ = playerPhysicsData->ZCoord();

				wayPointStored = true;

			}

			void TeleportToWaypoint()
			{
				SDK::CLocalPlayer localPlayer;

				auto playerInstance = localPlayer.PlayerInstance();

				if (!playerInstance)
					return;

				auto playerPhysics = playerInstance->PlayerPhysics();

				if (!playerPhysics)
					return;

				auto playerPhysicsData = playerPhysics->PhysicsData();

				if (!playerPhysicsData)
					return;

				playerPhysicsData->XCoord() = storedX;
				playerPhysicsData->YCoord() = storedY + 2.f;
				playerPhysicsData->ZCoord() = storedZ;

				wayPointStored = true;
			}

		}

		namespace Scripts
		{
			void Die()
			{
				SDK::CLocalPlayer localPlayer;

				auto playerInstance = localPlayer.PlayerInstance();

				if (playerInstance == nullptr)
					return;

				playerInstance->Health() = 0;
			}
		}
		/*
		Gives the user the ability to spawn items
		*/
		namespace ItemSpawning {

		}

		/*
		Responsible for setting settings that needs to be applied constantly such as health
		*/
		namespace Setter
		{
			void SetFeatures()
			{
				SDK::CLocalPlayer localPlayer;

				auto playerInstance = localPlayer.PlayerInstance();

				if (playerInstance == nullptr)
					return;

				if (Settings::infiniteHealth)
				{
					playerInstance->Health() = playerInstance->MaxHealth();
				}

				if (Settings::infiniteStamina)
				{
					playerInstance->Stamina() = playerInstance->MaxStamina();
				}


				static int originalCharacterType = 0;
				static int originalTeamType = 0;
				static bool characterTypeSet = false;

				if (characterTypeSet && Settings::characterType == 0)
				{
					playerInstance->TeamType() = (SDK::Internal::CPlayerIns::TeamType_t)originalTeamType;
					playerInstance->CharType() = (SDK::Internal::CPlayerIns::CharType_t)originalCharacterType;
					characterTypeSet = false;

				}

				if (Settings::characterType != 0)
				{
					if (!characterTypeSet)
					{
						characterTypeSet = true;
						originalTeamType = playerInstance->TeamType();
						originalCharacterType = playerInstance->CharType();
					}


					int characterType = 0;

					switch (Settings::characterType)
					{
					case 1:
						characterType = SDK::Internal::CPlayerIns::CHARTYPE_ALIVE;
						break;
					case 2:
						characterType = SDK::Internal::CPlayerIns::CHARTYPE_WHITEPHANTOM;
						break;
					case 3:
						characterType = SDK::Internal::CPlayerIns::CHARTYPE_REDPHANTOM;
						break;
					case 4:
						characterType = SDK::Internal::CPlayerIns::CHARTYPE_GREYGHOST;
						break;
					case 5:
						characterType = SDK::Internal::CPlayerIns::CHARTYPE_HOLLOW;
						break;
					case 6:
						characterType = SDK::Internal::CPlayerIns::CHARTYPE_FLATGREYGHOST;
						break;
					case 7:
						characterType = SDK::Internal::CPlayerIns::CHARTYPE_BLACKPHANTOM;
						break;
					case 8:
						characterType = SDK::Internal::CPlayerIns::CHARTYPE_LOWDETAIL;
						break;
					default:
						break;
					}
					if (characterType != 0) {
						playerInstance->CharType() = (SDK::Internal::CPlayerIns::CharType_t)characterType;

					}
				}

				auto pSpeedModifier = localPlayer.SpeedModifier();
				if (pSpeedModifier)
				{
					static bool speedSet = false;

					if (Settings::speedModifier == 1 && speedSet)
					{
						*pSpeedModifier = 1.f;
						speedSet = false;

					}

					if (Settings::speedModifier > 1)
					{
						*pSpeedModifier = (float)Settings::speedModifier;
						speedSet = true;
					}
				}
			}
		}

		/*
		The graphical user interface
		*/
		namespace GUI {

			int currentSelectedIndex = 0;
			int maxIndex = 10;
			bool guiOpen = false;

			void OnKey(Input::KeyDefinition_t key, int state) {

				static int lastAButtonState = 0;
				static int lastYButtonState = 0;

				if (key == Input::KEY_CONTROLLER_A)
					lastAButtonState = state;

				if (key == Input::KEY_CONTROLLER_Y)
					lastYButtonState = state;

				static bool once = false;
				if (lastAButtonState && lastYButtonState) {
					guiOpen = !guiOpen;
					once = true;
					return;
				}
				else {
					once = false;
				}

				if (!guiOpen)
					return;

				if (state)
				{
					switch (key)
					{
					case Input::KEY_CONTROLLER_DPAD_UP:
						if (currentSelectedIndex == 0)
							currentSelectedIndex = maxIndex;
						else
							currentSelectedIndex--;
						break;
					case Input::KEY_CONTROLLER_DPAD_DOWN:
						if (currentSelectedIndex >= maxIndex)
							currentSelectedIndex = 0;
						else
							currentSelectedIndex++;
						break;
					case Input::KEY_CONTROLLER_A:
					{
						switch (currentSelectedIndex)
						{
						case 0:
							Settings::infiniteHealth = !Settings::infiniteHealth;
							break;
						case 1:
							Settings::infiniteStamina = !Settings::infiniteStamina;
							break;
						case 6:
							Teleportation::TeleportToWaypoint();
							break;
						case 7:
							Scripts::Die();
							break;
						}
						break;
					}
					case Input::KEY_CONTROLLER_B:
					{

						switch (currentSelectedIndex)
						{
						case 0:
							Settings::infiniteHealth = !Settings::infiniteHealth;
							break;
						case 1:
							Settings::infiniteStamina = !Settings::infiniteStamina;
							break;
						case 6:
							Teleportation::StoreWaypoint();
							break;
						}
						break;

					}
					case Input::KEY_CONTROLLER_DPAD_LEFT:
					{
						switch (currentSelectedIndex)
						{
						case 2:
						{
							int maxCharacterType = sizeof(SDK::Internal::CPlayerIns::CHARTYPE_MAXIDX);

							if (Settings::characterType - 1 == -1) {
								Settings::characterType = maxCharacterType;
							}
							else {
								Settings::characterType--;
							}

							break;
						}
						case 3:
						{
							if (Settings::speedModifier != 1)
								Settings::speedModifier--;

							break;
						}
						}
						break;
					}
					case Input::KEY_CONTROLLER_DPAD_RIGHT:
					{
						switch (currentSelectedIndex)
						{
						case 2:
						{

							if (Settings::characterType + 1 > 8)
								Settings::characterType = 0;
							else
								Settings::characterType++;

							break;
						}
						case 3:
						{
							if (Settings::speedModifier != 5)
								Settings::speedModifier++;

							break;
						}
					
						}
						break;
					}
					}
				}
			}

			using namespace DirectX;
			using namespace DirectX::SimpleMath;

			void OnPresent(IDXGISwapChain* pSwapChain) {

				Setter::SetFeatures();

				if (!guiOpen)
					return;


				auto renderer = DirectX11::Rendering::CRenderer::GetRenderer(pSwapChain);

				static int scrollTextureId = 0;
				if (scrollTextureId == 0) {
					scrollTextureId = renderer->LoadTexture(scrollTexture, sizeof(scrollTexture));
				}

				static int bloodlineTextureId = 0;
				if (bloodlineTextureId == 0) {
					bloodlineTextureId = renderer->LoadTexture(bloodlineTexture, sizeof(bloodlineTexture));
				}


				renderer->DrawTexture(100, 50, scrollTextureId);
				
				int TextStartYOffset = 240;
				int TextStartXOffset = 300;

				renderer->DrawString(TextStartXOffset, 180, Colors::Ivory, 1.f, true, "Dark Souls");

				int TextLineIndex = 0;

				renderer->DrawTexture(200, TextStartYOffset + (20 * currentSelectedIndex) - 10, bloodlineTextureId);

				renderer->DrawString(TextStartXOffset, TextStartYOffset + (20 * TextLineIndex++), Settings::infiniteHealth ? Colors::Gold : Colors::Gray, 1.f, true, "Infinite Health");
				renderer->DrawString(TextStartXOffset, TextStartYOffset + (20 * TextLineIndex++), Settings::infiniteStamina ? Colors::Gold : Colors::Gray, 1.f, true, "Infinite Stamina");

				std::string characterTypeStr = "Charater type: ";
				switch (Settings::characterType)
				{
				case 0:
					characterTypeStr += "Default";
					break;
				case 1:
					characterTypeStr += "Human";
					break;
				case 2:
					characterTypeStr += "White phantom";
					break;
				case 3:
					characterTypeStr += "Red phantom";
					break;
				case 4:
					characterTypeStr += "Ghost";
					break;
				case 5:
					characterTypeStr += "Hollow";
					break;
				case 6:
					characterTypeStr += "Flat ghost";
					break;
				case 7:
					characterTypeStr += "Black phantom";
					break;
				case 8:
					characterTypeStr += "Low detail";
					break;
				default:
					characterTypeStr += "Uknown";
					break;
				}
				renderer->DrawString(TextStartXOffset, TextStartYOffset + (20 * TextLineIndex++), Settings::characterType != 0 ? Colors::Gold : Colors::Gray, 1.f, true, characterTypeStr.c_str());


				std::string speedModifierStr = "Character speed: ";
				switch (Settings::speedModifier)
				{
				case 1:
					speedModifierStr += "Default";
					break;
				case 2:
					speedModifierStr += "2x";
					break;
				case 3:
					speedModifierStr += "3x";
					break;
				case 4:
					speedModifierStr += "4x";
					break;
				case 5:
					speedModifierStr += "5x";
					break;
				default:
					speedModifierStr += "Unknown";
					break;
				}
				renderer->DrawString(TextStartXOffset, TextStartYOffset + (20 * TextLineIndex++), Settings::speedModifier != 1 ? Colors::Gold : Colors::Gray, 1.f, true, speedModifierStr.c_str());

				renderer->DrawString(TextStartXOffset, TextStartYOffset + (20 * TextLineIndex++), Settings::antiCheat ? Colors::Ivory : Colors::Gray, 1.f, true, "Reverse hollowing");
				//renderer->DrawString(TextStartXOffset, TextStartYOffset + (20 * TextLineIndex++), Settings::antiCheat ? Colors::Ivory : Colors::Gray, 1.f, true, "Fly mode");


				//renderer->DrawString(TextStartXOffset, TextStartYOffset + (20 * TextLineIndex++), Settings::antiCheat ? Colors::Ivory : Colors::Gray, 1.f, true, "Detect cheaters");
				//renderer->DrawString(TextStartXOffset, TextStartYOffset + (20 * TextLineIndex++), Settings::connectionInfo ? Colors::Ivory : Colors::Gray, 1.f, true, "Player network info");

				std::string giveSoulsStr = "Give ";
				switch (Settings::giveSoulsModifier)
				{
				case 0:
					giveSoulsStr += "1000 souls";
					break;
				case 1:
					giveSoulsStr += "10000 souls";
					break;
				case 2:
					giveSoulsStr += "100000 souls";
					break;
				case 3:
					giveSoulsStr += "1000000 souls";
					break;
				default:
					giveSoulsStr += "Unknown";
					break;
				}
				renderer->DrawString(TextStartXOffset, TextStartYOffset + (20 * TextLineIndex++), TextLineIndex == currentSelectedIndex ? Colors::Ivory : Colors::Gray, 1.f, true, giveSoulsStr.c_str());
			

				//std::string equipSpellStr = "Equip spell: ";
				//switch (Settings::giveSoulsModifier)
				//{
				//case 0:
				//	equipSpellStr += "Soul Arrow";
				//	break;
				//
				//default:
				//	equipSpellStr += "Unknown";
				//	break;
				//}
				//renderer->DrawString(TextStartXOffset, TextStartYOffset + (20 * TextLineIndex++), TextLineIndex == currentSelectedIndex ? Colors::Ivory : Colors::Gray, 1.f, true, equipSpellStr.c_str());



				//std::string kickPlayerStr = "Kick Player: ";
				//switch (Settings::giveSoulsModifier)
				//{
				//case 0:
				//	kickPlayerStr += "N/A";
				//	break;
				//default:
				//	kickPlayerStr += "Unknown";
				//	break;
				//}
				//renderer->DrawString(TextStartXOffset, TextStartYOffset + (20 * TextLineIndex++), TextLineIndex == currentSelectedIndex ? Colors::Ivory : Colors::Gray, 1.f, true, kickPlayerStr.c_str());


				std::string teleportStr = "Teleport";
				if (!Teleportation::wayPointStored) {
					teleportStr += ": No waypoints stored";
				}
			
				renderer->DrawString(TextStartXOffset, TextStartYOffset + (20 * TextLineIndex++), TextLineIndex == currentSelectedIndex ? Colors::Ivory : Colors::Gray, 1.f, true, teleportStr.c_str());
				renderer->DrawString(TextStartXOffset, TextStartYOffset + (20 * TextLineIndex++), TextLineIndex == currentSelectedIndex ? Colors::Ivory : Colors::Gray, 1.f, true, "Die");
			}
		}
	
	};

	namespace Handlers {
		/*
		Called right before pSwapChain->Present() is called. All drawing calls happening here will be rendered to the current frame.
		*/
		using namespace DirectX;
		using namespace DirectX::SimpleMath;

		void OnPresent(IDXGISwapChain* pSwapChain)
		{
			Features::GUI::OnPresent(pSwapChain);

			
			return;


			//if (!DirectX11::Drawing::PrepareDrawing(pSwapChain))
			//	return;
			//
			///* Watermark */
			//DirectX11::Drawing::DrawString(20, 20, ::DirectX::Colors::GhostWhite, 1.f, false, "Darksouls : Remastered Plugin v0.2");
			//


#ifdef _DEBUG

			//int StringOffset = 10;
			//
			//SDK::CLocalPlayer localPlayer;
			//
			//if (localPlayer.PlayerInstance())
			//{
			//	auto playerInstance = localPlayer.PlayerInstance();
			//
			//
			//	::DirectX11::Drawing::DrawRectangle(20, 20, 100, 100, ::DirectX::Colors::White);
			//
			//	DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "Health %d",		playerInstance->Health());
			//	DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "MaxHealth %d",	playerInstance->MaxHealth());
			//	
			//	DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "Stamina %d",	playerInstance->Stamina());
			//	DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "MaxStamina %d",	playerInstance->MaxStamina());
			//
			//	DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "CharType %d", playerInstance->CharType());
			//	DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "TeamType %d", playerInstance->TeamType());
			//
			//	if (playerInstance->PlayerPhysics())
			//	{
			//		auto playerPhysics = playerInstance->PlayerPhysics();
			//
			//		if (playerPhysics->PhysicsData())
			//		{
			//			auto playerPhysicsData = playerPhysics->PhysicsData();
			//
			//			DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "X %0.2f", playerPhysicsData->XCoord());
			//			DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "Y %0.2f", playerPhysicsData->YCoord());
			//			DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "Z %0.2f", playerPhysicsData->ZCoord());
			//		}
			//	}
			//
			//	if (playerInstance->PlayerGameData())
			//	{
			//		auto playerGameData = playerInstance->PlayerGameData();
			//
			//		DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "Vitality %d", playerGameData->Vitality());
			//		DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "Attunement %d", playerGameData->Attunement());
			//		DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "Eddurance %d", playerGameData->Eddurance());
			//		DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "Strenght %d", playerGameData->Strenght());
			//		DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "Dexterirty %d", playerGameData->Dexterirty());
			//		DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "Resistance %d", playerGameData->Resistance());
			//
			//		DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "Intelligence %d", playerGameData->Intelligence());
			//		DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "Faith %d", playerGameData->Faith());
			//		DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, "SoulLevel %d", playerGameData->SoulLevel());
			//		DirectX11::Drawing::DrawString(20, 20 * StringOffset++, ::DirectX::Colors::GhostWhite, 1.f, false, playerGameData->Gender() == SDK::Internal::CPlayerGameData::GENDER_MALE ? "Gender: Male" : "Gender: Female");
			//
			//	}
			//}
#endif
		}

		/*
		Called every time a key input happens
		*/
		void OnInput(Input::KeyDefinition_t key, float state)
		{
			Features::GUI::OnKey(key, state);

#ifdef _DEBUG

			msg("DSR::OnInput: Key: " << key << " state: " << state);
#endif

		}
	}


	bool InstallPlugin()
	{
		if (!Addresses::Initialize())
			return false;

		if (!DirectX11::Hook::HookDirectX(Handlers::OnPresent))
			return false;

		if (!Input::HookInput("DARK SOULS", Handlers::OnInput))
			return false;

		return true;
	}

	bool UninstallPlugin()
	{
		if (!DirectX11::Hook::UnhookDirectX())
			return false;

		if (!Input::UnhookInput())
			return false;

		return true;
	}
}