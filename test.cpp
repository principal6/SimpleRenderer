#include "SimpleRenderer.h"

#include <algorithm>

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

struct alignas(float) VS_INPUT
{
    SimpleRenderer::float4 _position;
    SimpleRenderer::float4 _color;
    SimpleRenderer::float2 _texcoord;
};

struct CB_MATRICES
{
    SimpleRenderer::float4x4 _projectionMatrix;
};

namespace GJK
{
    using namespace SimpleRenderer;
    size_t g_max_step = 0;
    struct Shape2D
    {
        std::vector<float2> _points;
        float2 _center;
        float2 support(const float2& direction) const
        {
            if (_points.empty())
            {
                return _center;
            }

            float max_dot = -1000.0f;
            size_t support_index = 0;
            for (size_t i = 0; i < _points.size(); i++)
            {
                const float dot = _points[i].dot(direction);
                if (dot > max_dot)
                {
                    max_dot = dot;
                    support_index = i;
                }
            }
            return _center + _points[support_index];
        }
        void rotate(const float yaw)
        {
            quaternion q = quaternion::makeFromAxisAngle(float3(0, 0, -1), yaw);
            float4 rotated;
            for (size_t i = 0; i < _points.size(); i++)
            {
                rotated = q.rotate(float4(_points[i].x, _points[i].y, 0, 1));
                _points[i].x = rotated.x;
                _points[i].y = rotated.y;
            }
        }
        void draw_line_semgments_to(std::vector<VS_INPUT>& vertices, std::vector<uint32>& indices)
        {
            if (_points.size() <= 1)
            {
                return;
            }

            for (size_t iter = 0; iter < _points.size(); iter++)
            {
                const size_t prev = (iter == 0 ? _points.size() - 1 : iter - 1);
                MeshGenerator<VS_INPUT>::push_2D_lineSegment(_center + _points[iter], _center + _points[prev], 1, vertices, indices);
            }
        }
        void draw_points_to(std::vector<VS_INPUT>& vertices, std::vector<uint32>& indices)
        {
            if (_points.size() <= 1)
            {
                return;
            }

            for (size_t iter = 0; iter < _points.size(); iter++)
            {
                MeshGenerator<VS_INPUT>::push_2D_circle(_center + _points[iter], 2.0f, 8, vertices, indices);
            }
            MeshGenerator<VS_INPUT>::push_2D_circle(_center, 4.0f, 8, vertices, indices);
        }
        void make_Minkowski_difference_shape(const Shape2D& a, const Shape2D& b)
        {
            _points.clear();
            for (size_t i = 0; i < a._points.size(); i++)
            {
                for (size_t j = 0; j < b._points.size(); j++)
                {
                    _points.push_back((a._center + a._points[i]) - (b._center + b._points[j]));
                }
            }
            convexify_Graham_scan();
        }

    private:
        void convexify_Graham_scan()
        {
            if (_points.empty())
            {
                return;
            }

            convexify_Graham_scan_sort_points();

            std::vector<size_t> convex_point_indices;
            convex_point_indices.push_back(0);
            convex_point_indices.push_back(1);
            for (size_t i = 2; i < _points.size(); i++)
            {
                const size_t index_c = convex_point_indices[convex_point_indices.size() - 2];
                const size_t index_b = convex_point_indices[convex_point_indices.size() - 1];
                const size_t index_a = i;
                const float2& c = _points[index_c];
                const float2& b = _points[index_b];
                const float2& a = _points[index_a];
                const float3 cb = b - c;
                const float3 ba = a - b;
                const float3 ba_x_cb = ba.cross(cb);
                const bool is_counter_clockwise_or_straight = ba_x_cb.z >= 0.0f;
                if (is_counter_clockwise_or_straight)
                {
                    convex_point_indices.push_back(index_a);
                }
                else
                {
                    convex_point_indices.pop_back();
                    --i;
                }
            }
            std::vector<float2> convex_points;
            for (const auto convex_point_index : convex_point_indices)
            {
                convex_points.push_back(_points[convex_point_index]);
            }
            _points = convex_points;
        }
        size_t convexify_Graham_scan_find_start_point() const
        {
            float2 min = float2(10000.0f, -10000.0f);
            size_t result = 0;
            for (size_t i = 0; i < _points.size(); i++)
            {
                if (_points[i].y >= min.y)
                {
                    if (_points[i].y == min.y)
                    {
                        if (_points[i].x < min.x)
                        {
                            min.x = _points[i].x;
                            result = i;
                        }
                    }
                    else
                    {
                        min.y = _points[i].y;
                        result = i;
                    }
                }
            }
            return result;
        }
        void convexify_Graham_scan_sort_points()
        {
            const size_t startPointIndex = convexify_Graham_scan_find_start_point();
            const float2& startPoint = _points[startPointIndex];
            struct AngleIndex
            {
                AngleIndex(const float theta, const size_t index) : _theta{ theta }, _index{ index } {__noop; }
                float _theta = 0.0f;
                size_t _index = 0;
                bool operator<(const AngleIndex& rhs) const { return _theta < rhs._theta; }
            };
            std::vector<AngleIndex> angleIndices;
            for (size_t i = 0; i < _points.size(); i++)
            {
                if (i == startPointIndex)
                {
                    continue;
                }

                const float2 v = _points[i] - startPoint;
                const float theta = ::atan2f(-v.y, v.x);
                angleIndices.push_back(AngleIndex(theta, i));
            }
            std::sort(angleIndices.begin(), angleIndices.end());
            std::vector<float2> orderedPoints;
            orderedPoints.push_back(startPoint);
            for (const auto& angleIndex : angleIndices)
            {
                orderedPoints.push_back(_points[angleIndex._index]);
            }
            _points = orderedPoints;
        }
    };

