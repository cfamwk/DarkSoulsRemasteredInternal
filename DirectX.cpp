#include "DirectX.h"
#include "Utility.h"
#include <vector>
#include <locale>
#include <codecvt>

namespace Plugin {

	namespace DirectX11
	{
		namespace Hook
		{

			bool						directXHooked = false;
			void*						pPresentPatchLoc = 0;
			std::vector<unsigned char>	originalBytes;

			/*

			*/
			bool GetDirectXAddress(void** & pSwapChainVTable)
			{
				HWND hWnd = GetForegroundWindow();

				ID3D11Device*			pDevice = NULL;
				ID3D11DeviceContext*	pContext = NULL;
				IDXGISwapChain*			pSwapChain = NULL;

				D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
				DXGI_SWAP_CHAIN_DESC swapChainDesc;
				ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
				swapChainDesc.BufferCount = 1;
				swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.OutputWindow = hWnd;
				swapChainDesc.SampleDesc.Count = 1;
				swapChainDesc.Windowed = GetWindowLong(hWnd, GWL_STYLE) & WS_POPUP != 0 ? FALSE : TRUE;
				swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

				if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, NULL, &pContext)))
					return false;

				pSwapChainVTable = *(void***)pSwapChain;

				pDevice->Release();
				pContext->Release();
				pSwapChain->Release();

