#include "Graphics.h"
 // Để sử dụng _com_error
#include <cassert>
#include "KinhLupException.h"
#include<sstream>
#include "dxerr.h"
#include <string>
#include <Windows.h>  
#include "GraphicsThrowMacros.h"
#include "d3dcompiler.h"
#define GFX_THROW_FAILED(hrcall) if(FAILED(hr =(hrcall))) throw Graphics::HrException(__LINE__,__FILE__,hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__,__FILE__,hr)

namespace wrl = Microsoft::WRL;

 ////////
std::string WStringToString(const std::wstring & ws) {
	std::string s(ws.begin(), ws.end());
	return s;
}
//
Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd ;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// for checking results of d3d
	HRESULT hr;
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	));
	
	// truy cập vào nguồn tài nguyên texture của swapchain(back buffer)
	// nắm handle của backbuffer
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		nullptr,
		&pTarget
	));
}


void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif // !NDEBUG

	if (FAILED(hr = pSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}

 void Graphics::DrawTestTriangle()
{
	namespace wrl = Microsoft::WRL;
	HRESULT hr;

	struct Vertex
	{
		float x;
		float y;
	};

	// Create vertex buffer
	const Vertex vertices[] = {
		{ 0.0f, 0.5f },
		{ 0.5f,-0.5f },
		{ -0.5f, -0.5f }
	};

	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	
	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(vertices);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;
	GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	
	// Bind vertex buffer to pipeline
	 UINT stride = sizeof(Vertex);
	 UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
	
	//Create pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	//Bind  pixel shader to pipeline 
	pContext->PSSetShader(pPixelShader.Get(), 0u, 0u);

	

	// Create vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso",&pBlob));
	GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(),pBlob->GetBufferSize(), nullptr,&pVertexShader));

	//Bind vertex shader to pipeline
	pContext->VSSetShader(pVertexShader.Get(), 0u, 0u);
	



	//input (vertex) layout(2d position only)
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	GFX_THROW_INFO(pDevice->CreateInputLayout(
		ied, (UINT)std::size(ied),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pInputLayout
	));

	// Bind vertex shader to pipeline
	pContext->IASetInputLayout(pInputLayout.Get());


	//Bind render target to pipeline
	pContext->OMSetRenderTargets(1u,pTarget.GetAddressOf(), nullptr);


	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	//bind viewport to pipeline
	pContext->RSSetViewports(1u, &vp);


	// Set primitive topology to triangle list
	pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	GFX_THROW_INFO_ONLY(pContext->Draw((UINT)std::size(vertices), 0u));
}


//Graphics Exception Stuff
Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:Exception(line, file), hr(hr) {
	for (const auto& m: infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	if (!info.empty())
	{
		info.pop_back();
	}
}
const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "Error Code : " << GetErrorCode() << std::endl
		<< "String : " << GetErrorString() << std::endl
		<< "Description : " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n Error info \n :" << GetErrorInfo() << std::endl;
	}
	oss	<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();

}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Kinh Lup Graphics HrException";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return WStringToString( DXGetErrorStringW(hr));
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	wchar_t buf[512];
	DXGetErrorDescriptionW(hr, buf, (UINT)_countof(buf));
	return WStringToString( buf);
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Kinhlup Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}


Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}


const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Kinh lup Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}
