#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <Windows.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace SimpleRenderer
{
#define MINT_LOG_ERROR(content) { std::cout << content; ::DebugBreak(); }
#define MINT_ASSERT(condition, content) if (!(condition)) { MINT_LOG_ERROR(content); }

#pragma region Aliases
    using Microsoft::WRL::ComPtr;
    using int8 = int8_t;
    using uint8 = uint8_t;
    using int16 = int16_t;
    using uint16 = uint16_t;
    using int32 = int32_t;
    using uint32 = uint32_t;
    using int64 = int64_t;
    using uint64 = uint64_t;
#pragma endregion

#pragma region Forward Declaration
    class Renderer;
    struct Shader;
#pragma endregion

    struct float2
    {
        constexpr float2() : float2(0, 0) { __noop; }
        constexpr float2(float x_, float y_) : x{ x_ }, y{ y_ } { __noop; }
        float& operator[](const uint32 index) { return f[index]; }
        const float& operator[](const uint32 index) const { return f[index]; }
        union { struct { float x; float y; }; float f[2]; };
    };
    struct float4
    {
        constexpr float4() : float4(0, 0, 0, 0) { __noop; }
        constexpr float4(float x_, float y_, float z_, float w_) : x{ x_ }, y{ y_ }, z{ z_ }, w{ w_ } { __noop; }
        float& operator[](const uint32 index) { return f[index]; }
        const float& operator[](const uint32 index) const { return f[index]; }
        float4 operator+() const { return *this; }
        float4 operator-() const { return float4(-x, -y, -z, -w); }
        float4 operator+(const float4& rhs) const { return float4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.z); }
        float4 operator-(const float4& rhs) const { return *this + (-rhs); }
        float4 operator*(const float s) const { return float4(x * s, y * s, z * s, w * s); }
        float4 operator/(const float s) const { return float4(x / s, y / s, z / s, w / s); }
        float4& operator+=(const float4& rhs) { *this = (*this + rhs); return *this; }
        float4& operator-=(const float4& rhs) { *this = (*this - rhs); return *this; }
        float4& operator*=(const float s) { *this = (*this * s); return *this; }
        float4& operator/=(const float s) { *this = (*this / s); return *this; }
        constexpr float dot(const float4& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w; }
        constexpr float lengthSq() const { return dot(*this); }
        float length() const { return ::sqrt(lengthSq()); }
        union { struct { float x; float y; float z; float w; }; float f[4]; };
    };
    struct float4x4
    {
        constexpr float4x4() : float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) { __noop; }
        constexpr float4x4(const float4(&rows)[4]) : _rows{ rows[0], rows[1], rows[2], rows[3] } { __noop; }
        constexpr float4x4(float _11_, float _12_, float _13_, float _14_, float _21_, float _22_, float _23_, float _24_, float _31_, float _32_, float _33_, float _34_, float _41_, float _42_, float _43_, float _44_) : _m{ _11_, _12_, _13_, _14_, _21_, _22_, _23_, _24_, _31_, _32_, _33_, _34_, _41_, _42_, _43_, _44_ } { __noop; }
        constexpr float4x4(const float(&m)[16]) : _m{ m[0],m[1],m[2],m[3],m[4],m[5],m[6],m[7],m[8],m[9],m[10],m[11],m[12],m[13],m[14],m[15] } { __noop; }
        void set(float _11_, float _12_, float _13_, float _14_, float _21_, float _22_, float _23_, float _24_, float _31_, float _32_, float _33_, float _34_, float _41_, float _42_, float _43_, float _44_)
        {
            _11 = _11_; _12 = _12_; _13 = _13_; _14 = _14_; _21 = _11_; _22 = _12_; _23 = _13_; _24 = _14_; _31 = _11_; _32 = _12_; _33 = _13_; _34 = _14_; _41 = _11_; _42 = _12_; _43 = _13_; _44 = _14_;
        }
        union {
            float _m[16]; float4 _rows[4];
            struct {
                float _11; float _12; float _13; float _14;
                float _21; float _22; float _23; float _24;
                float _31; float _32; float _33; float _34;
                float _41; float _42; float _43; float _44;
            };
        };
        void makeIdentity()
        {
            _11 = 1; _12 = 0; _13 = 0; _14 = 0; _21 = 0; _22 = 1; _23 = 0; _24 = 0; _31 = 0; _32 = 0; _33 = 1; _34 = 0; _41 = 0; _42 = 0; _43 = 0; _44 = 1;
        }
        void makePixelCoordinatesProjectionMatrix(const float2& screenSize)
        {
            makeIdentity();
            _11 = 2.0f / screenSize.x; _14 = -1.0f; _22 = -2.0f / screenSize.y; _24 = 1.0f;
        }
    };
    using Color = float4;

    enum class ShaderType
    {
        VertexShader,
        PixelShader,
    };
    enum class ResourceType
    {
        VertexBuffer,
        IndexBuffer,
        ConstantBuffer,
        //StructuredBuffer,
        //Teture2D,
    };

    struct ShaderHeader : public ID3DInclude
    {
    public:
        ShaderHeader() = default;
        virtual ~ShaderHeader() = default;

    public:
        virtual HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override final;
        virtual HRESULT Close(LPCVOID pData) override final { return S_OK; }

    public:
        std::string _headerName;
        std::string _headerContent;
    };
    struct InputElement
    {
        InputElement() = default;
        uint32 _byteOffset = 0;
        DXGI_FORMAT _format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        uint32 _inputSlot = 0;
        D3D11_INPUT_CLASSIFICATION _inputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
        std::string _semanticName;
        uint32 _semanticIndex = 0;
        uint32 _instanceStepRate = 0;
    };
    struct Shader
    {
        bool create(Renderer& renderer, const char* sourceCode, const ShaderType& shaderType, const char* shaderIdentifier, const char* entryPoint, const char* target, ShaderHeader* const shaderHeader = nullptr);
        bool createInputLayout(Renderer& renderer);

        ShaderType _type = ShaderType::VertexShader;
        std::vector<InputElement> _inputElements;
        ComPtr<ID3D11InputLayout> _inputLayout;

        ComPtr<ID3D10Blob> _shaderBlob;
        ComPtr<ID3D10Blob> _errorMessageBlob;
        ComPtr<ID3D11DeviceChild> _shader;
    };

    // Buffer or Texture
    class Resource
    {
    public:
        static constexpr DXGI_FORMAT kIndexBufferFormat = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;

    public:
        Resource() : _type{ ResourceType::VertexBuffer }, _byteSize{ 0 }, _elementStride{ 0 }, _elementMaxCount{ 0 } { __noop; }
        ~Resource() = default;

    public:
        bool create(Renderer& renderer, const ResourceType& type, const void* const content, const uint32 elementStride, const uint32 elementCount);
        bool update(Renderer& renderer, const void* const content, const uint32 elementStride, const uint32 elementCount);

    public:
        ID3D11Resource* getResource() const { return _resource.Get(); }

    public:
        ResourceType _type;
        uint32 _byteSize;
        uint32 _elementStride;
        uint32 _elementMaxCount;

    private:
        ComPtr<ID3D11Resource> _resource;
        ComPtr<ID3D11View> _view; // Only used for Texture and StructuredBuffer
    };

    class Renderer final
    {
    public:
        Renderer(const float2& windowSize, const Color& clearColor) : _hInstance{ nullptr }, _hWnd{ nullptr }, _windowSize{ windowSize }, _clearColor{ clearColor } { if (createWindow()) createDevice(); }
        ~Renderer() { destroyWindow(); }

    public:
        bool isRunning();
        bool beginRendering();
        void endRendering();
        void draw(const uint32 vertexCount)
        {
            _deviceContext->Draw(vertexCount, 0);
        }

    public:
        void bindShader(Shader& shader)
        {
            if (shader._type == ShaderType::VertexShader)
            {
                _deviceContext->VSSetShader(static_cast<ID3D11VertexShader*>(shader._shader.Get()), nullptr, 0);

                if (shader._inputLayout.Get() != nullptr)
                    _deviceContext->IASetInputLayout(shader._inputLayout.Get());
            }
            else if (shader._type == ShaderType::PixelShader)
            {
                _deviceContext->PSSetShader(static_cast<ID3D11PixelShader*>(shader._shader.Get()), nullptr, 0);
            }
        }
        void bindResource(Resource& resource, const uint32 slot);
        void bindResource(const ShaderType shaderType, Resource& resource, const uint32 slot);
        void useTrianglePrimitive();

    public:
        ID3D11Device* getDevice() const { return _device.Get(); }
        ID3D11DeviceContext* getDeviceContext() const { return _deviceContext.Get(); }

    private:
        bool createWindow();
        void destroyWindow();
        void createDevice();

    private:
        HINSTANCE _hInstance;
        HWND _hWnd;
        float2 _windowSize;
        Color _clearColor;

    private:
        ComPtr<IDXGISwapChain> _swapChain;
        ComPtr<ID3D11Device> _device;
        ComPtr<ID3D11DeviceContext> _deviceContext;
        ComPtr<ID3D11RenderTargetView> _backBufferRtv;
        ComPtr<ID3D11Texture2D> _depthStencilResource;
        ComPtr<ID3D11DepthStencilView> _depthStencilView;
    };