				return true;
			}


			/*
				Detours DirectX to provided function pointers
			*/
			bool HookDirectX(FNPresent pPresentDetour)
			{

				if (directXHooked)
					return true;

				unsigned char absoluteCallBytes[]{ 0xff, 0x15,0x02,0x00,0x00,0x00,0xeb,0x08,	1,1,1,1,1,1,1,1 };
				unsigned char storeStateBytes[]{
					0x50,		//push rax
					0x51,		//push rcx
					0x52,		//push rdx
					0x53,		//push rbx
					/*0x54,*/	//push rsp
					0x55,		//push rbp
					0x56,		//push rsi
					0x57,		//push rdi
					0x41,0x50,	//push r8
					0x41,0x51,	//push r9
					0x41,0x52,	//push r10
					0x41,0x53,	//push r11
					0x41,0x54,	//push r12
					0x41,0x55,	//push r13
					0x41,0x56,	//push r14
					0x41,0x57,	//push r15
					0x9c,		//push fq
				};
				unsigned char restoreStateBytes[]{
					0x9d,				//pop fq
					0x41, 0x8 + 0x57,	//pop r15
					0x41, 0x8 + 0x56,	//pop r14
					0x41, 0x8 + 0x55,	//pop r13
					0x41, 0x8 + 0x54,	//pop r12
					0x41, 0x8 + 0x53,	//pop r11
					0x41, 0x8 + 0x52,	//pop r10
					0x41, 0x8 + 0x51,	//pop r9
					0x41, 0x8 + 0x50,	//pop r8
					0x8 + 0x57,			//pop rdi
					0x8 + 0x56,			//pop rsi
					0x8 + 0x55,			//pop rbp
					/*0x8 + 0x54,*/		//pop rsp
					0x8 + 0x53,			//pop rbx
					0x8 + 0x52,			//pop rdx
					0x8 + 0x51,			//pop rcx
					0x8 + 0x50,			//pop rax
				};

				unsigned char absoluteJumpBytes[]{ 0xff, 0x25,0x00,0x00,0x00,0x00,				1,1,1,1,1,1,1,1 };

				void** SwapChainVTable = 0;

				if (!GetDirectXAddress(SwapChainVTable))
					return false;


				auto BuildExecBuffer = [&](const void* PatchLoc, std::vector<unsigned char>& execBufferInstructions, size_t& sizeOfOriginalBytes, const std::vector<unsigned char>* optionalArgInstructions = 0)
				{
					execBufferInstructions.clear();

					//Count size to be replaced
					ldasm_data ld = { 0 };
					sizeOfOriginalBytes = 0;
					while (sizeOfOriginalBytes < sizeof(absoluteCallBytes))
						sizeOfOriginalBytes += ldasm((unsigned char*)PatchLoc + sizeOfOriginalBytes, &ld, true);

					//Store state
					for (auto b : storeStateBytes)
						execBufferInstructions.push_back(b);

					//Call detour, push arguments if any
					if (optionalArgInstructions) {
						for (auto b : *optionalArgInstructions) {
							execBufferInstructions.push_back(b);
						}
					}
					*(void**)(&absoluteCallBytes[8]) = pPresentDetour;

					for (auto b : absoluteCallBytes)
						execBufferInstructions.push_back(b);

					//Cleanup stack if args
					if (optionalArgInstructions) {
						size_t numArgs = optionalArgInstructions->size();
						unsigned char stackSize = (unsigned char)numArgs * 8;
						execBufferInstructions.push_back(0x48);
						execBufferInstructions.push_back(0x83);
						execBufferInstructions.push_back(0xc4);
						execBufferInstructions.push_back(stackSize);
					}

					//Restore state
					for (auto b : restoreStateBytes)
						execBufferInstructions.push_back(b);

					//run replaced bytes
					for (size_t i = 0; i < sizeOfOriginalBytes; i++)
						execBufferInstructions.push_back(((unsigned char*)PatchLoc)[i]);

					//jump back
					*(void**)(&absoluteJumpBytes[6]) = (void*)((unsigned char*)PatchLoc + sizeOfOriginalBytes);
					for (auto b : absoluteJumpBytes)
						execBufferInstructions.push_back(b);

				};


				pPresentPatchLoc = (unsigned char*)SwapChainVTable[SWAPCHAIN_PRESENT_INDEX] + 0x5;//gameoverlayrenderer.dll (steam UI) sets a relative jump. We place our jump right after this

				//Build exec buffer
				std::vector<unsigned char> execBufferInstructions;
				std::vector<unsigned char> callArgInstructions;
				size_t sizeOfOriginalBytes = 0;
				callArgInstructions.push_back(0x51); //push rcx to get swapchain
				BuildExecBuffer(pPresentPatchLoc, execBufferInstructions, sizeOfOriginalBytes, &callArgInstructions);

				//Alloc and write to execbuffer
				void* pExecBuffer = VirtualAlloc(0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				memcpy(pExecBuffer, execBufferInstructions.data(), execBufferInstructions.size());

				//Store original bytes
				for (size_t i = 0; i < sizeOfOriginalBytes; i++)
					originalBytes.push_back(((unsigned char*)pPresentPatchLoc)[i]);

				//Apply patch to original code
				DWORD OldProtection = 0;
				VirtualProtect(pPresentPatchLoc, 1, PAGE_EXECUTE_READWRITE, &OldProtection);
				memset(pPresentPatchLoc, 0x90, sizeOfOriginalBytes); // make sure no instructions grow into eachother
				*(void**)(&absoluteJumpBytes[6]) = pExecBuffer;
				memcpy(pPresentPatchLoc, absoluteJumpBytes, sizeof(absoluteJumpBytes));
				VirtualProtect(pPresentPatchLoc, 1, OldProtection, &OldProtection);


				msg("DirectX11: ExecBuffer: 0x" << pExecBuffer);
				msg("DirectX11: pPresentPatchLoc: 0x" << pPresentPatchLoc);

				directXHooked = true;

				return true;
			}


			/*
				Restores DirectX original state
			*/
			bool UnhookDirectX()
			{
				if (!directXHooked)
					return true;

				DWORD OldProtection = 0;
				VirtualProtect(pPresentPatchLoc, 1, PAGE_EXECUTE_READWRITE, &OldProtection);
				memcpy(pPresentPatchLoc, originalBytes.data(), originalBytes.size());
				VirtualProtect(pPresentPatchLoc, 1, OldProtection, &OldProtection);

				Sleep(1000);
				directXHooked = false;

				return true;
			}
		}
		namespace Rendering
		{
			CRenderer* CRenderer::m_instance = nullptr;

			CRenderer::CRenderer(IDXGISwapChain * swapchain)
				: m_swapchain(swapchain)
			{
				m_swapchain->GetDevice(__uuidof(m_device), (void**)&m_device);
				m_swapchain->GetContainingOutput(&m_output);
				m_swapchain->GetDesc(&m_swapchainDesc);

				m_device->GetImmediateContext(&m_context);

				m_spriteFont = std::make_unique<SpriteFont>(SpriteFont(m_device, L"sylfaen.spritefont"));
				m_spriteBatch = std::make_unique<SpriteBatch>(SpriteBatch(m_context));
				m_basicEffect = std::make_unique<BasicEffect>(BasicEffect(m_device));

				m_commonStates = std::make_unique<CommonStates>(CommonStates(m_device));

				void const* shaderByteCode;
				size_t byteCodeLength;

				m_basicEffect->SetVertexColorEnabled(true);
				m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

				m_device->CreateInputLayout(VertexPositionColor::InputElements,
					VertexPositionColor::InputElementCount,
					shaderByteCode, byteCodeLength,
					m_inputLayout.ReleaseAndGetAddressOf());


				m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(PrimitiveBatch<VertexPositionColor>(m_context));
			}

			CRenderer * CRenderer::GetRenderer(IDXGISwapChain * swapchain)
			{

				if (!m_instance)
					m_instance = new CRenderer(swapchain);


				return m_instance;
			}

			inline void CRenderer::DestroyRenderer()
			{
				delete m_instance;
				m_instance = nullptr;
			}

			void CRenderer::DrawTriangle()
			{
				m_context->OMSetBlendState(m_commonStates->Opaque(), nullptr, 0xFFFFFFFF);
				m_context->OMSetDepthStencilState(m_commonStates->DepthNone(), 0);
				m_context->RSSetState(m_commonStates->CullNone());

				m_basicEffect->Apply(m_context);

				m_context->IASetInputLayout(m_inputLayout.Get());

				m_primitiveBatch->Begin();

				VertexPositionColor v1(Vector3(0.3f, 0.5f, 1.f), Colors::Yellow);
				VertexPositionColor v2(Vector3(0.5f, -0.5f, 1.f), Colors::White);
				VertexPositionColor v3(Vector3(-0.5f, -0.5f, 1.f), Colors::Green);

				m_primitiveBatch->DrawTriangle(v1, v2, v3);

				m_primitiveBatch->End();
			}



			void CRenderer::DrawString(float x, float y, XMVECTORF32 Color, float scale, bool center, const char * Format, ...)
			{
				char Buffer[1024] = { '\0' };
				va_list va_alist;
				va_start(va_alist, Format);
				vsprintf_s(Buffer, Format, va_alist);
				va_end(va_alist);

				size_t slen = strlen(Buffer) + 1;
				std::wstring ws(strlen(Buffer), L'#');
				mbstowcs(&ws[0], Buffer, slen);
				std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
				std::wstring output = converter.from_bytes(Buffer);

				XMFLOAT2 origin(0, 0);

				if (center)
				{
					::DirectX::XMVECTOR size = m_spriteFont->MeasureString(output.c_str());
					float sizeX = XMVectorGetX(size);
					float sizeY = XMVectorGetY(size);
					origin = XMFLOAT2(sizeX / 2, sizeY / 2);
				}

				m_spriteBatch->Begin();
				m_spriteFont->DrawString(m_spriteBatch.get(), output.c_str(), XMFLOAT2(x, y), Color, 0.0f, origin, scale);
				m_spriteBatch->End();
				return;
			}

			int CRenderer::LoadTexture(const uint8_t * pData, size_t size)
			{
				auto tex = new CRendererTexture(m_device, pData, size, m_textures.size());

				m_textures.push_back(tex);

				return tex->GetId();
			}

			void CRenderer::DrawTexture(float x, float y, int id)
			{
				m_spriteBatch->Begin(SpriteSortMode_Deferred, m_commonStates->NonPremultiplied());
				m_spriteBatch->Draw(m_textures[id]->Get(), XMFLOAT2(x, y), nullptr, Colors::White, 0.f, XMFLOAT2(0, 0));
				m_spriteBatch->End();
			}

			void CRenderer::DrawShape(float x0, float y0, const FXMVECTOR & color0, float x1, float y1, const FXMVECTOR & color1)
			{
				m_context->OMSetBlendState(m_commonStates->Opaque(), nullptr, 0xFFFFFFFF);
				m_context->OMSetDepthStencilState(m_commonStates->DepthNone(), 0);
				m_context->RSSetState(m_commonStates->CullNone());

				m_basicEffect->Apply(m_context);

				m_context->IASetInputLayout(m_inputLayout.Get());

				VertexPositionColor v0(Vector3(x0, y0, 1.f), color0);
				TranslateScreenToVertexPosistion(v0.position);

				VertexPositionColor v1(Vector3(x1, y1, 1.f), color1);
				TranslateScreenToVertexPosistion(v1.position);

				m_primitiveBatch->Begin();
				m_primitiveBatch->DrawLine(v0, v1);
				m_primitiveBatch->End();
			}

			void CRenderer::DrawShape(float x0, float y0, const FXMVECTOR & color0, float x1, float y1, const FXMVECTOR & color1, float x2, float y2, const FXMVECTOR & color2)
			{
				m_context->OMSetBlendState(m_commonStates->Opaque(), nullptr, 0xFFFFFFFF);
				m_context->OMSetDepthStencilState(m_commonStates->DepthNone(), 0);
				m_context->RSSetState(m_commonStates->CullNone());

				m_basicEffect->Apply(m_context);

				m_context->IASetInputLayout(m_inputLayout.Get());

				VertexPositionColor v0(Vector3(x0, y0, 1.f), color0);
				TranslateScreenToVertexPosistion(v0.position);

				VertexPositionColor v1(Vector3(x2, y2, 1.f), color2);
				TranslateScreenToVertexPosistion(v1.position);

				VertexPositionColor v2(Vector3(x1, y1, 1.f), color1);
				TranslateScreenToVertexPosistion(v2.position);


				m_primitiveBatch->Begin();
				m_primitiveBatch->DrawTriangle(v0, v1, v2);
				m_primitiveBatch->End();
			}

			void CRenderer::DrawShape(float x0, float y0, const FXMVECTOR & color0, float x1, float y1, const FXMVECTOR & color1, float x2, float y2, const FXMVECTOR & color2, float x3, float y3, const FXMVECTOR & color3)
			{
				m_context->OMSetBlendState(m_commonStates->Opaque(), nullptr, 0xFFFFFFFF);
				m_context->OMSetDepthStencilState(m_commonStates->DepthNone(), 0);
				m_context->RSSetState(m_commonStates->CullNone());

				m_basicEffect->Apply(m_context);

				m_context->IASetInputLayout(m_inputLayout.Get());

				VertexPositionColor v0(Vector3(x0, y0, 1.f), color0);
				TranslateScreenToVertexPosistion(v0.position);

				VertexPositionColor v1(Vector3(x1, y1, 1.f), color1);
				TranslateScreenToVertexPosistion(v1.position);

				VertexPositionColor v2(Vector3(x3, y3, 1.f), color3);
				TranslateScreenToVertexPosistion(v2.position);

				VertexPositionColor v3(Vector3(x2, y2, 1.f), color2);
				TranslateScreenToVertexPosistion(v3.position);

				m_primitiveBatch->Begin();
				m_primitiveBatch->DrawQuad(v0, v1, v2, v3);
				m_primitiveBatch->End();
			}
			
		}
	}
}