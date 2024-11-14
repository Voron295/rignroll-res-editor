#include "DirectX.h"
#include "VS.h"
#include "PS.h"
#include "PSUntex.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")

D3DSprite::D3DSprite(D3D *d3d, XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT2 uv1, XMFLOAT2 uv2, D3DXCOLOR color, ID3D11Texture2D *texture, bool clamp)
{
	m_d3dVertexBuffer = nullptr;
	//m_d3dIndexBuffer = nullptr;
	m_pPosition[0] = pos;
	m_pPosition[1] = size;
	m_pUV[0] = uv1;
	m_pUV[1] = uv2;
	m_pColor = color;
	m_pTexture = texture;
	m_pD3D = d3d;
	m_bClamp = clamp;
	
	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.ByteWidth = 6 * sizeof(DXUTSpriteVertex);
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.MiscFlags = 0;

	pos.x /= (float)d3d->m_dwWidth / 2;
	size.x /= (float)d3d->m_dwWidth / 2;
	pos.y /= (float)d3d->m_dwHeight / 2;
	size.y /= (float)d3d->m_dwHeight / 2;

	m_pRealPosition[0].x = pos.x - 1.f;
	m_pRealPosition[0].y = -pos.y + 1.f;
	m_pRealPosition[1].x = pos.x + size.x - 1.f;
	m_pRealPosition[1].y = -(pos.y + size.y) + 1.f;

	DXUTSpriteVertex spriteVerts[6] = {
		{ { m_pRealPosition[0].x, m_pRealPosition[0].y, 1.0f }, color, { uv1.x, uv1.y } },
		{ { m_pRealPosition[1].x, m_pRealPosition[0].y, 1.0f }, color, { uv2.x, uv1.y } },
		{ { m_pRealPosition[0].x, m_pRealPosition[1].y, 1.0f }, color, { uv1.x, uv2.y } },

		{ { m_pRealPosition[1].x, m_pRealPosition[0].y, 1.0f }, color, { uv2.x, uv1.y } },
		{ { m_pRealPosition[1].x, m_pRealPosition[1].y, 1.0f }, color, { uv2.x, uv2.y } },
		{ { m_pRealPosition[0].x, m_pRealPosition[1].y, 1.0f }, color, { uv1.x, uv2.y } }
	};

	d3d->m_d3dDevice->CreateBuffer(&BufferDesc, NULL, &m_d3dVertexBuffer);
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (S_OK == d3d->m_d3dDeviceContext->Map(m_d3dVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)) {
		CopyMemory(MappedResource.pData, (void*)spriteVerts, 6 * sizeof(DXUTSpriteVertex));
		d3d->m_d3dDeviceContext->Unmap(m_d3dVertexBuffer, 0);
	}

	// init texture
	m_pTextureResource = 0;
	if (texture)
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texture->GetDesc(&texDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Format = texDesc.Format;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		d3d->m_d3dDevice->CreateShaderResourceView(texture, &SRVDesc, &m_pTextureResource);
	}
}

void D3DSprite::Reset()
{
	SafeRelease(m_d3dVertexBuffer);
	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.ByteWidth = 6 * sizeof(DXUTSpriteVertex);
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.MiscFlags = 0;

	auto color = m_pColor;
	auto pos = m_pPosition[0];
	auto size = m_pPosition[1];

	pos.x /= (float)m_pD3D->m_dwWidth / 2;
	size.x /= (float)m_pD3D->m_dwWidth / 2;
	pos.y /= (float)m_pD3D->m_dwHeight / 2;
	size.y /= (float)m_pD3D->m_dwHeight / 2;

	m_pRealPosition[0].x = pos.x - 1.f;
	m_pRealPosition[0].y = -pos.y + 1.f;
	m_pRealPosition[1].x = pos.x + size.x - 1.f;
	m_pRealPosition[1].y = -(pos.y + size.y) + 1.f;

	DXUTSpriteVertex spriteVerts[6] = {
		{ { m_pRealPosition[0].x, m_pRealPosition[0].y, 1.0f }, color, { m_pUV[0].x, m_pUV[0].y } },
		{ { m_pRealPosition[1].x, m_pRealPosition[0].y, 1.0f }, color, { m_pUV[1].x, m_pUV[0].y } },
		{ { m_pRealPosition[0].x, m_pRealPosition[1].y, 1.0f }, color, { m_pUV[0].x, m_pUV[1].y } },

		{ { m_pRealPosition[1].x, m_pRealPosition[0].y, 1.0f }, color, { m_pUV[1].x, m_pUV[0].y } },
		{ { m_pRealPosition[1].x, m_pRealPosition[1].y, 1.0f }, color, { m_pUV[1].x, m_pUV[1].y } },
		{ { m_pRealPosition[0].x, m_pRealPosition[1].y, 1.0f }, color, { m_pUV[0].x, m_pUV[1].y } }
	};

	m_pD3D->m_d3dDevice->CreateBuffer(&BufferDesc, NULL, &m_d3dVertexBuffer);
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (S_OK == m_pD3D->m_d3dDeviceContext->Map(m_d3dVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)) {
		CopyMemory(MappedResource.pData, (void*)spriteVerts, 6 * sizeof(DXUTSpriteVertex));
		m_pD3D->m_d3dDeviceContext->Unmap(m_d3dVertexBuffer, 0);
	}
}

