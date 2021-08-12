#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <vector>

// DXTK
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PostProcess.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"

namespace Plugin {

	namespace DirectX11
	{
		using namespace DirectX;
		using namespace DirectX::SimpleMath;

		#define SWAPCHAIN_PRESENT_INDEX 8
		typedef void(*FNPresent)(IDXGISwapChain*);
	
		namespace Hook {
			bool HookDirectX(FNPresent pPresentDetour);
			bool UnhookDirectX();

		}

		namespace Rendering {

			class CRenderer
			{
			private:
				static CRenderer* m_instance;	
				HANDLE												m_window;
				IDXGISwapChain*										m_swapchain;
				ID3D11Device*										m_device;
				ID3D11DeviceContext*								m_context;
				IDXGIOutput*										m_output;
				IDXGISurface*										m_surface;

				DXGI_SWAP_CHAIN_DESC								m_swapchainDesc;

				std::unique_ptr<SpriteFont>							 m_spriteFont;
				std::unique_ptr<SpriteBatch>						 m_spriteBatch;
				std::unique_ptr<BasicEffect>						 m_basicEffect;
				std::unique_ptr<CommonStates>						 m_commonStates;
				std::unique_ptr<PrimitiveBatch<VertexPositionColor>> m_primitiveBatch;
				Microsoft::WRL::ComPtr<ID3D11InputLayout>			 m_inputLayout;

				class CRendererTexture
				{
				private:
					Microsoft::WRL::ComPtr<ID3D11Resource>				m_resource;
					Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_texture;
					int													m_id;
					Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_tex2d;

				public:
					CRendererTexture(ID3D11Device * device, const uint8_t * pData, size_t size, int id)
					{
						m_id = id;

						CreateWICTextureFromMemory(device, pData, size, m_resource.GetAddressOf(), m_texture.ReleaseAndGetAddressOf());

						m_resource.As(&m_tex2d);
					}

					int GetId()
					{
						return m_id;
					}

					ID3D11ShaderResourceView* Get()
					{
						return m_texture.Get();
					}
				};
				std::vector<CRendererTexture*> m_textures;

				CRenderer(IDXGISwapChain* swapchain);

				void TranslateScreenToVertexPosistion(XMFLOAT3& v)
				{

					v.x = (v.x / (m_swapchainDesc.BufferDesc.Width/2)) - 1.f;
					v.y = ((v.y / (m_swapchainDesc.BufferDesc.Height/2)) - 1.f) * -1.f;
				}

			public:
				static CRenderer* GetRenderer(IDXGISwapChain* swapchain);
				static void DestroyRenderer();

				void DrawTriangle();

				void DrawString(float x, float y, XMVECTORF32 Color, float scale, bool center, const char* Format, ...);

				int LoadTexture(const uint8_t * pData, size_t size);

				void DrawTexture(float x, float y, int id);

				void DrawShape(
					float x0, float y0, const FXMVECTOR& color0, 
					float x1, float y1, const FXMVECTOR& color1);

				void DrawShape(
					float x0, float y0, const FXMVECTOR& color0, 
					float x1, float y1, const FXMVECTOR& color1,
					float x2, float y2, const FXMVECTOR& color2);

				void DrawShape(
					float x0, float y0, const FXMVECTOR& color0,
					float x1, float y1, const FXMVECTOR& color1,
					float x2, float y2, const FXMVECTOR& color2,
					float x3, float y3, const FXMVECTOR& color3);
			};
		}
	
		namespace Drawing {
			bool PrepareDrawing(IDXGISwapChain * pSwapChain);
			void DrawRectangle(float x, float y, float w, float h, ::DirectX::XMVECTORF32 Color);
			void DrawString(float x, float y, ::DirectX::XMVECTORF32 Color, float scale, bool center, const char * Format, ...);
		}
	}
}