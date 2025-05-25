#ifndef GRAPHICS_ENGINE_H
#define GRAPHICS_ENGINE_H

// ===========================
// Backend Selection
// ===========================
#if defined(GRAPHICS_BACKEND_OPENGL)
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#elif defined(GRAPHICS_BACKEND_DX11)
#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#else
#error "Define one of GRAPHICS_BACKEND_OPENGL or GRAPHICS_BACKEND_DX11 before including this header."
#endif
//Uncomment the backend you want to use
//#define GRAPHICS_BACKEND_OPENGL
//#define GRAPHICS_BACKEND_DX11


//WIP GRAPHICS ENGINE
//Note: This is a simplified graphics engine that supports either OpenGL or DirectX 11. For this Development Library neither GLFW, directX or GLEW are setup so neither 'engine' will run.

namespace gfx
{

    class GraphicsEngine
    {
    public:
        GraphicsEngine();
        ~GraphicsEngine();

        bool Initialize(void* windowHandle, int width, int height);
        void BeginFrame(float r, float g, float b, float a);
        void EndFrame();
        void Cleanup();

    private:
#if defined(GRAPHICS_BACKEND_OPENGL)
        GLFWwindow* window;
#elif defined(GRAPHICS_BACKEND_DX11)
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* context = nullptr;
        IDXGISwapChain* swapChain = nullptr;
        ID3D11RenderTargetView* renderTargetView = nullptr;
#endif
    };

} // namespace gfx

// ===========================
// Implementation Inline
// ===========================
namespace gfx
{

    GraphicsEngine::GraphicsEngine() {}
    GraphicsEngine::~GraphicsEngine() { Cleanup(); }

    bool GraphicsEngine::Initialize(void* windowHandle, int width, int height)
    {
#if defined(GRAPHICS_BACKEND_DX11)
        DXGI_SWAP_CHAIN_DESC scd = {};
        scd.BufferCount = 1;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.OutputWindow = static_cast<HWND>(windowHandle);
        scd.SampleDesc.Count = 1;
        scd.Windowed = TRUE;

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
            nullptr, 0, D3D11_SDK_VERSION, &scd,
            &swapChain, &device, nullptr, &context
        );
        if (FAILED(hr)) return false;

        ID3D11Texture2D* backBuffer = nullptr;
        hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
        if (FAILED(hr)) return false;

        hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
        backBuffer->Release();
        if (FAILED(hr)) return false;

        context->OMSetRenderTargets(1, &renderTargetView, nullptr);
        return true;
#elif defined(GRAPHICS_BACKEND_OPENGL)
        if (!glfwInit()) return false;
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, "OpenGL Window", nullptr, nullptr);
        if (!window) return false;

        glfwMakeContextCurrent(window);
        if (glewInit() != GLEW_OK) return false;
        return true;
#endif
    }

    void GraphicsEngine::BeginFrame(float r, float g, float b, float a) //If we want to draw anything, we need to clear the screen first
    {
#if defined(GRAPHICS_BACKEND_DX11)
        float clearColor[4] = { r, g, b, a };
        context->ClearRenderTargetView(renderTargetView, clearColor);
#elif defined(GRAPHICS_BACKEND_OPENGL)
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);
#endif
    }

    void GraphicsEngine::EndFrame() //Present the frame to the screen (swap buffers)
    {
#if defined(GRAPHICS_BACKEND_DX11)
        swapChain->Present(1, 0);
#elif defined(GRAPHICS_BACKEND_OPENGL)
        glfwSwapBuffers(window);
        glfwPollEvents();
#endif
    }

    void GraphicsEngine::Cleanup()
    {
#if defined(GRAPHICS_BACKEND_DX11)
        if (renderTargetView) { renderTargetView->Release(); renderTargetView = nullptr; }
        if (swapChain) { swapChain->Release(); swapChain = nullptr; }
        if (context) { context->Release(); context = nullptr; }
        if (device) { device->Release(); device = nullptr; }
#elif defined(GRAPHICS_BACKEND_OPENGL)
        if (window) //Similar Process to DirextX but we only need to destroy the window
        {
            glfwDestroyWindow(window);
            glfwTerminate();
            window = nullptr;
        }
#endif
    }

} // namespace gfx

#endif // GRAPHICS_ENGINE_H

