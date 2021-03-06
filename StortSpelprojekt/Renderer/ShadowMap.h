#pragma once
#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <d3dcompiler.h>
#include <string>

/*

Before rendering to shadow map:

~ Activate FRONT-FACE culling
~ Set vertex buffer before the Draw/DrawIndexed call
~ Set topology

*/

namespace Renderer
{
	class ShadowMap
	{
	private:

		struct MatrixBufferPerObject
		{
			DirectX::XMMATRIX _modelWorld;
		};

		struct MatrixBufferPerFrame
		{
			DirectX::XMMATRIX _lightView;
			DirectX::XMMATRIX _lightProjection;
		};

		int _dimensions;

		ID3D11DepthStencilView*			_shadowDepthStencilView;
		ID3D11ShaderResourceView*		_shadowShaderResourceView;
		ID3D11DepthStencilState*        _shadowDepthStencilState;

		D3D11_VIEWPORT					_shadowViewport;

		ID3D11Buffer*					_matrixBufferPerObject;
		ID3D11Buffer*					_matrixBufferPerFrame;

		void CreateResources(ID3D11Device* device, int dimensions);

	public:

		ShadowMap(ID3D11Device* device, int dimensions);
		virtual ~ShadowMap();

		void Resize(ID3D11Device* device, int dimensions);

		ID3D11ShaderResourceView* GetShadowSRV();
		int GetSize();

		void SetShadowGenerationStage(ID3D11DeviceContext* deviceContext);
		void SetDataPerObject(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX* modelWorld);
		void SetDataPerFrame(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX* lightView, DirectX::XMMATRIX* lightProjection);
	};
}
