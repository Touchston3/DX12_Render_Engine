#pragma once
#include <d3d12.h>
#include "../d3dx12.h"
#include <wrl.h>
class Root_Signature
{
	class Parameter
	{
		
	};
public:
	Root_Signature(const std::vector<Root_Signature::Parameter>& constants, const std::vector<Root_Signature::Parameter>& samplers);
	inline ID3D12RootSignature* Get() const { return m_root_signature.Get(); }
private:
	const Microsoft::WRL::ComPtr<ID3D12RootSignature> m_root_signature;
	const CD3DX12_ROOT_SIGNATURE_DESC m_description;
	const std::vector<Root_Signature::Parameter> m_constants; 
	const std::vector<Root_Signature::Parameter> m_samplers;
};

