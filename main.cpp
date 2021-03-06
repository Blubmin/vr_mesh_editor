/**
  \file minimalOpenGL/main.cpp
  \author Morgan McGuire, http://casual-effects.com
  Distributed with the G3D Innovation Engine http://casual-effects.com/g3d

  Features demonstrated:
   * Window, OpenGL, and extension initialization
   * Triangle mesh rendering (GL Vertex Array Buffer)
   * Texture map loading (GL Texture Object)
   * Shader loading (GL Program and Shader Objects)
   * Fast shader argument binding (GL Uniform Buffer Objects)
   * Offscreen rendering / render-to-texture (GL Framebuffer Object)
   * Ray tracing
   * Procedural texture
   * Tiny vector math library
   * Mouse and keyboard handling

  This is a minimal example of an OpenGL 4 program using only
  GLFW and GLEW libraries to simplify initialization. It does
  not depend on G3D or any other external libraries at all. 
  You could use SDL or another thin library instead of those two.
  If you want to use VR, this also requires the OpenVR library.
  (All dependencies are included with G3D)
  
  This is useful as a testbed when isolating driver bugs and 
  seeking a minimal context. 

  It is also helpful if you're new to computer graphics and wish to
  see the underlying hardware API without the high-level features that
  G3D provides.

  I chose OpenGL 4.1 because it is the newest OpenGL available on OS
  X, and thus the newest OpenGL that can be used across the major PC
  operating systems of Windows, Linux, OS X, and Steam.

  If you're interested in other minimal graphics code for convenience,
  also look at the stb libraries for single-header, dependency-free support
  for image loading, parsing, fonts, noise, etc.:
     https://github.com/nothings/stb

  And a SDL-based minimal OpenGL program at:
     https://gist.github.com/manpat/112f3f31c983ccddf044
  
  Reference Frames:
      Object: The object being rendered (the Shape in this example) relative to its own origin
      World:  Global reference frame
      Body:   Controlled by keyboard and mouse
      Head:   Controlled by tracking (or fixed relative to the body for non-VR)
      Camera: Fixed relative to the head. The camera is the eye.
 */

// Uncomment to add VR support
#define _VR

// To switch the box to a teapot, uncomment the following two lines
//#include "teapot.h"
//#define Shape Teapot

////////////////////////////////////////////////////////////////////////////////

#include <conio.h>
#include <exception>
#include <iostream>

#include "minimalOpenGL.h"

#include <engine_base\Program.h>
#include <engine_base\GLSL.h>
#include "Input.h"
#include "Mesh.h"
#include "matrix.h"

#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

using namespace glm;
using namespace std;

#ifdef _VR
#   include "minimalOpenVR.h"
#endif

GLFWwindow* window = nullptr;

#ifdef _VR
    vr::IVRSystem* hmd = nullptr;
#endif

#ifndef Shape
#   define Shape Cube
#endif


    
Program* prog;
vec3 lft, rht;
vec3 mesh_translate;
vec3 translate_pos;
float scale_dist;
bool is_translating = false;
bool is_extruding = false;
bool is_scaling = false;
float world_scale = .1f;
float height_offset = .5f;

Vector4 ConvertSteamToPosition(const vr::HmdMatrix34_t &matPose) {
	Matrix4x4 matrixObj(
		matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
		matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
		matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
		matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
	);
	return Vector4(matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1);
}

Matrix4x4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose) {
	Matrix4x4 matrixObj(
		matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
		matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
		matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
		matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
	);
	return matrixObj;
}


