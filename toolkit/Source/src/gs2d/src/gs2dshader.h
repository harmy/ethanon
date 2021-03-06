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

#ifndef GS2D_SHADER_H_
#define GS2D_SHADER_H_

#include "gs2dmath.h"
#include <boost/shared_array.hpp>

namespace gs2d {

enum GS_SHADER_PROFILE
{
	GSSP_NONE =-1,
	GSSP_MODEL_1 = 0,
	GSSP_MODEL_2 = 1,
	GSSP_MODEL_3 = 2,
	GSSP_HIGHEST = 3,
};

enum GS_SHADER_FOCUS
{
	GSSF_PIXEL = 0,
	GSSF_VERTEX = 1,
	GSSF_NONE = 2
};

class Video;

/**
 * \brief Provides a render context to the shaders
 */
class ShaderContext
{
public:
	virtual boost::any GetContextPointer() = 0;
	virtual bool DisableTextureParams() = 0;

protected:
	virtual bool CheckForError(const str_type::string& situation) = 0;
};

typedef boost::shared_ptr<ShaderContext> ShaderContextPtr;

/**
 * \brief Abstracts a shader object
 *
 * Stores, manages and binds a fragment or vertex shader.
 */
class Shader
{
public:

	virtual bool LoadShaderFromFile(ShaderContextPtr context, const str_type::string& fileName, const GS_SHADER_FOCUS focus,
		const GS_SHADER_PROFILE profile = GSSP_HIGHEST,	const char *entry = 0) = 0;

	virtual bool LoadShaderFromString(ShaderContextPtr context, const str_type::string& shaderName, const std::string& codeAsciiString, const GS_SHADER_FOCUS focus,
		const GS_SHADER_PROFILE profile = GSSP_HIGHEST,	const char *entry = 0) = 0;

	virtual bool ConstantExist(const str_type::string& name) = 0;
	virtual bool SetConstant(const str_type::string& name, const GS_COLOR dw) = 0;
	virtual bool SetConstant(const str_type::string& name, const math::Vector4 &v) = 0;
	virtual bool SetConstant(const str_type::string& name, const math::Vector3 &v) = 0;
	virtual bool SetConstant(const str_type::string& name, const math::Vector2 &v) = 0;
	virtual bool SetConstant(const str_type::string& name, const float x, const float y, const float z, const float w) = 0;
	virtual bool SetConstant(const str_type::string& name, const float x, const float y, const float z) = 0;
	virtual bool SetConstant(const str_type::string& name, const float x, const float y) = 0;
	virtual bool SetConstant(const str_type::string& name, const float x) = 0;
	virtual bool SetConstant(const str_type::string& name, const int n) = 0;
	virtual bool SetConstantArray(const str_type::string& name, unsigned int nElements, const boost::shared_array<const math::Vector2>& v) = 0;
	virtual bool SetMatrixConstant(const str_type::string& name, const math::Matrix4x4 &matrix) = 0;
	virtual bool SetTexture(const str_type::string& name, TextureWeakPtr pTexture) = 0;

	virtual bool SetShader() = 0;
	virtual GS_SHADER_FOCUS GetShaderFocus() const = 0;
	virtual GS_SHADER_PROFILE GetShaderProfile() const = 0;
	virtual void UnbindShader() = 0;

};

typedef boost::shared_ptr<Shader> ShaderPtr;

} // namespace gs2d

#endif 