#pragma region Function Definitions
    HRESULT ShaderHeader::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
    {
        pFileName = _headerName.c_str();
        *ppData = _headerContent.c_str();
        *pBytes = static_cast<UINT>(_headerContent.length());
        return S_OK;
    }

    bool Shader::create(Renderer& renderer, const char* sourceCode, const ShaderType& shaderType, const char* shaderIdentifier, const char* entryPoint, const char* target, ShaderHeader* const shaderHeader)
    {
        if (sourceCode == nullptr)
        {
            MINT_LOG_ERROR("Must exist source code!");
            return false;
        }

        if (entryPoint == nullptr)
        {
            MINT_LOG_ERROR("Must specify entry point!");
            return false;
        }

        if (target == nullptr)
        {
            MINT_LOG_ERROR("Must specify target!");
            return false;
        }

        _type = shaderType;

        const UINT debugFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
        HRESULT result = D3DCompile(sourceCode, ::strlen(sourceCode), shaderIdentifier, nullptr, shaderHeader, entryPoint, target, debugFlag, 0, _shaderBlob.ReleaseAndGetAddressOf(), _errorMessageBlob.ReleaseAndGetAddressOf());
        if (FAILED(result))
        {
            std::string errorMessages(reinterpret_cast<char*>(_errorMessageBlob->GetBufferPointer()));
            MINT_LOG_ERROR("Shader compile failed.");
            return false;
        }

        if (shaderType == ShaderType::VertexShader)
        {
            if (FAILED(renderer.getDevice()->CreateVertexShader(_shaderBlob->GetBufferPointer(), _shaderBlob->GetBufferSize(), NULL, reinterpret_cast<ID3D11VertexShader**>(_shader.ReleaseAndGetAddressOf()))))
            {
                return false;
            }
            return true;
        }
        else if (shaderType == ShaderType::PixelShader)
        {
            if (FAILED(renderer.getDevice()->CreatePixelShader(_shaderBlob->GetBufferPointer(), _shaderBlob->GetBufferSize(), NULL, reinterpret_cast<ID3D11PixelShader**>(_shader.ReleaseAndGetAddressOf()))))
            {
                return false;
            }
            return true;
        }
        return false;
    }

    bool Shader::createInputLayout(Renderer& renderer)
    {
        std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs;
        inputElementDescs.resize(_inputElements.size());
        for (uint32 index = 0; index < inputElementDescs.size(); ++index)
        {
            inputElementDescs[index].AlignedByteOffset = _inputElements[index]._byteOffset;
            inputElementDescs[index].Format = _inputElements[index]._format;
            inputElementDescs[index].InputSlot = _inputElements[index]._inputSlot;
            inputElementDescs[index].InputSlotClass = _inputElements[index]._inputSlotClass;
            inputElementDescs[index].SemanticName = _inputElements[index]._semanticName.c_str();
            inputElementDescs[index].SemanticIndex = _inputElements[index]._semanticIndex;
            inputElementDescs[index].InstanceDataStepRate = _inputElements[index]._instanceStepRate;
        }
        if (FAILED(renderer.getDevice()->CreateInputLayout(&inputElementDescs[0], static_cast<UINT>(_inputElements.size()),
            _shaderBlob->GetBufferPointer(), _shaderBlob->GetBufferSize(), _inputLayout.ReleaseAndGetAddressOf())))
        {
            return false;
        }
        return true;
    }

    bool Resource::create(Renderer& renderer, const ResourceType& type, const void* const content, const uint32 elementStride, const uint32 elementCount)
    {
        _type = type;

        ComPtr<ID3D11Resource> newResource;
        D3D11_BUFFER_DESC bufferDescriptor{};
        bufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
        bufferDescriptor.ByteWidth = elementStride * elementCount;
        bufferDescriptor.BindFlags = D3D11_BIND_FLAG(1 << ((uint32)_type)); // !!! CAUTION !!!
        bufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
        bufferDescriptor.MiscFlags = 0;
        bufferDescriptor.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA subresourceData{};
        subresourceData.pSysMem = content;

        if (SUCCEEDED(renderer.getDevice()->CreateBuffer(&bufferDescriptor, (content != nullptr) ? &subresourceData : nullptr, reinterpret_cast<ID3D11Buffer**>(newResource.ReleaseAndGetAddressOf()))))
        {
            _byteSize = bufferDescriptor.ByteWidth;
            _elementStride = elementStride;
            _elementMaxCount = elementCount;

            std::swap(_resource, newResource);
            return true;
        }
        return false;
    }

    bool Resource::update(Renderer& renderer, const void* const content, const uint32 elementStride, const uint32 elementCount)
    {
        class SafeResourceMapper
        {
        public:
            SafeResourceMapper(Renderer& renderer, ID3D11Resource* const resource, const uint32 subresource)
                : _renderer{ renderer }
                , _resource{ resource }
                , _subresource{ subresource }
                , _mappedSubresource{}
            {
                if (FAILED(_renderer.getDeviceContext()->Map(_resource, _subresource, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &_mappedSubresource)))
                {
                    _mappedSubresource.pData = nullptr;
                    _mappedSubresource.DepthPitch = 0;
                    _mappedSubresource.RowPitch = 0;
                }
            }
            ~SafeResourceMapper()
            {
                if (isValid() == true)
                {
                    _renderer.getDeviceContext()->Unmap(_resource, _subresource);
                }
            }
            bool isValid() const noexcept
            {
                return _mappedSubresource.pData != nullptr;
            }
            void set(const void* const data, const uint32 size) noexcept
            {
                ::memcpy(_mappedSubresource.pData, data, size);
            }

        private:
            Renderer& _renderer;
            ID3D11Resource* const _resource;
            const uint32 _subresource;
            D3D11_MAPPED_SUBRESOURCE _mappedSubresource;
        };

        SafeResourceMapper safeResourceMapper(renderer, _resource.Get(), 0);
        if (safeResourceMapper.isValid())
        {
            safeResourceMapper.set(content, elementStride * elementCount);
            return true;
        }
        return false;
    }
    static LRESULT WINAPI windowProcedure(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
    {
        switch (Msg)
        {
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        }
        return ::DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    bool Renderer::isRunning()
    {
        if (!_hWnd) return false;
        MSG msg{};
        if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) == false) { return true; }
        switch (msg.message)
        {
        case WM_QUIT:
            destroyWindow();
            return false;
        default:
            break;
        }
        ::DispatchMessage(&msg);
        ::TranslateMessage(&msg);
        return true;
    }

    bool Renderer::beginRendering()
    {
        _deviceContext->ClearRenderTargetView(_backBufferRtv.Get(), _clearColor.f);
        _deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        useTrianglePrimitive();
        return true;
    }

    void Renderer::endRendering()
    {
        _swapChain->Present(0, 0);
    }

    void Renderer::bindResource(const ShaderType shaderType, Resource& resource, const uint32 slot)
    {
        if (resource._type == ResourceType::ConstantBuffer)
        {
            ID3D11Buffer* buffers[1]{ static_cast<ID3D11Buffer*>(resource.getResource()) };
            if (shaderType == ShaderType::VertexShader)
            {
                _deviceContext->VSSetConstantBuffers(slot, 1, buffers);
            }
            else if (shaderType == ShaderType::PixelShader)
            {
                _deviceContext->PSSetConstantBuffers(slot, 1, buffers);
            }
            else
            {
                MINT_LOG_ERROR("!!!");
            }
        }
        else
        {
            MINT_LOG_ERROR("!!!");
        }
    }
    void Renderer::bindResource(Resource& resource, const uint32 slot)
    {
        if (resource._type == ResourceType::VertexBuffer)
        {
            ID3D11Buffer* buffers[1]{ static_cast<ID3D11Buffer*>(resource.getResource()) };
            uint32 strides[1]{ resource._elementStride };
            uint32 offsets[1]{ 0 };
            _deviceContext->IASetVertexBuffers(slot, 1, buffers, strides, offsets);
        }
        else if (resource._type == ResourceType::IndexBuffer)
        {
            _deviceContext->IASetIndexBuffer(static_cast<ID3D11Buffer*>(resource.getResource()), Resource::kIndexBufferFormat, 0);
        }
        else
        {
            MINT_LOG_ERROR("!!!");
        }
    }

    void Renderer::useTrianglePrimitive()
    {
        _deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    bool Renderer::createWindow()
    {
        _hInstance = ::GetModuleHandle(nullptr);
        WNDCLASSEX wndClassEx{};
        wndClassEx.cbSize = sizeof(wndClassEx);
        wndClassEx.hInstance = _hInstance;
        wndClassEx.lpszClassName = TEXT("SimpleRenderer Window");
        wndClassEx.hbrBackground = ::CreateSolidBrush(RGB(255, 255, 255));
        wndClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wndClassEx.lpfnWndProc = windowProcedure;
        ::RegisterClassEx(&wndClassEx);
        _hWnd = ::CreateWindowEx(0, wndClassEx.lpszClassName, TEXT("TEST"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, (int)_windowSize.x, (int)_windowSize.y, nullptr, nullptr, _hInstance, nullptr);
        if (_hWnd) ::ShowWindow(_hWnd, SW_SHOWDEFAULT);
        return (_hWnd != nullptr);
    }

    void Renderer::destroyWindow()
    {
        if (!_hWnd) return;
        ::DestroyWindow(_hWnd);
        _hWnd = nullptr;
    }

    void Renderer::createDevice()
    {
        DXGI_SWAP_CHAIN_DESC swapChainDescriptor{};
        swapChainDescriptor.BufferCount = 1;
        swapChainDescriptor.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDescriptor.BufferDesc.Width = static_cast<UINT>(_windowSize.x);
        swapChainDescriptor.BufferDesc.Height = static_cast<UINT>(_windowSize.y);
        swapChainDescriptor.BufferDesc.RefreshRate.Denominator = 1;
        swapChainDescriptor.BufferDesc.RefreshRate.Numerator = 60;
        swapChainDescriptor.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
        swapChainDescriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDescriptor.Flags = 0;
        swapChainDescriptor.OutputWindow = _hWnd;
        swapChainDescriptor.SampleDesc.Count = 1;
        swapChainDescriptor.SampleDesc.Quality = 0;
        swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
        swapChainDescriptor.Windowed = TRUE;
        if (FAILED(::D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
            &swapChainDescriptor, _swapChain.ReleaseAndGetAddressOf(), _device.ReleaseAndGetAddressOf(), nullptr, _deviceContext.ReleaseAndGetAddressOf())))
        {
            MINT_LOG_ERROR("Failed to create Device and SwapChain.");
            return;
        }

        ComPtr<ID3D11Texture2D> backBuffer;
        _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.ReleaseAndGetAddressOf()));
        if (FAILED(_device->CreateRenderTargetView(backBuffer.Get(), nullptr, _backBufferRtv.ReleaseAndGetAddressOf())))
        {
            MINT_LOG_ERROR("Failed to get BackBuffer.");
            return;
        }

        D3D11_TEXTURE2D_DESC depthStencilResourceDescriptor{};
        depthStencilResourceDescriptor.Width = static_cast<UINT>(_windowSize.x);
        depthStencilResourceDescriptor.Height = static_cast<UINT>(_windowSize.y);
        depthStencilResourceDescriptor.MipLevels = 1;
        depthStencilResourceDescriptor.ArraySize = 1;
        depthStencilResourceDescriptor.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilResourceDescriptor.SampleDesc.Count = 1;
        depthStencilResourceDescriptor.SampleDesc.Quality = 0;
        depthStencilResourceDescriptor.Usage = D3D11_USAGE_DEFAULT;
        depthStencilResourceDescriptor.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilResourceDescriptor.CPUAccessFlags = 0;
        depthStencilResourceDescriptor.MiscFlags = 0;
        if (FAILED(_device->CreateTexture2D(&depthStencilResourceDescriptor, nullptr, _depthStencilResource.ReleaseAndGetAddressOf())))
        {
            MINT_LOG_ERROR("Failed to create Depth-Stencil texture.");
            return;
        }
        if (FAILED(_device->CreateDepthStencilView(_depthStencilResource.Get(), nullptr, _depthStencilView.ReleaseAndGetAddressOf())))
        {
            MINT_LOG_ERROR("Failed to create Depth-Stencil view.");
            return;
        }

        D3D11_VIEWPORT viewport{};
        viewport.Width = _windowSize.x;
        viewport.Height = _windowSize.y;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        _deviceContext->RSSetViewports(1, &viewport);

        _deviceContext->OMSetRenderTargets(1, _backBufferRtv.GetAddressOf(), _depthStencilView.Get());
    }
#pragma endregion
}
