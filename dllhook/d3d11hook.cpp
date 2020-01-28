#include "common.h"
#include "log.h"
#include "d3d11hook.h"

#include "MinHook/include/MinHook.h"

#pragma comment(lib, "d3d11.lib")

#define HookImpl(VTableItem, Fun)\
	if (Hook_API(reinterpret_cast<LPVOID>(VTableItem), Fun##_CallBack, reinterpret_cast<LPVOID*>(&(Raw_##Fun))) != S_OK) { return S_FALSE; }

funptr GetTable(LPVOID TableBase)
{
	funptr pVTable = reinterpret_cast<funptr>(TableBase);
	return reinterpret_cast<funptr>(pVTable[0]);
}

HRESULT Hook_API(LPVOID pVTableItem, LPVOID API_CallBack, LPVOID* Raw_API)
{
	if (MH_CreateHook(pVTableItem, API_CallBack, Raw_API) != MH_OK) { return -1; }
	if (MH_EnableHook(pVTableItem) != MH_OK) { return -1; }
	return S_OK;
}

//////////////////////////////ID3D11DeviceContext//////////////////////////////////
PSSetShaderResourcesCallBack Raw_PSSetShaderResources = nullptr;

PSSetShaderCallBack Raw_PSSetShader = nullptr;

VSSetShaderCallBack Raw_VSSetShader = nullptr;

DrawIndexedCallBack Raw_DrawIndexed = nullptr;

DrawCallBack Raw_Draw = nullptr;

IASetInputLayoutCallback Raw_IASetInputLayout = nullptr;

IASetVertexBuffersCallBack Raw_IASetVertexBuffers = nullptr;

IASetIndexBufferCallBack Raw_IASetIndexBuffer = nullptr;

DrawIndexedInstancedCallBack Raw_DrawIndexedInstanced = nullptr;

DrawInstancedCallBack Raw_DrawInstanced = nullptr;

VSSetShaderResourcesCallback Raw_VSSetShaderResources = nullptr;

OMSetRenderTargetsCallback Raw_OMSetRenderTargets = nullptr;

DispatchCallback Raw_Dispatch = nullptr;

VSSetConstantBuffers1CallBack Raw_VSSetConstantBuffers1 = nullptr;

PSSetConstantBuffers1CallBack Raw_PSSetConstantBuffers1 = nullptr;

//////////////////////////////ID3D11Device//////////////////////////////////
CreateInputLayoutCallBack Raw_CreateInputLayout = nullptr;

CreateVertexShaderCallBack Raw_CreateVertexShader = nullptr;

CreatePixelShaderCallBack Raw_CreatePixelShader = nullptr;

//////////////////////////////ID3D11DeviceContext//////////////////////////////////
void __stdcall PSSetShaderResources_CallBack(ID3D11DeviceContext* context,
	UINT StartSlot,
	UINT NumViews,
	ID3D11ShaderResourceView* const* ppShaderResourceViews)
{

	Raw_PSSetShaderResources(context, StartSlot, NumViews, ppShaderResourceViews);
}

void __stdcall PSSetShader_CallBack(ID3D11DeviceContext* context,
	ID3D11PixelShader* pPixelShader,
	ID3D11ClassInstance* const* ppClassInstances,
	UINT NumClassInstances)
{
	Raw_PSSetShader(context, pPixelShader, ppClassInstances, NumClassInstances);
}

void __stdcall VSSetShader_CallBack(ID3D11DeviceContext* context,
	ID3D11VertexShader* pVertexShader,
	ID3D11ClassInstance* const* ppClassInstances,
	UINT NumClassInstances)
{

	Raw_VSSetShader(context, pVertexShader, ppClassInstances, NumClassInstances);
}

void __stdcall DrawIndexed_CallBack(ID3D11DeviceContext* context,
	UINT IndexCount,
	UINT StartIndexLocation,
	INT BaseVertexLocation)
{

	static std::once_flag flag;
	std::call_once(flag, [IndexCount]() {
		LogUtils::Instance()->LogInfo("[context] DrawIndexed_CallBack IndexCount: %d", IndexCount);
	});

	Raw_DrawIndexed(context, IndexCount, StartIndexLocation, BaseVertexLocation);
}

void __stdcall Draw_CallBack(ID3D11DeviceContext* context,
	UINT VertexCount,
	UINT StartVertexLocation)
{

	Raw_Draw(context, VertexCount, StartVertexLocation);
}

void __stdcall IASetInputLayout_CallBack(ID3D11DeviceContext* context,
	ID3D11InputLayout* pInputLayout)
{

	Raw_IASetInputLayout(context, pInputLayout);
}

void __stdcall IASetVertexBuffers_CallBack(ID3D11DeviceContext* context,
	UINT StartSlot,
	UINT NumBuffers,
	ID3D11Buffer* const* ppVertexBuffers,
	const UINT* pStrides,
	const UINT* pOffsets)
{
	Raw_IASetVertexBuffers(context, StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}

void __stdcall IASetIndexBuffer_CallBack(ID3D11DeviceContext* context,
	ID3D11Buffer* pIndexBuffer,
	DXGI_FORMAT Format,
	UINT Offset)
{
	Raw_IASetIndexBuffer(context, pIndexBuffer, Format, Offset);
}

void __stdcall DrawIndexedInstanced_CallBack(ID3D11DeviceContext* context,
	UINT IndexCountPerInstance,
	UINT InstanceCount,
	UINT StartIndexLocation,
	INT BaseVertexLocation,
	UINT StartInstanceLocation)
{
	Raw_DrawIndexedInstanced(context, IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

void __stdcall DrawInstanced_CallBack(ID3D11DeviceContext* context,
	UINT VertexCountPerInstance,
	UINT InstanceCount,
	UINT StartVertexLocation,
	UINT StartInstanceLocation)
{
	Raw_DrawInstanced(context, VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

void __stdcall VSSetShaderResources_CallBack(ID3D11DeviceContext* context,
	UINT StartSlot,
	UINT NumViews,
	ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
	Raw_VSSetShaderResources(context, StartSlot, NumViews, ppShaderResourceViews);
}

void __stdcall OMSetRenderTargets_CallBack(ID3D11DeviceContext* context,
	UINT NumViews,
	ID3D11RenderTargetView* const* ppRenderTargetViews,
	ID3D11DepthStencilView* pDepthStencilView)
{
	Raw_OMSetRenderTargets(context, NumViews, ppRenderTargetViews, pDepthStencilView);
}

void __stdcall Dispatch_CallBack(ID3D11DeviceContext* context,
	UINT ThreadGroupCountX,
	UINT ThreadGroupCountY,
	UINT ThreadGroupCountZ)
{
	Raw_Dispatch(context, ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}


void __stdcall VSSetConstantBuffers1_CallBack(ID3D11DeviceContext* context,
	UINT StartSlot,
	UINT NumBuffers,
	ID3D11Buffer* const* ppConstantBuffers,
	const UINT* pFirstConstant,
	const UINT* pNumConstants)
{
	Raw_VSSetConstantBuffers1(context, StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}

void __stdcall PSSetConstantBuffers1_CallBack(ID3D11DeviceContext* context,
	UINT StartSlot,
	UINT NumBuffers,
	ID3D11Buffer* const* ppConstantBuffers,
	const UINT* pFirstConstant,
	const UINT* pNumConstants)
{
	Raw_PSSetConstantBuffers1(context, StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}


//////////////////////////////Device//////////////////////////////////
HRESULT __stdcall CreateInputLayout_CallBack(ID3D11Device* device,
	const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
	UINT NumElements,
	const void* pShaderBytecodeWithInputSignature,
	SIZE_T BytecodeLength,
	ID3D11InputLayout** ppInputLayout)
{

	static std::once_flag flag;
	std::call_once(flag, []() {
		LogUtils::Instance()->LogInfo("[device] CreateLayout\n");
	});

	return Raw_CreateInputLayout(device, pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);
}

HRESULT __stdcall CreateVertexShader_CallBack(ID3D11Device* device,
	const void* pShaderBytecode,
	SIZE_T BytecodeLength,
	ID3D11ClassLinkage* pClassLinkage,
	ID3D11VertexShader** ppVertexShader)
{

	return Raw_CreateVertexShader(device, pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);
}

HRESULT __stdcall CreatePixelShader_CallBack(ID3D11Device* device,
	const void* pShaderBytecode,
	SIZE_T BytecodeLength,
	ID3D11ClassLinkage* pClassLinkage,
	ID3D11PixelShader** ppPixelShader)
{

	return Raw_CreatePixelShader(device, pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
}



//////////////////////////////////////////////////////////////////////////
HRESULT HookDevice(funptr pDeviceVTable)
{
	HookImpl(pDeviceVTable[11], CreateInputLayout);
	HookImpl(pDeviceVTable[12], CreateVertexShader);
	HookImpl(pDeviceVTable[15], CreatePixelShader);
	return S_OK;
}


HRESULT HookContext(funptr pContextVTable)
{

	HookImpl(pContextVTable[8], PSSetShaderResources);
	HookImpl(pContextVTable[9], PSSetShader);
	HookImpl(pContextVTable[11], VSSetShader);
	HookImpl(pContextVTable[12], DrawIndexed);

	HookImpl(pContextVTable[13], Draw);
	HookImpl(pContextVTable[17], IASetInputLayout);
	HookImpl(pContextVTable[18], IASetVertexBuffers);
	HookImpl(pContextVTable[19], IASetIndexBuffer);

	HookImpl(pContextVTable[20], DrawIndexedInstanced);
	HookImpl(pContextVTable[21], DrawInstanced);
	HookImpl(pContextVTable[25], VSSetShaderResources);
	HookImpl(pContextVTable[33], OMSetRenderTargets);

	HookImpl(pContextVTable[41], Dispatch);
	HookImpl(pContextVTable[119], VSSetConstantBuffers1);
	HookImpl(pContextVTable[123], PSSetConstantBuffers1);

	return S_OK;

}


typedef HRESULT(__stdcall *D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
D3D11PresentHook Raw_Present = nullptr;

HHOOK gMsgHook = NULL;
LRESULT CALLBACK MsgHookProc(int nCode, WPARAM wp, LPARAM lp)
{
	CWPSTRUCT cwp = *(CWPSTRUCT*)lp;
	int msg = LOWORD(cwp.wParam);

	switch (msg)
	{
		case 0x4d: // F1
			LogUtils::Instance()->LogInfo("[hook!] windows key F1 pressed\n");
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			LogUtils::Instance()->LogInfo("[hook!] windows key pressed %d, %d, %d\n", nCode, cwp.lParam, lp);
			break;
	}

	return CallNextHookEx(gMsgHook, nCode, wp, lp);
}

HRESULT __stdcall Present_CallBack(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	static std::once_flag flag;
	std::call_once(flag, [&pSwapChain]() {
		gMsgHook = SetWindowsHookEx(WH_GETMESSAGE, MsgHookProc, 0, GetCurrentThreadId());
		LogUtils::Instance()->LogInfo("HookWindowProc: %s", gMsgHook != NULL ? "OK" : "FAILED");

		// real device & context that the game used!
		ID3D11Device* device = nullptr;
		pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&device);
		HookDevice(GetTable(device));

		ID3D11DeviceContext* context = nullptr;
		device->GetImmediateContext(&context);
		HookContext(GetTable(context));

		LogUtils::Instance()->LogInfo("Present_CallBack: swapChain: %p, device: %p, context: %p", pSwapChain, device, context);

	});


	return Raw_Present(pSwapChain, SyncInterval, Flags);
}

DWORD __stdcall InitializeHook()
{
	WNDCLASSEX windowClass;
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = TEXT("dx11");
	windowClass.hIconSm = NULL;
	::RegisterClassEx(&windowClass);
	HWND hWnd = ::CreateWindow(windowClass.lpszClassName, TEXT("DirectX Window"), WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, windowClass.hInstance, NULL);

	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1 };

	DXGI_RATIONAL refreshRate;
	refreshRate.Numerator = 60;
	refreshRate.Denominator = 1;

	DXGI_MODE_DESC bufferDesc;
	bufferDesc.Width = 100;
	bufferDesc.Height = 100;
	bufferDesc.RefreshRate = refreshRate;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc = sampleDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.Windowed = 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ID3D11Device*            pd3dDevice = nullptr;
	ID3D11DeviceContext*     pd3dContext = nullptr;
	IDXGISwapChain*          pSwapChain = nullptr;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels,
			ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pd3dDevice,
			&featureLevel, &pd3dContext);

	if (FAILED(hr))
	{
		MessageBox(hWnd, L"Failed to create device and swapchain.", L"Fatal Error", MB_ICONERROR);
		return S_FALSE;
	}

	LogUtils::Instance()->LogInfo("[device] temp SwapChain: %p, Device: %p, Context: %p\n", pSwapChain, pd3dDevice, pd3dContext);
	funptr pSwapChainVTable = GetTable(pSwapChain);	

	if (MH_Initialize() != MH_OK)
	{
		MessageBox(hWnd, L"MH_Initialize Failed.", L"Fatal Error", MB_ICONERROR);
		return S_FALSE;
	}

	HookImpl(pSwapChainVTable[8], Present);

	LogUtils::Instance()->LogInfo("Hook_SwapChain::Present OK!");

	pd3dDevice->Release();
	pd3dContext->Release();
	pSwapChain->Release();

	LogUtils::Instance()->LogInfo("InitializeHook OK!");

	return S_OK;

}

void StartHookDX11()
{
	CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(InitializeHook), nullptr, 0, nullptr);
}

void ImplHookDX11_Shutdown()
{
	if (MH_DisableHook(MH_ALL_HOOKS)) { return; };
	if (MH_Uninitialize()) { return; }
}

