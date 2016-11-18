#pragma once
class Rendering {
	IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
	ID3D11Device *dev;                     // the pointer to our Direct3D device interface
	ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
	ID3D11RenderTargetView *backbuffer;    // the pointer to the backbuffer
	ID3D11InputLayout *pLayout;            // the pointer to the input layout
	ID3D11VertexShader *pVS;               // the pointer to the vertex shader
	ID3D11PixelShader *pPS;                // the pointer to the pixel shader
	ID3D11Buffer *pVBuffer;                // the pointer to the vertex buffer
	ID3D11Buffer *pIBuffer;                // the pointer to the vertex buffer
	ID3D11Texture2D *pMainTex2D;		//the pointer to the texture object that hold the kinect rgb image
	ID3D11SamplerState *pMainTex2DSampler; //pointer to the sampler object for the texture object above to be used in the pixel shader.
	ID3D11ShaderResourceView        *pMainTex2DSRView;//the pointer to the subresourceview object that shows how to display the texture object from above
	unsigned int pVCount;

public:										   // function prototypes
	void InitD3D(HWND hWnd);     // sets up and initializes Direct3D
	void InitGraphics(void *data);    // creates the shape to render
	void InitPipeline(void);    // loads and prepares the shaders

	void UpdateTextureBuffer(void *data, unsigned int point_count,unsigned int bytes_per_pixel);

	void Render(void);

	void CleanD3D(void);         // closes Direct3D and releases memory
};