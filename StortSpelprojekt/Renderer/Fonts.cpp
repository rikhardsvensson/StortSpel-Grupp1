#include "Fonts.h"

namespace Renderer
{
	Fonts::Fonts()
	{
		_cFont = nullptr;
	}

	void Fonts::Release()
	{
		SAFE_RELEASE(_factory);
		SAFE_RELEASE(_fontWrapper);

		SAFE_RELEASE(_cFont->writeFactory);
		SAFE_RELEASE(_cFont->writeTextLayout);
		SAFE_RELEASE(_cFont->fontCollection);
		SAFE_RELEASE(_cFont->textLayout);
		SAFE_DELETE(_cFont);
	}

	HRESULT Fonts::Initialize(ID3D11Device* device, LPCWSTR font)
	{
		HRESULT hr = S_OK;

		hr = FW1CreateFactory(FW1_VERSION, &_factory);
		IF_HR(hr);

		hr = _factory->CreateFontWrapper(device, font, &_fontWrapper);
		IF_HR(hr);

		return hr;
	}

	HRESULT Fonts::Initialize(ID3D11Device* device, LPCWSTR font, LPCWSTR filePath)
	{
		HRESULT hr = S_OK;

		//
		hr = FW1CreateFactory(FW1_VERSION, &_factory);
		IF_HR(hr);

		hr = _factory->CreateFontWrapper(device, L"Calibri", &_fontWrapper);
		IF_HR(hr);
		//		
		hr = _fontWrapper->GetDWriteFactory(&_cFont->writeFactory);
		IF_HR(hr);

		// Set up custom font collection
		IDWriteFontCollectionLoader *pCollectionLoader = new CCollectionLoader(filePath);

		hr = _cFont->writeFactory->RegisterFontCollectionLoader(pCollectionLoader);
		IF_HR(hr);

		hr = _cFont->writeFactory->CreateCustomFontCollection(pCollectionLoader, NULL, 0, &_cFont->fontCollection);
		if (FAILED(hr))
		{
			MessageBox(NULL, TEXT("CreateCustomFontCollection - Din custom font ar antagligen inte kompatibel med DirectWrite. Eller att den inte hittas."), NULL, MB_OK);
		}
		IF_HR(hr);

		_cFont->fontName = font;

		return hr;
	}

	HRESULT Fonts::DrawString(ID3D11DeviceContext* deviceContext, const WCHAR* text, float fontSize, float x, float y, UINT32 color)
	{
		HRESULT hr = S_OK;

		if (_cFont == nullptr)
		{
			_fontWrapper->DrawString(deviceContext, text, fontSize, x, y, color, FW1_RESTORESTATE);
		}
		else
		{
			//if anything in the font changed needs changing
			if (_cFont->fontSize != fontSize || 0 != wcscmp(_cFont->fontText, text))
			{
				//For Fontsize
				IDWriteTextFormat *textFormat;
				hr = _cFont->writeFactory->CreateTextFormat(
					_cFont->fontName,
					_cFont->fontCollection,
					DWRITE_FONT_WEIGHT_NORMAL,
					DWRITE_FONT_STYLE_NORMAL,
					DWRITE_FONT_STRETCH_NORMAL,
					fontSize,
					L"",
					&textFormat
					);
				IF_HR(hr);

				SAFE_RELEASE(_cFont->textLayout);
				//For string update
				hr = _cFont->writeFactory->CreateTextLayout(
					text,
					sizeof(text) / sizeof(text[0]),
					textFormat,
					0.0f,
					0.0f,
					&_cFont->textLayout
					);
				IF_HR(hr);

				_cFont->textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
				SAFE_RELEASE(textFormat);
			}

			_fontWrapper->DrawTextLayout(deviceContext, _cFont->textLayout, x, y, color, FW1_RESTORESTATE);
		}

		return hr;
	}
}