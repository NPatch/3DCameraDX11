#include "stdafx.h"
#include "Rendering.h"

#include <cstdio>

#include "KinectManager.h"

#pragma comment(lib, "d3d11.lib")

void Rendering::InitD3D(HWND hWnd)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hWnd;                                // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.BufferDesc.Width = SCREEN_WIDTH;                    // set the back buffer width
	scd.BufferDesc.Height = SCREEN_HEIGHT;                  // set the back buffer height
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching

	// create a device, device context and swap chain using the information in the scd struct
	unsigned int creationFlags = 0;

#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		creationFlags,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		NULL,
		&devcon);

	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();

	// set the render target as the back buffer
	devcon->OMSetRenderTargets(1, &backbuffer, NULL);

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;

	devcon->RSSetViewports(1, &viewport);
}

void Rendering::InitPipeline()
{
	// load and compile the two shaders
	ID3DBlob *VS = NULL, *PS = NULL, *Errors = NULL;

	HRESULT hr = D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL, "main", "vs_5_0", 0, 0, &VS, &Errors);

	if (FAILED(hr))    // if there are any errors...
		MessageBox(NULL, L"The vertex shader failed to compile.", L"Error", MB_OK);

	hr = D3DCompileFromFile(L"PixelShader.hlsl", NULL, NULL, "main", "ps_5_0", 0, 0, &PS, &Errors);

	if (Errors || FAILED(hr))    // if there are any errors...
		MessageBox(NULL, L"The pixel shader failed to compile.", L"Error", MB_OK);


	// encapsulate both shaders into shader objects
	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	// set the shader objects
	devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS, 0, 0);

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);
}

