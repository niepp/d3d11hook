#pragma once

/*
D3D11 Methods Table :
SwapChain
[0]   QueryInterface
[1]   AddRef
[2]   Release
[3]   SetPrivateData
[4]   SetPrivateDataInterface
[5]   GetPrivateData
[6]   GetParent
[7]   GetDevice
[8]   Present
[9]   GetBuffer
[10]  SetFullscreenState
[11]  GetFullscreenState
[12]  GetDesc
[13]  ResizeBuffers
[14]  ResizeTarget
[15]  GetContainingOutput
[16]  GetFrameStatistics
[17]  GetLastPresentCount

device
[18]  QueryInterface
[19]  AddRef
[20]  Release
[21]  CreateBuffer
[22]  CreateTexture1D
[23]  CreateTexture2D
[24]  CreateTexture3D
[25]  CreateShaderResourceView
[26]  CreateUnorderedAccessView
[27]  CreateRenderTargetView
[28]  CreateDepthStencilView
[29]  CreateInputLayout
[30]  CreateVertexShader
[31]  CreateGeometryShader
[32]  CreateGeometryShaderWithStreamOutput
[33]  CreatePixelShader
[34]  CreateHullShader
[35]  CreateDomainShader
[36]  CreateComputeShader
[37]  CreateClassLinkage
[38]  CreateBlendState
[39]  CreateDepthStencilState
[40]  CreateRasterizerState
[41]  CreateSamplerState
[42]  CreateQuery
[43]  CreatePredicate
[44]  CreateCounter
[45]  CreateDeferredContext
[46]  OpenSharedResource
[47]  CheckFormatSupport
[48]  CheckMultisampleQualityLevels
[49]  CheckCounterInfo
[50]  CheckCounter
[51]  CheckFeatureSupport
[52]  GetPrivateData
[53]  SetPrivateData
[54]  SetPrivateDataInterface
[55]  GetFeatureLevel
[56]  GetCreationFlags
[57]  GetDeviceRemovedReason
[58]  GetImmediateContext
[59]  SetExceptionMode
[60]  GetExceptionMode

devicecontext
[61]  QueryInterface
[62]  AddRef
[63]  Release
[64]  GetDevice
[65]  GetPrivateData
[66]  SetPrivateData
[67]  SetPrivateDataInterface
[68]  SetConstantBuffers
[69]  SetShaderResources
[70]  SetShader
[71]  SetSamplers
[72]  SetShader
[73]  DrawIndexed
[74]  Draw
[75]  Map
[76]  Unmap
[77]  SetConstantBuffers
[78]  IASetInputLayout
[79]  IASetVertexBuffers
[80]  IASetIndexBuffer
[81]  DrawIndexedInstanced
[82]  DrawInstanced
[83]  SetConstantBuffers
[84]  SetShader
[85]  IASetPrimitiveTopology
[86]  SetShaderResources
[87]  SetSamplers
[88]  Begin
[89]  End
[90]  GetData
[91]  SetPredication
[92]  SetShaderResources
[93]  SetSamplers
[94]  OMSetRenderTargets
[95]  OMSetRenderTargetsAndUnorderedAccessViews
[96]  OMSetBlendState
[97]  OMSetDepthStencilState
[98]  SOSetTargets
[99]  DrawAuto
[100] DrawIndexedInstancedIndirect
[101] DrawInstancedIndirect
[102] Dispatch
[103] DispatchIndirect
[104] RSSetState
[105] RSSetViewports
[106] RSSetScissorRects
[107] CopySubresourceRegion
[108] CopyResource
[109] UpdateSubresource
[110] CopyStructureCount
[111] ClearRenderTargetView
[112] ClearUnorderedAccessViewUint
[113] ClearUnorderedAccessViewFloat
[114] ClearDepthStencilView
[115] GenerateMips
[116] SetResourceMinLOD
[117] GetResourceMinLOD
[118] ResolveSubresource
[119] ExecuteCommandList
[120] SetShaderResources
[121] SetShader
[122] SetSamplers
[123] SetConstantBuffers
[124] SetShaderResources
[125] SetShader
[126] SetSamplers
[127] SetConstantBuffers
[128] SetShaderResources
[129] CSSetUnorderedAccessViews
[130] SetShader
[131] SetSamplers
[132] SetConstantBuffers
[133] VSGetConstantBuffers
[134] PSGetShaderResources
[135] PSGetShader
[136] PSGetSamplers
[137] VSGetShader
[138] PSGetConstantBuffers
[139] IAGetInputLayout
[140] IAGetVertexBuffers
[141] IAGetIndexBuffer
[142] GSGetConstantBuffers
[143] GSGetShader
[144] IAGetPrimitiveTopology
[145] VSGetShaderResources
[146] VSGetSamplers
[147] GetPredication
[148] GSGetShaderResources
[149] GSGetSamplers
[150] OMGetRenderTargets
[151] OMGetRenderTargetsAndUnorderedAccessViews
[152] OMGetBlendState
[153] OMGetDepthStencilState
[154] SOGetTargets
[155] RSGetState
[156] RSGetViewports
[157] RSGetScissorRects
[158] HSGetShaderResources
[159] HSGetShader
[160] HSGetSamplers
[161] HSGetConstantBuffers
[162] DSGetShaderResources
[163] DSGetShader
[164] DSGetSamplers
[165] DSGetConstantBuffers
[166] CSGetShaderResources
[167] CSGetUnorderedAccessViews
[168] CSGetShader
[169] CSGetSamplers
[170] CSGetConstantBuffers
[171] ClearState
[172] Flush
[173] GetType
[174] GetContextFlags
[175] FinishCommandList
[176] CopySubresourceRegion1
[177] UpdateSubresource1
[178] DiscardResource
[179] DiscardView
[180] SetConstantBuffers1
[181] SetConstantBuffers1
[182] SetConstantBuffers1
[183] SetConstantBuffers1
[184] SetConstantBuffers1
[185] SetConstantBuffers1
[186] VSGetConstantBuffers1
[187] HSGetConstantBuffers1
[188] DSGetConstantBuffers1
[189] GSGetConstantBuffers1
[190] PSGetConstantBuffers1
[191] CSGetConstantBuffers1
[192] SwapDeviceContextState
[193] ClearView
[194] DiscardView1
[195] UpdateTileMappings
[196] CopyTileMappings
[197] CopyTiles
[198] UpdateTiles
[199] ResizeTilePool
[200] TiledResourceBarrier
[201] IsAnnotationEnabled
[202] SetMarkerInt
[203] BeginEventInt
[204] EndEvent
*/