void D3DSprite::Resize(float aspect)
{
	m_pPosition[1].x *= aspect;
	m_pPosition[1].y *= aspect;
	Reset();
}

void D3DSprite::SetPos(float x, float y)
{
	m_pPosition[0].x = x;
	m_pPosition[0].y = y;

	Reset();
}

void D3DSprite::GetPos(float *x, float *y)
{
	if (x)
		*x = m_pPosition[0].x;
	if (y)
		*y = m_pPosition[0].y;
}

void D3DSprite::SetTexture(ID3D11Texture2D *texture)
{
	if (m_pTextureResource)
		SafeRelease(m_pTextureResource);

	if (texture)
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texture->GetDesc(&texDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Format = texDesc.Format;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		m_pD3D->m_d3dDevice->CreateShaderResourceView(texture, &SRVDesc, &m_pTextureResource);
	}
}

D3DSprite::~D3DSprite()
{
	SafeRelease(m_pTextureResource);
	SafeRelease(m_d3dVertexBuffer);
}

D3D::D3D()
{
	m_bLinearSampler = true;

	// Direct3D device and swap chain.
	m_d3dDevice = nullptr;
	m_d3dDeviceContext = nullptr;
	m_d3dSwapChain = nullptr;

	// Render target view for the back buffer of the swap chain.
	m_d3dRenderTargetView = nullptr;
	// Depth/stencil view for use as a depth buffer.
	m_d3dDepthStencilView = nullptr;
	// A texture to associate to the depth stencil view.
	m_d3dDepthStencilBuffer = nullptr;

	// Define the functionality of the depth/stencil stages.
	m_d3dDepthStencilState = nullptr;
	// Define the functionality of the rasterizer stage.
	m_d3dRasterizerState = nullptr;
	m_Viewport = { 0 };

	// Vertex buffer data
	m_d3dInputLayout = nullptr;

	// Shader data
	m_d3dVertexShader = nullptr;
	m_d3dPixelShader = nullptr;
	m_d3dPixelShaderUntex = nullptr;
}

D3D::~D3D()
{
	SafeRelease(m_d3dInputLayout);
	SafeRelease(m_d3dVertexShader);
	SafeRelease(m_d3dPixelShader);
	SafeRelease(m_d3dPixelShaderUntex);

	SafeRelease(m_d3dDepthStencilView);
	SafeRelease(m_d3dRenderTargetView);
	SafeRelease(m_d3dDepthStencilBuffer);
	SafeRelease(m_d3dDepthStencilState);
	SafeRelease(m_d3dRasterizerState);
	SafeRelease(m_d3dSamplerState);
	SafeRelease(m_d3dSamplerStateLinear);
	SafeRelease(m_d3dSamplerStateClamp);
	SafeRelease(m_d3dBlendState);
	SafeRelease(m_d3dSwapChain);
	SafeRelease(m_d3dDeviceContext);

	SafeRelease(m_d3dDevice);
	/*ID3D11Debug *m_d3dDebug;
	m_d3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&m_d3dDebug));
	m_d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);*/
}

