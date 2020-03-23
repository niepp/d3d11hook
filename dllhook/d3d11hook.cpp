#include "MinHook/include/MinHook.h"

#include "common.h"
#include "log.h"
#include "utility.h"
#include "d3d11hook.h"

#pragma comment(lib, "d3d11.lib")

#define HookImpl(VTableItem, Fun)\
	if (Hook_API(reinterpret_cast<LPVOID>(VTableItem), Fun##_CallBack, reinterpret_cast<LPVOID*>(&(Raw_##Fun))) != S_OK) { return S_FALSE; }

enum DrawType
{
	Draw = 0,
	DrawIndex,
};

struct DrawCallSpec
{
	DrawType draw_type;
	int passidx;
	size_t ib_count;
	size_t vb_count;
	size_t cb_count;
	size_t world_offset;
	size_t viewprojection_offset;
	ConstantBufferParams curConstBuf;
	int curPassIdx;

	DrawCallSpec()
	{
		Reset();
	}

	void Reset()
	{
		curConstBuf.Reset();
		curPassIdx = 0;
	}

};

struct CameraDrawCall : DrawCallSpec
{
	CameraParams cameraParams;
};

struct CarDrawCall : DrawCallSpec
{
	CarParams carParams;
};

enum RecordState
{
	end,
	begin,
	recording,
};

struct CaptureContext
{
	RecordState record;
	uint32_t frame;

	CameraDrawCall camCapture;
	CarDrawCall carCapture;

	std::vector<FrameParams> frameParamsVec;

	CaptureContext(const std::wstring &cfgPath)
		: record(RecordState::end), frame(0)
	{
		ReadDrawcallFromCfg(cfgPath, L"camera", camCapture);
		ReadDrawcallFromCfg(cfgPath, L"car", carCapture);
	}

	void Reset()
	{
		ChangeRecordState(RecordState::end);
		frameParamsVec.clear();
	}

	bool IsRecording()
	{
		return record == RecordState::recording;
	}

	void ChangeRecordState(RecordState rs)
	{
		LogUtils::Instance()->LogInfo("[capture] record: %d -> %d! \n", record, rs);
		record = rs;
	}

	void OnFrameEnd()
	{
		if (record == RecordState::recording)
		{
			FrameParams frameparams;
			frameparams.timems = get_now_ms();
			frameparams.cameraParams = camCapture.cameraParams;
			frameparams.carParams = carCapture.carParams;
			frameParamsVec.push_back(frameparams);
		}
		else if (record == RecordState::begin)
		{
			ChangeRecordState(RecordState::recording);
		}

		camCapture.Reset();
		carCapture.Reset();
		++frame;
	}

private:
	void ReadDrawcallFromCfg(const std::wstring &cfgPath, const LPCWSTR &objName, DrawCallSpec &dc)
	{
		const LPCWSTR &strCfgPath = cfgPath.data();
		dc.draw_type = (DrawType)GetPrivateProfileInt(objName, L"drawtype", 0, strCfgPath);
		dc.passidx = GetPrivateProfileInt(objName, L"pass_idx", 0, strCfgPath);
		dc.ib_count = GetPrivateProfileInt(objName, L"ib_count", 0, strCfgPath);
		dc.vb_count = GetPrivateProfileInt(objName, L"vb_count", 0, strCfgPath);
		dc.cb_count = GetPrivateProfileInt(objName, L"cb_count", 0, strCfgPath);
		dc.world_offset = GetPrivateProfileInt(objName, L"world_offset", 0, strCfgPath);
		dc.viewprojection_offset = GetPrivateProfileInt(objName, L"viewproj_offset", 0, strCfgPath);
	}

};

std::wstring GetFullPath(const LPCWSTR &cfgFileName)
{
	WCHAR strPath[MAX_PATH + 1];
	GetModuleFileName(NULL, strPath, MAX_PATH + 1);
	std::wstring rawfilename(strPath);
	
	auto str_replace = [](std::wstring &str, wchar_t src, wchar_t dst) {
		for (auto &c : str) {
			if (c == src) {
				c = dst;
			}
		}
	};

	str_replace(rawfilename, '\\', '/');
	size_t pos = rawfilename.rfind('/');
	std::wstring path = rawfilename.substr(0, pos + 1);
	path += cfgFileName;
	return path;
}

CaptureContext g_CaptureContext(GetFullPath(L"InjectConfig.ini"));

bool g_enableWireFrame = false;
ID3D11RasterizerState *g_rState = nullptr;

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

IASetInputLayoutCallBack Raw_IASetInputLayout = nullptr;

IASetVertexBuffersCallBack Raw_IASetVertexBuffers = nullptr;

IASetIndexBufferCallBack Raw_IASetIndexBuffer = nullptr;

DrawIndexedInstancedCallBack Raw_DrawIndexedInstanced = nullptr;

DrawInstancedCallBack Raw_DrawInstanced = nullptr;

VSSetShaderResourcesCallBack Raw_VSSetShaderResources = nullptr;

OMSetRenderTargetsCallBack Raw_OMSetRenderTargets = nullptr;

DispatchCallBack Raw_Dispatch = nullptr;

RSSetStateCallBack Raw_RSSetState = nullptr;

VSSetConstantBuffers1CallBack Raw_VSSetConstantBuffers1 = nullptr;

PSSetConstantBuffers1CallBack Raw_PSSetConstantBuffers1 = nullptr;

VSSetConstantBuffersCallBack Raw_VSSetConstantBuffers = nullptr;

PSSetConstantBuffersCallBack Raw_PSSetConstantBuffers = nullptr;

UpdateSubresourceCallBack Raw_UpdateSubresource = nullptr;

//////////////////////////////ID3D11Device//////////////////////////////////
CreateBufferCallBack Raw_CreateBuffer = nullptr;

CreateInputLayoutCallBack Raw_CreateInputLayout = nullptr;

CreateVertexShaderCallBack Raw_CreateVertexShader = nullptr;

CreatePixelShaderCallBack Raw_CreatePixelShader = nullptr;

//////////////////////////////ID3D11DeviceContext//////////////////////////////////

void EnableWireframe(ID3D11DeviceContext* context)
{
	ID3D11RasterizerState *rState;
	context->RSGetState(&rState);
	D3D11_RASTERIZER_DESC rDesc;
	rState->GetDesc(&rDesc);
	rDesc.FillMode = g_enableWireFrame ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;

	if (g_rState == nullptr)
	{
		ID3D11Device *pD3d = nullptr;
		context->GetDevice(&pD3d);
		pD3d->CreateRasterizerState(&rDesc, &g_rState);
	}
	Raw_RSSetState(context, g_rState);
}

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


void ReadCar(UINT IndexCount, CarDrawCall &carCap)
{
	ConstantBufferParams &carCB = carCap.curConstBuf;

	if (IndexCount == carCap.ib_count)
	{
		if (carCap.curPassIdx++ == carCap.passidx)
		{
			if (carCB.datanum > 0)
			{
				LogUtils::Instance()->LogInfo("[drawcall]-----------frame: %d----------- car | IndexCount: %d, cbnum: %d"
					, g_CaptureContext.frame, IndexCount, carCB.datanum);

				float *pdata = (float*)(carCB.pdata + carCap.world_offset);
				Matrix world;
				for (int i = 0; i < 16; ++i)
				{
					world.v[i] = pdata[i];
				}
				// FetchPositionRotation(world.v, carCap.carParams.pos, carCap.carParams.euler);
			}
			carCB.Reset();
		}
	}

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

	if (IndexCount > 10)
	{
		EnableWireframe(context);
	}

	if (g_CaptureContext.IsRecording())
	{
		ReadCar(IndexCount, g_CaptureContext.carCapture);
	}

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

void __stdcall RSSetState_CallBack(ID3D11DeviceContext* context,
	ID3D11RasterizerState *pRasterizerState)
{
	Raw_RSSetState(context, pRasterizerState);
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

void __stdcall VSSetConstantBuffers_CallBack(ID3D11DeviceContext* context,
	UINT StartSlot,
	UINT NumBuffers,
	ID3D11Buffer* const* ppConstantBuffers)
{
	Raw_VSSetConstantBuffers(context, StartSlot, NumBuffers, ppConstantBuffers);
}

void __stdcall PSSetConstantBuffers_CallBack(ID3D11DeviceContext* context,
	UINT StartSlot,
	UINT NumBuffers,
	ID3D11Buffer* const* ppConstantBuffers)
{
	Raw_PSSetConstantBuffers(context, StartSlot, NumBuffers, ppConstantBuffers);
}

void HoldConstantBuffer(UINT byteWidth, const void *pSrcData, DrawCallSpec &dcCap)
{
	if (byteWidth == dcCap.cb_count)
	{
		auto &dcCB = dcCap.curConstBuf;
		dcCB.datanum = byteWidth;
		dcCB.pdata = (const unsigned char*)pSrcData;
	}
}

void __stdcall UpdateSubresource_CallBack(ID3D11DeviceContext* context,
	ID3D11Resource  *pDstResource,
	UINT            DstSubresource,
	const D3D11_BOX *pDstBox,
	const void      *pSrcData,
	UINT            SrcRowPitch,
	UINT            SrcDepthPitch)
{
	if (g_CaptureContext.IsRecording())
	{
		ID3D11Buffer *pbuffer = nullptr;
		HRESULT hr = pDstResource->QueryInterface(IID_ID3D11Buffer, (void **)&pbuffer);
		if (hr == S_OK)
		{
			if (pbuffer != nullptr && pSrcData != nullptr)
			{
				D3D11_BUFFER_DESC desc;
				pbuffer->GetDesc(&desc);

				HoldConstantBuffer(desc.ByteWidth, pSrcData, g_CaptureContext.camCapture);
				HoldConstantBuffer(desc.ByteWidth, pSrcData, g_CaptureContext.carCapture);

			}
		}
	}

	Raw_UpdateSubresource(context, pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);

}


//////////////////////////////Device//////////////////////////////////
HRESULT __stdcall CreateBuffer_CallBack(ID3D11Device* device,
	const D3D11_BUFFER_DESC      *pDesc,
	const D3D11_SUBRESOURCE_DATA *pInitialData,
	ID3D11Buffer                 **ppBuffer)
{
	return Raw_CreateBuffer(device, pDesc, pInitialData, ppBuffer);
}

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
	HookImpl(pDeviceVTable[3], CreateBuffer);
	HookImpl(pDeviceVTable[11], CreateInputLayout);
	HookImpl(pDeviceVTable[12], CreateVertexShader);
	HookImpl(pDeviceVTable[15], CreatePixelShader);
	return S_OK;
}


HRESULT HookContext(funptr pContextVTable)
{
	HookImpl(pContextVTable[7], VSSetConstantBuffers);
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

	HookImpl(pContextVTable[43], RSSetState);
	
	HookImpl(pContextVTable[48], UpdateSubresource);
	HookImpl(pContextVTable[66], PSSetConstantBuffers);

	HookImpl(pContextVTable[119], VSSetConstantBuffers1);
	HookImpl(pContextVTable[123], PSSetConstantBuffers1);

	return S_OK;

}


typedef HRESULT(__stdcall *D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
D3D11PresentHook Raw_Present = nullptr;

HHOOK gMsgHook = NULL;

LRESULT OnKeyDown(int key)
{
	switch (key)
	{
	case VK_F1:
		g_enableWireFrame = !g_enableWireFrame;
		if (g_rState != nullptr)
		{
			g_rState->Release();
			g_rState = nullptr;
		}
		LogUtils::Instance()->LogInfo("[hook!] enable WireFrame: %s\n", g_enableWireFrame ? "true" : "false");
		break;
	case VK_F12:
		LogUtils::Instance()->LogInfo("[hook!] windows key F12 pressed\n");
		if (g_CaptureContext.record == RecordState::end)
		{
			g_CaptureContext.ChangeRecordState(RecordState::begin);
		}
		else if (g_CaptureContext.record == RecordState::recording)
		{
			g_CaptureContext.Reset();
		}
		break;
	default:
		break;
	}
	return S_OK;
}

LRESULT CALLBACK MsgHookProc(int nCode, WPARAM wp, LPARAM lp)
{
	MSG *pmsg = (MSG*)(lp);
	int msg = LOWORD(pmsg->message);
	switch (msg)
	{
		case WM_KEYDOWN:
		{
			OnKeyDown(pmsg->wParam);
		}
		break;
	}

	if (nCode < 0)
	{
		return CallNextHookEx(gMsgHook, nCode, wp, lp);
	}

	return S_OK;

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
	
	g_CaptureContext.OnFrameEnd();

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
	DWORD lpThreadId = 0;
	HANDLE tid = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(InitializeHook), nullptr, 0, &lpThreadId);
	LogUtils::Instance()->LogInfo("StartHookDX11! thread(%p), id = %u", tid, lpThreadId);
}

void ImplHookDX11_Shutdown()
{
	if (MH_DisableHook(MH_ALL_HOOKS)) { return; };
	if (MH_Uninitialize()) { return; }
}

