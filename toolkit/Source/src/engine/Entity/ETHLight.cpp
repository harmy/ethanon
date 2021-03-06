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

#include "ETHLight.h"

ETHLight::ETHLight(const bool active) :
	active(active),
	staticLight(false),
	pos(Vector3(0,0,0)),
	color(Vector3(1,1,1)),
	range(256.0f),
	haloSize(64.0f),
	haloBrightness(1.0f),
	castShadows(false),
	haloBitmap(GS_L(""))
{
}

bool ETHLight::ReadFromXMLFile(TiXmlElement *pElement)
{
	int nActive;
	pElement->QueryIntAttribute(GS_L("active"), &nActive);
	active = static_cast<ETH_BOOL>(nActive);

	int nStatic;
	pElement->QueryIntAttribute(GS_L("static"), &nStatic);
	staticLight = static_cast<ETH_BOOL>(nStatic);

	int nCastShadows;
	pElement->QueryIntAttribute(GS_L("castShadows"), &nCastShadows);
	castShadows = static_cast<ETH_BOOL>(nCastShadows);

	pElement->QueryFloatAttribute(GS_L("range"), &range);
	pElement->QueryFloatAttribute(GS_L("haloBrightness"), &haloBrightness);
	pElement->QueryFloatAttribute(GS_L("haloSize"), &haloSize);

	TiXmlNode *pNode;
	TiXmlElement *pIter;

	pNode = pElement->FirstChild(GS_L("Position"));
	pIter = pNode->ToElement();
	if (pNode)
	{
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("x"), &pos.x);
			pIter->QueryFloatAttribute(GS_L("y"), &pos.y);
			pIter->QueryFloatAttribute(GS_L("z"), &pos.z);
		}
	}

	pNode = pElement->FirstChild(GS_L("Color"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("r"), &color.x);
			pIter->QueryFloatAttribute(GS_L("g"), &color.y);
			pIter->QueryFloatAttribute(GS_L("b"), &color.z);
		}
	}

	pNode = pElement->FirstChild(GS_L("HaloBitmap"));
	if (pNode)
	{
		TiXmlElement *pStringElement = pNode->ToElement();
		if (pStringElement)
		{
			haloBitmap = pStringElement->GetText();
		}
	}
	return true;
}

bool ETHLight::WriteToXMLFile(TiXmlElement *pRoot) const
{
	TiXmlElement *pLightRoot = new TiXmlElement(GS_L("Light"));
	pRoot->LinkEndChild(pLightRoot); 

	TiXmlElement *pElement;
	pElement = new TiXmlElement(GS_L("Position"));
	pLightRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), pos.x);
	pElement->SetDoubleAttribute(GS_L("y"), pos.y);
	pElement->SetDoubleAttribute(GS_L("z"), pos.z);

	pElement = new TiXmlElement(GS_L("Color"));
	pLightRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("r"), color.x);
	pElement->SetDoubleAttribute(GS_L("g"), color.y);
	pElement->SetDoubleAttribute(GS_L("b"), color.z);

	if (haloBitmap != GS_L(""))
	{
		pElement = new TiXmlElement(GS_L("HaloBitmap"));
		pElement->LinkEndChild(new TiXmlText(haloBitmap));
		pLightRoot->LinkEndChild(pElement);
	}

	pLightRoot->SetAttribute(GS_L("active"), active);
	pLightRoot->SetAttribute(GS_L("static"), staticLight);
	pLightRoot->SetAttribute(GS_L("castShadows"), castShadows);
	pLightRoot->SetDoubleAttribute(GS_L("range"), range);
	pLightRoot->SetDoubleAttribute(GS_L("haloBrightness"), haloBrightness);
	pLightRoot->SetDoubleAttribute(GS_L("haloSize"), haloSize);
	return true;
}

bool ETHLight::IsActive() const
{
	return ETHGlobal::ToBool(active);
}

void ETHLight::SetLightScissor(const VideoPtr& video, const Vector2& zAxisDir) const
{
	const float squareEdgeSize = range * 2.0f;
	Vector2 sum(zAxisDir * pos.z * 2.0f);
	sum.x = Abs(sum.x);
	sum.y = Abs(sum.y);
	const Vector2 squareSize(Vector2(squareEdgeSize, squareEdgeSize) + sum);
	const Vector2 absPos(ETHGlobal::ToScreenPos(pos, zAxisDir) - video->GetCameraPos() - (squareSize * 0.5f));
	video->SetScissor(Rect2D(absPos.ToVector2i(), squareSize.ToVector2i()));
}