UINT D3D::Init(HINSTANCE instance, HWND hWnd)
{
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	// Compute the exact client dimensions. This will be used
	// to initialize the render targets for our swap chain.
	m_dwWidth = clientRect.right - clientRect.left;
	m_dwHeight = clientRect.bottom - clientRect.top;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = m_dwWidth;
	swapChainDesc.BufferDesc.Height = m_dwHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = TRUE;

	UINT createDeviceFlags = 0;
#if _DEBUG
	createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_9_1
	};

	// This will be the feature level that 
	// is used to create our device and swap chain.
	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, createDeviceFlags, featureLevels, _countof(featureLevels),
		D3D11_SDK_VERSION, &swapChainDesc, &m_d3dSwapChain, &m_d3dDevice, &featureLevel,
		&m_d3dDeviceContext);

	if (FAILED(hr))
	{
		return -1;
	}

	ID3D11Texture2D* backBuffer;
	hr = m_d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(hr))
	{
		return -1;
	}

	hr = m_d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &m_d3dRenderTargetView);
	if (FAILED(hr))
	{
		return -1;
	}

	SafeRelease(backBuffer);

	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0; // No CPU access required.
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.Width = m_dwWidth;
	depthStencilBufferDesc.Height = m_dwHeight;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = m_d3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &m_d3dDepthStencilBuffer);
	if (FAILED(hr))
	{
		return -1;
	}

	hr = m_d3dDevice->CreateDepthStencilView(m_d3dDepthStencilBuffer, nullptr, &m_d3dDepthStencilView);
	if (FAILED(hr))
	{
		return -1;
	}

	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilStateDesc.DepthEnable = FALSE;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilStateDesc.StencilEnable = FALSE;

	hr = m_d3dDevice->CreateDepthStencilState(&depthStencilStateDesc, &m_d3dDepthStencilState);

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state object.
	hr = m_d3dDevice->CreateRasterizerState(&rasterizerDesc, &m_d3dRasterizerState);
	if (FAILED(hr))
	{
		return -1;
	}

	D3D11_BLEND_DESC BSDesc;
	ZeroMemory(&BSDesc, sizeof(D3D11_BLEND_DESC));

	BSDesc.RenderTarget[0].BlendEnable = TRUE;
	BSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BSDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BSDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BSDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BSDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BSDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;

	m_d3dDevice->CreateBlendState(&BSDesc, &m_d3dBlendState);

	D3D11_SAMPLER_DESC SSDesc;
	ZeroMemory(&SSDesc, sizeof(D3D11_SAMPLER_DESC));
	SSDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SSDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SSDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SSDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SSDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SSDesc.MaxAnisotropy = 0;
	SSDesc.MinLOD = 0;
	SSDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	m_d3dDevice->CreateSamplerState(&SSDesc, &m_d3dSamplerState);

	SSDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SSDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SSDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	m_d3dDevice->CreateSamplerState(&SSDesc, &m_d3dSamplerStateLinear);

	SSDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	m_d3dDevice->CreateSamplerState(&SSDesc, &m_d3dSamplerStateClamp);

	m_Viewport.Width = static_cast<float>(m_dwWidth);
	m_Viewport.Height = static_cast<float>(m_dwHeight);
	m_Viewport.TopLeftX = 0.0f;
	m_Viewport.TopLeftY = 0.0f;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	hr = m_d3dDevice->CreateVertexShader(g_pShaderVS, sizeof(g_pShaderVS), NULL, &m_d3dVertexShader);
	hr = m_d3dDevice->CreatePixelShader(g_pShaderPS, sizeof(g_pShaderPS), NULL, &m_d3dPixelShader);
	hr = m_d3dDevice->CreatePixelShader(g_pShaderPSUntex, sizeof(g_pShaderPSUntex), NULL, &m_d3dPixelShaderUntex);
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = m_d3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), g_pShaderVS, sizeof(g_pShaderVS), &m_d3dInputLayout);
	return 0;
}

