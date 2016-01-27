#pragma once

#define RENDERER_EXPORT __declspec(dllexport)

#include "DirectXHandler.h"
#include "ShaderHandler.h"
#include <DirectXMath.h>
#include "RenderUtils.h"
#include "HUDElement.h"
#include "ShadowMap.h"
#include "Spotlight.h"
#include "Node.h"
#include "FontWrapper.h"

namespace Renderer
{
	class RENDERER_EXPORT RenderModule
	{
	private:

		struct ScreenQuadVertex
		{
			float _x, _y, _z;
			float _u, _v;
		};

		struct MatrixBufferPerObject
		{
			DirectX::XMMATRIX _world;
			DirectX::XMFLOAT3 _colorOffset;
		};
		
		struct MatrixBufferPerSkinnedObject
		{
			DirectX::XMMATRIX   _world;

			//TODO: Should be in a separate buffer to enable world matrix update per object. /Jonas - Assigned to Fredrik
			DirectX::XMFLOAT4X4 _bones[30];
		};

		struct MatrixBufferPerFrame
		{
			DirectX::XMMATRIX _viewMatrix;
			DirectX::XMMATRIX _projectionMatrix;
			DirectX::XMFLOAT3 _ambientLight;
		};

		struct MatrixBufferHud
		{
			DirectX::XMMATRIX _model;
		};

		struct MatrixBufferLightPassPerFrame
		{
			DirectX::XMMATRIX _invertedView;
			DirectX::XMMATRIX _invertedProjection;
		};

		struct MatrixBufferLightPassPerLight
		{
			DirectX::XMMATRIX _viewMatrix;
			DirectX::XMMATRIX _projectionMatrix;
			DirectX::XMFLOAT3 _position;
			float			  _angle;
			DirectX::XMFLOAT3 _direction;
			float			  _intensity;
			DirectX::XMFLOAT3 _color;
			float			  _range;
			int				  _shadowMapDimensions;
		};

		ID3D11Buffer*		_matrixBufferPerObject;
		ID3D11Buffer* 		_matrixBufferPerSkinnedObject;
		ID3D11Buffer*		_matrixBufferPerFrame;
		ID3D11Buffer*		_matrixBufferLightPassPerFrame;
		ID3D11Buffer*		_matrixBufferLightPassPerLight;
		ID3D11Buffer*		_screenQuad;
		ID3D11Buffer*		_matrixBufferHUD;

		DirectXHandler*		_d3d;
		ShaderHandler*		_shaderHandler;

		int _screenWidth;
		int _screenHeight;

		void InitializeConstantBuffers();
		void InitializeScreenQuadBuffer();

		void SetDataPerObject(DirectX::XMMATRIX* world, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* specular, DirectX::XMFLOAT3 colorOffset = DirectX::XMFLOAT3(0, 0, 0));
		void SetDataPerSkinnedObject(DirectX::XMMATRIX* world, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* specular, std::vector<DirectX::XMFLOAT4X4>* extra, DirectX::XMFLOAT3 colorOffset = DirectX::XMFLOAT3(0, 0, 0));
		void SetDataPerMesh(ID3D11Buffer* vertexBuffer, int vertexSize);
		void SetShadowMapDataPerObject(DirectX::XMMATRIX* world);

		void Render(GUI::Node* current, DirectX::XMMATRIX* transform, FontWrapper* fontWrapper);
		void Render(std::vector<HUDElement>* imageData);

		ShadowMap* _shadowMap;

	public:

		//TODO: Import these from settings file
		const int SHADOWMAP_DIMENSIONS = 256;
		const DirectX::XMFLOAT3 AMBIENT_LIGHT = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);

		enum ShaderStage { GEO_PASS, SHADOW_GENERATION, LIGHT_APPLICATION, GRID_STAGE, ANIM_STAGE, HUD_STAGE };

		RenderModule(HWND hwnd, int screenWidth, int screenHeight);
		~RenderModule();

		void ResizeResources(HWND hwnd, int windowWidth, int windowHeight);

		void SetDataPerFrame(DirectX::XMMATRIX* view, DirectX::XMMATRIX* projection);
		void SetShadowMapDataPerSpotlight(DirectX::XMMATRIX* lightView, DirectX::XMMATRIX* lightProjection);

		void SetLightDataPerFrame(DirectX::XMMATRIX* camView, DirectX::XMMATRIX* camProjection);
		void SetLightDataPerSpotlight(Spotlight* spotlight);

		void SetShaderStage(ShaderStage stage);

		void BeginScene(float red, float green, float blue, float alpha);
		void Render(DirectX::XMMATRIX* world, RenderObject* renderObject, DirectX::XMFLOAT3 colorOffset = DirectX::XMFLOAT3(0, 0, 0), std::vector<DirectX::XMFLOAT4X4>* extra = nullptr);
		void Render(GUI::Node* root, FontWrapper* fontWrapper);
		void RenderLineList(DirectX::XMMATRIX* world, ID3D11Buffer* lineList, int nrOfPoints, DirectX::XMFLOAT3 colorOffset = DirectX::XMFLOAT3(0,0,0)); //TODO: Test if grid can be rendered /Jonas
		void RenderShadowMap(DirectX::XMMATRIX* world, RenderObject* renderObject);
		void RenderScreenQuad();
		void EndScene();

		void RenderLightVolume(ID3D11Buffer* volume, DirectX::XMMATRIX* world, int vertexCount, int vertexSize);

		ID3D11Device* GetDevice() const;
		ID3D11DeviceContext* GetDeviceContext() const;
	};
}