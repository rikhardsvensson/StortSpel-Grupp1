#pragma once

#define RENDERER_EXPORT __declspec(dllexport)

#include "DirectXHandler.h"
#include "ShaderHandler.h"
#include <DirectXMath.h>
#include "RenderUtils.h"
#include "ShadowMap.h"
#include "Spotlight.h"
#include "Pointlight.h"
#include "GUI elements/Node.h"
#include "FontWrapper.h"
#include "../System/Settings/settings.h"
#include "ParticleSystem/ParticleUtils.h"

/*

Constant buffer register setup:
----Reg--------Resource----------------------------------------------------------------------------------------------------------------------------------------
|    0	  |	   GeoPerFrame. Camera view and projection matrices. Used when rendering objects to give them the correct position from the view of the camera.   |
|    1	  |	   PerObject. World matrix  and color offset of the object.																				          |
|    2	  |	   LightPerFrame. Inverted camera view and projection. Used to reconstruct world position from the cam depth map.			                      |
|    3	  |    LightPerLight. Spotlight data such as angle and range etc. Pointlight data such as range and position										  |
|    4    |    ShadowMapPerFrame. Light view and projection matrices. Used to generate the shadow map.                                                        |
|    5    |    PerSkinnedObject. Animation data matrices																									  |
|    6    |    PerBillboardedObject. Holds necessary data used to generate for example billboarded particles. Geometry shader								  |
---------------------------------------------------------------------------------------------------------------------------------------------------------------

Render targets:
-----Name-----------Pos-------Description----------------------------------------------------------------------------------------------------------------------
|	 Backbuffer	     x    |   This is the render target that is ultimately drawn to the screen in EndScene. Does not have a shader resource view              |
|    Diffuse         0    |	  Holds the diffuse colors of all rendered objects. Sampled from to e.g. calculate lighting.                                      |
|    Normals         1    |	  Holds the normals of the rendered pixels. Sampled from to e.g. calculate lighting.		                                      |
|	 TempBackBuffer  2	  |   Holds backbuffer copy for FXAA to be able to write and read from backbuffer												      |
|    CameraDepthMap  3    |	  Technically not a render target, but an automatically generated depth map from the view of the camera against rendered objects. |
|						  |   Sampled from to e.g. reconstruct the world position of a pixel.											                      |
|    ShadowMap       4    |   Technically not a render target, but an automatically generated depth map from the view of the light against rendered objects.  |
|					      |   Sampled from to calculate lighting/shadows. Located in ShadowMap-class, set in RenderModule.						              |
---------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPORTANT:

~ For blending to work, never use 1.0f as alpha during the geometry pass.
~ If other shaders than pixel and vertex are used, make sure to set that shader to nullptr in ShaderHandler set shaders function for those stages who does not use it

Before rendering to shadow map:
~ Activate FRONT-FACE culling
~ Set vertex buffer before the Draw/DrawIndexed call
~ Set topology

Billboarding:
~ When rendering billboarded objects, make sure that the TEXTURE_COUNT and the hard-coded length of the texture array in BillboardingPS match.
  A "dynamic" amount of textures can be used, as long as the number is lower than the hard-coded number in BillboardingPS, and the actual number is sent to the shader so it know which ones to sample from.
~ The billboarded objects should use textures with alpha channel to determine what parts of the texture to render. Alpha-blending has to be activated before this can be used.
~ The scaling of particles is doubled, so if 0.5f is sent to it, the final quad will have 1.0f as scale. That is because the data that is sent to the shader contains the center points of all particles.
  Therefore it will offset the scale from the middle, in all four directions.

*/

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
			DirectX::XMMATRIX _bones[50];
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
			DirectX::XMFLOAT4 _colorOffset;
			int _hasTexture;
		};

		struct MatrixBufferLightPassPerFrame
		{
			DirectX::XMMATRIX _invertedView;
			DirectX::XMMATRIX _invertedProjection;
			int _screenWidth;
			int _screenHeight;
		};

		struct MatrixBufferLightPassPerSpotlight
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

		struct MatrixBufferLightPassPerPointlight
		{
			DirectX::XMFLOAT3 _position;
			float			  _intensity;
			DirectX::XMFLOAT3 _color;
			float			  _range;
		};

		struct MatrixBufferPerParticleEmitter
		{
			DirectX::XMMATRIX _world;
			DirectX::XMMATRIX _camView;
			DirectX::XMMATRIX _camProjection;
			DirectX::XMFLOAT3 _camPosition;
			float _scale;
			int _isIcon;
		};

		ID3D11Buffer*		_matrixBufferPerObject;
		ID3D11Buffer* 		_matrixBufferPerSkinnedObject;
		ID3D11Buffer*		_matrixBufferPerFrame;
		ID3D11Buffer*		_matrixBufferLightPassPerFrame;
		ID3D11Buffer*		_matrixBufferLightPassPerSpotlight;
		ID3D11Buffer*		_matrixBufferLightPassPerPointlight;
		ID3D11Buffer*		_screenQuad;
		ID3D11Buffer*		_selectionQuad;
		ID3D11Buffer*		_matrixBufferHUD;
		ID3D11Buffer*		_matrixBufferParticles;

		DirectXHandler*		_d3d;
		ShaderHandler*		_shaderHandler;

		System::Settings*	_settings;

		DirectX::XMFLOAT3	_ambientLight;
		bool				_antialiasingEnabled;
		bool				_shadowsEnabled;

		void InitializeConstantBuffers();
		void InitializeScreenQuadBuffer();

		void SetDataPerObject(DirectX::XMMATRIX* world, const DirectX::XMFLOAT3& colorOffset = DirectX::XMFLOAT3(0, 0, 0));
		void SetDataPerSkinnedObject(DirectX::XMMATRIX* world, DirectX::XMMATRIX* extra, int bonecount, const DirectX::XMFLOAT3& colorOffset = DirectX::XMFLOAT3(0, 0, 0));
		void SetDataPerMesh(ID3D11Buffer* vertexBuffer, int vertexSize);
		void SetShadowMapDataPerObject(DirectX::XMMATRIX* world);

		void Render(GUI::Node* current, DirectX::XMMATRIX* transform, FontWrapper* fontWrapper, int brightness);

		ShadowMap* _shadowMap;

	public:
		enum ShaderStage { GEO_PASS, SHADOW_GENERATION, LIGHT_APPLICATION_SPOTLIGHT, LIGHT_APPLICATION_POINTLIGHT, RENDER_LINESTRIP, ANIM_STAGE, HUD_STAGE, AA_STAGE, BILLBOARDING_STAGE, ANIM_SHADOW_GENERATION };

		RenderModule(HWND hwnd, System::Settings* settings);
		~RenderModule();

		void ResizeResources(System::Settings* settings);

		void SetDataPerFrame(DirectX::XMMATRIX* view, DirectX::XMMATRIX* projection);
		void SetDataPerObjectType(RenderObject* renderObject);
		void SetDataPerLineList(ID3D11Buffer* lineList, int vertexSize);
		void SetDataPerParticleEmitter(const DirectX::XMFLOAT3& position, DirectX::XMMATRIX* camView, DirectX::XMMATRIX* camProjection, 
									   const DirectX::XMFLOAT3& camPos, float scale, ID3D11ShaderResourceView** textures, int textureCount, int isIcon);

		void SetShadowMapDataPerObjectType(RenderObject* renderObject);
		void SetShadowMapDataPerSpotlight(DirectX::XMMATRIX* lightView, DirectX::XMMATRIX* lightProjection);

		void SetLightDataPerFrame(DirectX::XMMATRIX* camView, DirectX::XMMATRIX* camProjection);
		void SetLightDataPerSpotlight(Spotlight* spotlight);
		void SetLightDataPerPointlight(Pointlight* pointlight);

		void SetShaderStage(ShaderStage stage);

		DirectX::XMFLOAT3 GetAmbientLight() const;
		void SetAmbientLight(const DirectX::XMFLOAT3 &ambientLight);
		void SetAntialiasingEnabled(bool enabled);
		void SetShadowsEnabled(bool enabled);

		void BeginScene(float red, float green, float blue, float alpha, bool clearBackBuffer);
		void Render(DirectX::XMMATRIX* world, int vertexBufferSize, const DirectX::XMFLOAT3& colorOffset = DirectX::XMFLOAT3(0, 0, 0));
		void RenderAnimation(DirectX::XMMATRIX* world, int vertexBufferSize, DirectX::XMMATRIX* extra, int bonecount, const DirectX::XMFLOAT3& colorOffset = DirectX::XMFLOAT3(0, 0, 0));
		void Render(GUI::Node* root, FontWrapper* fontWrapper, int brightness);
		void RenderLineStrip(DirectX::XMMATRIX* world, int nrOfPoints, const DirectX::XMFLOAT3& colorOffset = DirectX::XMFLOAT3(0,0,0));
		void RenderShadowMap(DirectX::XMMATRIX* world, int vertexBufferSize, DirectX::XMMATRIX* animTransformData = nullptr, int bonecount = 0);
		void RenderSelectionQuad(float lastX, float lastY, float currentX, float currentY);
		void RenderScreenQuad();
		void RenderParticles(ID3D11Buffer* particlePointsBuffer, int vertexCount, int vertexSize);
		void RenderVertexBuffer(ID3D11Buffer* vertexBuffer, DirectX::XMMATRIX* world, int vertexCount, int vertexSize);
		void EndScene();

		ID3D11Device* GetDevice() const;
		ID3D11DeviceContext* GetDeviceContext() const;
	};
}