    struct Simplex
    {
        Simplex() : _points{}, _validPointCount{ 0 } { __noop; }
        Simplex(const float2& a) : Simplex() { append(a); }
        Simplex(const float2& a, const float2& b) : Simplex() { append(b); append(a); }
        void append(const float2& point) { _points[_validPointCount] = point; ++_validPointCount; }
        const float2& a() const { return _points[_validPointCount - 1]; }
        const float2& b() const { return _points[_validPointCount - 2]; }
        const float2& c() const { return _points[_validPointCount - 3]; }
        void draw_to(const float2& offset, std::vector<VS_INPUT>& vertices, std::vector<uint32>& indices)
        {
            MeshGenerator<VS_INPUT>::push_2D_circle(offset, 4.0f, 8, vertices, indices);

            for (size_t i = 0; i < _validPointCount; i++)
            {
                MeshGenerator<VS_INPUT>::push_2D_circle(offset + _points[i], 4.0f, 8, vertices, indices);

                const size_t prev = (i == 0 ? _validPointCount - 1 : i - 1);
                MeshGenerator<VS_INPUT>::push_2D_lineSegment(offset + _points[i], offset + _points[prev], 2.0f, vertices, indices);
            }
        }
        float2 _points[3];
        size_t _validPointCount = 0;
    };

