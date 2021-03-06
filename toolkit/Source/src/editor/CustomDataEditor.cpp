/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#include "CustomDataEditor.h"
#include <unicode/utf8converter.h>
#include <string>
#include <sstream>
#include <map>
using namespace std;

CustomDataEditor::CustomDataEditor()
{
	m_lastEntityID =-2;
	m_cdesState = CDES_IDLE;
	m_lastData.clear();
}

void CustomDataEditor::LoadEditor(EditorBase *pEditor)
{
	m_options.SetupMenu(pEditor->GetVideoHandler(), pEditor->GetInputHandler(),
		L"+ New variable", pEditor->GetMenuSize(), pEditor->GetMenuWidth()*2, true);
	m_options.AddButton(L"int");
	m_options.AddButton(L"uint");
	m_options.AddButton(L"float");
	m_options.AddButton(L"string");
}

float CustomDataEditor::DoEditor(const Vector2 &v2Pos, ETHEntity* pEntity, EditorBase *pEditor)
{
	if (!pEntity)
	{
		m_inVariableName.SetActive(false);
		m_inValueInput.SetActive(false);
		m_cdesState = CDES_IDLE;
		return 0.0f;
	}

	float fR = 0.0f;
	VideoPtr video = pEditor->GetVideoHandler();
	InputPtr input = pEditor->GetInputHandler();

	pEditor->ShadowPrint(v2Pos+Vector2(0,fR), L"Custom data:");
	fR += pEditor->GetMenuSize();

	const GSGUI_BUTTON r = m_options.PlaceMenu(v2Pos+Vector2(0,fR));
	fR += m_options.GetCurrentHeight();
	fR += pEditor->GetMenuSize()/2;

	// if the state is set to input a new variable name...
	if (m_cdesState >= CDES_ADDING_FLOAT && m_cdesState <= CDES_ADDING_STRING)
	{
		InputVariableName(pEntity, pEditor);
	}

	// edit values
	EditVariable(pEntity, pEditor);

	// checks if the user pressed the "Add variable" button and handles it
	if (r.text != L"")
	{
		m_inVariableName.SetupMenu(video, input, pEditor->GetMenuSize(), pEditor->GetMenuWidth()*4.0f, 32, true);
		if (r.text == L"int")
		{
			m_cdesState = CDES_ADDING_INT;
		}
		else if (r.text == L"uint")
		{
			m_cdesState = CDES_ADDING_UINT;
		}
		else if (r.text == L"float")
		{
			m_cdesState = CDES_ADDING_FLOAT;
		}
		else if (r.text == L"string")
		{
			m_cdesState = CDES_ADDING_STRING;
		}
	}

	fR += PlaceButtons(v2Pos+Vector2(0,fR), pEntity, pEditor);
	fR += pEditor->GetMenuSize()/2;

	if (!pEntity->HasCustomData() && m_cdesState == CDES_IDLE)
	{
		Rebuild(pEntity, pEditor);
		m_inVariableName.SetActive(false);
		m_inValueInput.SetActive(false);
	}

	return fR;
}

void CustomDataEditor::InputVariableName(ETHEntity* pEntity, EditorBase *pEditor)
{
	VideoPtr video = pEditor->GetVideoHandler();
	const Vector2 v2Pos = video->GetScreenSizeF()/2.0f - Vector2(m_inVariableName.GetWidth()/2.0f, 0.0f);

	if (m_inVariableName.IsActive())
	{
		DrawInputFieldRect(v2Pos, &m_inVariableName, pEditor, L"Enter variable name");
		m_inVariableName.PlaceInput(v2Pos);

		wstringstream ss;
		ss << ETHCustomDataManager::GetDataName(m_cdesState);
		pEditor->ShadowPrint(v2Pos-Vector2(0.0f,m_inVariableName.GetSize()), ss.str().c_str(),
			L"Verdana14_shadow.fnt", GS_BLACK);

		// if it has just been unactivated
		if (!m_inVariableName.IsActive())
		{
			if (m_inVariableName.GetValue() != L"")
			{
				switch (m_cdesState)
				{
				case CDES_ADDING_INT:
					pEntity->SetInt(m_inVariableName.GetValue(), 0);
					break;
				case CDES_ADDING_UINT:
					pEntity->SetUInt(m_inVariableName.GetValue(), 0);
					break;
				case CDES_ADDING_FLOAT:
					pEntity->SetFloat(m_inVariableName.GetValue(), 0.0f);
					break;
				case CDES_ADDING_STRING:
					pEntity->SetString(m_inVariableName.GetValue(), L"none");
					break;
				}
				Rebuild(pEntity, pEditor);
			}
			m_cdesState = CDES_IDLE;
		}
	}
}

