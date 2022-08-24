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

    VS_OUTPUT main(VS_INPUT input)
    {
        VS_OUTPUT output;
        output.screenPosition = input.position;
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

    struct VS_INPUT
    {
        float4 _position;
    };

    Renderer renderer{ Renderer(float2(800, 600), Color(0, 0.5f, 1, 1)) };

    ShaderHeader shaderHeaderStreamData;
    shaderHeaderStreamData._headerName = "StreamData";
    shaderHeaderStreamData._headerContent = kStreamData;

    Shader vertexShader0;
    vertexShader0.create(renderer, kVertexShader0, ShaderType::VertexShader, "VertexShader0", "main", "vs_5_0", &shaderHeaderStreamData);
    D3D11_INPUT_ELEMENT_DESC inputElementDesc{};
    inputElementDesc.AlignedByteOffset = 0;
    inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDesc.InputSlot = 0;
    inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    inputElementDesc.SemanticName = "POSITION";
    inputElementDesc.SemanticIndex = 0;
    inputElementDesc.InstanceDataStepRate = 0;
    vertexShader0._inputElementDescs.push_back(inputElementDesc);
    vertexShader0.createInputLayout(renderer);

    Shader pixelShader0;
    pixelShader0.create(renderer, kPixelShader0, ShaderType::PixelShader, "PixelShader0", "main", "ps_5_0", &shaderHeaderStreamData);
    renderer.bindShader(vertexShader0);
    renderer.bindShader(pixelShader0);

    Resource vertexBuffer;
    std::vector<VS_INPUT> vertices;
    {
        vertices.resize(3);
        vertices[0]._position = float4(-0.5f, 0.5f, 0.0f, 1.0f);
        vertices[1]._position = float4(+0.5f, 0.5f, 0.0f, 1.0f);
        vertices[2]._position = float4(0.0f, -0.5f, 0.0f, 1.0f);
        vertexBuffer.create(renderer, ResourceType::VertexBuffer, &vertices[0], sizeof(VS_INPUT), (uint32)vertices.size());
    }
    renderer.bindResource(vertexBuffer, 0);

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