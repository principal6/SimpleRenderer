#include "SimpleRenderer.h"

const char kShaderHeaderCode_StreamData[] =
R"(
    struct VS_INPUT
    {
        float4 position : POSITION0;
        float4 color : COLOR0;
        float4 texcoord : TEXCOORD0;
    };
    struct VS_OUTPUT
    {
        float4 screenPosition : SV_POSITION;
        float4 color : COLOR0;
        float4 texcoord : TEXCOORD0;
    };
)";

const char kVertexShaderCode[] =
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
        output.color = input.color;
        output.texcoord = input.texcoord;
        return output;
    }
)";

const char kPixelShaderCode[] =
R"(
    #include "StreamData"
    
    float4 main(VS_OUTPUT input) : SV_Target
    {
        return input.color;
    }
)";

int main()
{
    using namespace SimpleRenderer;

    struct alignas(float) VS_INPUT
    {
        float4 _position;
        float4 _color;
        float2 _texcoord;
    };
    
    struct CB_MATRICES
    {
        float4x4 _projectionMatrix;
    };

    constexpr float2 kScreenSize = float2(800, 600);
    
    Renderer renderer{ Renderer(kScreenSize, Color(0, 0.5f, 1, 1)) };
    
    ShaderHeaderSet shaderHeaderSet;
    shaderHeaderSet.pushShaderHeader("StreamData", kShaderHeaderCode_StreamData);

    Shader vertexShader0;
    vertexShader0.create(renderer, kVertexShaderCode, ShaderType::VertexShader, "VertexShader0", "main", "vs_5_0", &shaderHeaderSet);
    renderer.bindShader(vertexShader0);

    ShaderInputLayout shaderInputLayout;
    shaderInputLayout.pushInputElement(ShaderInputLayout::createInputElementFloat4("POSITION", 0));
    shaderInputLayout.pushInputElement(ShaderInputLayout::createInputElementFloat4("COLOR", 0));
    shaderInputLayout.pushInputElement(ShaderInputLayout::createInputElementFloat2("TEXCOORD", 0));
    shaderInputLayout.create(renderer, vertexShader0);
    renderer.bindShaderInputLayout(shaderInputLayout);

    Shader pixelShader0;
    pixelShader0.create(renderer, kPixelShaderCode, ShaderType::PixelShader, "PixelShader0", "main", "ps_5_0", &shaderHeaderSet);
    renderer.bindShader(pixelShader0);

    Resource vscbMatrices;
    {
        CB_MATRICES cb_matrices;
        cb_matrices._projectionMatrix.makePixelCoordinatesProjectionMatrix(kScreenSize);
        vscbMatrices.createBuffer(renderer, ResourceType::ConstantBuffer, &cb_matrices, sizeof(CB_MATRICES), 1);
    }
    renderer.bindShaderResource(ShaderType::VertexShader, vscbMatrices, 0);

    Resource vertexBuffer;
    Resource indexBuffer;
    std::vector<VS_INPUT> vertices;
    std::vector<uint32> indices;
    {
        MeshGenerator<VS_INPUT>::push_2D_rectangle(float2(200, 150), float2(200, 150), 0.3f, vertices, indices);
        MeshGenerator<VS_INPUT>::push_2D_triangle(float2(500, 150), float2(550, 250), float2(450, 250), vertices, indices);
        MeshGenerator<VS_INPUT>::push_2D_circle(float2(200, 350), 100, 16, vertices, indices);
        MeshGenerator<VS_INPUT>::push_2D_lineSegment(float2(200, 550), float2(600, 350), 2, vertices, indices);
        MeshGenerator<VS_INPUT>::fillVertexColor(vertices, float4(1, 0.5f, 1, 1));
        vertices[0]._color = float4(1, 0, 0, 1);
        vertices[1]._color = float4(1, 1, 0, 1);
        vertices[2]._color = float4(0, 1, 1, 1);
        vertices[3]._color = float4(1, 0, 1, 1);
        vertexBuffer.createBuffer(renderer, ResourceType::VertexBuffer, &vertices[0], sizeof(VS_INPUT), (uint32)vertices.size());
        indexBuffer.createBuffer(renderer, ResourceType::IndexBuffer, &indices[0], sizeof(uint32), (uint32)indices.size());
    }
    
    while (renderer.isRunning())
    {
        if (renderer.beginRendering())
        {
            //renderer.bindShader(pixelShader0);
            //renderer.bindInput(vertexBuffer, 0);
            //renderer.bindInput(indexBuffer, 0);
            //renderer.drawIndexed((uint32)indices.size());

            renderer.drawText("abc");

            renderer.endRendering();
        }
    }
    return 0;
}