//////////////////////////////ID3D11DeviceContext//////////////////////////////////
typedef void(__stdcall* PSSetShaderResourcesCallBack)(ID3D11DeviceContext* context,
	UINT StartSlot,
	UINT NumViews,
	ID3D11ShaderResourceView* const* ppShaderResourceViews);

typedef void(__stdcall* PSSetShaderCallBack)(ID3D11DeviceContext* context,
	ID3D11PixelShader* pPixelShader,
	ID3D11ClassInstance* const* ppClassInstances,
	UINT NumClassInstances);

typedef void(__stdcall* VSSetShaderCallBack)(ID3D11DeviceContext* context,
	ID3D11VertexShader* pVertexShader,
	ID3D11ClassInstance* const* ppClassInstances,
	UINT NumClassInstances);

typedef void(__stdcall* DrawIndexedCallBack)(ID3D11DeviceContext* context,
	UINT IndexCount,
	UINT StartIndexLocation,
	INT BaseVertexLocation);

typedef void(__stdcall* DrawCallBack)(ID3D11DeviceContext* context,
	UINT VertexCount,
	UINT StartVertexLocation);

typedef void(__stdcall* IASetInputLayoutCallback)(ID3D11DeviceContext* context,
	ID3D11InputLayout* pInputLayout);

typedef void(__stdcall* IASetVertexBuffersCallBack)(ID3D11DeviceContext* context,
	UINT StartSlot,
	UINT NumBuffers,
	ID3D11Buffer* const* ppVertexBuffers,
	const UINT* pStrides,
	const UINT* pOffsets);

typedef void(__stdcall* IASetIndexBufferCallBack)(ID3D11DeviceContext* context,
	ID3D11Buffer* pIndexBuffer,
	DXGI_FORMAT Format,
	UINT Offset);

typedef void(__stdcall* DrawIndexedInstancedCallBack)(ID3D11DeviceContext* context,
	UINT IndexCountPerInstance,
	UINT InstanceCount,
	UINT StartIndexLocation,
	INT BaseVertexLocation,
	UINT StartInstanceLocation);

typedef void(__stdcall* DrawInstancedCallBack)(ID3D11DeviceContext* context,
	UINT VertexCountPerInstance,
	UINT InstanceCount,
	UINT StartVertexLocation,
	UINT StartInstanceLocation);

typedef void(__stdcall* VSSetShaderResourcesCallback)(ID3D11DeviceContext* context,
	UINT StartSlot,
	UINT NumViews,
	ID3D11ShaderResourceView* const* ppShaderResourceViews);

typedef void(__stdcall* OMSetRenderTargetsCallback)(ID3D11DeviceContext* context,
	UINT NumViews,
	ID3D11RenderTargetView* const* ppRenderTargetViews,
	ID3D11DepthStencilView* pDepthStencilView);

typedef void(__stdcall* DispatchCallback)(ID3D11DeviceContext* context,
	UINT ThreadGroupCountX,
	UINT ThreadGroupCountY,
	UINT ThreadGroupCountZ);

typedef void(__stdcall* VSSetConstantBuffers1CallBack)(ID3D11DeviceContext* context,
	UINT StartSlot,
	UINT NumBuffers,
	ID3D11Buffer* const* ppConstantBuffers,
	const UINT* pFirstConstant,
	const UINT* pNumConstants);

typedef void(__stdcall* PSSetConstantBuffers1CallBack)(ID3D11DeviceContext* context,
	UINT StartSlot,
	UINT NumBuffers,
	ID3D11Buffer* const* ppConstantBuffers,
	const UINT* pFirstConstant,
	const UINT* pNumConstants);

//////////////////////////////ID3D11Device//////////////////////////////////
typedef HRESULT(__stdcall* CreateInputLayoutCallBack)(ID3D11Device* device,
	const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
	UINT NumElements,
	const void* pShaderBytecodeWithInputSignature,
	SIZE_T BytecodeLength,
	ID3D11InputLayout** ppInputLayout);

typedef HRESULT(__stdcall* CreateVertexShaderCallBack)(ID3D11Device* device, 
	const void* pShaderBytecode, 
	SIZE_T BytecodeLength, 
	ID3D11ClassLinkage* pClassLinkage, 
	ID3D11VertexShader** ppVertexShader);

typedef HRESULT(__stdcall* CreatePixelShaderCallBack)(ID3D11Device* device,
	const void* pShaderBytecode,
	SIZE_T BytecodeLength,
	ID3D11ClassLinkage* pClassLinkage,
	ID3D11PixelShader** ppPixelShader);

//////////////////////////////////////////////////////////////////////////

void StartHookDX11();