void run() {
	uint32_t framebufferWidth = 1280, framebufferHeight = 720;
#   ifdef _VR
	const int numEyes = 2;
	hmd = initOpenVR(framebufferWidth, framebufferHeight);
	assert(hmd);
#   else
	const int numEyes = 1;
#   endif

	const int windowHeight = 720;
	const int windowWidth = (framebufferWidth * windowHeight) / framebufferHeight;

	window = initOpenGL(windowWidth, windowHeight, "minimalOpenGL");

	Vector3 bodyTranslation(0.0f, 1.6f, 5.0f);
	Vector3 bodyRotation;

    //////////////////////////////////////////////////////////////////////
    // Allocate the frame buffer. This code allocates one framebuffer per eye.
    // That requires more GPU memory, but is useful when performing temporal 
    // filtering or making render calls that can target both simultaneously.

    GLuint framebuffer[numEyes];
    glGenFramebuffers(numEyes, framebuffer);

    GLuint colorRenderTarget[numEyes], depthRenderTarget[numEyes];
    glGenTextures(numEyes, colorRenderTarget);
    glGenTextures(numEyes, depthRenderTarget);
    for (int eye = 0; eye < numEyes; ++eye) {
        glBindTexture(GL_TEXTURE_2D, colorRenderTarget[eye]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, framebufferWidth, framebufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glBindTexture(GL_TEXTURE_2D, depthRenderTarget[eye]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, framebufferWidth, framebufferHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[eye]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorRenderTarget[eye], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_TEXTURE_2D, depthRenderTarget[eye], 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /////////////////////////////////////////////////////////////////
    // Load vertex array buffers

    lft = vec3(0, 2, -2);
    rht = vec3(0, 2, -1);
    mesh_translate = vec3(0, 1.0f, 5.0f);

	prog = new Program("flat.vert", "flat.frag");
	Mesh box = Mesh();
	Mesh sphere = Mesh("../assets/sphere.obj");

    /////////////////////////////////////////////////////////////////////
    // Create the main shader
    const GLuint shader = createShaderProgram(loadTextFile("../min.vrt"), loadTextFile("../min.pix"));

    // Binding points for attributes and uniforms discovered from the shader
    const GLint positionAttribute   = glGetAttribLocation(shader,  "position");
    const GLint normalAttribute     = glGetAttribLocation(shader,  "normal");
    const GLint texCoordAttribute   = glGetAttribLocation(shader,  "texCoord");
    const GLint tangentAttribute    = glGetAttribLocation(shader,  "tangent");    
    const GLint colorTextureUniform = glGetUniformLocation(shader, "colorTexture");

    const GLuint uniformBlockIndex = glGetUniformBlockIndex(shader, "Uniform");
    const GLuint uniformBindingPoint = 1;
    glUniformBlockBinding(shader, uniformBlockIndex, uniformBindingPoint);

    GLuint uniformBlock;
    glGenBuffers(1, &uniformBlock);

    {
        // Allocate space for the uniform block buffer
        GLint uniformBlockSize;
        glGetActiveUniformBlockiv(shader, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);
        glBindBuffer(GL_UNIFORM_BUFFER, uniformBlock);
        glBufferData(GL_UNIFORM_BUFFER, uniformBlockSize, nullptr, GL_DYNAMIC_DRAW);
    }

    const GLchar* uniformName[] = {
        "Uniform.objectToWorldNormalMatrix",
        "Uniform.objectToWorldMatrix",
        "Uniform.modelViewProjectionMatrix",
        "Uniform.light",
        "Uniform.cameraPosition"};

    const int numBlockUniforms = sizeof(uniformName) / sizeof(uniformName[0]);
#   ifdef _DEBUG
    {
        GLint debugNumUniforms = 0;
        glGetProgramiv(shader, GL_ACTIVE_UNIFORMS, &debugNumUniforms);
        for (GLint i = 0; i < debugNumUniforms; ++i) {
            GLchar name[1024];
            GLsizei size = 0;
            GLenum type = GL_NONE;
            glGetActiveUniform(shader, i, sizeof(name), nullptr, &size, &type, name);
            std::cout << "Uniform #" << i << ": " << name << "\n";
        }
        assert(debugNumUniforms >= numBlockUniforms);
    }
#   endif

    // Map uniform names to indices within the block
    GLuint uniformIndex[numBlockUniforms];
    glGetUniformIndices(shader, numBlockUniforms, uniformName, uniformIndex);
    assert(uniformIndex[0] < 10000);

    // Map indices to byte offsets
    GLint  uniformOffset[numBlockUniforms];
    glGetActiveUniformsiv(shader, numBlockUniforms, uniformIndex, GL_UNIFORM_OFFSET, uniformOffset);
    assert(uniformOffset[0] >= 0);

    // Load a texture map
    GLuint colorTexture = GL_NONE;
    {
        int textureWidth, textureHeight, channels;
        std::vector<std::uint8_t> data;
        loadBMP("../color.bmp", textureWidth, textureHeight, channels, data);

        glGenTextures(1, &colorTexture);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, textureWidth, textureHeight, 0, (channels == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

	// Create the main shader

    GLuint trilinearSampler = GL_NONE;
    {
        glGenSamplers(1, &trilinearSampler);
        glSamplerParameteri(trilinearSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(trilinearSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(trilinearSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(trilinearSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

#   ifdef _VR
        vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
#   endif

    // Main loop:
    int timer = 0;
    while (!glfwWindowShouldClose(window)) {
        GLSL::check_gl_error("Loop");
        assert(glGetError() == GL_NONE);

        const float nearPlaneZ = -0.1f;
        const float farPlaneZ = -100.0f;
        const float verticalFieldOfView = 45.0f * PI / 180.0f;

        Matrix4x4 eyeToHead[numEyes], projectionMatrix[numEyes], headToBodyMatrix;
#       ifdef _VR
        getEyeTransformations(hmd, trackedDevicePose, nearPlaneZ, farPlaneZ, headToBodyMatrix.data, eyeToHead[0].data, eyeToHead[1].data, projectionMatrix[0].data, projectionMatrix[1].data);
#       else
        projectionMatrix[0] = Matrix4x4::perspective(float(framebufferWidth), float(framebufferHeight), nearPlaneZ, farPlaneZ, verticalFieldOfView);
#       endif

        // printf("float nearPlaneZ = %f, farPlaneZ = %f; int width = %d, height = %d;\n", nearPlaneZ, farPlaneZ, framebufferWidth, framebufferHeight);

        const Matrix4x4& bodyToWorldMatrix =
            Matrix4x4::translate(bodyTranslation) *
            Matrix4x4::roll(bodyRotation.z) *
            Matrix4x4::yaw(bodyRotation.y) *
            Matrix4x4::pitch(bodyRotation.x);

        const Matrix4x4& headToWorldMatrix = bodyToWorldMatrix * headToBodyMatrix;

        for (int eye = 0; eye < numEyes; ++eye) {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[eye]);
            glViewport(0, 0, framebufferWidth, framebufferHeight);

            glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            const Matrix4x4& objectToWorldMatrix = Matrix4x4::translate(mesh_translate.x,
                mesh_translate.y,
                mesh_translate.z);
            const Matrix3x3& objectToWorldNormalMatrix = Matrix3x3(objectToWorldMatrix).transpose().inverse();
            const Matrix4x4& cameraToWorldMatrix = headToWorldMatrix * eyeToHead[eye];

            const Vector3& light = Vector3(1.0f, 0.5f, 0.2f).normalize();

            // Draw the background
            // drawSky(framebufferWidth, framebufferHeight, nearPlaneZ, farPlaneZ, cameraToWorldMatrix.data, projectionMatrix[eye].inverse().data, &light.x);

            ////////////////////////////////////////////////////////////////////////
            // Draw a mesh
            glEnable(GL_DEPTH_TEST);
            //glDepthFunc(GL_LESS);
            //glEnable(GL_CULL_FACE);
            //glDepthMask(GL_TRUE);
            //glLineWidth(2.0);
            //glPointSize(2);

            glUseProgram(prog->prog);

            //// in position
            //glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
            //glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
            //glEnableVertexAttribArray(positionAttribute);

            //// in normal
            //glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
            //glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
            //glEnableVertexAttribArray(normalAttribute);

            //// in tangent
            //if (tangentAttribute != -1) {
            //    // Only bind if used
            //    glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
            //    glVertexAttribPointer(tangentAttribute, 4, GL_FLOAT, GL_FALSE, 0, 0);
            //    glEnableVertexAttribArray(tangentAttribute);
            //}

            //// in texCoord 
            //glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
            //glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
            //glEnableVertexAttribArray(texCoordAttribute);

            //// indexBuffer
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

            // uniform colorTexture (samplers cannot be placed in blocks)
            /*const GLint colorTextureUnit = 0;
            glActiveTexture(GL_TEXTURE0 + colorTextureUnit);
            glBindTexture(GL_TEXTURE_2D, colorTexture);
            glBindSampler(colorTextureUnit, trilinearSampler);
            glUniform1i(colorTextureUniform, colorTextureUnit);*/


            glUniform3f(prog->getUniformHandle("uDirLight"), mesh_translate.x - bodyTranslation.x, 
                                                             mesh_translate.y - bodyTranslation.y, 
                                                             mesh_translate.z - bodyTranslation.z);

            // Other uniforms in the interface block
            {
                //glBindBufferBase(GL_UNIFORM_BUFFER, uniformBindingPoint, uniformBlock);

                //GLubyte* ptr = (GLubyte*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
                // mat3 is passed to openGL as if it was mat4 due to padding rules.
                /*for (int row = 0; row < 3; ++row) {
                    memcpy(ptr + uniformOffset[0] + sizeof(float) * 4 * row, objectToWorldNormalMatrix.data + row * 3, sizeof(float) * 3);
                }*/

                //memcpy(ptr + uniformOffset[1], objectToWorldMatrix.data, sizeof(objectToWorldMatrix));

                //const Matrix4x4& modelViewProjectionMatrix = projectionMatrix[eye] * cameraToWorldMatrix.inverse() * objectToWorldMatrix;
                //memcpy(ptr + uniformOffset[2], modelViewProjectionMatrix.data, sizeof(modelViewProjectionMatrix));
                //memcpy(ptr + uniformOffset[3], &light.x, sizeof(light));
                //const Vector4& cameraPosition = cameraToWorldMatrix.col(3);
                //memcpy(ptr + uniformOffset[4], &cameraPosition.x, sizeof(Vector3));
                //glUnmapBuffer(GL_UNIFORM_BUFFER);
            }
            glm::mat4 model_transform;
            glm::translate(model_transform, glm::vec3(0, 5, -10));
            const Matrix4x4& modelViewProjectionMatrix = projectionMatrix[eye] * cameraToWorldMatrix.inverse() * objectToWorldMatrix;
            glUniformMatrix4fv(prog->getUniformHandle("uMVP"), 1, GL_TRUE, modelViewProjectionMatrix.data);
            glUniformMatrix4fv(prog->getUniformHandle("uModelMatrix"), 1, GL_FALSE, objectToWorldMatrix.data);
            //glUniformMatrix4fv(prog->getUniformHandle("uViewMatrix"), 1, GL_FALSE, cameraToWorldMatrix.data);

            glBindVertexArray(box.vao());
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glUniform3f(prog->getUniformHandle("uDiffuseColor"), .8, .8, .8);
            box.draw_tris();
            glUniform3f(prog->getUniformHandle("uDiffuseColor"), .5, .5, .5);
            //glLineWidth(1.1);
            box.draw_edges();
            glUniform3f(prog->getUniformHandle("uDiffuseColor"), .4, .4, .4);
            glPointSize(5);
            box.draw_verts();
            glBindVertexArray(0);


            const Matrix4x4& objectToWorldMatrixLft = Matrix4x4::translate(lft.x, lft.y, lft.z) * Matrix4x4::scale(world_scale * .5, world_scale * .5, world_scale * .5);
            const Matrix4x4& modelViewProjectionMatrixLft = projectionMatrix[eye] * cameraToWorldMatrix.inverse() * objectToWorldMatrixLft;

            const Matrix4x4& objectToWorldMatrixRht = Matrix4x4::translate(rht.x, rht.y, rht.z) * Matrix4x4::scale(world_scale * .5, world_scale * .5, world_scale * .5);
            const Matrix4x4& modelViewProjectionMatrixRht = projectionMatrix[eye] * cameraToWorldMatrix.inverse() * objectToWorldMatrixRht;

            glBindVertexArray(sphere.vao());
            glUniform3f(prog->getUniformHandle("uDiffuseColor"), 0, .3, .8);

            glUniformMatrix4fv(prog->getUniformHandle("uMVP"), 1, GL_TRUE, modelViewProjectionMatrixLft.data);
            glUniformMatrix4fv(prog->getUniformHandle("uModelMatrix"), 1, GL_FALSE, objectToWorldMatrixLft.data);
            sphere.draw_tris();

            glUniformMatrix4fv(prog->getUniformHandle("uMVP"), 1, GL_TRUE, modelViewProjectionMatrixRht.data);
            glUniformMatrix4fv(prog->getUniformHandle("uModelMatrix"), 1, GL_FALSE, objectToWorldMatrixRht.data);
            sphere.draw_tris();

            glBindVertexArray(0);

            glUseProgram(0);
#           ifdef _VR
            {
                const vr::Texture_t tex = { reinterpret_cast<void*>(intptr_t(colorRenderTarget[eye])), vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
                vr::VRCompositor()->Submit(vr::EVREye(eye), &tex);
            }
#           endif
        } // for each eye

        ////////////////////////////////////////////////////////////////////////
#       ifdef _VR
            // Tell the compositor to begin work immediately instead of waiting for the next WaitGetPoses() call
        vr::VRCompositor()->PostPresentHandoff();
#       endif

        // Mirror to the window
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, GL_NONE);

        // Display what has been drawn on the main window
        glfwSwapBuffers(window);

        // Check for events
        Input::clear();
        glfwPollEvents();

        // Handle events
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }

        // WASD keyboard movement
        const float cameraMoveSpeed = 0.01f;
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W)) { bodyTranslation += Vector3(headToWorldMatrix * Vector4(0, 0, -cameraMoveSpeed, 0)); }
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S)) { bodyTranslation += Vector3(headToWorldMatrix * Vector4(0, 0, +cameraMoveSpeed, 0)); }
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A)) { bodyTranslation += Vector3(headToWorldMatrix * Vector4(-cameraMoveSpeed, 0, 0, 0)); }
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D)) { bodyTranslation += Vector3(headToWorldMatrix * Vector4(+cameraMoveSpeed, 0, 0, 0)); }
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_C)) { bodyTranslation.y -= cameraMoveSpeed; }
        if ((GLFW_PRESS == glfwGetKey(window, GLFW_KEY_SPACE)) || (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_Z))) { bodyTranslation.y += cameraMoveSpeed; }