void Rendering::InitGraphics(void *tex_initial_data)
{
	// create vertices to represent the corners of the cube
	KinectManagement::sVertexType OurVertices[] =
	{
		{ -1.0f, -1.0f, 1.0f, 0.0f, 0.0f },    // side 1
		{ 1.0f, -1.0f, 1.0f, 0.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f,  1.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f,  1.0f, 1.0f },

		{ -1.0f, -1.0f, -1.0f, 0.0f, 0.0f },    // side 2
		{ -1.0f, 1.0f, -1.0f,  0.0f, 1.0f },
		{ 1.0f, -1.0f, -1.0f,  1.0f, 0.0f },
		{ 1.0f, 1.0f, -1.0f, 1.0f, 1.0f },

		{ -1.0f, 1.0f, -1.0f, 0.0f, 0.0f },    // side 3
		{ -1.0f, 1.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, -1.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },

		{ -1.0f, -1.0f, -1.0f, 0.0f, 0.0f },    // side 4
		{ 1.0f, -1.0f, -1.0f, 0.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f, 1.0f, 0.0f },
		{ 1.0f, -1.0f, 1.0f, 1.0f, 1.0f },

		{ 1.0f, -1.0f, -1.0f,0.0f, 0.0f },    // side 5
		{ 1.0f, 1.0f, -1.0f, 0.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },

		{ -1.0f, -1.0f, -1.0f,0.0f, 0.0f },    // side 6
		{ -1.0f, -1.0f, 1.0f, 0.0f, 1.0f },
		{ -1.0f, 1.0f, -1.0f, 1.0f, 0.0f },
		{ -1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
	};

	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(KinectManagement::sVertexType) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA vBufData;
	ZeroMemory(&vBufData, sizeof(vBufData));

	vBufData.pSysMem = OurVertices;
	vBufData.SysMemPitch = 0;
	vBufData.SysMemSlicePitch = 0;

	dev->CreateBuffer(&bd, &vBufData, &pVBuffer);

	//// copy the vertices into the buffer
	//D3D11_MAPPED_SUBRESOURCE ms;
	//devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
	//memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
	//devcon->Unmap(pVBuffer, NULL);


	// create the index buffer out of DWORDs
	DWORD OurIndices[] =
	{
		0, 1, 2,    // side 1
		2, 1, 3,
		4, 5, 6,    // side 2
		6, 5, 7,
		8, 9, 10,    // side 3
		10, 9, 11,
		12, 13, 14,    // side 4
		14, 13, 15,
		16, 17, 18,    // side 5
		18, 17, 19,
		20, 21, 22,    // side 6
		22, 21, 23,
	};

	// create the index buffer
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(DWORD) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iBufData;
	ZeroMemory(&iBufData, sizeof(iBufData));

	iBufData.pSysMem = OurIndices;
	iBufData.SysMemPitch = 0;
	iBufData.SysMemSlicePitch = 0;

	dev->CreateBuffer(&bd, &iBufData, &pIBuffer);

	//devcon->Map(pIBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
	//memcpy(ms.pData, OurIndices, sizeof(OurIndices));                   // copy the data
	//devcon->Unmap(pIBuffer, NULL);

	//Create the description for the texture object
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = KINECT_STREAM_COLOR_WIDTH;
	desc.Height = KINECT_STREAM_COLOR_HEIGHT;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	//D3D11_SUBRESOURCE_DATA initial_data;
	//initial_data.pSysMem = tex_initial_data;
	//initial_data.SysMemPitch = 0;
	//initial_data.SysMemSlicePitch = 0;
	pMainTex2D = NULL;
	HRESULT hr = dev->CreateTexture2D(&desc,NULL, &pMainTex2D);

	if (FAILED(hr)) {
		return;
	}

	//Creating the subresource view description that says how to view the texture as the shader input
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1;
	srv_desc.Texture2D.MostDetailedMip = 0;

	dev->CreateShaderResourceView(pMainTex2D, &srv_desc,&pMainTex2DSRView);

	//Create the sampler
	D3D11_SAMPLER_DESC sampl_dec;
	sampl_dec.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampl_dec.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampl_dec.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampl_dec.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampl_dec.MaxLOD = sampl_dec.MinLOD = sampl_dec.MipLODBias = 0;
	sampl_dec.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampl_dec.MaxAnisotropy = 1;
	sampl_dec.BorderColor[0] = 0;
	sampl_dec.BorderColor[1] = 0;
	sampl_dec.BorderColor[2] = 0;
	sampl_dec.BorderColor[3] = 0;
	dev->CreateSamplerState(&sampl_dec, &pMainTex2DSampler);
	devcon->PSSetSamplers(0, 1, &pMainTex2DSampler);



	//"Binding the texture object as shader input"
	devcon->PSSetShaderResources(0, 1, &pMainTex2DSRView);
}

void Rendering::Render()
{
	// clear the back buffer to a deep blue
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	devcon->ClearRenderTargetView(backbuffer, color);

	//// do 3D rendering on the back buffer here
	//// select which vertex buffer to display
	UINT stride = sizeof(KinectManagement::sVertexType);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	devcon->IASetIndexBuffer(pIBuffer, DXGI_FORMAT_R16_UINT, 0);

	// select which primtive type we are using
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw the vertex buffer to the back buffer
	devcon->DrawIndexed(36, 0, 0);
	
	// switch the back buffer and the front buffer
	swapchain->Present(0, 0);
}

void Rendering::UpdateTextureBuffer(void *data, unsigned int point_count,unsigned int byte_per_pixel) {
	//1.Get a pointer to the texture memory by passing in D3D11_MAP_WRITE_DISCARD when calling ID3D11DeviceContext::Map.
	//2.Write data to the memory.
	//3.Call ID3D11DeviceContext::Unmap when you are finished writing data.
	D3D11_MAPPED_SUBRESOURCE ms;
	devcon->Map(pMainTex2D, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
	memcpy(ms.pData, data, byte_per_pixel*point_count);                 // copy the data
	devcon->Unmap(pMainTex2D, NULL);   
}

void Rendering::CleanD3D(void)
{
	swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

												   // close and release all existing COM objects
	pLayout->Release();
	pVS->Release();
	pPS->Release();
	pVBuffer->Release();
	pIBuffer->Release();
	pMainTex2D->Release();
	pMainTex2DSampler->Release();
	pMainTex2DSRView->Release();
	swapchain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();
}
