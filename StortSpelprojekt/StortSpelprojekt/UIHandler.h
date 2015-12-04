#ifndef UIHANDLER_H
#define UIHANDLER_H

#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

#include "Fonts.h"
#include "Window.h"

class UIHandler
{
private:
	struct TextInfo
	{
		DirectX::XMFLOAT2		position;
		float					fontSize;
		UINT32					color;
		std::wstring			text;
		int						textId;

		TextInfo(DirectX::XMFLOAT2 position_, float fontSize_, UINT32 color_, std::wstring text_, int textId_)
		{
			position	= position_;
			fontSize	= fontSize_;
			color		= color_;
			text		= text_;
			textId		= textId_;
		}
	};
	struct FontInfo
	{
		const WCHAR*			filePath;
		std::vector<TextInfo>	text;
		Renderer::Fonts			fontDevice;
		FontInfo(
			const WCHAR* fontPath_,
			DirectX::XMFLOAT2 position_,
			float fontSize_,
			UINT32 color_,
			std::wstring text_,
			int textId_,
			bool custom_,
			ID3D11Device* device_)
		{
			filePath = fontPath_;
			text.push_back(TextInfo(position_, fontSize_, color_, text_, textId_));
			//Setting up the fontDevice
			if (!custom_)
			{
				fontDevice.Initialize(device_, fontPath_);
			}
			else
			{
				fontDevice.Initialize(device_, fontPath_, fontPath_);
			}
		}
		void AddText(DirectX::XMFLOAT2 position_, float fontSize_, UINT32 color_, std::wstring text_, int textId_)
		{
			text.push_back(TextInfo(position_, fontSize_, color_, text_, textId_));
		}
		void Release()
		{
			text.clear();
			//removes all allocated space.
			std::vector<TextInfo>().swap(text);
		}
	};
private:
	std::vector<FontInfo>		_fonts;
	int							_textId;
	System::WindowSettings		_windowSettings;
	ID3D11Device*				_device;

public:
	UIHandler(ID3D11Device* device, System::WindowSettings windowSettings);
	~UIHandler();

	void Update();
	void Render(ID3D11DeviceContext* _deviceContext);
	void OnResize(System::WindowSettings windowSettings);
	
	//Adds a default font in the windows library.
	int AddFont(const WCHAR* fontName, DirectX::XMFLOAT2 position, float fontSize, UINT32 color, std::wstring text);
	//Adds a custom font from a sourcedirectory folder.
	int AddCustomFont(const WCHAR* filePath, DirectX::XMFLOAT2 position, float fontSize, UINT32 color, std::wstring text);

	//Remove a font and all text below this font
	bool RemoveFont(const WCHAR* filePathOrName);
	//Remove an individual text
	bool RemoveText(int id);

};

#endif