    inline bool passes_origin(const float2& point, const float2& direction)
    {
        return point.dot(direction) > 0.0f;
    }
    inline float2 support(const Shape2D& shape_a, const Shape2D& shape_b, const float2& direction)
    {
        return shape_a.support(direction) - shape_b.support(-direction);
    }
    inline bool intersects_simplex(const Shape2D& shape_a, const Shape2D& shape_b, Simplex& simplex, float2& direction)
    {
        if (simplex._validPointCount == 2)
        {
            // 1-simplex: line segment
            const float2& a = simplex.a();
            const float2& b = simplex.b();
            const float3 ao = -a;
            const float3 ab = b - a;
            const float3 ab_x_ao = ab.cross(ao);
            if (ab_x_ao.lengthSq() == 0.0f)
            {
                simplex = Simplex(a);
                direction = ao;
                return false;
            }
            direction = ab_x_ao.cross(ab);
            direction.normalize();
            return false;
        }
        else
        {
            // 2-simplex: triangle
            const float2& a = simplex.a();
            const float2& b = simplex.b();
            const float2& c = simplex.c();
            const float3 ao = -a;
            const float3 ab = b - a;
            const float3 ac = c - a;
            const float3 ab_x_ac = ab.cross(ac);

            // edge AB
            float3 n = ab.cross(ab_x_ac);
            n.normalize();
            if (n.dot(ao) > 0.0f)
            {
                const float l = ab.length();
                const float proj = ab.dot(ao) / l;
                if (proj >= l)
                {
                    // B
                    const float2 bo = -b;
                    direction = bo;
                    direction.normalize();
                    simplex = Simplex(b);
                }
                //else if (proj < 0.0f)
                //{
                //    // A
                //    direction = ao;
                //    direction.normalize();
                //    simplex = Simplex(a);
                //}
                else
                {
                    // AB
                    direction = n;
                    direction.normalize();
                    simplex = Simplex(a, b);
                }
                return false;
            }

            // edge AC
            n = ab_x_ac.cross(ac);
            if (n.dot(ao) > 0.0f)
            {
                const float l = ac.length();
                const float proj = ac.dot(ao) / l;
                if (proj >= l)
                {
                    // C
                    const float2 co = -c;
                    direction = co;
                    direction.normalize();
                    simplex = Simplex(c);
                }
                //else if (proj < 0.0f)
                //{
                //    // A
                //    direction = ao;
                //    direction.normalize();
                //    simplex = Simplex(a);
                //}
                else
                {
                    // AC
                    direction = n;
                    direction.normalize();
                    simplex = Simplex(a, c);
                }
                return false;
            }

            // inside
            return true;
        }
    }
    struct DebugData
    {
        Simplex _simplex;
        float2 _direction;
    };
    bool intersects(const Shape2D& shape_a, const Shape2D& shape_b, DebugData* const outDebugData = nullptr)
    {
        size_t step = 0;
        bool result = true;
        Simplex simplex;
        float2 direction = float2(1, 0);
        float2 Minkowski_support = support(shape_a, shape_b, direction);
        simplex.append(Minkowski_support);
        if (step < g_max_step)
        {
            direction = -Minkowski_support;
            direction.normalize();
            while (true)
            {
                if (step >= g_max_step)
                {
                    break;
                }

                Minkowski_support = support(shape_a, shape_b, direction);
                if (passes_origin(Minkowski_support, direction) == false)
                {
                    result = false;
                    break;
                }

                simplex.append(Minkowski_support);

                if (intersects_simplex(shape_a, shape_b, simplex, direction) == true)
                {
                    result = true;
                    break;
                }

                ++step;
            }
        }
        if (outDebugData != nullptr)
        {
            outDebugData->_simplex = simplex;
            outDebugData->_direction = direction;
        }
        return result;
    }
}

