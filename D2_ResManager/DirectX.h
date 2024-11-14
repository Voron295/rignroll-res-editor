#ifndef DIRECTX_INCLUDE_H
#define DIRECTX_INCLUDE_H
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <d3dx9.h>
using namespace DirectX;

template<typename T>
inline void SafeRelease(T& ptr)
{
	if (ptr != NULL)
	{
		ptr->Release();
		ptr = NULL;
	}
}

struct DXUTSpriteVertex
{
	D3DXVECTOR3 vPos;
	D3DXCOLOR vColor;
	D3DXVECTOR2 vTex;
};

class D3DSprite
{
private:
	ID3D11Buffer* m_d3dVertexBuffer;
	XMFLOAT2 m_pPosition[2];
	XMFLOAT2 m_pUV[2];
	XMFLOAT2 m_pRealPosition[2];
	D3DXCOLOR m_pColor;
	ID3D11Texture2D *m_pTexture;
	ID3D11ShaderResourceView *m_pTextureResource;
	class D3D *m_pD3D;
	bool m_bClamp;

	D3DSprite(class D3D *d3d, XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT2 uv1, XMFLOAT2 uv2, D3DXCOLOR color, ID3D11Texture2D *texture, bool clamp);
public:
	friend class D3D;

	void Reset();
	void Resize(float aspect);
	void SetPos(float x, float y);
	void GetPos(float *x, float *y);
	void SetTexture(ID3D11Texture2D *texture);
	~D3DSprite();
};

class D3D
{
private:
	int m_dwWidth;
	int m_dwHeight;
	HWND m_hWnd;
	bool m_bLinearSampler;

	// directx interface
	ID3D11Device* m_d3dDevice;
	ID3D11DeviceContext* m_d3dDeviceContext;
	IDXGISwapChain* m_d3dSwapChain;
	ID3D11RenderTargetView* m_d3dRenderTargetView;
	ID3D11DepthStencilView* m_d3dDepthStencilView;
	ID3D11Texture2D* m_d3dDepthStencilBuffer;
	ID3D11DepthStencilState* m_d3dDepthStencilState;
	ID3D11RasterizerState* m_d3dRasterizerState;
	ID3D11BlendState *m_d3dBlendState;
	ID3D11SamplerState *m_d3dSamplerState;
	ID3D11SamplerState *m_d3dSamplerStateLinear;
	ID3D11SamplerState *m_d3dSamplerStateClamp;
	D3D11_VIEWPORT m_Viewport;

	ID3D11InputLayout* m_d3dInputLayout;

	ID3D11VertexShader* m_d3dVertexShader;
	ID3D11PixelShader* m_d3dPixelShader;
	ID3D11PixelShader* m_d3dPixelShaderUntex;

public:
	friend class D3DSprite;

	D3D();
	~D3D();

	UINT Init(HINSTANCE instance, HWND hWnd);
	void Begin(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil);
	void End();
	void DrawSprite(D3DSprite *sprite);
	void Resize(int width, int height);
	void SetLinearSampler(bool enable);
	bool GetLinearSampler();
	D3DSprite *CreateSprite(XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT2 uv1, XMFLOAT2 uv2, D3DXCOLOR color, ID3D11Texture2D *texture, bool clamp = true);
	ID3D11Texture2D *CreateTexture(D3DCOLOR *data, int width, int height);
};

#endif