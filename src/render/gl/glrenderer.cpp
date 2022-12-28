#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string.h>
#include <stdio.h>

#include "render/gl/glrenderer.h"
#include "render/gl/glvertexbuffer.h"
#include "render/gl/glindexbuffer.h"

namespace solunar
{

GLRenderer::GLRenderer(GLFWwindow* window) :
	m_window(window),
	m_renderFeature(GLRenderFeature_None),
	m_separatedShaderObjects(GLShaderFeature_None)
{
	glfwMakeContextCurrent(window);

	// Load OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		// do something we in trouble :(((
	}

	initRenderFeatures();
}

GLRenderer::~GLRenderer()
{
}

void GLRenderer::initialize()
{
}

void GLRenderer::shutdown()
{
}

void GLRenderer::beginFrame()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.5f, 0.5, 0.5f, 1.0f);
}

void GLRenderer::endFrame()
{
	glfwSwapBuffers(m_window);
}

IBuffer* GLRenderer::createBuffer(const BufferDesc& bufferDesc, const SubresourceDesc& subresourceDesc)
{
	switch (bufferDesc.m_bufferType)
	{
	case BufferType_VertexBuffer:
		return new GLVertexBuffer(bufferDesc, subresourceDesc);
	case BufferType_IndexBuffer:
		return new GLIndexBuffer(bufferDesc, subresourceDesc);
	case BufferType_ConstantBuffer:
		break;
	default:
		break;
	}
	return nullptr;
}

void GLRenderer::setVertexBuffer(IBuffer* buffer, uint32_t stride, uint32_t offset)
{
	IGLBufferBase* bufferBase = (IGLBufferBase*)buffer;
	if (bufferBase)
		bufferBase->bind();
	else
		glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLRenderer::setIndexBuffer(IBuffer* buffer)
{
	IGLBufferBase* bufferBase = (IGLBufferBase*)buffer;
	if (bufferBase)
		bufferBase->bind();
	else
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLRenderer::initRenderFeatures()
{
	GLint extensionCount = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);

	//////////////////////////////////////////////////////////////////////////
	// Direct State Access

	// Try to find EXT_direct_state_access
	for (int i = 0; i < extensionCount; i++) {
		const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
		if (strcmp(extension, "GL_EXT_direct_state_access") == 0)
			m_renderFeature = GLRenderFeature_EXTDSA;
	}

	// Try to find ARB_direct_state_access
	for (int i = 0; i < extensionCount; i++) {
		const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
		if (strcmp(extension, "GL_ARB_direct_state_access") == 0)
			m_renderFeature = GLRenderFeature_ARBDSA;
	}

	// log what we found
	if (m_renderFeature == GLRenderFeature_ARBDSA)
		printf("found ARB_direct_state_access\n");
	else if (m_renderFeature == GLRenderFeature_EXTDSA)
		printf("found EXT_direct_state_access\n");
	else
		printf("not found any direct state access extension\n");

	//////////////////////////////////////////////////////////////////////////
	// Separated shader objects

	// Try to find EXT_separate_shader_objects
	for (int i = 0; i < extensionCount; i++) {
		const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
		if (strcmp(extension, "GL_EXT_separate_shader_objects") == 0)
			m_separatedShaderObjects = GLShaderFeature_EXTSeparated;
	}

	// Try to find ARB_separate_shader_objects
	for (int i = 0; i < extensionCount; i++) {
		const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
		if (strcmp(extension, "GL_ARB_separate_shader_objects") == 0)
			m_separatedShaderObjects = GLShaderFeature_ARBSeparated;
	}

	// log what we found
	if (m_separatedShaderObjects == GLShaderFeature_ARBSeparated)
		printf("found ARB_separate_shader_objects\n");
	else if (m_separatedShaderObjects == GLShaderFeature_EXTSeparated)
		printf("found EXT_separate_shader_objects\n");
	else
		printf("not found any separated shader objects extension\n");
}

IRenderer* createGLRenderer(GLFWwindow* window)
{
	return new GLRenderer(window);
}

}