int main()
{
    using namespace SimpleRenderer;

    constexpr float2 kScreenSize = float2(800, 600);

    Renderer renderer{ Renderer(kScreenSize, Color(0, 0.5f, 1, 1)) };

    ShaderHeaderSet shaderHeaderSet;
    shaderHeaderSet.pushShaderHeader("StreamData", kShaderHeaderCode_StreamData);

    Shader vertexShader0;
    vertexShader0.create(renderer, kVertexShaderCode, ShaderType::VertexShader, "VertexShader0", "main", "vs_5_0", &shaderHeaderSet);

    ShaderInputLayout shaderInputLayout;
    shaderInputLayout.pushInputElement(ShaderInputLayout::createInputElementFloat4("POSITION", 0));
    shaderInputLayout.pushInputElement(ShaderInputLayout::createInputElementFloat4("COLOR", 0));
    shaderInputLayout.pushInputElement(ShaderInputLayout::createInputElementFloat2("TEXCOORD", 0));
    shaderInputLayout.create(renderer, vertexShader0);

    Shader pixelShader0;
    pixelShader0.create(renderer, kPixelShaderCode, ShaderType::PixelShader, "PixelShader0", "main", "ps_5_0", &shaderHeaderSet);

    Resource vscbMatrices;
    CB_MATRICES cb_matrices;
    cb_matrices._projectionMatrix.makePixelCoordinatesProjectionMatrix(kScreenSize);
    //cb_matrices._projectionMatrix.makePerspectiveProjectionMatrix(kPi * 0.25f, 0.001f, 1000.0f, kScreenSize.x / kScreenSize.y);
    vscbMatrices.createBuffer(renderer, ResourceType::ConstantBuffer, &cb_matrices, sizeof(CB_MATRICES), 1);

    std::vector<VS_INPUT> vertices;
    std::vector<uint32> indices;
    GJK::Shape2D shape_a;
    shape_a._center = float2(100, 120);
    shape_a._points.push_back(float2(-20, -45));
    shape_a._points.push_back(float2(-50, 0));
    shape_a._points.push_back(float2(-10, 30));
    shape_a._points.push_back(float2(30, 20));
    shape_a._points.push_back(float2(50, -10));
    shape_a.rotate(0.375f);
    shape_a.draw_line_semgments_to(vertices, indices);
    MeshGenerator<VS_INPUT>::push_2D_circle(shape_a._center, 2.0f, 16, vertices, indices);
    GJK::Shape2D shape_b;
    shape_b._center = float2(200, 100);
    shape_b._points.push_back(float2(80, -50));
    shape_b._points.push_back(float2(-80, -50));
    shape_b._points.push_back(float2(-80, 50));
    shape_b._points.push_back(float2(80, 50));
    shape_b.rotate(kPi * 0.125f);
    shape_b.draw_line_semgments_to(vertices, indices);
    MeshGenerator<VS_INPUT>::push_2D_circle(shape_b._center, 2.0f, 16, vertices, indices);
    GJK::Shape2D shape_Minkowski;
    shape_Minkowski._center = kScreenSize * 0.5f;
    shape_Minkowski.make_Minkowski_difference_shape(shape_a, shape_b);
    shape_Minkowski.draw_points_to(vertices, indices);
    shape_Minkowski.draw_line_semgments_to(vertices, indices);
    MeshGenerator<VS_INPUT>::fillVertexColor(vertices, float4(1, 1, 1, 1));

    Resource vertexBuffer;
    Resource indexBuffer;
    vertexBuffer.createBuffer(renderer, ResourceType::VertexBuffer, &vertices[0], sizeof(VS_INPUT), (uint32)vertices.size());
    indexBuffer.createBuffer(renderer, ResourceType::IndexBuffer, &indices[0], sizeof(uint32), (uint32)indices.size());

    while (renderer.isRunning())
    {
        if (renderer.beginRendering())
        {
            {
                vertices.clear();
                indices.clear();
                shape_a.draw_line_semgments_to(vertices, indices);
                shape_b.draw_line_semgments_to(vertices, indices);
                shape_Minkowski.draw_points_to(vertices, indices);
                shape_Minkowski.draw_line_semgments_to(vertices, indices);
                MeshGenerator<VS_INPUT>::fillVertexColor(vertices, float4(1, 1, 1, 1));

                {
                    const size_t vertex_offset = vertices.size();
                    GJK::DebugData debugData;
                    GJK::intersects(shape_a, shape_b, &debugData);
                    debugData._simplex.draw_to(shape_Minkowski._center, vertices, indices);
                    const float2& support_a = shape_a.support(debugData._direction);
                    const float2& support_b = shape_b.support(-debugData._direction);
                    MeshGenerator<VS_INPUT>::push_2D_lineSegment(shape_a._center, shape_a._center + debugData._direction * 40.0f, 2.0f, vertices, indices);
                    MeshGenerator<VS_INPUT>::push_2D_lineSegment(shape_b._center, shape_b._center - debugData._direction * 40.0f, 2.0f, vertices, indices);
                    MeshGenerator<VS_INPUT>::push_2D_circle(support_a, 4.0f, 8, vertices, indices);
                    MeshGenerator<VS_INPUT>::push_2D_circle(support_b, 4.0f, 8, vertices, indices);
                    MeshGenerator<VS_INPUT>::fillVertexColor(vertex_offset, vertices, float4(1, 0, 1, 1));
                }

                vertexBuffer.update(renderer, &vertices[0], sizeof(VS_INPUT), (uint32)vertices.size());
                indexBuffer.update(renderer, &indices[0], sizeof(uint32), (uint32)indices.size());
            }

            renderer.bindShader(vertexShader0);
            renderer.bindShaderInputLayout(shaderInputLayout);
            renderer.bindShader(pixelShader0);
            renderer.bindInput(vertexBuffer, 0);
            renderer.bindInput(indexBuffer, 0);
            renderer.bindShaderResource(ShaderType::VertexShader, vscbMatrices, 0);
            renderer.drawIndexed((uint32)indices.size());

            if (renderer.getKeyboardChar() == '2')
            {
                ++GJK::g_max_step;
            }
            else if (renderer.getKeyboardChar() == '1')
            {
                if (GJK::g_max_step > 0)
                {
                    --GJK::g_max_step;
                }
            }
            /*
            if (renderer.isMouseLeftButtonUp())
            {
                ++GJK::g_max_step;
            }
            else if (renderer.isMouseRightButtonUp())
            {
                if (GJK::g_max_step > 0)
                {
                    --GJK::g_max_step;
                }
            }
            */

            //char buffer[8]{};
            //for (size_t i = 0; i < shapeMinkowski._points.size(); ++i)
            //{
            //    ::_itoa_s(static_cast<int>(i), buffer, 10);
            //    const auto& point = shapeMinkowski._points[i];
            //    renderer.drawText(buffer, shapeMinkowski._center + point);
            //}

            renderer.endRendering();
        }
    }
    return 0;
}
