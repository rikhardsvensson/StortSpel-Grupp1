#include "TextBox.h"

GUI::TextBox::TextBox(Node* textNode, int characterLimit, bool allowMultipleLines, bool allowOnlyNumbers, bool clearDefaultTextOnFirstEnter)
{
	_textNode = textNode;
	_boxNode = _textNode->GetParent();
	_characterLimit = characterLimit;
	_allowMultipleLines = allowMultipleLines;
	_allowOnlyNumbers = allowOnlyNumbers;
	_clearDefaultTextOnFirstEnter = clearDefaultTextOnFirstEnter;
	_firstTimeEditingText = true;
	_isSelectedForEditing = false;
	_showTextMarker = false;
	_textMarkerFrameTimer = 0;
}

GUI::TextBox::TextBox()
{

}

GUI::TextBox::~TextBox()
{

}

GUI::TextBox& GUI::TextBox::operator=(const GUI::TextBox &other)
{
	_textNode = other._textNode;
	_boxNode = other._boxNode;
	_characterLimit = other._characterLimit;
	_allowMultipleLines = other._allowMultipleLines;
	_allowOnlyNumbers = other._allowOnlyNumbers;
	_clearDefaultTextOnFirstEnter = other._clearDefaultTextOnFirstEnter;
	_firstTimeEditingText = other._firstTimeEditingText;
	_isSelectedForEditing = other._isSelectedForEditing;
	_showTextMarker = other._showTextMarker;
	_textMarkerFrameTimer = other._textMarkerFrameTimer;
	return *this;
}

void GUI::TextBox::Update()
{
	if (_textMarkerFrameTimer > TOGGLE_TEXT_MARKER_ON_FRAME)
	{
		_showTextMarker = !_showTextMarker;
		_textMarkerFrameTimer = 0;
	}
	_textMarkerFrameTimer++;
}

GUI::Node* GUI::TextBox::GetTextNode() const
{
	return _textNode;
}

void GUI::TextBox::SetTextNode(GUI::Node* textNode)
{
	_textNode = textNode;
}

GUI::Node* GUI::TextBox::GetBoxNode() const
{
	return _boxNode;
}

void GUI::TextBox::SetBoxNode(GUI::Node* boxNode)
{
	_boxNode = boxNode;
}

std::wstring GUI::TextBox::GetText() const
{
	if (_textNode != nullptr)
	{
		std::wstring text = _textNode->GetText();
		if (_showTextMarker)
		{
			text += TEXT_MARKER;
		}
		return _textNode->GetText();
	}
	return L"";
}

void GUI::TextBox::SetText(std::wstring text)
{
	if (_textNode != nullptr)
	{
		if (_showTextMarker)
		{
			text += TEXT_MARKER;
		}
		_textNode->SetText(text);
	}
}

int GUI::TextBox::GetCharacterLimit() const
{
	return _characterLimit;
}

void GUI::TextBox::SetCharacterLimit(const int &characterLimit)
{
	_characterLimit = characterLimit;
}

bool GUI::TextBox::GetOnlyNumbersAllowed()
{
	return _allowOnlyNumbers;
}

void GUI::TextBox::SetOnlyNumbersAllowed(bool allowOnlyNumbers)
{
	_allowOnlyNumbers = allowOnlyNumbers;
}

void GUI::TextBox::SelectTextBox()
{
	_oldText = _textNode->GetText();
	if (_clearDefaultTextOnFirstEnter && _firstTimeEditingText)
	{
		SetText(System::StringToWstring(""));
		_firstTimeEditingText = false;
	}
}

void GUI::TextBox::DeselectTextBox()
{
	if (_showTextMarker)
	{
		std::wstring text = _textNode->GetText();
		text.erase(text.end() - 1);
		_textNode->SetText(text);
		_showTextMarker = false;
		_textMarkerFrameTimer = 0;
	}
	if (_allowOnlyNumbers)
	{
		std::string text = System::WStringToString(_textNode->GetText());
		if (!IsInteger(text))
		{
			_textNode->SetText(_oldText);
		}
	}
}

bool GUI::TextBox::GetAllowMultipleLines() const
{
	return _allowMultipleLines;
}

bool GUI::TextBox::IsInteger(const std::string & s)
{
	return !s.empty() && s.find_first_not_of("+-0123456789") == std::string::npos;
}