#ifndef _VR

        if (Input::key_pressed_down(GLFW_KEY_UP)) { rht += vec3(1, 0, 0) * cameraMoveSpeed * 0.5f; }
        if (Input::key_pressed_down(GLFW_KEY_DOWN)) { rht -= vec3(1, 0, 0) * cameraMoveSpeed * 0.5f; }
        if (Input::key_pressed_down(GLFW_KEY_LEFT)) { rht += vec3(0, 0, 1) * cameraMoveSpeed * 0.5f; }
        if (Input::key_pressed_down(GLFW_KEY_RIGHT)) { rht -= vec3(0, 0, 1) * cameraMoveSpeed * 0.5f; }
        if (Input::key_pressed_down(GLFW_KEY_PAGE_UP)) { rht += vec3(0, 1, 0) * cameraMoveSpeed * 0.5f; }
        if (Input::key_pressed_down(GLFW_KEY_PAGE_DOWN)) { rht -= vec3(0, 1, 0) * cameraMoveSpeed * 0.5f; }
        if (Input::button_pressed_down(GLFW_MOUSE_BUTTON_RIGHT)) { box.select(rht - mesh_translate, world_scale); }
        if (Input::key_pressed_down(GLFW_KEY_DELETE)) { box.deselect(); }

        if (Input::key_pressed_down(GLFW_KEY_RIGHT_SHIFT)) { 
            if (!is_translating) {
                translate_pos = rht;
                is_translating = true;
            }
            else {
                box.translate_selected(rht - translate_pos);
                translate_pos = rht;
            }
        }
        if (Input::key_released(GLFW_KEY_RIGHT_SHIFT)) { is_translating = false; }

        if (Input::key_pressed_down(GLFW_KEY_ENTER)) {
            if (!is_extruding) {
                box.extrude();
                translate_pos = rht;
                is_extruding = true;
        }
            else {
                box.translate_selected(rht - translate_pos);
                translate_pos = rht;
            }
        }
        if (Input::key_released(GLFW_KEY_ENTER)) { is_extruding = false; }

        if (Input::key_pressed_down(GLFW_KEY_RIGHT_CONTROL)) {
            if (!is_scaling) {
                scale_dist = glm::distance(lft, rht);
                is_scaling = true;
            }
            else {
                box.scale_selected((glm::distance(lft, rht) / scale_dist) - 1);
                scale_dist = glm::distance(lft, rht);
            }
        }
        if (Input::key_released(GLFW_KEY_RIGHT_CONTROL)) { is_scaling = false; }

