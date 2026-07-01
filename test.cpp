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
		vector<float2> _points;
		float2 _center;
		float2 Support(const float2& direction) const
		{
			if (_points.empty())
			{
				return _center;
			}

			float maxDotProduct = -1000.0f;
			size_t supportIndex = 0;
			for (size_t i = 0; i < _points.size(); i++)
			{
				const float dotProduct = _points[i].Dot(direction);
				if (dotProduct > maxDotProduct)
				{
					maxDotProduct = dotProduct;
					supportIndex = i;
				}
			}
			return _center + _points[supportIndex];
		}
		void Rotate(const float yaw)
		{
			quaternion q = quaternion::MakeByAxisAngle(float3(0, 0, -1), yaw);
			float4 rotated;
			for (size_t i = 0; i < _points.size(); i++)
			{
				rotated = q.Rotate(float4(_points[i].x, _points[i].y, 0, 1));
				_points[i].x = rotated.x;
				_points[i].y = rotated.y;
			}
		}
		void DrawLineSegmentsTo(const Color& color, vector<VS_INPUT>& vertices, vector<uint32>& indices)
		{
			if (_points.size() <= 1)
			{
				return;
			}

			for (size_t iter = 0; iter < _points.size(); iter++)
			{
				const size_t prev = (iter == 0 ? _points.size() - 1 : iter - 1);
				MeshGenerator<VS_INPUT>::Push2DLineSegment(color, _center + _points[iter], _center + _points[prev], 2.0f, vertices, indices);
			}
		}
		void DrawPointsTo(const Color& color, vector<VS_INPUT>& vertices, vector<uint32>& indices)
		{
			if (_points.size() <= 1)
			{
				return;
			}

			for (size_t iter = 0; iter < _points.size(); iter++)
			{
				MeshGenerator<VS_INPUT>::Push2DCircle(color, _center + _points[iter], 3.0f, 8, vertices, indices);
			}
		}
		void MakeMinkowskiDifferenceShape(const Shape2D& a, const Shape2D& b)
		{
			_center = float2(0, 0);

			_points.clear();
			for (size_t i = 0; i < a._points.size(); i++)
			{
				for (size_t j = 0; j < b._points.size(); j++)
				{
					_points.push_back(a._points[i] - b._points[j]);
				}
			}
			ConvexifyGrahamScan();
		}

	private:
		void ConvexifyGrahamScan()
		{
			if (_points.empty())
			{
				return;
			}

			ConvexifyGrahamScanSortPoints();

			vector<size_t> convex_point_indices;
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
				const float3 ba_x_cb = ba.Cross(cb);
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
			vector<float2> convex_points;
			for (const auto convex_point_index : convex_point_indices)
			{
				convex_points.push_back(_points[convex_point_index]);
			}
			_points = convex_points;
		}
		size_t ConvexifyGrahamScanFindStartPoint() const
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
						min.x = _points[i].x;
						result = i;
					}
				}
			}
			return result;
		}
		void ConvexifyGrahamScanSortPoints()
		{
			const size_t startPointIndex = ConvexifyGrahamScanFindStartPoint();
			const float2& startPoint = _points[startPointIndex];
			struct AngleIndex
			{
				AngleIndex(const float theta, const size_t index) : _theta{ theta }, _index{ index } { __noop; }
				float _theta = 0.0f;
				size_t _index = 0;
				bool operator<(const AngleIndex& rhs) const { return _theta < rhs._theta; }
			};
			vector<AngleIndex> angleIndices;
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
			vector<float2> orderedPoints;
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
		Simplex(const float2& a) : Simplex() { Append(a); }
		Simplex(const float2& a, const float2& b) : Simplex() { Append(b); Append(a); }
		void Append(const float2& point) { _points[_validPointCount] = point; ++_validPointCount; }
		const float2& A() const { return _points[_validPointCount - 1]; }
		const float2& B() const { return _points[_validPointCount - 2]; }
		const float2& C() const { return _points[_validPointCount - 3]; }
		void DrawTo(const Color& color, const Color& color_a, const float2& offset, vector<VS_INPUT>& vertices, vector<uint32>& indices)
		{
			for (size_t i = 0; i < _validPointCount; i++)
			{
				const bool is_a = (i == _validPointCount - 1);
				MeshGenerator<VS_INPUT>::Push2DCircle((is_a ? color_a : color), offset + _points[i], 4.0f, 8, vertices, indices);

				const size_t prev = (i == 0 ? _validPointCount - 1 : i - 1);
				MeshGenerator<VS_INPUT>::Push2DLineSegment(color, offset + _points[i], offset + _points[prev], 2.0f, vertices, indices);
			}
		}
		const float2& GetClosestPointToOrigin() const
		{
			size_t min_index = 0;
			float min_distance_sq = 99999.0f;
			for (size_t i = 0; i < _validPointCount; i++)
			{
				const float distance_sq = _points[i].LengthSq();
				if (distance_sq < min_distance_sq)
				{
					min_distance_sq = distance_sq;
					min_index = i;
				}
			}
			return _points[min_index];
		}
		float2 _points[3];
		size_t _validPointCount = 0;
	};

	inline bool PassesOrigin(const float2& point, const float2& direction)
	{
		return point.Dot(direction) > 0.0f;
	}
	inline float2 Support(const Shape2D& shape_a, const Shape2D& shape_b, const float2& direction)
	{
		return shape_a.Support(direction) - shape_b.Support(-direction);
	}
	inline bool IntersectsSimplex(const Shape2D& shape_a, const Shape2D& shape_b, Simplex& simplex, float2& direction)
	{
		if (simplex._validPointCount == 2)
		{
			// 1-simplex: line segment
			const float2& a = simplex.A();
			const float2& b = simplex.B();
			const float3 ao = -a;
			const float3 ab = b - a;
			const float3 ab_x_ao = ab.Cross(ao);
			if (ab_x_ao.LengthSq() == 0.0f)
			{
				simplex = Simplex(a);
				direction = ao;
				return false;
			}
			direction = ab_x_ao.Cross(ab);
			direction.Normalize();
			return false;
		}
		else
		{
			// 2-simplex: triangle
			const float2& a = simplex.A();
			const float2& b = simplex.B();
			const float2& c = simplex.C();
			const float3 ao = -a;
			const float3 ab = b - a;
			const float3 ac = c - a;
			const float3 ab_x_ac = ab.Cross(ac);

			// edge AB
			float3 n = ab.Cross(ab_x_ac);
			n.Normalize();
			if (n.Dot(ao) > 0.0f)
			{
				const float l = ab.Length();
				const float proj = ab.Dot(ao) / l;
				if (proj >= l)
				{
					// B
					const float2 bo = -b;
					direction = bo;
					direction.Normalize();
					simplex = Simplex(b);
				}
				//else if (proj < 0.0f)
				//{
				//    // A
				//    direction = ao;
				//    direction.Normalize();
				//    simplex = Simplex(a);
				//}
				else
				{
					// AB
					direction = n;
					direction.Normalize();
					simplex = Simplex(a, b);
				}
				return false;
			}

			// edge AC
			n = ab_x_ac.Cross(ac);
			if (n.Dot(ao) > 0.0f)
			{
				const float l = ac.Length();
				const float proj = ac.Dot(ao) / l;
				if (proj >= l)
				{
					// C
					const float2 co = -c;
					direction = co;
					direction.Normalize();
					simplex = Simplex(c);
				}
				//else if (proj < 0.0f)
				//{
				//    // A
				//    direction = ao;
				//    direction.Normalize();
				//    simplex = Simplex(a);
				//}
				else
				{
					// AC
					direction = n;
					direction.Normalize();
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
	bool Intersects(const Shape2D& shape_a, const Shape2D& shape_b, const float2& initialDirection, DebugData* const outDebugData = nullptr)
	{
		size_t step = 0;
		bool result = false;
		Simplex simplex;
		float2 direction = initialDirection;
		float2 Minkowski_support = Support(shape_a, shape_b, direction);
		simplex.Append(Minkowski_support);
		float2 prev_direction = direction;
		if (step < g_max_step)
		{
			direction = -Minkowski_support;
			direction.Normalize();
			while (true)
			{
				if (step >= g_max_step)
				{
					break;
				}

				prev_direction = direction;
				Minkowski_support = Support(shape_a, shape_b, direction);
				if (PassesOrigin(Minkowski_support, direction) == false)
				{
					result = false;
					break;
				}

				simplex.Append(Minkowski_support);

				if (IntersectsSimplex(shape_a, shape_b, simplex, direction) == true)
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
			outDebugData->_direction = prev_direction;
		}
		return result;
	}
}

int main()
{
	using namespace SimpleRenderer;
	constexpr uint2 kScreenSize = uint2(800, 600);
	Window window;
	if (window.Create(Window::CreateDesc("SampleMain", kScreenSize)) == false)
	{
		SR_LOG_ERROR("Failed to create window!");
		return -1;
	}
	App app{ App(window, Color(0, 0.5f, 1, 1)) };
	RenderDevice& renderDevice = app.GetRenderDevice();

	ShaderHeaderSet shaderHeaderSet;
	shaderHeaderSet.PushShaderHeader("StreamData", kShaderHeaderCode_StreamData);

	Shader vertexShader0;
	renderDevice.CreateShader(kVertexShaderCode, ShaderType::VertexShader, "VertexShader0", "main", "vs_5_0", &shaderHeaderSet, vertexShader0);

	vector<ShaderInputElement> shaderInputElements;
	shaderInputElements.push_back(ShaderInputElement::CreateInputelement_float4("POSITION", 0));
	shaderInputElements.push_back(ShaderInputElement::CreateInputelement_float4("COLOR", 0));
	shaderInputElements.push_back(ShaderInputElement::CreateInputelement_float2("TEXCOORD", 0));
	ShaderInputLayout shaderInputLayout;
	renderDevice.CreateShaderInputLayout(vertexShader0, shaderInputElements, shaderInputLayout);

	Shader pixelShader0;
	renderDevice.CreateShader(kPixelShaderCode, ShaderType::PixelShader, "PixelShader0", "main", "ps_5_0", &shaderHeaderSet, pixelShader0);

	Resource vscbMatrices;
	CB_MATRICES cb_matrices;
	cb_matrices._projectionMatrix.MakePixelCoordsProjectionMatrix(kScreenSize);
	//cb_matrices._projectionMatrix.MakePerspectiveProjectionMatrix(kPi * 0.25f, 0.001f, 1000.0f, kScreenSize.x / kScreenSize.y);
	renderDevice.CreateBuffer(ResourceType::ConstantBuffer, &cb_matrices, sizeof(CB_MATRICES), 1, vscbMatrices);

	bool is_shapes_loaded = false;
	float2 positions_source[2]{};
	float2 positions[2]{};
	float2 positions_prev[2]{};
	float thetas[3]{};
	float thetas_prev[3]{};
	GJK::Shape2D shape_sources[2];
	GJK::Shape2D shapes[2];
	GJK::Shape2D shape_Minkowski;
	Resource vertexBuffer;
	Resource indexBuffer;
	renderDevice.CreateBuffer(ResourceType::VertexBuffer, nullptr, sizeof(VS_INPUT), 512, vertexBuffer);
	renderDevice.CreateBuffer(ResourceType::IndexBuffer, nullptr, sizeof(uint32), 1024, indexBuffer);
	vector<VS_INPUT> vertices;
	vector<uint32> indices;
	uint32 mode = 0;
	uint32 selection = 0;
	float2 initial_direction = float2(1, 0);
	const Color white_color = Color(1, 1, 1, 1);
	const Color orange_color = Color(1, 0.5f, 0, 1);
	const Color yellow_color = Color(1, 1, 0, 1);
	const Color dark_gray_color = Color(0.25f, 0.25f, 0.25f, 1);
	const Color blue_color = Color(0, 0, 1, 1);
	const Color magenta_color = Color(1, 0, 1, 1);
	const float2 minkowski_shape_offset = float2(kScreenSize) * 0.5f + float2(100, 100);
	while (app.IsRunning())
	{
		if (app.GetKeyboardChar() == 'w')
		{
			++GJK::g_max_step;
		}
		else if (app.GetKeyboardChar() == 'q')
		{
			if (GJK::g_max_step > 0)
			{
				--GJK::g_max_step;
			}
		}
		else if (app.GetKeyboardChar() == 'e')
		{
			mode = 0;
		}
		else if (app.GetKeyboardChar() == 'r')
		{
			mode = 1;
		}
		else if (app.GetKeyboardChar() == '1')
		{
			selection = 0;
		}
		else if (app.GetKeyboardChar() == '2')
		{
			selection = 1;
		}
		else if (app.GetKeyboardChar() == '3')
		{
			selection = 2;
		}
		else if (app.GetKeyboardChar() == '0')
		{
			if (mode == 0)
			{
				if (selection <= 1)
				{
					positions[selection] = positions_prev[selection] = positions_source[selection];
				}
			}
			else
			{
				thetas[selection] = thetas_prev[selection] = 0.0f;
			}
		}

		if (app.GetKeyboardUpKey() == Window::Key::Enter || is_shapes_loaded == false)
		{
			String shapes_content;
			ReadFile("shapes.txt", shapes_content);

			XML xml;
			if (xml.Parse(shapes_content) == true)
			{
				uint32 shape_index = 0;
				const XML::Node& root_node = xml.GetRootNode();
				for (const auto& root_child_node_ID : root_node._child_node_IDs)
				{
					const XML::Node& shape_node = xml.GetNode(root_child_node_ID);
					for (const auto& shape_child_node_ID : shape_node._child_node_IDs)
					{
						const XML::Node& shape_child_node = xml.GetNode(shape_child_node_ID);
						if (shape_child_node.GetName() == "center")
						{
							XML::Attribute attribute = shape_child_node.GetAttribute(0);
							const float x = std::stof(attribute.GetValue());
							attribute = attribute.GetNextAttribute();
							const float y = std::stof(attribute.GetValue());

							positions_source[shape_index] = float2(x, y);
						}
						else if (shape_child_node.GetName() == "points")
						{
							shape_sources[shape_index]._points.clear();

							for (XML::Node point_node = shape_child_node.GetChildNode(0); point_node.IsValid(); point_node = point_node.GetNextSiblingNode())
							{
								XML::Attribute attribute = point_node.GetAttribute(0);
								const float x = std::stof(attribute.GetValue());
								attribute = attribute.GetNextAttribute();
								const float y = std::stof(attribute.GetValue());

								shape_sources[shape_index]._points.push_back(float2(x, y));
							}
						}
					}

					++shape_index;
				}
			}

			positions[0] = positions_prev[0] = positions_source[0];
			positions[1] = positions_prev[1] = positions_source[1];

			shape_sources[0]._center = positions[0];
			shape_sources[1]._center = positions[1];

			shapes[0] = shape_sources[0];
			shapes[1] = shape_sources[1];

			is_shapes_loaded = true;
		}

		if (app.IsMouseLButtonPressed())
		{
			if (mode == 0)
			{
				if (selection <= 1)
				{
					positions_prev[selection] = positions[selection];
				}
			}
			else
			{
				thetas_prev[selection] = thetas[selection];
			}
		}
		if (app.IsMouseLButtonDown())
		{
			if (mode == 0)
			{
				if (selection <= 1)
				{
					positions[selection].x = positions_prev[selection].x + app.GetMouseMoveDelta().x;
					positions[selection].y = positions_prev[selection].y + app.GetMouseMoveDelta().y;
				}
			}
			else
			{
				const float theta = (app.GetMouseMoveDelta().x + app.GetMouseMoveDelta().y) * 0.03125f;
				thetas[selection] = thetas_prev[selection] + theta;
			}
		}

		if (selection <= 1)
		{
			shape_sources[selection]._center = positions[selection];
			shapes[selection]._center = shape_sources[selection]._center;

			shapes[selection] = shape_sources[selection];
			shapes[selection].Rotate(thetas[selection]);
		}
		else
		{
			const quaternion rotation = quaternion::MakeByAxisAngle(float3(0, 0, -1), thetas[selection]);
			initial_direction = rotation.Rotate(float2(1, 0));
		}

		const float2 minkowski_shape_center_in_minkowski_space = shapes[0]._center - shapes[1]._center;
		const float2 minkowski_space_origin = float2(kScreenSize) * 0.5f + float2(0, 120);
		shape_Minkowski.MakeMinkowskiDifferenceShape(shapes[0], shapes[1]);
		shape_Minkowski._center = minkowski_space_origin + minkowski_shape_center_in_minkowski_space;

		app.BeginRendering();
		{
			{
				vertices.clear();
				indices.clear();

				GJK::DebugData debugData;
				const bool intersected = GJK::Intersects(shapes[0], shapes[1], initial_direction, &debugData);

				const Color shape_color = (intersected ? Color(0, 1, 0, 1) : white_color);
				MeshGenerator<VS_INPUT>::Push2DCircle(white_color, shapes[0]._center, 4.0f, 8, vertices, indices);
				MeshGenerator<VS_INPUT>::Push2DCircle(white_color, shapes[1]._center, 4.0f, 8, vertices, indices);
				shapes[0].DrawPointsTo(shape_color, vertices, indices);
				shapes[0].DrawLineSegmentsTo(shape_color, vertices, indices);
				shapes[1].DrawPointsTo(shape_color, vertices, indices);
				shapes[1].DrawLineSegmentsTo(shape_color, vertices, indices);

				shape_Minkowski.DrawPointsTo(dark_gray_color, vertices, indices);
				shape_Minkowski.DrawLineSegmentsTo(dark_gray_color, vertices, indices);
				MeshGenerator<VS_INPUT>::Push2DCircle(Color(0.5f, 1.0f, 0.25f, 1.0f), minkowski_space_origin + debugData._simplex.GetClosestPointToOrigin(), 8.0f, 8, vertices, indices);

				{
					const Color color_latest = Color(0.5f, 0, 1, 1);
					const Color color_shape_a = orange_color;
					const Color color_shape_b = blue_color;
					const float2& support_a = shapes[0].Support(debugData._direction);
					const float2& support_b = shapes[1].Support(-debugData._direction);
					MeshGenerator<VS_INPUT>::Push2DCircle(color_latest, support_a, 4.0f, 8, vertices, indices);
					MeshGenerator<VS_INPUT>::Push2DCircle(color_latest, support_b, 4.0f, 8, vertices, indices);
					MeshGenerator<VS_INPUT>::Push2DArrow(color_shape_a, shapes[0]._center, support_a, 2.0f, 0.125f, 2.0f, vertices, indices);
					MeshGenerator<VS_INPUT>::Push2DArrow(color_shape_b, shapes[1]._center, support_b, 2.0f, 0.125f, 2.0f, vertices, indices);
					MeshGenerator<VS_INPUT>::Push2DArrow(color_latest, shapes[0]._center, shapes[0]._center + debugData._direction * 32.0f, 2.0f, 0.25f, 3.0f, vertices, indices);
					MeshGenerator<VS_INPUT>::Push2DArrow(color_latest, shapes[1]._center, shapes[1]._center - debugData._direction * 32.0f, 2.0f, 0.25f, 3.0f, vertices, indices);

					const float2 support_a_from_o = support_a - shapes[0]._center;
					const float2 support_b_from_o = shapes[1]._center - support_b;
					debugData._simplex.DrawTo(magenta_color, color_latest, minkowski_space_origin, vertices, indices);
					MeshGenerator<VS_INPUT>::Push2DCircle(white_color, shape_Minkowski._center, 4.0f, 8, vertices, indices);
					MeshGenerator<VS_INPUT>::Push2DArrow(color_shape_a, shape_Minkowski._center, shape_Minkowski._center + support_a_from_o, 2.0f, 0.125f, 2.0f, vertices, indices);
					MeshGenerator<VS_INPUT>::Push2DArrow(color_shape_b, shape_Minkowski._center + support_a_from_o, shape_Minkowski._center + support_a_from_o + support_b_from_o, 2.0f, 0.125f, 2.0f, vertices, indices);
					MeshGenerator<VS_INPUT>::Push2DArrow(color_latest, shape_Minkowski._center, shape_Minkowski._center + debugData._direction * 32.0f, 2.0f, 0.25f, 3.0f, vertices, indices);

					MeshGenerator<VS_INPUT>::Push2DArrow(white_color, minkowski_space_origin - float2(200, 0), minkowski_space_origin + float2(200, 0), 1.0f, 0.0625f, 4.0f, vertices, indices);
					MeshGenerator<VS_INPUT>::Push2DArrow(white_color, minkowski_space_origin + float2(0, 200), minkowski_space_origin - float2(0, 200), 1.0f, 0.0625f, 4.0f, vertices, indices);
				}

				renderDevice.UpdateShaderResource(&vertices[0], sizeof(VS_INPUT), (uint32)vertices.size(), vertexBuffer);
				renderDevice.UpdateShaderResource(&indices[0], sizeof(uint32), (uint32)indices.size(), indexBuffer);
			}

			renderDevice.BindShader(vertexShader0);
			renderDevice.BindShaderInputLayout(shaderInputLayout);
			renderDevice.BindShader(pixelShader0);
			renderDevice.BindInput(vertexBuffer, 0);
			renderDevice.BindInput(indexBuffer, 0);
			renderDevice.BindShaderResource(ShaderType::VertexShader, vscbMatrices, 0);
			renderDevice.DrawIndexed((uint32)indices.size());

			app.DrawTextAt(Color(0, 1, 1, 1), "GJK Algorithm Test", float2(10, 10));
			app.DrawTextAt((selection == 0 ? yellow_color : white_color), "1: shape A", float2(10, 40));
			app.DrawTextAt((selection == 1 ? yellow_color : white_color), "2: shape B", float2(10, 60));
			app.DrawTextAt((selection == 2 ? yellow_color : white_color), "3: initial direction", float2(10, 80));
			app.DrawTextAt((selection == 2 ? yellow_color : white_color), "0: reset", float2(10, 100));
			app.DrawTextAt((mode == 0 ? yellow_color : white_color), "e: translate", float2(10, 140));
			app.DrawTextAt((mode == 1 ? yellow_color : white_color), "r: Rotate", float2(10, 160));
			app.DrawTextAt(white_color, "current gjk_max_step: " + ToString(GJK::g_max_step), float2(10, 180));
			app.DrawTextAt(white_color, "q: --gjk_max_step", float2(10, 200));
			app.DrawTextAt(white_color, "w: ++gjk_max_step", float2(10, 220));

			app.DrawTextAt(white_color, "ENTER: load shapes from file", float2(10, 260));
		}
		app.EndRendering();
	}
	return 0;
}
