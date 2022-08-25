#include "SimpleRenderer.h"

const char kStreamData[] =
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

const char kVertexShader0[] =
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

const char kPixelShader0[] =
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
    
    CB_MATRICES cb_matrices;
    cb_matrices._projectionMatrix.makePixelCoordinatesProjectionMatrix(kScreenSize);
    Renderer renderer{ Renderer(kScreenSize, Color(0, 0.5f, 1, 1)) };

    ShaderHeader shaderHeaderStreamData;
    shaderHeaderStreamData._headerName = "StreamData";
    shaderHeaderStreamData._headerContent = kStreamData;

    Shader vertexShader0;
    vertexShader0.create(renderer, kVertexShader0, ShaderType::VertexShader, "VertexShader0", "main", "vs_5_0", &shaderHeaderStreamData);
    ShaderInputLayout shaderInputLayout;
    ShaderInputLayout::InputElement shaderInputElement;
    shaderInputElement._semanticName = "POSITION";
    shaderInputLayout._inputElements.push_back(shaderInputElement);
    shaderInputLayout.create(renderer, vertexShader0);
    renderer.bindShaderInputLayout(shaderInputLayout);
    renderer.bindShader(vertexShader0);

    Shader pixelShader0;
    pixelShader0.create(renderer, kPixelShader0, ShaderType::PixelShader, "PixelShader0", "main", "ps_5_0", &shaderHeaderStreamData);
    renderer.bindShader(pixelShader0);

    Resource vscbMatrices;
    vscbMatrices.create(renderer, ResourceType::ConstantBuffer, &cb_matrices, sizeof(CB_MATRICES), 1);

    Resource vertexBuffer;
    std::vector<VS_INPUT> vertices;
    {
        vertices.resize(3);
        vertices[0]._position = float4(0, 0, 0, 1);
        vertices[1]._position = float4(400, 0, 0, 1);
        vertices[2]._position = float4(400, 300, 0, 1);
        vertexBuffer.create(renderer, ResourceType::VertexBuffer, &vertices[0], sizeof(VS_INPUT), (uint32)vertices.size());
    }
    renderer.bindResource(vertexBuffer, 0);
    renderer.bindResource(ShaderType::VertexShader, vscbMatrices, 0);

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