#else
        int count = 0;
        bool lft_a, lft_grip, lft_trigger, lft_touch;
        bool rht_a, rht_grip, rht_trigger, rht_touch;

        for (int d = 0; d < vr::k_unMaxTrackedDeviceCount; ++d) {
            if (!trackedDevicePose[d].bPoseIsValid) continue;
            if (hmd->GetTrackedDeviceClass(d) != vr::TrackedDeviceClass_Controller) continue;
			Vector4 pos = ConvertSteamToPosition(trackedDevicePose[d].mDeviceToAbsoluteTracking);

            vr::VRControllerState_t state;
            hmd->GetControllerState(d, &state, sizeof(vr::VRControllerState_t));
            uint64 a = vr::ButtonMaskFromId(vr::k_EButton_A);
            uint64 grip = vr::ButtonMaskFromId(vr::k_EButton_Grip);
            uint64 trigger = vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger);
            uint64 touch = vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad);

            if (!count++) {
                lft = vec3(pos.x, pos.y + height_offset, pos.z) + mesh_translate;
                lft_a = (state.ulButtonPressed & a);
                lft_grip = (state.ulButtonPressed & grip);
                lft_trigger = (state.ulButtonPressed & trigger);
                lft_touch = (state.ulButtonTouched & touch);
            }
            else {
                rht = vec3(pos.x, pos.y + height_offset, pos.z) + mesh_translate;
                rht_a = (state.ulButtonPressed & a);
                rht_grip = (state.ulButtonPressed & grip);
                rht_trigger = (state.ulButtonPressed & trigger);
                rht_touch = (state.ulButtonTouched & touch);
            }
        }

        bool a = lft_a || rht_a;
        bool grip = lft_grip || rht_grip;
		bool touch = true;
        bool trigger = lft_trigger || rht_trigger;

        if (touch && grip) { 
			box.select(rht - mesh_translate, world_scale * .5);
			box.select(lft - mesh_translate, world_scale * .5);
		}
        if (a) { box.deselect(); }

        if (touch && trigger) {
            if (!is_translating) {
                translate_pos = rht;
                is_translating = true;
            }
            else {
                box.translate_selected(rht - translate_pos);
                translate_pos = rht;
            }
        }
        if (is_translating && !(touch && trigger)) {
            is_translating = false;
        }

        if (touch && trigger && grip) {
            if (!is_extruding) {
                box.extrude();
                translate_pos = rht;
                is_extruding = true;
            }
            else {
                box.translate_selected(rht - translate_pos);
                translate_pos = rht;
            }
        }
        if (is_extruding && !(touch && trigger && grip)) {
            is_extruding = false;
        }

        if (lft_trigger && rht_trigger) {
            if (!is_scaling) {
                scale_dist = glm::distance(lft, rht);
                is_scaling = true;
            }
            else {
                box.scale_selected((glm::distance(lft, rht) / scale_dist) - 1);
                scale_dist = glm::distance(lft, rht);
            }
        }
        if (is_scaling && !(lft_trigger && rht_trigger)) { 
            is_scaling = false; 
        }
