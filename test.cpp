﻿#include "SimpleRenderer.h"

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
			quaternion q = quaternion::make_from_axis_angle(float3(0, 0, -1), yaw);
			float4 rotated;
			for (size_t i = 0; i < _points.size(); i++)
			{
				rotated = q.rotate(float4(_points[i].x, _points[i].y, 0, 1));
				_points[i].x = rotated.x;
				_points[i].y = rotated.y;
			}
		}
		void draw_line_semgments_to(const Color& color, std::vector<VS_INPUT>& vertices, std::vector<uint32>& indices)
		{
			if (_points.size() <= 1)
			{
				return;
			}

			for (size_t iter = 0; iter < _points.size(); iter++)
			{
				const size_t prev = (iter == 0 ? _points.size() - 1 : iter - 1);
				MeshGenerator<VS_INPUT>::push_2D_lineSegment(color, _center + _points[iter], _center + _points[prev], 2.0f, vertices, indices);
			}
		}
		void draw_points_to(const Color& color, std::vector<VS_INPUT>& vertices, std::vector<uint32>& indices)
		{
			if (_points.size() <= 1)
			{
				return;
			}

			for (size_t iter = 0; iter < _points.size(); iter++)
			{
				MeshGenerator<VS_INPUT>::push_2D_circle(color, _center + _points[iter], 3.0f, 8, vertices, indices);
			}
		}
		void make_Minkowski_difference_shape(const Shape2D& a, const Shape2D& b)
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
						min.x = _points[i].x;
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
				AngleIndex(const float theta, const size_t index) : _theta{ theta }, _index{ index } { __noop; }
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
		void draw_to(const Color& color, const Color& color_a, const float2& offset, std::vector<VS_INPUT>& vertices, std::vector<uint32>& indices)
		{
			for (size_t i = 0; i < _validPointCount; i++)
			{
				const bool is_a = (i == _validPointCount - 1);
				MeshGenerator<VS_INPUT>::push_2D_circle((is_a ? color_a : color), offset + _points[i], 4.0f, 8, vertices, indices);

				const size_t prev = (i == 0 ? _validPointCount - 1 : i - 1);
				MeshGenerator<VS_INPUT>::push_2D_lineSegment(color, offset + _points[i], offset + _points[prev], 2.0f, vertices, indices);
			}
		}
		const float2& get_closest_point_to_origin() const
		{
			size_t min_index = 0;
			float min_distance_sq = 99999.0f;
			for (size_t i = 0; i < _validPointCount; i++)
			{
				const float distance_sq = _points[i].length_sq();
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
			if (ab_x_ao.length_sq() == 0.0f)
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
	bool intersects(const Shape2D& shape_a, const Shape2D& shape_b, const float2& initialDirection, DebugData* const outDebugData = nullptr)
	{
		size_t step = 0;
		bool result = false;
		Simplex simplex;
		float2 direction = initialDirection;
		float2 Minkowski_support = support(shape_a, shape_b, direction);
		simplex.append(Minkowski_support);
		float2 prev_direction = direction;
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

				prev_direction = direction;
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
			outDebugData->_direction = prev_direction;
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
	shaderHeaderSet.push_shader_header("StreamData", kShaderHeaderCode_StreamData);

	Shader vertexShader0;
	vertexShader0.create(renderer, kVertexShaderCode, ShaderType::VertexShader, "VertexShader0", "main", "vs_5_0", &shaderHeaderSet);

	ShaderInputLayout shaderInputLayout;
	shaderInputLayout.push_InputElement(ShaderInputLayout::create_InputElement_float4("POSITION", 0));
	shaderInputLayout.push_InputElement(ShaderInputLayout::create_InputElement_float4("COLOR", 0));
	shaderInputLayout.push_InputElement(ShaderInputLayout::create_InputElement_float2("TEXCOORD", 0));
	shaderInputLayout.create(renderer, vertexShader0);

	Shader pixelShader0;
	pixelShader0.create(renderer, kPixelShaderCode, ShaderType::PixelShader, "PixelShader0", "main", "ps_5_0", &shaderHeaderSet);

	Resource vscbMatrices;
	CB_MATRICES cb_matrices;
	cb_matrices._projectionMatrix.make_pixel_coordinates_projection_matrix(kScreenSize);
	//cb_matrices._projectionMatrix.make_perspective_projection_matrix(kPi * 0.25f, 0.001f, 1000.0f, kScreenSize.x / kScreenSize.y);
	vscbMatrices.create_buffer(renderer, ResourceType::ConstantBuffer, &cb_matrices, sizeof(CB_MATRICES), 1);

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
	vertexBuffer._type = ResourceType::VertexBuffer;
	Resource indexBuffer;
	indexBuffer._type = ResourceType::IndexBuffer;
	std::vector<VS_INPUT> vertices;
	std::vector<uint32> indices;
	uint32 mode = 0;
	uint32 selection = 0;
	float2 initial_direction = float2(1, 0);
	const Color white_color = Color(1, 1, 1, 1);
	const Color orange_color = Color(1, 0.5f, 0, 1);
	const Color yellow_color = Color(1, 1, 0, 1);
	const Color dark_gray_color = Color(0.25f, 0.25f, 0.25f, 1);
	const Color blue_color = Color(0, 0, 1, 1);
	const Color magenta_color = float4(1, 0, 1, 1);
	const float2 minkowski_shape_offset = kScreenSize * 0.5f + float2(100, 100);
	while (renderer.is_running())
	{
		if (renderer.get_keyboard_char() == 'w')
		{
			++GJK::g_max_step;
		}
		else if (renderer.get_keyboard_char() == 'q')
		{
			if (GJK::g_max_step > 0)
			{
				--GJK::g_max_step;
			}
		}
		else if (renderer.get_keyboard_char() == 'e')
		{
			mode = 0;
		}
		else if (renderer.get_keyboard_char() == 'r')
		{
			mode = 1;
		}
		else if (renderer.get_keyboard_char() == '1')
		{
			selection = 0;
		}
		else if (renderer.get_keyboard_char() == '2')
		{
			selection = 1;
		}
		else if (renderer.get_keyboard_char() == '3')
		{
			selection = 2;
		}
		else if (renderer.get_keyboard_char() == '0')
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

		if (renderer.get_keyboard_up_key() == Renderer::Key::Enter || is_shapes_loaded == false)
		{
			std::string shapes_content;
			read_file("shapes.txt", shapes_content);

			XML xml;
			if (xml.parse(shapes_content) == true)
			{
				uint32 shape_index = 0;
				const XML::Node& root_node = xml.get_root_node();
				for (const auto& root_child_node_ID : root_node._child_node_IDs)
				{
					const XML::Node& shape_node = xml.get_node(root_child_node_ID);
					for (const auto& shape_child_node_ID : shape_node._child_node_IDs)
					{
						const XML::Node& shape_child_node = xml.get_node(shape_child_node_ID);
						if (shape_child_node.get_name() == "center")
						{
							XML::Attribute attribute = shape_child_node.get_attribute(0);
							const float x = std::stof(attribute.get_value());
							attribute = attribute.get_next_attribute();
							const float y = std::stof(attribute.get_value());

							positions_source[shape_index] = float2(x, y);
						}
						else if (shape_child_node.get_name() == "points")
						{
							shape_sources[shape_index]._points.clear();

							for (XML::Node point_node = shape_child_node.get_child_node(0); point_node.is_valid(); point_node = point_node.get_next_sibling())
							{
								XML::Attribute attribute = point_node.get_attribute(0);
								const float x = std::stof(attribute.get_value());
								attribute = attribute.get_next_attribute();
								const float y = std::stof(attribute.get_value());

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

		if (renderer.is_mouse_L_button_pressed())
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
		if (renderer.is_mouse_L_button_down())
		{
			if (mode == 0)
			{
				if (selection <= 1)
				{
					positions[selection].x = positions_prev[selection].x + renderer.get_mouse_move_delta().x;
					positions[selection].y = positions_prev[selection].y + renderer.get_mouse_move_delta().y;
				}
			}
			else
			{
				const float theta = (renderer.get_mouse_move_delta().x + renderer.get_mouse_move_delta().y) * 0.03125f;
				thetas[selection] = thetas_prev[selection] + theta;
			}
		}

		if (selection <= 1)
		{
			shape_sources[selection]._center = positions[selection];
			shapes[selection]._center = shape_sources[selection]._center;

			shapes[selection] = shape_sources[selection];
			shapes[selection].rotate(thetas[selection]);
		}
		else
		{
			const quaternion rotation = quaternion::make_from_axis_angle(float3(0, 0, -1), thetas[selection]);
			initial_direction = rotation.rotate(float2(1, 0));
		}

		const float2 minkowski_shape_center_in_minkowski_space = shapes[0]._center - shapes[1]._center;
		const float2 minkowski_space_origin = kScreenSize * 0.5f + float2(0, 120);
		shape_Minkowski.make_Minkowski_difference_shape(shapes[0], shapes[1]);
		shape_Minkowski._center = minkowski_space_origin + minkowski_shape_center_in_minkowski_space;

		renderer.begin_rendering();
		{
			{
				vertices.clear();
				indices.clear();

				GJK::DebugData debugData;
				const bool intersected = GJK::intersects(shapes[0], shapes[1], initial_direction, &debugData);

				const Color shape_color = (intersected ? Color(0, 1, 0, 1) : white_color);
				MeshGenerator<VS_INPUT>::push_2D_circle(white_color, shapes[0]._center, 4.0f, 8, vertices, indices);
				MeshGenerator<VS_INPUT>::push_2D_circle(white_color, shapes[1]._center, 4.0f, 8, vertices, indices);
				shapes[0].draw_points_to(shape_color, vertices, indices);
				shapes[0].draw_line_semgments_to(shape_color, vertices, indices);
				shapes[1].draw_points_to(shape_color, vertices, indices);
				shapes[1].draw_line_semgments_to(shape_color, vertices, indices);

				shape_Minkowski.draw_points_to(dark_gray_color, vertices, indices);
				shape_Minkowski.draw_line_semgments_to(dark_gray_color, vertices, indices);
				MeshGenerator<VS_INPUT>::push_2D_circle(Color(0.5f, 1.0f, 0.25f, 1.0f), minkowski_space_origin + debugData._simplex.get_closest_point_to_origin(), 8.0f, 8, vertices, indices);

				{
					const Color color_latest = Color(0.5f, 0, 1, 1);
					const Color color_shape_a = orange_color;
					const Color color_shape_b = blue_color;
					const float2& support_a = shapes[0].support(debugData._direction);
					const float2& support_b = shapes[1].support(-debugData._direction);
					MeshGenerator<VS_INPUT>::push_2D_circle(color_latest, support_a, 4.0f, 8, vertices, indices);
					MeshGenerator<VS_INPUT>::push_2D_circle(color_latest, support_b, 4.0f, 8, vertices, indices);
					MeshGenerator<VS_INPUT>::push_2D_arrow(color_shape_a, shapes[0]._center, support_a, 2.0f, 0.125f, 2.0f, vertices, indices);
					MeshGenerator<VS_INPUT>::push_2D_arrow(color_shape_b, shapes[1]._center, support_b, 2.0f, 0.125f, 2.0f, vertices, indices);
					MeshGenerator<VS_INPUT>::push_2D_arrow(color_latest, shapes[0]._center, shapes[0]._center + debugData._direction * 32.0f, 2.0f, 0.25f, 3.0f, vertices, indices);
					MeshGenerator<VS_INPUT>::push_2D_arrow(color_latest, shapes[1]._center, shapes[1]._center - debugData._direction * 32.0f, 2.0f, 0.25f, 3.0f, vertices, indices);

					const float2 support_a_from_o = support_a - shapes[0]._center;
					const float2 support_b_from_o = shapes[1]._center - support_b;
					debugData._simplex.draw_to(magenta_color, color_latest, minkowski_space_origin, vertices, indices);
					MeshGenerator<VS_INPUT>::push_2D_circle(white_color, shape_Minkowski._center, 4.0f, 8, vertices, indices);
					MeshGenerator<VS_INPUT>::push_2D_arrow(color_shape_a, shape_Minkowski._center, shape_Minkowski._center + support_a_from_o, 2.0f, 0.125f, 2.0f, vertices, indices);
					MeshGenerator<VS_INPUT>::push_2D_arrow(color_shape_b, shape_Minkowski._center + support_a_from_o, shape_Minkowski._center + support_a_from_o + support_b_from_o, 2.0f, 0.125f, 2.0f, vertices, indices);
					MeshGenerator<VS_INPUT>::push_2D_arrow(color_latest, shape_Minkowski._center, shape_Minkowski._center + debugData._direction * 32.0f, 2.0f, 0.25f, 3.0f, vertices, indices);

					MeshGenerator<VS_INPUT>::push_2D_arrow(white_color, minkowski_space_origin - float2(200, 0), minkowski_space_origin + float2(200, 0), 1.0f, 0.0625f, 4.0f, vertices, indices);
					MeshGenerator<VS_INPUT>::push_2D_arrow(white_color, minkowski_space_origin + float2(0, 200), minkowski_space_origin - float2(0, 200), 1.0f, 0.0625f, 4.0f, vertices, indices);
				}

				vertexBuffer.update(renderer, &vertices[0], sizeof(VS_INPUT), (uint32)vertices.size());
				indexBuffer.update(renderer, &indices[0], sizeof(uint32), (uint32)indices.size());
			}

			renderer.bind_Shader(vertexShader0);
			renderer.bind_ShaderInputLayout(shaderInputLayout);
			renderer.bind_Shader(pixelShader0);
			renderer.bind_input(vertexBuffer, 0);
			renderer.bind_input(indexBuffer, 0);
			renderer.bind_ShaderResource(ShaderType::VertexShader, vscbMatrices, 0);
			renderer.draw_indexed((uint32)indices.size());

			renderer.draw_text(Color(0, 1, 1, 1), "GJK Algorithm Test", float2(10, 10));
			renderer.draw_text((selection == 0 ? yellow_color : white_color), "1: shape A", float2(10, 40));
			renderer.draw_text((selection == 1 ? yellow_color : white_color), "2: shape B", float2(10, 60));
			renderer.draw_text((selection == 2 ? yellow_color : white_color), "3: initial direction", float2(10, 80));
			renderer.draw_text((selection == 2 ? yellow_color : white_color), "0: reset", float2(10, 100));

			renderer.draw_text((mode == 0 ? yellow_color : white_color), "e: translate", float2(10, 140));
			renderer.draw_text((mode == 1 ? yellow_color : white_color), "r: rotate", float2(10, 160));
			renderer.draw_text(white_color, "current gjk_max_step: " + std::to_string(GJK::g_max_step), float2(10, 180));
			renderer.draw_text(white_color, "q: --gjk_max_step", float2(10, 200));
			renderer.draw_text(white_color, "w: ++gjk_max_step", float2(10, 220));

			renderer.draw_text(white_color, "ENTER: load shapes from file", float2(10, 260));

			//char buffer[8]{};
			//for (size_t i = 0; i < shapeMinkowski._points.size(); ++i)
			//{
			//    ::_itoa_s(static_cast<int>(i), buffer, 10);
			//    const auto& point = shapeMinkowski._points[i];
			//    renderer.draw_text(buffer, shapeMinkowski._center + point);
			//}
		}
		renderer.end_rendering();
	}
	return 0;
}
