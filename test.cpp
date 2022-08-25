#include "SimpleRenderer.h"

const char kStreamDataCode[] =
R"(
    struct VS_INPUT
    {
        float4 position : POSITION0;
    };
    struct VS_OUTPUT
    {
        float4 screenPosition : SV_POSITION;
    };
)";

const char kVertexShaderCode0[] =
R"(
    #include "StreamData"

    cbuffer CB_MATRICES
    {
        float4x4 _cbProjectionMatrix;
    };

    VS_OUTPUT main(VS_INPUT input)
    {
        VS_OUTPUT output;
        output.screenPosition = mul(input.position, _cbProjectionMatrix);
        output.screenPosition /= output.screenPosition.w;
        return output;
    }
)";

const char kPixelShaderCode0[] =
R"(
    #include "StreamData"

    float4 main(VS_OUTPUT input) : SV_Target
    {
        return float4(0, 1, 0, 1);
    }
)";

int main()
{
    using namespace SimpleRenderer;

    struct alignas(float) VS_INPUT
    {
        float4 _position;
    };
    
    struct CB_MATRICES
    {
        float4x4 _projectionMatrix;
    };

    constexpr float2 kScreenSize = float2(800, 600);
    
    Renderer renderer{ Renderer(kScreenSize, Color(0, 0.5f, 1, 1)) };
    
    ShaderHeader shaderHeaderStreamData;
    shaderHeaderStreamData._headerName = "StreamData";
    shaderHeaderStreamData._headerCode = kStreamDataCode;

    Shader vertexShader0;
    vertexShader0.create(renderer, kVertexShaderCode0, ShaderType::VertexShader, "VertexShader0", "main", "vs_5_0", &shaderHeaderStreamData);
    renderer.bindShader(vertexShader0);

    ShaderInputLayout shaderInputLayout;
    shaderInputLayout.pushInputElement(ShaderInputLayout::createInputElementFloat4("POSITION", 0));
    shaderInputLayout.create(renderer, vertexShader0);
    renderer.bindShaderInputLayout(shaderInputLayout);

    Shader pixelShader0;
    pixelShader0.create(renderer, kPixelShaderCode0, ShaderType::PixelShader, "PixelShader0", "main", "ps_5_0", &shaderHeaderStreamData);
    renderer.bindShader(pixelShader0);

    Resource vscbMatrices;
    {
        CB_MATRICES cb_matrices;
        cb_matrices._projectionMatrix.makePixelCoordinatesProjectionMatrix(kScreenSize);
        vscbMatrices.create(renderer, ResourceType::ConstantBuffer, &cb_matrices, sizeof(CB_MATRICES), 1);
    }
    renderer.bindShaderResource(ShaderType::VertexShader, vscbMatrices, 0);

    Resource vertexBuffer;
    std::vector<VS_INPUT> vertices;
    {
        vertices.resize(3);
        vertices[0]._position = float4(0, 0, 0, 1);
        vertices[1]._position = float4(400, 0, 0, 1);
        vertices[2]._position = float4(400, 300, 0, 1);
        vertexBuffer.create(renderer, ResourceType::VertexBuffer, &vertices[0], sizeof(VS_INPUT), (uint32)vertices.size());
    }
    renderer.bindInput(vertexBuffer, 0);

    while (renderer.isRunning())
    {
        if (renderer.beginRendering())
        {
            renderer.draw((uint32)vertices.size());
            renderer.endRendering();
        }
    }
    return 0;
}