#endif // !_VR

        box.bind_geom();

        // Keep the camera above the ground
        if (bodyTranslation.y < 0.01f) { bodyTranslation.y = 0.01f; }

        static bool inDrag = false;
        const float cameraTurnSpeed = 0.005f;
        if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
            static double startX, startY;
            double currentX, currentY;

            glfwGetCursorPos(window, &currentX, &currentY);
            if (inDrag) {
                bodyRotation.y -= float(currentX - startX) * cameraTurnSpeed;
                bodyRotation.x -= float(currentY - startY) * cameraTurnSpeed;
            }
            inDrag = true; startX = currentX; startY = currentY;
        } else {
            inDrag = false;
        }

        ++timer;
    }

#   ifdef _VR
        if (hmd != nullptr) {
            vr::VR_Shutdown();
        }
#   endif

    // Close the GL context and release all resources
    glfwTerminate();

    return;
}

int main(int argc, char** argv)
{
	std::cout << "Minimal OpenGL 4.1 Example by Morgan McGuire\n\nW, A, S, D, C, Z keys to translate\nMouse click and drag to rotate\nESC to quit\n\n";
	std::cout << std::fixed;
	try
	{
		run();
	}
	catch (const exception& e)
	{
		char temp;
		cerr << e.what() << endl;
		cerr << "Press anything to quit..." << endl;
		_getch();
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

#ifdef _WINDOWS
    int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw) {
        return main(0, nullptr);
    }
#endif