void CustomDataEditor::ShowInScreenCustomData(const ETHEntity* pEntity, EditorBase *pEditor) const
{
	map<wstring, ETHCustomDataPtr> dataMap;
	pEntity->GetCustomDataManager()->CopyMap(dataMap);

	const Vector2 v2Pos(pEntity->GetPositionXY()-Vector2(0,pEntity->GetPosition().z)
		- pEditor->GetVideoHandler()->GetCameraPos());
	pEditor->ShadowPrint(v2Pos, pEntity->GetCustomDataManager()->GetDebugStringData().c_str());
}

float CustomDataEditor::PlaceButtons(const Vector2 &v2Pos, const ETHEntity* pEntity, EditorBase *pEditor)
{
	VideoPtr video = pEditor->GetVideoHandler();
	InputPtr input = pEditor->GetInputHandler();
	if (pEntity->HasCustomData())
	{
		m_customDataButtonList.PlaceMenu(v2Pos);

		// if the cursor is over, show message
		if (m_customDataButtonList.IsMouseOver())
		{
			pEditor->ShadowPrint(input->GetCursorPositionF(video)-Vector2(0,pEditor->GetMenuSize()), L"Click to edit");
		}
	}
	else
	{
		m_customDataButtonList.Clear();
	}
	return static_cast<float>(m_customDataButtonList.GetNumButtons())*pEditor->GetMenuSize();
}

bool CustomDataEditor::IsCursorAvailable() const
{
	if (m_inVariableName.IsActive() || m_inValueInput.IsActive())
	{
		return false;
	}
	return (!m_options.IsMouseOver() && (m_cdesState == CDES_IDLE) && !m_customDataButtonList.IsMouseOver());
}

bool CustomDataEditor::IsFieldActive() const
{
	if (m_inVariableName.IsActive())
		return true;
	if (m_inValueInput.IsActive())
		return true;
	return false;
}

void CustomDataEditor::Rebuild(const ETHEntity* pEntity, EditorBase *pEditor)
{
	m_customDataButtonList.Clear();
	m_customDataButtonList.SetupMenu(pEditor->GetVideoHandler(), pEditor->GetInputHandler(),
		pEditor->GetMenuSize(), pEditor->GetMenuWidth()*2, true, true, false);

	map<wstring, ETHCustomDataPtr> dataMap;
	pEntity->GetCustomDataManager()->CopyMap(dataMap);

	for (map<wstring, ETHCustomDataPtr>::const_iterator iter = dataMap.begin();
		 iter != dataMap.end(); iter++)
	{
		wstringstream ss;
		ss << L" = ";
		bool editable = true;
		switch (iter->second->GetType())
		{
		case ETHDT_INT:
			ss << iter->second->GetInt();
			break;
		case ETHDT_UINT:
			ss << iter->second->GetUInt();
			break;
		case ETHDT_FLOAT:
			ss << iter->second->GetFloat();
			break;
		case ETHDT_STRING:
			ss << L"\"" << utf8::c(iter->second->GetString()).wc_str() << L"\"";
			break;
		default:
			editable = false;
		};
		if (editable)
			m_customDataButtonList.AddButton(utf8::c(iter->first).wc_str(), false, ss.str().c_str());
	}
}