void D3D::Begin(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil)
{
	m_d3dDeviceContext->ClearRenderTargetView(m_d3dRenderTargetView, clearColor);
	if (!m_d3dDepthStencilView)
		return;
	m_d3dDeviceContext->ClearDepthStencilView(m_d3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
	m_d3dDeviceContext->OMSetRenderTargets(1, &m_d3dRenderTargetView, m_d3dDepthStencilView);
	m_d3dDeviceContext->RSSetViewports(1, &m_Viewport);
}

void D3D::End()
{
	m_d3dSwapChain->Present(1, 0);
}

D3DSprite *D3D::CreateSprite(XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT2 uv1, XMFLOAT2 uv2, D3DXCOLOR color, ID3D11Texture2D *texture, bool clamp)
{
	D3DSprite *sprite = new D3DSprite(this, pos, size, uv1, uv2, color, texture, clamp);
	return sprite;
}

void D3D::DrawSprite(D3DSprite *sprite)
{
	UINT Stride = sizeof(DXUTSpriteVertex);
	UINT Offset = 0;

	m_d3dDeviceContext->VSSetShader(m_d3dVertexShader, NULL, 0);
	m_d3dDeviceContext->HSSetShader(NULL, NULL, 0);
	m_d3dDeviceContext->DSSetShader(NULL, NULL, 0);
	m_d3dDeviceContext->GSSetShader(NULL, NULL, 0);
	if (sprite->m_pTexture)
	{
		m_d3dDeviceContext->PSSetShader(m_d3dPixelShader, NULL, 0);
		m_d3dDeviceContext->PSSetShaderResources(0, 1, &sprite->m_pTextureResource);
	}
	else
	{
		m_d3dDeviceContext->PSSetShader(m_d3dPixelShaderUntex, NULL, 0);
	}
		

	// States
	m_d3dDeviceContext->OMSetDepthStencilState(m_d3dDepthStencilState, 0);
	m_d3dDeviceContext->RSSetState(m_d3dRasterizerState);
	float BlendFactor[4] = { 0, 0, 0, 0 };
	m_d3dDeviceContext->OMSetBlendState(m_d3dBlendState, BlendFactor, 0xFFFFFFFF);
	if (!sprite->m_bClamp)
		m_d3dDeviceContext->PSSetSamplers(0, 1, &m_d3dSamplerState);
	else if (m_bLinearSampler)
		m_d3dDeviceContext->PSSetSamplers(0, 1, &m_d3dSamplerStateLinear);
	else
		m_d3dDeviceContext->PSSetSamplers(0, 1, &m_d3dSamplerStateClamp);

	m_d3dDeviceContext->IASetVertexBuffers(0, 1, &sprite->m_d3dVertexBuffer, &Stride, &Offset);
	m_d3dDeviceContext->IASetInputLayout(m_d3dInputLayout);
	m_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3dDeviceContext->Draw(6, 0);
}

void D3D::Resize(int width, int height)
{
	m_dwWidth = width;
	m_dwHeight = height;
	m_Viewport.Width = static_cast<float>(m_dwWidth);
	m_Viewport.Height = static_cast<float>(m_dwHeight);

	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_d3dDeviceContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	SafeRelease(m_d3dRenderTargetView);
	SafeRelease(m_d3dDepthStencilView);
	SafeRelease(m_d3dDepthStencilBuffer);
	m_d3dDeviceContext->Flush();

	m_d3dSwapChain->ResizeBuffers(1, m_dwWidth, m_dwHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	ID3D11Texture2D* backBuffer;
	m_d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	m_d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &m_d3dRenderTargetView);
	SafeRelease(backBuffer);

	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0; // No CPU access required.
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.Width = m_dwWidth;
	depthStencilBufferDesc.Height = m_dwHeight;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	m_d3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &m_d3dDepthStencilBuffer);

	m_d3dDevice->CreateDepthStencilView(m_d3dDepthStencilBuffer, nullptr, &m_d3dDepthStencilView);
}

ID3D11Texture2D *D3D::CreateTexture(D3DCOLOR *data, int width, int height)
{
	D3D11_TEXTURE2D_DESC tdesc;
	ZeroMemory(&tdesc, sizeof(tdesc));
	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tdesc.CPUAccessFlags = 0;
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tdesc.Width = width;
	tdesc.Height = height;
	tdesc.MipLevels = 1;
	tdesc.ArraySize = 1;
	tdesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA tbsd;
	tbsd.pSysMem = (void *)data;
	tbsd.SysMemPitch = width * sizeof(D3DCOLOR);
	tbsd.SysMemSlicePitch = width * height * sizeof(D3DCOLOR); // Not needed since this is a 2d texture

	ID3D11Texture2D *texture;
	HRESULT hr = m_d3dDevice->CreateTexture2D(&tdesc, &tbsd, &texture);
	if (!SUCCEEDED(hr))
		return 0;
	return texture;
}


void D3D::SetLinearSampler(bool enable)
{
	m_bLinearSampler = enable;
}

bool D3D::GetLinearSampler()
{
	return m_bLinearSampler;
}