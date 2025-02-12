/************************************************************************************

Authors     :   Bradley Austin Davis <bdavis@saintandreas.org>
Copyright   :   Copyright Brad Davis. All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/
 

#include <iostream>
#include <memory>
#include <exception>
#include <algorithm>
#include <Windows.h>

#define __STDC_FORMAT_MACROS 1

#define FAIL(X) throw std::runtime_error(X)

///////////////////////////////////////////////////////////////////////////////
//
// GLM is a C++ math library meant to mirror the syntax of GLSL 
//

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Import the most commonly used types into the default namespace
using glm::ivec3;
using glm::ivec2;
using glm::uvec2;
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::quat;

using namespace std;

///////////////////////////////////////////////////////////////////////////////
//
// GLEW gives cross platform access to OpenGL 3.x+ functionality.  
//

#include <GL/glew.h>

bool checkFramebufferStatus(GLenum target = GL_FRAMEBUFFER) {
	GLuint status = glCheckFramebufferStatus(target);
	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE:
		return true;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cerr << "framebuffer incomplete attachment" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cerr << "framebuffer missing attachment" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cerr << "framebuffer incomplete draw buffer" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cerr << "framebuffer incomplete read buffer" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		std::cerr << "framebuffer incomplete multisample" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		std::cerr << "framebuffer incomplete layer targets" << std::endl;
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cerr << "framebuffer unsupported internal format or image" << std::endl;
		break;

	default:
		std::cerr << "other framebuffer error" << std::endl;
		break;
	}

	return false;
}

bool checkGlError() {
	GLenum error = glGetError();
	if (!error) {
		return false;
	}
	else {
		switch (error) {
		case GL_INVALID_ENUM:
			std::cerr << ": An unacceptable value is specified for an enumerated argument.The offending command is ignored and has no other side effect than to set the error flag.";
			break;
		case GL_INVALID_VALUE:
			std::cerr << ": A numeric argument is out of range.The offending command is ignored and has no other side effect than to set the error flag";
			break;
		case GL_INVALID_OPERATION:
			std::cerr << ": The specified operation is not allowed in the current state.The offending command is ignored and has no other side effect than to set the error flag..";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cerr << ": The framebuffer object is not complete.The offending command is ignored and has no other side effect than to set the error flag.";
			break;
		case GL_OUT_OF_MEMORY:
			std::cerr << ": There is not enough memory left to execute the command.The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
			break;
		case GL_STACK_UNDERFLOW:
			std::cerr << ": An attempt has been made to perform an operation that would cause an internal stack to underflow.";
			break;
		case GL_STACK_OVERFLOW:
			std::cerr << ": An attempt has been made to perform an operation that would cause an internal stack to overflow.";
			break;
		}
		return true;
	}
}

void glDebugCallbackHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, GLvoid* data) {
	OutputDebugStringA(msg);
	std::cout << "debug call: " << msg << std::endl;
}

//////////////////////////////////////////////////////////////////////
//
// GLFW provides cross platform window creation
//

#include <GLFW/glfw3.h>

namespace glfw {
	inline GLFWwindow * createWindow(const uvec2 & size, const ivec2 & position = ivec2(INT_MIN)) {
		GLFWwindow * window = glfwCreateWindow(size.x, size.y, "glfw", nullptr, nullptr);
		if (!window) {
			FAIL("Unable to create rendering window");
		}
		if ((position.x > INT_MIN) && (position.y > INT_MIN)) {
			glfwSetWindowPos(window, position.x, position.y);
		}
		return window;
	}
}

// A class to encapsulate using GLFW to handle input and render a scene
class GlfwApp {

protected:
	uvec2 windowSize;
	ivec2 windowPosition;
	GLFWwindow * window{ nullptr };
	unsigned int frame{ 0 };

public:
	GlfwApp() {
		// Initialize the GLFW system for creating and positioning windows
		if (!glfwInit()) {
			FAIL("Failed to initialize GLFW");
		}
		glfwSetErrorCallback(ErrorCallback);
	}

	virtual ~GlfwApp() {
		if (nullptr != window) {
			glfwDestroyWindow(window);
		}
		glfwTerminate();
	}

	virtual int run() {
		preCreate();

		window = createRenderingTarget(windowSize, windowPosition);

		if (!window) {
			std::cout << "Unable to create OpenGL window" << std::endl;
			return -1;
		}

		postCreate();

		initGl();

		while (!glfwWindowShouldClose(window)) {
			++frame;
			glfwPollEvents();
			update();
			draw();
			finishFrame();
		}

		shutdownGl();

		return 0;
	}


protected:
	virtual GLFWwindow * createRenderingTarget(uvec2 & size, ivec2 & pos) = 0;

	virtual void draw() = 0;

	void preCreate() {
		glfwWindowHint(GLFW_DEPTH_BITS, 16);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	}


	void postCreate() {
		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetMouseButtonCallback(window, MouseButtonCallback);
		glfwMakeContextCurrent(window);

		// Initialize the OpenGL bindings
		// For some reason we have to set this experminetal flag to properly
		// init GLEW if we use a core context.
		glewExperimental = GL_TRUE;
		if (0 != glewInit()) {
			FAIL("Failed to initialize GLEW");
		}
		glGetError();

		if (GLEW_KHR_debug) {
			GLint v;
			glGetIntegerv(GL_CONTEXT_FLAGS, &v);
			if (v & GL_CONTEXT_FLAG_DEBUG_BIT) {
				//glDebugMessageCallback(glDebugCallbackHandler, this);
			}
		}
	}

	virtual void initGl() {
	}

	virtual void shutdownGl() {
	}

	virtual void finishFrame() {
		glfwSwapBuffers(window);
	}

	virtual void destroyWindow() {
		glfwSetKeyCallback(window, nullptr);
		glfwSetMouseButtonCallback(window, nullptr);
		glfwDestroyWindow(window);
	}

	virtual void onKey(int key, int scancode, int action, int mods) {
		if (GLFW_PRESS != action) {
			return;
		}

		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, 1);
			return;
		}
	}

	virtual void update() {}

	virtual void onMouseButton(int button, int action, int mods) {}

protected:
	virtual void viewport(const ivec2 & pos, const uvec2 & size) {
		glViewport(pos.x, pos.y, size.x, size.y);
	}

private:

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		GlfwApp * instance = (GlfwApp *)glfwGetWindowUserPointer(window);
		instance->onKey(key, scancode, action, mods);
	}

	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		GlfwApp * instance = (GlfwApp *)glfwGetWindowUserPointer(window);
		instance->onMouseButton(button, action, mods);
	}

	static void ErrorCallback(int error, const char* description) {
		FAIL(description);
	}
};

//////////////////////////////////////////////////////////////////////
//
// The Oculus VR C API provides access to information about the HMD
//

#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>

namespace ovr {

	// Convenience method for looping over each eye with a lambda
	template <typename Function>
	inline void for_each_eye(Function function) {
		for (ovrEyeType eye = ovrEyeType::ovrEye_Left;
			eye < ovrEyeType::ovrEye_Count;
			eye = static_cast<ovrEyeType>(eye + 1)) {
			function(eye);
		}
	}

	inline mat4 toGlm(const ovrMatrix4f & om) {
		return glm::transpose(glm::make_mat4(&om.M[0][0]));
	}

	inline mat4 toGlm(const ovrFovPort & fovport, float nearPlane = 0.01f, float farPlane = 10000.0f) {
		return toGlm(ovrMatrix4f_Projection(fovport, nearPlane, farPlane, true));
	}

	inline vec3 toGlm(const ovrVector3f & ov) {
		return glm::make_vec3(&ov.x);
	}

	inline vec2 toGlm(const ovrVector2f & ov) {
		return glm::make_vec2(&ov.x);
	}

	inline uvec2 toGlm(const ovrSizei & ov) {
		return uvec2(ov.w, ov.h);
	}

	inline quat toGlm(const ovrQuatf & oq) {
		return glm::make_quat(&oq.x);
	}

	inline mat4 toGlm(const ovrPosef & op) {
		mat4 orientation = glm::mat4_cast(toGlm(op.Orientation));
		mat4 translation = glm::translate(mat4(), ovr::toGlm(op.Position));
		return translation * orientation;
	}

	inline ovrMatrix4f fromGlm(const mat4 & m) {
		ovrMatrix4f result;
		mat4 transposed(glm::transpose(m));
		memcpy(result.M, &(transposed[0][0]), sizeof(float) * 16);
		return result;
	}

	inline ovrVector3f fromGlm(const vec3 & v) {
		ovrVector3f result;
		result.x = v.x;
		result.y = v.y;
		result.z = v.z;
		return result;
	}

	inline ovrVector2f fromGlm(const vec2 & v) {
		ovrVector2f result;
		result.x = v.x;
		result.y = v.y;
		return result;
	}

	inline ovrSizei fromGlm(const uvec2 & v) {
		ovrSizei result;
		result.w = v.x;
		result.h = v.y;
		return result;
	}

	inline ovrQuatf fromGlm(const quat & q) {
		ovrQuatf result;
		result.x = q.x;
		result.y = q.y;
		result.z = q.z;
		result.w = q.w;
		return result;
	}
}
#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <cmath>
class RiftManagerApp {
protected:
	ovrSession _session;
	ovrHmdDesc _hmdDesc;
	ovrGraphicsLuid _luid;

public:
	RiftManagerApp() {
		if (!OVR_SUCCESS(ovr_Create(&_session, &_luid))) {
			FAIL("Unable to create HMD session");
		}

		_hmdDesc = ovr_GetHmdDesc(_session);
	}

	~RiftManagerApp() {
		ovr_Destroy(_session);
		_session = nullptr;
	}
};



//////////////////////////////////////////////////////////////////////
//
// The remainder of this code is specific to the scene we want to 
// render.  I use oglplus to render an array of cubes, but your 
// application would perform whatever rendering you want
//


//////////////////////////////////////////////////////////////////////
//
// OGLplus is a set of wrapper classes for giving OpenGL a more object
// oriented interface
//
#define OGLPLUS_USE_GLCOREARB_H 0
#define OGLPLUS_USE_GLEW 1
#define OGLPLUS_USE_BOOST_CONFIG 0
#define OGLPLUS_NO_SITE_CONFIG 1
#define OGLPLUS_LOW_PROFILE 1

#pragma warning( disable : 4068 4244 4267 4065)
#include <oglplus/config/basic.hpp>
#include <oglplus/config/gl.hpp>
#include <oglplus/all.hpp>
#include <oglplus/interop/glm.hpp>
#include <oglplus/bound/texture.hpp>
#include <oglplus/bound/framebuffer.hpp>
#include <oglplus/bound/renderbuffer.hpp>
#include <oglplus/bound/buffer.hpp>
#include <oglplus/shapes/cube.hpp>
#include <oglplus/shapes/wrapper.hpp>/
#pragma warning( default : 4068 4244 4267 4065)



namespace Attribute {
	enum {
		Position = 0,
		TexCoord0 = 1,
		Normal = 2,
		Color = 3,
		TexCoord1 = 4,
		InstanceTransform = 5,
	};
}

static const char * VERTEX_SHADER = R"SHADER(
#version 410 core

uniform mat4 ProjectionMatrix = mat4(1);
uniform mat4 CameraMatrix = mat4(1);

layout(location = 0) in vec4 Position;
layout(location = 2) in vec3 Normal;
layout(location = 5) in mat4 InstanceTransform;

out vec3 vertNormal;

void main(void) {
   mat4 ViewXfm = CameraMatrix * InstanceTransform;
   //mat4 ViewXfm = CameraMatrix;
   vertNormal = Normal;
   gl_Position = ProjectionMatrix * ViewXfm * Position;
}
)SHADER";

static const char * FRAGMENT_SHADER = R"SHADER(
#version 410 core

in vec3 vertNormal;
out vec4 fragColor;

void main(void) {
    vec3 color = vertNormal;
    if (!all(equal(color, abs(color)))) {
        color = vec3(1.0) - abs(color);
    }
    fragColor = vec4(color, 1.0);
}
)SHADER";

// a class for encapsulating building and rendering an RGB cube
struct ColorCubeScene {

	// Program
	oglplus::shapes::ShapeWrapper cube;
	oglplus::Program prog;
	oglplus::VertexArray vao;
	GLuint instanceCount;
	oglplus::Buffer instances;

	// VBOs for the cube's vertices and normals

	const unsigned int GRID_SIZE{ 5 };

public:
	ColorCubeScene() : cube({ "Position", "Normal" }, oglplus::shapes::Cube()) {
		using namespace oglplus;
		try {
			// attach the shaders to the program
			prog.AttachShader(
				FragmentShader()
				.Source(GLSLSource(String(FRAGMENT_SHADER)))
				.Compile()
			);
			prog.AttachShader(
				VertexShader()
				.Source(GLSLSource(String(VERTEX_SHADER)))
				.Compile()
			);
			prog.Link();
		}
		catch (ProgramBuildError & err) {
			FAIL((const char*)err.what());
		}

		// link and use it
		prog.Use();

		vao = cube.VAOForProgram(prog);
		vao.Bind();
		// Create a cube of cubes
		{
			std::vector<mat4> instance_positions;
			for (unsigned int z = 0; z < GRID_SIZE; ++z) {
				for (unsigned int y = 0; y < GRID_SIZE; ++y) {
					for (unsigned int x = 0; x < GRID_SIZE; ++x) {
						int xpos = (x - (GRID_SIZE / 2)) * 2;
						int ypos = (y - (GRID_SIZE / 2)) * 2;
						int zpos = (z - (GRID_SIZE / 2)) * 2;
						vec3 relativePosition = vec3(xpos, ypos, zpos);
						if (relativePosition == vec3(0)) {
							continue;
						}
						instance_positions.push_back(glm::translate(glm::mat4(1.0f), relativePosition));
					}
				}
			}

			Context::Bound(Buffer::Target::Array, instances).Data(instance_positions);
			instanceCount = (GLuint)instance_positions.size();
			int stride = sizeof(mat4);
			for (int i = 0; i < 4; ++i) {
				VertexArrayAttrib instance_attr(prog, Attribute::InstanceTransform + i);
				size_t offset = sizeof(vec4) * i;
				instance_attr.Pointer(4, DataType::Float, false, stride, (void*)offset);
				instance_attr.Divisor(1);
				instance_attr.Enable();
			}
		}
	}

	void render(const mat4 & projection, const mat4 & modelview) {
		using namespace oglplus;
		prog.Use();
		Uniform<mat4>(prog, "ProjectionMatrix").Set(projection);
		Uniform<mat4>(prog, "CameraMatrix").Set(modelview);  // cse190: this is default for normal rendering with head tracking
															 //	Uniform<mat4>(prog, "CameraMatrix").Set(identity);   
															 // cse190: changing modelview to identity freezes scene and head motion
		vao.Bind();
		cube.Draw(instanceCount);
	}
};

#include "Cube.h"
#include "Sphere.h"
#include "Shader.h"
#include "CrystalBody.h"
#include "CrystalArm.h"
#include "CrystalSoldier.h"
#include "rpc/client.h"
#include <iostream>
#include <string>
#include <vector>
#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <cmath>
#include "rpc\msgpack.hpp"
using std::string;
#define VERTEX_SHADER_PATH ".\\shader.vert"
#define FRAGMENT_SHADER_PATH ".\\shader.frag"
#define SKYBOX_VERTEX_SHADER_PATH ".\\skybox.vert"
#define SKYBOX_FRAGMENT_SHADER_PATH ".\\skybox.frag"
#define BULLET_VERTEX_SHADER_PATH ".\\bullet.vert"
#define BULLET_FRAGMENT_SHADER_PATH ".\\bullet.frag"
#define CRYSTAL_VERTEX_SHADER_PATH ".\\crystal.vert"
#define CRYSTAL_FRAGMENT_SHADER_PATH ".\\crystal.frag"
class RiftApp : public GlfwApp, public RiftManagerApp {

private:
	GLuint _fbo{ 0 };
	GLuint _depthBuffer{ 0 };
	ovrTextureSwapChain _eyeTexture;
	GLuint _mirrorFbo{ 0 };
	ovrMirrorTexture _mirrorTexture;
	ovrEyeRenderDesc _eyeRenderDescs[2];
	mat4 _eyeProjections[2];
	ovrLayerEyeFov _sceneLayer;
	ovrViewScaleDesc _viewScaleDesc;
	uvec2 _renderTargetSize;
	uvec2 _mirrorSize;
	ovrEyeRenderDesc left_erd_var, right_erd_var, left_default_erd, right_default_erd;

public:
	int RiftApp::buttonA_status;
	int RiftApp::buttonB_status;
	glm::mat4 camera[2];
	glm::mat4 last_ori[2];
	glm::mat4 last_pos[2];
	ovrPosef last_pose[2];
	bool RiftApp::rThumbButton_status;
	float RiftApp::rThumbStick_val;
	ovrEyeType RiftApp::eye;
	ovrPosef RiftApp::eyePoses[2];
	GLuint quadVAO, quadVBO;
	Cube *cube, *skybox;
	Sphere *bullet;
	GLint shaderProgram, skyboxShader, bulletShader, crystalShader;
	mat4 cubeInitPos;
	mat4 leftQuadPos, rightQuadPos, floorQuadPos;
	bool buttonA_pressed;
	bool rThumb_pressed, lThumb_pressed;
	bool buttonB_pressed;
	bool buttonX_pressed;
	int buttonX_status;
	float lThumbStick_val;
	mat4 cubeModelview;
	bool freeze;
	glm::mat4 projection, headPose[2];
	GLuint leftwireVAO, leftwireVBO;

	//variables for final projects
	bool rVibrate, lVibrate;
	double displayMidpointSeconds = 0.0;
	float rotateAngle = 0.0f;
	ovrPosef leftHandPose;
	ovrPosef rightHandPose;
	glm::vec3 lastLeftEndPoint, lastRightEndPoint;
	glm::vec3 startpos ;
	glm::vec3 displace = glm::vec3(0.0f,0.0f,0.0f);
	int maxEnemy = 5;
	CrystalSoldier *soldier1;
	vector<CrystalSoldier*> enemies;

	//client init
	rpc::client *c;
	float deviceId;
	float deviceType;
	bool needNextData = true;
	std::future<RPCLIB_MSGPACK::object_handle> data;
	vector<vector<float>> soldiersData;


	RiftApp() {
		using namespace ovr;
		RiftApp::buttonA_status = 0;
		RiftApp::buttonB_status = 0;
		RiftApp::rThumbButton_status = false;
		RiftApp::rThumbStick_val = 0;
		_viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;

		//Init client
		deviceType = 1.0f;
		//c = new rpc::client("192.168.1.24", 8080);
		c = new rpc::client("localhost", 8080);
		vector<vector<float>> rst = c->call("register", deviceType).as<vector<vector<float>>>();
		deviceId = rst[0][0];
		startpos = vec3(rst[1][0], rst[1][1], rst[1][2]);
		
		
		memset(&_sceneLayer, 0, sizeof(ovrLayerEyeFov));
		_sceneLayer.Header.Type = ovrLayerType_EyeFov;
		_sceneLayer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;

		ovr::for_each_eye([&](ovrEyeType eye) {
			if (eye == ovrEye_Left) {
				left_erd_var = _eyeRenderDescs[eye] = ovr_GetRenderDesc(_session, eye, _hmdDesc.DefaultEyeFov[eye]);
				left_default_erd = _eyeRenderDescs[eye] = ovr_GetRenderDesc(_session, eye, _hmdDesc.DefaultEyeFov[eye]);
			}
			else if (eye == ovrEye_Right) {
				right_erd_var = _eyeRenderDescs[eye] = ovr_GetRenderDesc(_session, eye, _hmdDesc.DefaultEyeFov[eye]);
				right_default_erd = _eyeRenderDescs[eye] = ovr_GetRenderDesc(_session, eye, _hmdDesc.DefaultEyeFov[eye]);
			}

			ovrEyeRenderDesc& erd = _eyeRenderDescs[eye] = ovr_GetRenderDesc(_session, eye, _hmdDesc.DefaultEyeFov[eye]);
		
			ovrMatrix4f ovrPerspectiveProjection =
				ovrMatrix4f_Projection(erd.Fov, 0.01f, 1000.0f, ovrProjection_ClipRangeOpenGL);
			_eyeProjections[eye] = ovr::toGlm(ovrPerspectiveProjection);
			_viewScaleDesc.HmdToEyeOffset[eye] = erd.HmdToEyeOffset; // cse190: adjust the eye separation here - need to use 3D vector from central point on Rift for each eye

			
			ovrFovPort & fov = _sceneLayer.Fov[eye] = _eyeRenderDescs[eye].Fov;
			auto eyeSize = ovr_GetFovTextureSize(_session, eye, fov, 1.0f);
			_sceneLayer.Viewport[eye].Size = eyeSize;
			_sceneLayer.Viewport[eye].Pos = { (int)_renderTargetSize.x, 0 };

			_renderTargetSize.y = std::max(_renderTargetSize.y, (uint32_t)eyeSize.h);
			_renderTargetSize.x += eyeSize.w;
		});
		// Make the on screen window 1/4 the resolution of the render target
		_mirrorSize = _renderTargetSize;
		_mirrorSize /= 4;
	}

protected:
	GLFWwindow * createRenderingTarget(uvec2 & outSize, ivec2 & outPosition) override {
		return glfw::createWindow(_mirrorSize);
	}

	void changePoints(std::vector<GLfloat> &pts, ovrVector3f pos) {
		for (int i = 0; i < 7; ++i) {
			pts[i * 6] = pos.x;
			pts[i * 6 + 1] = pos.y;
			pts[i * 6 + 2] = pos.z;
		}
	}

	void initGl() override {
		GlfwApp::initGl();
		
		// Disable the v-sync for buffer swap
		glfwSwapInterval(0);

		ovrTextureSwapChainDesc desc = {};
		desc.Type = ovrTexture_2D;
		desc.ArraySize = 1;
		desc.Width = _renderTargetSize.x;
		desc.Height = _renderTargetSize.y;
		desc.MipLevels = 1;
		desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.SampleCount = 1;
		desc.StaticImage = ovrFalse;
		ovrResult result = ovr_CreateTextureSwapChainGL(_session, &desc, &_eyeTexture);
		_sceneLayer.ColorTexture[0] = _eyeTexture;
		if (!OVR_SUCCESS(result)) {
			FAIL("Failed to create swap textures");
		}

		int length = 0;
		result = ovr_GetTextureSwapChainLength(_session, _eyeTexture, &length);
		if (!OVR_SUCCESS(result) || !length) {
			FAIL("Unable to count swap chain textures");
		}
		for (int i = 0; i < length; ++i) {
			GLuint chainTexId;
			ovr_GetTextureSwapChainBufferGL(_session, _eyeTexture, i, &chainTexId);
			glBindTexture(GL_TEXTURE_2D, chainTexId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		// Set up the framebuffer object
		glGenFramebuffers(1, &_fbo);
		glGenRenderbuffers(1, &_depthBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _renderTargetSize.x, _renderTargetSize.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		ovrMirrorTextureDesc mirrorDesc;
		memset(&mirrorDesc, 0, sizeof(mirrorDesc));
		mirrorDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
		mirrorDesc.Width = _mirrorSize.x;
		mirrorDesc.Height = _mirrorSize.y;
		if (!OVR_SUCCESS(ovr_CreateMirrorTextureGL(_session, &mirrorDesc, &_mirrorTexture))) {
			FAIL("Could not create mirror texture");
		}
		glGenFramebuffers(1, &_mirrorFbo);


		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		ovr_RecenterTrackingOrigin(_session);
		//Button signal initialization
		buttonA_pressed = false;
		rThumb_pressed = false;
		lThumb_pressed = false;
		buttonB_pressed = false;
		buttonX_pressed = false;
		buttonX_status = 0;
		lThumbStick_val = 1.0f;
		freeze = false;
		shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
		skyboxShader = LoadShaders(SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH);
		bulletShader = LoadShaders(BULLET_VERTEX_SHADER_PATH, BULLET_FRAGMENT_SHADER_PATH);
		crystalShader = LoadShaders(CRYSTAL_VERTEX_SHADER_PATH, CRYSTAL_FRAGMENT_SHADER_PATH);

		lastLeftEndPoint = vec3(-0.3f, 1.0f, 0.0f);
		lastRightEndPoint = vec3(0.3f, 1.0f, 0.0f);

		cube = new Cube();
		skybox = new Cube(0);
		bullet = new Sphere(0.1f, 100, 100, vec3(0.0, 0.0, -0.2f));
	

		soldier1 = new CrystalSoldier();
		for (int i = 0; i < maxEnemy; i++) {
			enemies.push_back(new CrystalSoldier());
		}

}

	void onKey(int key, int scancode, int action, int mods) override {
		if (GLFW_PRESS == action) switch (key) {
		case GLFW_KEY_R:
			ovr_RecenterTrackingOrigin(_session);
			return;
		}

		GlfwApp::onKey(key, scancode, action, mods);
	}

	bool checkCollision(vec3 armTop, vec3 initPos, vec3 displace) {

		vec3 curPos = initPos + displace;
	
		float minX = curPos.x - 0.25;
		float maxX = curPos.x + 0.25;
		float minY = curPos.y - 1.0f;
		float maxY = curPos.y;
		float maxZ = curPos.z + 0.25f;
		float minZ = curPos.z - 0.25f;
		
		return (armTop.x >= minX && armTop.x <= maxX) &&
			(armTop.y >= minY && armTop.y <= maxY) &&
			(armTop.z >= minZ && armTop.z <= maxZ);
		
	}

	

	glm::mat4 fromGLfloat16(GLfloat temp[]) {
		return glm::mat4(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7], temp[8], temp[9], temp[10], temp[11], temp[12], temp[13], temp[14], temp[15]);
	}

	void draw() final override {

		ovrPosef eyePoses[2];
		ovrInputState inputState;
		ovr_GetEyePoses(_session, frame, true, _viewScaleDesc.HmdToEyeOffset, eyePoses, &_sceneLayer.SensorSampleTime);
		/*
		if (OVR_SUCCESS(ovr_GetInputState(_session, ovrControllerType_Touch, &inputState))) {
			if (inputState.Thumbstick[ovrHand_Left].x != 0) {
				displace.x += inputState.Thumbstick[ovrHand_Left].x / 150.0f;
 			}
			if (inputState.Thumbstick[ovrHand_Left].y != 0) {
				displace.z -= inputState.Thumbstick[ovrHand_Left].y / 150.0f;
			}
		}*/
		int curIndex;
		ovr_GetTextureSwapChainCurrentIndex(_session, _eyeTexture, &curIndex);
		GLuint curTexId;
		ovr_GetTextureSwapChainBufferGL(_session, _eyeTexture, curIndex, &curTexId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		vec3 rTop, lTop;
		bool rVib = false;
		bool lVib = false;

		if (needNextData) {
			data = c->async_call("get", deviceId);
			needNextData = false;
		}
		if (needNextData == false && data._Is_ready()) {
			//cout << "get data" << endl;
			soldiersData = data.get().as<std::vector<vector<float>>>();
			needNextData = true;
		}

		ovr::for_each_eye([&](ovrEyeType eye) {
			const auto& vp = _sceneLayer.Viewport[eye];
			glViewport(vp.Pos.x, vp.Pos.y, vp.Size.w, vp.Size.h);
			_sceneLayer.RenderPose[eye] = eyePoses[eye];

			//Head Tracking
			Eigen::Quaternionf rt(eyePoses[eye].Orientation.w, eyePoses[eye].Orientation.x, eyePoses[eye].Orientation.y, eyePoses[eye].Orientation.z);
			Eigen::Vector3f euler = rt.toRotationMatrix().eulerAngles(0, 1, 2);
			glm::mat4 headRotate = glm::mat4(1.0f);
			headRotate *= glm::rotate(mat4(1.0f), euler[0], vec3(1.0f, 0.0f, 0.0f));
			headRotate *= glm::rotate(mat4(1.0f), euler[1], vec3(0.0f, 1.0f, 0.0f));
			headRotate *= glm::rotate(mat4(1.0f), euler[2], vec3(0.0f, 0.0f, 1.0f));
			glm::vec4 direc = glm::vec4(0.0f,0.0f,1.0f,0.0f);
			direc = headRotate * direc;
			glm::vec3 d3 = glm::vec3(direc.x, 0.0f, direc.z);
			d3 = glm::normalize(d3);
			GLfloat d = glm::acos(glm::dot(d3,glm::vec3(0.0f,0.0f,1.0f)));
			if (glm::dot(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f),d3), glm::vec3(0.0f, 1.0f, 0.0f)) < 0) {
				d =  - d;
			}

			if(eye == ovrEye_Left){
				if (OVR_SUCCESS(ovr_GetInputState(_session, ovrControllerType_Touch, &inputState))) {
					displace.x += inputState.Thumbstick[ovrHand_Left].x * glm::cos(d) * 0.01 - inputState.Thumbstick[ovrHand_Left].y * glm::sin(d) * 0.01;
					displace.z += -inputState.Thumbstick[ovrHand_Left].y * glm::cos(d) * 0.01 - inputState.Thumbstick[ovrHand_Left].x * glm::sin(d) * 0.01;
				}
			}
			
			//glm::mat4 v = glm::inverse(ovr::toGlm(eyePoses[eye]));
			//vec4 temp = yheadRotate * vec4(displace,0.0f);
			//displace = vec3(temp.x, temp.y, temp.z);

		
			glm::mat4 v = glm::inverse(glm::translate(glm::mat4(1.0f), startpos + vec3(0.0f, 0.0f, 0.0f) + displace) * headRotate);
			//glm::vec3 eyepos = glm::make_vec3(&eyePoses[eye].Position.x);
			//glm::mat4 t = glm::translate(glm::mat4(1.0f), startpos+displace);
			//glm::mat4 t = mat4(1.0f);

			//glm::mat4 leftarm = glm::translate(glm::mat4(1.0f), glm::vec3(-0.3, -0.25, 0.0));
			//glm::mat4 rightarm = glm::translate(glm::mat4(1.0f), glm::vec3(0.3, -0.25, 0.0));
			glm::mat4 cubePos = glm::scale(mat4(1.0f),vec3(0.2,0.2,0.2)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.3, 0.0, -1.0));
			

			//Hands Tracking
			ovrTrackingState trackState = ovr_GetTrackingState(_session, displayMidpointSeconds, ovrTrue);
			leftHandPose = trackState.HandPoses[ovrHand_Left].ThePose;
			rightHandPose = trackState.HandPoses[ovrHand_Right].ThePose;
			glm::vec3 leftEndPoint = glm::vec3(leftHandPose.Position.x, leftHandPose.Position.y, leftHandPose.Position.z);
			glm::vec3 rightEndPoint = glm::vec3(rightHandPose.Position.x, rightHandPose.Position.y, rightHandPose.Position.z);
			
			
			//Send data to server
			if (eye == ovrEye_Left) {

				std::vector<float> displaceVec;
				displaceVec.push_back(displace.x); displaceVec.push_back(displace.y); displaceVec.push_back(displace.z);
				std::vector<float> lArmPos;
				lArmPos.push_back(leftEndPoint.x); lArmPos.push_back(leftEndPoint.y); lArmPos.push_back(leftEndPoint.z);
				std::vector<float> rArmPos;
				rArmPos.push_back(rightEndPoint.x); rArmPos.push_back(rightEndPoint.y); rArmPos.push_back(rightEndPoint.z);

				c->async_call("set", deviceId, deviceType, displaceVec, lArmPos, rArmPos, d);
			}




			//Drawing
			skybox->draw(skyboxShader, glm::inverse(ovr::toGlm(eyePoses[eye])), _eyeProjections[eye]);
			//skybox->draw(skyboxShader, v, _eyeProjections[eye]);
			//bullet->draw(bulletShader, v, _eyeProjections[eye]);
			//cube->draw(shaderProgram, v * cubePos , _eyeProjections[eye]);
			soldier1->moveSoldier(startpos,displace);
			soldier1->rotateSoldier(d);
			soldier1->rotateArm(leftEndPoint, rightEndPoint, d);
			//soldier1->draw(shaderProgram, v, _eyeProjections[eye]);
			soldier1->draw(crystalShader, v, _eyeProjections[eye], false);
			if (eye == ovrEye_Left) {
				lTop = soldier1->getLArmTopPoint();
				rTop = soldier1->getRArmTopPoint();
			}



			
			for (int i = 0; i < soldiersData.size()/5; i++) {
				int n = 5 * i;
				vec3 initPos = vec3(soldiersData[n][0], soldiersData[n][1], soldiersData[n][2]);
				vec3 dis = vec3(soldiersData[n+1][0], soldiersData[n+1][1], soldiersData[n+1][2]);
				vec3 lArm = vec3(soldiersData[n+2][0], soldiersData[n+2][1], soldiersData[n+2][2]);
				vec3 rArm = vec3(soldiersData[n + 3][0], soldiersData[n + 3][1], soldiersData[n + 3][2]);
				float hAngle = soldiersData[n + 4][0];
				enemies[i]->moveSoldier(initPos,dis);
				enemies[i]->rotateArm(lArm,rArm,hAngle);
				enemies[i]->rotateSoldier(hAngle);
				enemies[i]->draw(crystalShader, v, _eyeProjections[eye], true);

				if (eye == ovrEye_Left) {
					rVib = rVib || checkCollision(soldier1->getRArmTopPoint(), initPos, dis);
					lVib = lVib || checkCollision(soldier1->getLArmTopPoint(), initPos, dis);
				}

			}
			
			//cout << " x: " << pos[0] << " y:" << pos[1] << " z: " << pos[2] << endl;

		});
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		ovr_CommitTextureSwapChain(_session, _eyeTexture);
		ovrLayerHeader* headerList = &_sceneLayer.Header;
		ovr_SubmitFrame(_session, frame, &_viewScaleDesc, &headerList, 1);

		GLuint mirrorTextureId;
		ovr_GetMirrorTextureBufferGL(_session, _mirrorTexture, &mirrorTextureId);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, _mirrorFbo);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTextureId, 0);
		glBlitFramebuffer(0, 0, _mirrorSize.x, _mirrorSize.y, 0, _mirrorSize.y, _mirrorSize.x, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		//cout << rVib << endl;
		
		if (lVib) {
			ovr_SetControllerVibration(_session, ovrControllerType_LTouch, 1.0f, 1.0f);
		}
		else {
			ovr_SetControllerVibration(_session, ovrControllerType_LTouch, 1.0f, 0.0f);
			
		}
		if (rVib) {
			ovr_SetControllerVibration(_session, ovrControllerType_RTouch, 1.0f, 1.0f);
		}
		else {
			ovr_SetControllerVibration(_session, ovrControllerType_RTouch, 1.0f, 0.0f);
		}
	}



};

int main(int argc, char** argv)
{
	int result = -1;
	try {
		if (!OVR_SUCCESS(ovr_Initialize(nullptr))) {
			FAIL("Failed to initialize the Oculus SDK");
		}
		result = RiftApp().run();
	}
	catch (std::exception & error) {
		OutputDebugStringA(error.what());
		std::cerr << error.what() << std::endl;
	}
	ovr_Shutdown();
	return result;
}