void CustomDataEditor::EditVariable(ETHEntity* pEntity, EditorBase *pEditor)
{
	if (m_cdesState != CDES_IDLE)
		return;

	VideoPtr video = pEditor->GetVideoHandler();
	InputPtr input = pEditor->GetInputHandler();

	GSGUI_BUTTON r = m_customDataButtonList.GetFirstActiveButton();

	if (m_lastData != r.text || pEntity->GetID() != m_lastEntityID)
	{
		m_inValueInput.SetupMenu(
			video, input, pEditor->GetMenuSize(),
			pEditor->GetMenuWidth()*4.0f, 144, false, 
			pEntity->GetCustomDataManager()->GetValueAsString(r.text)
		);
		m_lastData = r.text;
		m_lastEntityID = pEntity->GetID();
	}

	if (r.text != L"")
	{
		m_inValueInput.SetActive(true);
		const Vector2 v2Pos = video->GetScreenSizeF()/2.0f - Vector2(m_inValueInput.GetWidth()/2.0f, 0.0f);

		DrawInputFieldRect(v2Pos, &m_inValueInput, pEditor, L"Leave blank to erase variable");

		const ETH_CUSTOM_DATA_TYPE type = pEntity->CheckCustomData(r.text);
		switch (type)
		{
		case ETHDT_INT:
			pEntity->SetInt(r.text, ETHGlobal::ParseInt(utf8::c(m_inValueInput.PlaceInput(v2Pos)).wc_str()));
			break;
		case ETHDT_UINT:
			pEntity->SetUInt(r.text, ETHGlobal::ParseUInt(utf8::c(m_inValueInput.PlaceInput(v2Pos)).wc_str()));
			break;
		case ETHDT_STRING:
			pEntity->SetString(r.text, utf8::c(m_inValueInput.PlaceInput(v2Pos)).wstr());
			break;
		case ETHDT_FLOAT:
			pEntity->SetFloat(r.text, ETHGlobal::ParseFloat(utf8::c(m_inValueInput.PlaceInput(v2Pos)).wc_str()));
			break;
		};
		wstringstream ss;
		ss << ETHCustomDataManager::GetDataName(type) << L" " << r.text << L":";
		pEditor->ShadowPrint(v2Pos-Vector2(0.0f,m_inValueInput.GetSize()), ss.str().c_str(),
			L"Verdana14_shadow.fnt", GS_BLACK);
	}
	else
	{
		Rebuild(pEntity, pEditor);
	}

	if (!m_inValueInput.IsActive())
	{
		if (m_inValueInput.GetValue() == L"")
		{
			pEntity->EraseData(r.text);
			Rebuild(pEntity, pEditor);
		}
		m_customDataButtonList.DeactivateButton(r.text);
	}
}

void CustomDataEditor::DrawInputFieldRect(const Vector2 &v2Pos,
										   const GS_GUI *pGui,
										   EditorBase *pEditor,
										   const wchar_t *text) const
{
	VideoPtr video = pEditor->GetVideoHandler();
	const Vector2 v2BorderSize(5,5);
	const Vector2 v2FinalPos(v2Pos-v2BorderSize-Vector2(0,pGui->GetSize()));
	const Vector2 v2Size(Vector2(pGui->GetWidth(), pGui->GetSize()*2+pEditor->GetMenuSize())+v2BorderSize*2);
	video->DrawRectangle(v2FinalPos, v2Size,
		pGui->GetGUIStyle()->active_top, pGui->GetGUIStyle()->active_top,
		pGui->GetGUIStyle()->active_bottom, pGui->GetGUIStyle()->active_bottom);
	pEditor->ShadowPrint(Vector2(v2FinalPos.x, v2FinalPos.y+v2Size.y-pEditor->GetMenuSize()), text, GS_BLACK);
}
