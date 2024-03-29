#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <Windows.h>
#include <windowsx.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <unordered_map>
#include <d3dcompiler.h>
#include <fstream>

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
	struct float4;
	struct quaternion;
	class Renderer;
	struct Shader;
#pragma endregion

#pragma region Constants
	constexpr float kPi = 3.14159265f;
	constexpr float k2Pi = kPi * 2.0f;
	constexpr float kPiOver2 = kPi * 0.5f;

	// ### Copy and paste to see this better ###
	// Replace ', ' with '_'
	// Replace '0b' with '.'
	// Replace  '0' with ' '
	// Size: 16 x 60
	static constexpr uint32 kFontTextureWidth = 128;
	static constexpr uint32 kFontTextureHeight = 60;
	static constexpr uint32 kFontTextureByteCount = kFontTextureWidth * kFontTextureHeight;
	static constexpr uint32 kFontTextureGlyphWidth = 8;
	static constexpr uint32 kFontTextureGlyphHeight = 10;
	static constexpr uint32 kFontTextureGlyphCountInRow = kFontTextureWidth / kFontTextureGlyphWidth;
	static constexpr const byte kFontTextureRawBitData[kFontTextureByteCount / 8]
	{
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00010000, 0b00101000, 0b00100100, 0b00010000, 0b00000000, 0b00000000, 0b00010000, 0b00001000, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010,
		0b00000000, 0b00010000, 0b00101000, 0b00100100, 0b00111100, 0b01100010, 0b00110000, 0b00010000, 0b00010000, 0b00010000, 0b01000100, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000100,
		0b00000000, 0b00010000, 0b00000000, 0b01111110, 0b01010000, 0b10010100, 0b01001000, 0b00000000, 0b00100000, 0b00001000, 0b00101000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00001000,
		0b00000000, 0b00010000, 0b00000000, 0b00100100, 0b00111100, 0b01101000, 0b00110010, 0b00000000, 0b00100000, 0b00001000, 0b11111110, 0b11111110, 0b00000000, 0b11111110, 0b00000000, 0b00010000,
		0b00000000, 0b00010000, 0b00000000, 0b01111110, 0b00010100, 0b00010110, 0b01001100, 0b00000000, 0b00100000, 0b00001000, 0b00101000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00100000,
		0b00000000, 0b00000000, 0b00000000, 0b00100100, 0b01111000, 0b00101010, 0b10001100, 0b00000000, 0b00010000, 0b00010000, 0b01000100, 0b00010000, 0b00001000, 0b00000000, 0b00011000, 0b01000000,
		0b00000000, 0b00010000, 0b00000000, 0b00100100, 0b00010000, 0b01000110, 0b01110010, 0b00000000, 0b00001000, 0b00100000, 0b00000000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b10000000,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b01111100, 0b00010000, 0b01111100, 0b01111100, 0b00001000, 0b11111110, 0b01111100, 0b11111110, 0b01111100, 0b01111100, 0b00000000, 0b00000000, 0b00001000, 0b00000000, 0b01000000, 0b01111100,
		0b10000110, 0b00110000, 0b10000010, 0b10000010, 0b00011000, 0b10000000, 0b10000010, 0b00000100, 0b10000010, 0b10000010, 0b00010000, 0b00010000, 0b00010000, 0b11111110, 0b00100000, 0b10000010,
		0b10001010, 0b00010000, 0b00000010, 0b00000010, 0b00101000, 0b10111100, 0b10000000, 0b00001000, 0b10000010, 0b10000010, 0b00000000, 0b00000000, 0b00100000, 0b00000000, 0b00010000, 0b00000010,
		0b10010010, 0b00010000, 0b00000100, 0b01111100, 0b01001000, 0b11000010, 0b10111100, 0b00010000, 0b01111100, 0b01111110, 0b00000000, 0b00000000, 0b01000000, 0b00000000, 0b00001000, 0b00011100,
		0b10100010, 0b00010000, 0b00011000, 0b00000010, 0b11111110, 0b00000010, 0b11000010, 0b00100000, 0b10000010, 0b00000010, 0b00010000, 0b00010000, 0b00100000, 0b11111110, 0b00010000, 0b00010000,
		0b11000010, 0b00010000, 0b01000000, 0b10000010, 0b00001000, 0b10000010, 0b10000010, 0b00100000, 0b10000010, 0b10000010, 0b00000000, 0b00100000, 0b00010000, 0b00000000, 0b00100000, 0b00000000,
		0b01111100, 0b00111000, 0b11111110, 0b01111100, 0b00011100, 0b01111100, 0b01111100, 0b00100000, 0b01111100, 0b01111100, 0b00000000, 0b00000000, 0b00001000, 0b00000000, 0b01000000, 0b00010000,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b00111000, 0b00010000, 0b11111100, 0b01111100, 0b11111100, 0b11111110, 0b11111110, 0b01111100, 0b10000010, 0b00111000, 0b00011100, 0b10000100, 0b10000000, 0b11000110, 0b11000010, 0b01111100,
		0b01000100, 0b00101000, 0b10000010, 0b10000010, 0b10000010, 0b10000000, 0b10000000, 0b10000000, 0b10000010, 0b00010000, 0b00001000, 0b10001000, 0b10000000, 0b10101010, 0b10100010, 0b10000010,
		0b10111010, 0b01000100, 0b10000010, 0b10000000, 0b10000010, 0b10000000, 0b10000000, 0b10000000, 0b10000010, 0b00010000, 0b00001000, 0b10010000, 0b10000000, 0b10010010, 0b10100010, 0b10000010,
		0b10101010, 0b01111100, 0b11111100, 0b10000000, 0b10000010, 0b11111110, 0b11111110, 0b10000110, 0b11111110, 0b00010000, 0b00001000, 0b11100000, 0b10000000, 0b10010010, 0b10010010, 0b10000010,
		0b10101110, 0b10000010, 0b10000010, 0b10000000, 0b10000010, 0b10000000, 0b10000000, 0b10000010, 0b10000010, 0b00010000, 0b00001000, 0b10010000, 0b10000000, 0b10000010, 0b10010010, 0b10000010,
		0b01010100, 0b10000010, 0b10000010, 0b10000010, 0b10000010, 0b10000000, 0b10000000, 0b10000010, 0b10000010, 0b00010000, 0b10001000, 0b10001000, 0b10000000, 0b10000010, 0b10001010, 0b10000010,
		0b00111010, 0b10000010, 0b11111100, 0b01111100, 0b11111100, 0b11111110, 0b10000000, 0b01111100, 0b10000010, 0b00111000, 0b01110000, 0b10000100, 0b11111110, 0b10000010, 0b10000110, 0b01111100,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b11111100, 0b01111100, 0b11111100, 0b01111100, 0b11111110, 0b10000010, 0b10000010, 0b10010010, 0b10000010, 0b10000010, 0b11111110, 0b00111000, 0b10000000, 0b00111000, 0b00010000, 0b00000000,
		0b10000010, 0b10000010, 0b10000010, 0b10000010, 0b00010000, 0b10000010, 0b10000010, 0b10010010, 0b01000100, 0b01000100, 0b00000100, 0b00100000, 0b01000000, 0b00001000, 0b00101000, 0b00000000,
		0b10000010, 0b10000010, 0b10000010, 0b10000000, 0b00010000, 0b10000010, 0b10000010, 0b10010010, 0b00101000, 0b00101000, 0b00001000, 0b00100000, 0b00100000, 0b00001000, 0b01000100, 0b00000000,
		0b11111100, 0b10000010, 0b11111100, 0b11111100, 0b00010000, 0b10000010, 0b01000100, 0b10010010, 0b00010000, 0b00010000, 0b00010000, 0b00100000, 0b00010000, 0b00001000, 0b00000000, 0b00000000,
		0b10000000, 0b10001010, 0b10000100, 0b00000010, 0b00010000, 0b10000010, 0b01000100, 0b10010010, 0b00101000, 0b00010000, 0b00100000, 0b00100000, 0b00001000, 0b00001000, 0b00000000, 0b00000000,
		0b10000000, 0b10000100, 0b10000010, 0b10000010, 0b00010000, 0b10000010, 0b00101000, 0b10010010, 0b01000100, 0b00010000, 0b01000000, 0b00100000, 0b00000100, 0b00001000, 0b00000000, 0b00000000,
		0b10000000, 0b01111010, 0b10000010, 0b01111100, 0b00010000, 0b01111100, 0b00010000, 0b01101100, 0b10000010, 0b00010000, 0b11111110, 0b00111000, 0b00000010, 0b00111000, 0b00000000, 0b01111100,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b00100000, 0b00000000, 0b01000000, 0b00000000, 0b00000100, 0b00000000, 0b00001100, 0b00000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000000,
		0b00010000, 0b00000000, 0b01000000, 0b00000000, 0b00000100, 0b00000000, 0b00010000, 0b00000000, 0b01000000, 0b00010000, 0b00001000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000000,
		0b00001000, 0b01110100, 0b01011000, 0b00111000, 0b00110100, 0b00111000, 0b00010000, 0b00110100, 0b01000000, 0b00000000, 0b00000000, 0b01000100, 0b00010000, 0b10101000, 0b10111000, 0b00111000,
		0b00000000, 0b10001100, 0b01100100, 0b01000100, 0b01001100, 0b01000100, 0b01111100, 0b01001100, 0b01011000, 0b00010000, 0b00001000, 0b01001000, 0b00010000, 0b01010100, 0b01000100, 0b01000100,
		0b00000000, 0b10000100, 0b01000100, 0b01000000, 0b01000100, 0b01111100, 0b00010000, 0b01000100, 0b01100100, 0b00010000, 0b00001000, 0b01110000, 0b00010000, 0b01010100, 0b01000100, 0b01000100,
		0b00000000, 0b10001100, 0b01000100, 0b01000100, 0b01000100, 0b01000000, 0b00010000, 0b00111100, 0b01000100, 0b00010000, 0b00001000, 0b01001000, 0b00010000, 0b01010100, 0b01000100, 0b01000100,
		0b00000000, 0b01110010, 0b01111000, 0b00111000, 0b00111100, 0b00111000, 0b00010000, 0b00000100, 0b01000100, 0b00011000, 0b01001000, 0b01000100, 0b00101100, 0b01010100, 0b01000100, 0b00111000,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00110000, 0b00111000, 0b00000000, 0b00000000, 0b00110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11111110,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00011000, 0b00010000, 0b00110000, 0b00000000, 0b11000110,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100000, 0b00010000, 0b00001000, 0b00000000, 0b10101010,
		0b01011100, 0b01110100, 0b01011000, 0b00111100, 0b01111100, 0b01000100, 0b01000100, 0b01000100, 0b01000100, 0b10001000, 0b11111000, 0b00100000, 0b00010000, 0b00001000, 0b11100000, 0b10010010,
		0b01100010, 0b10001100, 0b01100100, 0b01000010, 0b00010000, 0b01000100, 0b01000100, 0b01010100, 0b00101000, 0b10001000, 0b00010000, 0b01100000, 0b00010000, 0b00001100, 0b10010010, 0b10010010,
		0b01000010, 0b10000100, 0b01000000, 0b00111000, 0b00010000, 0b01000100, 0b01000100, 0b01010100, 0b00010000, 0b10001000, 0b00100000, 0b00100000, 0b00010000, 0b00001000, 0b00001110, 0b10101010,
		0b01111100, 0b01111100, 0b01000000, 0b00000110, 0b00010010, 0b01000100, 0b00101000, 0b01010100, 0b00101000, 0b01111000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000000, 0b11000110,
		0b01000000, 0b00000100, 0b01000000, 0b01111100, 0b00001100, 0b00111010, 0b00010000, 0b00101000, 0b01000100, 0b00001000, 0b11111000, 0b00011000, 0b00010000, 0b00110000, 0b00000000, 0b10000010,
		0b01000000, 0b00000100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11111110,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
	};
#pragma endregion

	struct float2
	{
		constexpr float2() : float2(0, 0) { __noop; }
		constexpr explicit float2(float s) : x{ s }, y{ s } { __noop; }
		constexpr float2(float x_, float y_) : x{ x_ }, y{ y_ } { __noop; }
		float& operator[](const uint32 index) { return f[index]; }
		const float& operator[](const uint32 index) const { return f[index]; }
		constexpr float2 operator+() const { return *this; }
		constexpr float2 operator-() const { return float2(-x, -y); }
		float2& operator+=(const float2& rhs) { x += rhs.x; y += rhs.y; return *this; }
		float2& operator-=(const float2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
		constexpr float2 operator+(const float2& rhs) const { return float2(x + rhs.x, y + rhs.y); }
		constexpr float2 operator-(const float2& rhs) const { return float2(x - rhs.x, y - rhs.y); }
		float2& operator*=(const float s) { x *= s; y *= s; return *this; }
		float2& operator/=(const float s) { x /= s; y /= s; return *this; }
		constexpr float2 operator*(const float s) const { return float2(x * s, y * s); }
		constexpr float2 operator/(const float s) const { return float2(x / s, y / s); }
		constexpr float dot(const float2& rhs) const { return x * rhs.x + y * rhs.y; }
		constexpr float length_sq() const { return dot(*this); }
		float length() const { return ::sqrt(length_sq()); }
		void normalize() { *this /= length(); }
		union { struct { float x; float y; }; float f[2]; };
		float2 operator*(const float2& rhs) { return float2(x * rhs.x, y * rhs.y); }
	};
	struct float3
	{
		constexpr float3() : float3(0, 0, 0) { __noop; }
		constexpr float3(const float2& rhs) : x{ rhs.x }, y{ rhs.y }, z{ 0 } { __noop; }
		constexpr float3(float x_, float y_, float z_) : x{ x_ }, y{ y_ }, z{ z_ } { __noop; }
		operator float2() const { return float2(x, y); }
		float& operator[](const uint32 index) { return f[index]; }
		const float& operator[](const uint32 index) const { return f[index]; }
		float3 operator+() const { return *this; }
		float3 operator-() const { return float3(-x, -y, -z); }
		float3 operator+(const float3& rhs) const { return float3(x + rhs.x, y + rhs.y, z + rhs.z); }
		float3 operator-(const float3& rhs) const { return *this + (-rhs); }
		float3 operator*(const float s) const { return float3(x * s, y * s, z * s); }
		float3 operator/(const float s) const { return float3(x / s, y / s, z / s); }
		float3& operator+=(const float3& rhs) { *this = (*this + rhs); return *this; }
		float3& operator-=(const float3& rhs) { *this = (*this - rhs); return *this; }
		float3& operator*=(const float s) { *this = (*this * s); return *this; }
		float3& operator/=(const float s) { *this = (*this / s); return *this; }
		constexpr float dot(const float3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
		constexpr float3 cross(const float3& rhs) const { return float3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x); }
		constexpr float length_sq() const { return dot(*this); }
		float length() const { return ::sqrt(length_sq()); }
		void normalize() { *this /= length(); }
		float3 compute_normalized() const { float3 result = *this; result.normalize(); return result; }
		void set_point(const float2& position) { x = position.x; y = position.y; z = 0; }
		union { struct { float x; float y; float z; }; float f[3]; };
	};
	struct float4
	{
		constexpr float4() : float4(0, 0, 0, 0) { __noop; }
		constexpr float4(const float2& rhs) : x{ rhs.x }, y{ rhs.y }, z{ 0 }, w{ 1 } { __noop; }
		constexpr float4(float x_, float y_, float z_, float w_) : x{ x_ }, y{ y_ }, z{ z_ }, w{ w_ } { __noop; }
		operator float2() const { return float2(x, y); }
		float& operator[](const uint32 index) { return f[index]; }
		const float& operator[](const uint32 index) const { return f[index]; }
		float4 operator+() const { return *this; }
		float4 operator-() const { return float4(-x, -y, -z, -w); }
		float4 operator+(const float4& rhs) const { return float4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
		float4 operator-(const float4& rhs) const { return *this + (-rhs); }
		float4 operator*(const float s) const { return float4(x * s, y * s, z * s, w * s); }
		float4 operator/(const float s) const { return float4(x / s, y / s, z / s, w / s); }
		float4& operator+=(const float4& rhs) { *this = (*this + rhs); return *this; }
		float4& operator-=(const float4& rhs) { *this = (*this - rhs); return *this; }
		float4& operator*=(const float s) { *this = (*this * s); return *this; }
		float4& operator/=(const float s) { *this = (*this / s); return *this; }
		constexpr float dot(const float4& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w; }
		constexpr float length_sq() const { return dot(*this); }
		float length() const { return ::sqrt(length_sq()); }
		void normalize() { *this /= length(); }
		float4 compute_normalized() const { float4 result = *this; result.normalize(); return result; }
		void set_point(const float2& position) { x = position.x; y = position.y; z = 0; w = 1; }
		union { struct { float x; float y; float z; float w; }; float f[4]; };
	};
	// quaternion = xi + yj + zk + w
	struct quaternion
	{
		quaternion() : quaternion(0, 0, 0, 1) { __noop; }
		quaternion(float x_, float y_, float z_, float w_) : x{ x_ }, y{ y_ }, z{ z_ }, w{ w_ } { __noop; }
		quaternion& operator*=(const quaternion& rhs) noexcept
		{
			float x_ = +w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
			float y_ = +w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x;
			float z_ = +w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w;
			float w_ = +w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;
			x = x_;
			y = y_;
			z = z_;
			w = w_;
			return *this;
		}
		float4 rotate(const float4& v) const noexcept
		{
			quaternion q = *this;
			q *= quaternion(v.x, v.y, v.z, 0);
			q *= quaternion::conjugate(*this);
			return float4(q.x, q.y, q.z, v.w);
		}
		static quaternion make_from_axis_angle(float3 axis, const float angle) noexcept
		{
			axis.normalize();
			const float half_angle = angle * 0.5f;
			const float cos_half = ::cos(half_angle);
			const float sin_half = ::sin(half_angle);
			return quaternion(sin_half * axis.x, sin_half * axis.y, sin_half * axis.z, cos_half);
		}
		void get_axis_angle(float3& axis, float& angle) const noexcept
		{
			angle = ::acos(w) * 2.0f;

			const float sinHalfAngle = 1.0f - w * w;
			if (sinHalfAngle == 0.0f)
			{
				axis[0] = 1;
				axis[1] = 0;
				axis[2] = 0;
			}
			else
			{
				axis[0] = x / sinHalfAngle;
				axis[1] = y / sinHalfAngle;
				axis[2] = z / sinHalfAngle;

				const float normSq = axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2];
				const float norm = ::sqrt(normSq);
				axis[0] /= norm;
				axis[1] /= norm;
				axis[2] /= norm;
			}
		}
		static quaternion conjugate(const quaternion& q) noexcept { return quaternion(-q.x, -q.y, -q.z, q.w); }
		float x; float y; float z; float w;
	};
	struct float2x2
	{
		constexpr float2x2() : float2x2(1, 0, 0, 1) { __noop; }
		constexpr float2x2(float _11_, float _12_, float _21_, float _22_) : _m{ _11_, _12_, _21_, _22_ } { __noop; }
		constexpr float2x2(const float(&m)[4]) : _m{ m[0],m[1],m[2],m[3] } { __noop; }
		void set(float _11_, float _12_, float _21_, float _22_)
		{
			_11 = _11_; _12 = _12_; _21 = _21_; _22 = _22_;
		}
		void make_identity()
		{
			set(1.0f, 0.0f, 0.0f, 1.0f);
		}
		void make_zero()
		{
			set(0.0f, 0.0f, 0.0f, 0.0f);
		}
		void make_rotationMatrix(const float theta)
		{
			const float cosTheta = ::cos(theta);
			const float sinTheta = ::sin(theta);
			set(cosTheta, sinTheta, -sinTheta, cosTheta);
		}
		float2 operator*(const float2& rhs) const
		{
			return float2(_11 * rhs.x + _12 * rhs.y, _21 * rhs.x + _22 * rhs.y);
		}
		union {
			float _m[4]; float2 _rows[2];
			struct {
				float _11; float _12;
				float _21; float _22;
			};
		};
	};
	struct float4x4
	{
		constexpr float4x4() : float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) { __noop; }
		constexpr float4x4(const float4(&rows)[4]) : _rows{ rows[0], rows[1], rows[2], rows[3] } { __noop; }
		constexpr float4x4(float _11_, float _12_, float _13_, float _14_, float _21_, float _22_, float _23_, float _24_, float _31_, float _32_, float _33_, float _34_, float _41_, float _42_, float _43_, float _44_) : _m{ _11_, _12_, _13_, _14_, _21_, _22_, _23_, _24_, _31_, _32_, _33_, _34_, _41_, _42_, _43_, _44_ } { __noop; }
		constexpr float4x4(const float(&m)[16]) : _m{ m[0],m[1],m[2],m[3],m[4],m[5],m[6],m[7],m[8],m[9],m[10],m[11],m[12],m[13],m[14],m[15] } { __noop; }
		void set(float _11_, float _12_, float _13_, float _14_, float _21_, float _22_, float _23_, float _24_, float _31_, float _32_, float _33_, float _34_, float _41_, float _42_, float _43_, float _44_)
		{
			_11 = _11_; _12 = _12_; _13 = _13_; _14 = _14_; _21 = _21_; _22 = _22_; _23 = _23_; _24 = _24_; _31 = _31_; _32 = _32_; _33 = _33_; _34 = _34_; _41 = _41_; _42 = _42_; _43 = _43_; _44 = _44_;
		}
		void make_identity()
		{
			_11 = 1; _12 = 0; _13 = 0; _14 = 0; _21 = 0; _22 = 1; _23 = 0; _24 = 0; _31 = 0; _32 = 0; _33 = 1; _34 = 0; _41 = 0; _42 = 0; _43 = 0; _44 = 1;
		}
		void make_zero()
		{
			_11 = 0; _12 = 0; _13 = 0; _14 = 0; _21 = 0; _22 = 0; _23 = 0; _24 = 0; _31 = 0; _32 = 0; _33 = 0; _34 = 0; _41 = 0; _42 = 0; _43 = 0; _44 = 0;
		}
		void make_pixel_coordinates_projection_matrix(const float2& screenSize)
		{
			make_identity();
			_11 = 2.0f / screenSize.x; _14 = -1.0f; _22 = -2.0f / screenSize.y; _24 = 1.0f;
		}
		void make_perspective_projection_matrix(const float FOVAngle, const float nearDepthAbs, const float farDepthAbs, const float screenWidthOverHeight)
		{
			make_zero();
			bool isRightHanded = true;
			const float halfFOVAngle = FOVAngle * 0.5f;
			const float a = 1.0f / (tanf(halfFOVAngle) * screenWidthOverHeight);
			const float b = 1.0f / (tanf(halfFOVAngle));
			const float c = (farDepthAbs / (nearDepthAbs - farDepthAbs)) * (isRightHanded ? +1.0f : -1.0f);
			const float d = (farDepthAbs * nearDepthAbs) / (nearDepthAbs - farDepthAbs);
			const float e = (isRightHanded ? -1.0f : +1.0f);
			_11 = a;
			_22 = b;
			_33 = c;
			_34 = d;
			_43 = e;
		}
		void pre_translate(const float x, const float y, const float z) noexcept
		{
			_14 += x; _24 += y; _34 += z;
		}
		void post_scale(const float x, const float y, const float z) noexcept
		{
			_11 *= x; _12 *= y; _13 *= z;
			_21 *= x; _22 *= y; _23 *= z;
			_31 *= x; _32 *= y; _33 *= z;
			_41 *= x; _42 *= y; _43 *= z;
		}
		static float4x4 create_rotation_matrix(const quaternion& q)
		{
			float3 axis;
			float angle;
			q.get_axis_angle(axis, angle);
			return create_rotation_matrix(axis, angle);
		}
		static float4x4 create_rotation_matrix(const float3& axis, const float angle)
		{
			// (v * r)r(1 - cosθ) + vcosθ + (r X v)sinθ
			const float3 r = axis.compute_normalized();
			const float c = ::cosf(angle);
			const float s = ::sinf(angle);
			const float rx = r.x;
			const float ry = r.y;
			const float rz = r.z;
			float4x4 result
			(
				(1 - c) * rx * rx + c, (1 - c) * ry * rx - (rz * s), (1 - c) * rz * rx + (ry * s), 0,
				(1 - c) * rx * ry + (rz * s), (1 - c) * ry * ry + c, (1 - c) * rz * ry - (rx * s), 0,
				(1 - c) * rx * rz - (ry * s), (1 - c) * ry * rz + (rx * s), (1 - c) * rz * rz + c, 0,
				0, 0, 0, 1
			);
			return result;
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
	};
	struct Transform2D
	{
		Transform2D() : Transform2D(0.0f) { __noop; }
		Transform2D(const float rotation) : Transform2D(rotation, float2(0.0f, 0.0f)) { __noop; }
		Transform2D(const float rotation, const float2& translation) : _scale{ 1.0f, 1.0f }, _rotation{ rotation }, _translation{ translation } { __noop; }
		Transform2D& operator*=(const Transform2D& rhs)
		{
			MINT_ASSERT(_scale.x == _scale.y, "Shear is not supported for Transform2D!!!");

			float2x2 rotationMatrix;
			rotationMatrix.make_rotationMatrix(_rotation);

			float2 t = rotationMatrix * rhs._translation;
			_translation += (rotationMatrix * rhs._translation) * _scale;

			_rotation += rhs._rotation;

			return *this;
		}
		static Transform2D interpolate(const Transform2D& a, const Transform2D& b, const float t)
		{
			const float factorA = (1.0f - t);
			const float factorB = t;
			Transform2D result;
			result._scale = a._scale * factorA + b._scale * factorB;
			result._rotation = a._rotation * factorA + b._rotation * factorB;
			result._translation = a._translation * factorA + b._translation * factorB;
			return result;
		}
		float2 _scale;
		float _rotation;
		float2 _translation;
	};
	struct Transform
	{
		float4x4 create_float4x4() const
		{
			// SRT matrix for column vector is like below:
			// SRT = T * R * S
			// which is the same as below..
			float4x4 matrix = float4x4::create_rotation_matrix(_rotation);
			matrix.pre_translate(_translation.x, _translation.y, _translation.z);
			matrix.post_scale(_scale.x, _scale.y, _scale.z);
			return matrix;
		}
		void make_from_float4x4(const float4x4 m) noexcept
		{
			// SRT Matrix
			// 
			// | s_x * r_11  s_y * r_12  s_z * r_13  t_x |
			// | s_x * r_21  s_y * r_22  s_z * r_23  t_y |
			// | s_x * r_31  s_y * r_32  s_z * r_33  t_z |
			// | 0           0           0           1   |

			// s
			_scale.x = ::sqrtf((m._11 * m._11) + (m._21 * m._21) + (m._31 * m._31));
			_scale.y = ::sqrtf((m._12 * m._12) + (m._22 * m._22) + (m._32 * m._32));
			_scale.z = ::sqrtf((m._13 * m._13) + (m._23 * m._23) + (m._33 * m._33));

			// r
			float4x4 rotationMatrix;
			rotationMatrix._11 = m._11 / _scale.x; rotationMatrix._12 = m._12 / _scale.y; rotationMatrix._13 = m._13 / _scale.z;
			rotationMatrix._21 = m._21 / _scale.x; rotationMatrix._22 = m._22 / _scale.y; rotationMatrix._23 = m._23 / _scale.z;
			rotationMatrix._31 = m._31 / _scale.x; rotationMatrix._32 = m._32 / _scale.y; rotationMatrix._33 = m._33 / _scale.z;

			// t
			_translation.x = m._14;
			_translation.y = m._24;
			_translation.z = m._34;
		}
		float3 _scale = float3(1, 1, 1);
		quaternion _rotation;
		float3 _translation;
	};
	using Color = float4;

	enum class ShaderType
	{
		VertexShader,
		PixelShader,
	};
	enum class TextureFormat
	{
		R8_UNORM,
		R8G8B8A8_UNORM,
	};
	enum class ResourceType
	{
		VertexBuffer,
		IndexBuffer,
		ConstantBuffer,
		//StructuredBuffer,
		Teture2D,
	};

	struct alignas(float) DEFAULT_FONT_VS_INPUT
	{
		float4 _position;
		float4 _color;
		float2 _texcoord;
	};

	struct DEFAULT_FONT_CB_MATRICES
	{
		float4x4 _projectionMatrix;
	};

	const char kDefaultFontShaderHeaderCode[] =
		R"(
        struct DEFAULT_FONT_VS_INPUT
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

	const char kDefaultFontVertexShaderCode[] =
		R"(
        #include "DefaultFontShaderHeader"
    
        cbuffer DEFAULT_CB_MATRICES
        {
            float4x4 g_cbProjectionMatrix;
        };
    
        VS_OUTPUT main(DEFAULT_FONT_VS_INPUT input)
        {
            VS_OUTPUT output;
            output.screenPosition = mul(input.position, g_cbProjectionMatrix);
            output.screenPosition /= output.screenPosition.w;
            output.color = input.color;
            output.texcoord = input.texcoord;
            return output;
        }
    )";

	const char kDefaultFontPixelShaderCode[] =
		R"(
        #include "DefaultFontShaderHeader"
    
        sampler g_sampler0;
        Texture2D<float4> g_texture0;
        float4 main(VS_OUTPUT input) : SV_Target
        {
            float4 sampled = g_texture0.Sample(g_sampler0, input.texcoord.xy).r;
            clip (sampled.a - 0.125);
            float4 sampledBold = sqrt(sqrt(sampled));
            return float4(sampledBold) * input.color;
        }
    )";

	struct DefaultFontGlyphMeta
	{
		DefaultFontGlyphMeta() :DefaultFontGlyphMeta(0, 0, 0, 1, 1) { __noop; }
		DefaultFontGlyphMeta(byte ch, float u0, float v0, float u1, float v1) : _ch{ ch }, _u0{ u0 }, _v0{ v0 }, _u1{ u1 }, _v1{ v1 } { __noop; }

		byte _ch;
		float _u0;
		float _v0;
		float _u1;
		float _v1;
	};

	class DefaultFontData
	{
	public:
		void push_glyph(const DefaultFontGlyphMeta& glyphMeta);
		const DefaultFontGlyphMeta& get_GlyphMeta(const byte& ch) const;

	private:
		std::vector<DefaultFontGlyphMeta> _glyphMetas;
		std::unordered_map<byte, uint64> _glyphMap;
	};

	struct ShaderHeaderSet : public ID3DInclude
	{
	public:
		ShaderHeaderSet() = default;
		virtual ~ShaderHeaderSet() = default;

	public:
		void push_shader_header(const std::string& headerName, const std::string& headerCode);

	public:
		virtual HRESULT WINAPI Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override final;
		virtual HRESULT WINAPI Close(LPCVOID pData) override final { return S_OK; }

	public:
		std::vector<std::string> _headerNames;
		std::vector<std::string> _headerCodes;
	};

	struct ShaderInputLayout
	{
		struct InputElement
		{
			DXGI_FORMAT _format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			uint32 _inputSlot = 0;
			D3D11_INPUT_CLASSIFICATION _inputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
			const char* _semanticName = nullptr;
			uint32 _semanticIndex = 0;
			uint32 _instanceStepRate = 0;
		};

		static InputElement create_InputElement_float4(const char* const semanticName, const uint32 semanticIndex) { return __create_InputElement_common(DXGI_FORMAT_R32G32B32A32_FLOAT, semanticName, semanticIndex); }
		static InputElement create_InputElement_float3(const char* const semanticName, const uint32 semanticIndex) { return __create_InputElement_common(DXGI_FORMAT_R32G32B32_FLOAT, semanticName, semanticIndex); }
		static InputElement create_InputElement_float2(const char* const semanticName, const uint32 semanticIndex) { return __create_InputElement_common(DXGI_FORMAT_R32G32_FLOAT, semanticName, semanticIndex); }
		static InputElement create_InputElement_float(const char* const semanticName, const uint32 semanticIndex) { return __create_InputElement_common(DXGI_FORMAT_R32_FLOAT, semanticName, semanticIndex); }

		void clear_InputElements();
		void push_InputElement(const InputElement& newInputElement);

		bool create(Renderer& renderer, const Shader& vertexShader);

	private:
		static InputElement __create_InputElement_common(const DXGI_FORMAT format, const char* const semanticName, const uint32 semanticIndex)
		{
			InputElement inputElement;
			inputElement._format = format;
			inputElement._semanticName = semanticName;
			inputElement._semanticIndex = semanticIndex;
			return inputElement;
		}

		uint32 compute_InputElement_byte_size(const D3D11_INPUT_ELEMENT_DESC& inputElementDesc)
		{
			switch (inputElementDesc.Format)
			{
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
				return 16;
			case DXGI_FORMAT_R32G32B32_FLOAT:
				return 12;
			case DXGI_FORMAT_R32G32_FLOAT:
				return 8;
			case DXGI_FORMAT_R32_FLOAT:
				return 4;
			default:
				break;
			}
			MINT_LOG_ERROR("!!!");
			return 0;
		}

	public:
		ComPtr<ID3D11InputLayout> _inputLayout;

	private:
		std::vector<D3D11_INPUT_ELEMENT_DESC> _inputElements;
		uint32 _inputTotalByteSize = 0;
	};

	struct Shader
	{
		bool create(Renderer& renderer, const char* sourceCode, const ShaderType& shaderType, const char* shaderIdentifier, const char* entryPoint, const char* target, ShaderHeaderSet* const shaderHeaderSet = nullptr);

		ShaderType _type = ShaderType::VertexShader;
		ComPtr<ID3D10Blob> _shaderBlob;
		ComPtr<ID3D10Blob> _errorMessageBlob;
		ComPtr<ID3D11DeviceChild> _shader;
	};

	// Buffer or Texture
	class Resource
	{
	public:
		//static constexpr DXGI_FORMAT kIndexBufferFormat = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
		static constexpr DXGI_FORMAT kIndexBufferFormat = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;

	public:
		Resource() : _type{ ResourceType::VertexBuffer }, _format{ TextureFormat::R8G8B8A8_UNORM }, _byteSize{ 0 }, _elementStride{ 0 }, _elementMaxCount{ 0 }, _width{ 0 } { __noop; }
		~Resource() = default;

	public:
		bool create_texture2D(Renderer& renderer, const TextureFormat& format, const void* const resourceContent, const uint32 width, const uint32 height);
		bool create_buffer(Renderer& renderer, const ResourceType& type, const void* const content, const uint32 elementStride, const uint32 elementCount);
		bool update(Renderer& renderer, const void* const content, const uint32 elementStride, const uint32 elementCount);

	private:
		static DXGI_FORMAT __convert_to_DXGI_FORMAT(const TextureFormat& format);
		static uint32 __compute_element_stride(const TextureFormat& format);

	public:
		ID3D11Resource* get_resource() const { return _resource.Get(); }
		ID3D11View* get_view() const { return _view.Get(); }

	public:
		ResourceType _type;
		TextureFormat _format;
		uint32 _byteSize;
		uint32 _elementStride;
		uint32 _elementMaxCount;
		uint32 _width;

	private:
		ComPtr<ID3D11Resource> _resource;
		ComPtr<ID3D11View> _view; // Only used for Texture and StructuredBuffer
	};

	template<typename Vertex>
	class MeshGenerator
	{
	public:
		static void push_3D_triangle(const Color& color, const float4& a, const float4& b, const float4& c, std::vector<Vertex>& vertices, std::vector<uint32>& indices)
		{
			const uint64 vertexBase = vertices.size();
			vertices.resize(vertexBase + 3);

			const uint64 indexBase = indices.size();
			indices.reserve(indexBase + 3);

			vertices[vertexBase + 0]._position = a;
			vertices[vertexBase + 0]._color = color;
			vertices[vertexBase + 1]._position = b;
			vertices[vertexBase + 1]._color = color;
			vertices[vertexBase + 2]._position = c;
			vertices[vertexBase + 2]._color = color;

			push_index(indices, vertexBase + 0);
			push_index(indices, vertexBase + 1);
			push_index(indices, vertexBase + 2);
		}

		static void push_2D_triangle(const Color& color, const float2& a, const float2& b, const float2& c, std::vector<Vertex>& vertices, std::vector<uint32>& indices)
		{
			const uint64 vertexBase = vertices.size();
			vertices.resize(vertexBase + 3);

			const uint64 indexBase = indices.size();
			indices.reserve(indexBase + 3);

			vertices[vertexBase + 0]._position = float4(a.x, a.y, 0, 1);
			vertices[vertexBase + 0]._color = color;
			vertices[vertexBase + 1]._position = float4(b.x, b.y, 0, 1);
			vertices[vertexBase + 1]._color = color;
			vertices[vertexBase + 2]._position = float4(c.x, c.y, 0, 1);
			vertices[vertexBase + 2]._color = color;

			push_index(indices, vertexBase + 0);
			push_index(indices, vertexBase + 1);
			push_index(indices, vertexBase + 2);
		}

		static void push_2D_rectangle(const Color& color, const float2& size, const float2& centerPosition, const float rotationAngle, std::vector<Vertex>& vertices, std::vector<uint32>& indices)
		{
			const uint64 vertexBase = vertices.size();
			vertices.resize(vertexBase + 4);

			const uint64 indexBase = indices.size();
			indices.reserve(indexBase + 6);

			const float2& halfSize = size * 0.5f;
			const float cosTheta = ::cos(rotationAngle);
			const float sinTheta = ::sin(rotationAngle);
			const float2 rotatedX = float2(+cosTheta, -sinTheta);
			const float2 rotatedY = float2(+sinTheta, +cosTheta);
			vertices[vertexBase + 0]._position.set_point(centerPosition - rotatedX * halfSize.x - rotatedY * halfSize.y);
			vertices[vertexBase + 0]._color = color;
			vertices[vertexBase + 1]._position.set_point(centerPosition - rotatedX * halfSize.x + rotatedY * halfSize.y);
			vertices[vertexBase + 1]._color = color;
			vertices[vertexBase + 2]._position.set_point(centerPosition + rotatedX * halfSize.x + rotatedY * halfSize.y);
			vertices[vertexBase + 2]._color = color;
			vertices[vertexBase + 3]._position.set_point(centerPosition + rotatedX * halfSize.x - rotatedY * halfSize.y);
			vertices[vertexBase + 3]._color = color;

			push_index(indices, vertexBase + 0);
			push_index(indices, vertexBase + 1);
			push_index(indices, vertexBase + 2);

			push_index(indices, vertexBase + 0);
			push_index(indices, vertexBase + 2);
			push_index(indices, vertexBase + 3);
		}

		static void push_2D_circle(const Color& color, const float2& centerPosition, float radius, uint32 sideCount, std::vector<Vertex>& vertices, std::vector<uint32>& indices)
		{
			radius = max(radius, 1.0f);
			sideCount = max(sideCount, 4);

			const uint64 vertexBase = vertices.size();
			vertices.resize(vertexBase + sideCount + 1);
			const uint64 indexBase = indices.size();
			indices.reserve(indexBase + static_cast<uint64>(sideCount) * 3);

			vertices[vertexBase]._position = float4(centerPosition.x, centerPosition.y, 0, 1);
			vertices[vertexBase]._color = color;
			for (uint32 sideIndex = 0; sideIndex < sideCount; ++sideIndex)
			{
				const float theta = (k2Pi * sideIndex) / sideCount;
				const float x = radius * ::cos(theta);
				const float y = -radius * ::sin(theta);
				vertices[vertexBase + sideIndex + 1]._position = float4(centerPosition.x + x, centerPosition.y + y, 0, 1);
				vertices[vertexBase + sideIndex + 1]._color = color;

				push_index(indices, vertexBase + 0);
				push_index(indices, vertexBase + sideIndex + 1);
				push_index(indices, vertexBase + sideIndex + 2);
			}
			indices[indices.size() - 1] = static_cast<uint32>(vertexBase + 1);
		}

		static void push_2D_lineSegment(const Color& color, const float2& a, const float2& b, float thickness, std::vector<Vertex>& vertices, std::vector<uint32>& indices)
		{
			thickness = max(thickness, 1.0f);

			const float2 ab = b - a;
			const float l = ab.length();
			if (l == 0.0f)
			{
				return;
			}

			const float2 direction = ab / l;
			const float rotationAngle = ::atan2f(-direction.y, direction.x);
			const float2 m = (a + b) * 0.5f;
			push_2D_rectangle(color, float2(l, thickness), m, rotationAngle, vertices, indices);
		}

		static void push_2D_arrow(const Color& color, const float2& a, const float2& b, float thickness, float head_length_ratio, float head_width_scale, std::vector<Vertex>& vertices, std::vector<uint32>& indices)
		{
			thickness = max(thickness, 1.0f);

			const float2 ab = b - a;
			const float l = ab.length();
			if (l == 0.0f)
			{
				return;
			}

			const float2 direction = ab / l;
			const float rotationAngle = ::atan2f(-direction.y, direction.x);
			const float2 m = (a + b) * 0.5f;
			push_2D_rectangle(color, float2(l, thickness), m, rotationAngle, vertices, indices);

			const float2 head_base = a + direction * l * (1.0f - head_length_ratio);
			const quaternion rotation = quaternion::make_from_axis_angle(float3(0, 0, -1), kPi * 0.5f);
			const float2 head_left_direction = rotation.rotate(direction);
			const float2 head_left = head_base + head_left_direction * thickness * head_width_scale;
			const float2 head_right = head_base - head_left_direction * thickness * head_width_scale;
			const float2& head_top = b;
			push_2D_triangle(color, head_right, head_top, head_left, vertices, indices);
		}

		static void fill_vertex_color(std::vector<Vertex>& vertices, const Color& color)
		{
			for (auto& vertex : vertices)
			{
				vertex._color = color;
			}
		}

		static void fill_vertex_color(const size_t vertexOffset, std::vector<Vertex>& vertices, const Color& color)
		{
			for (size_t i = vertexOffset; i < vertices.size(); i++)
			{
				vertices[i]._color = color;
			}
		}

	private:
		static void push_index(std::vector<uint32>& indices, const uint64 index)
		{
			indices.push_back(static_cast<uint32>(index));
		}
	};

	class Renderer final
	{
	public:
		enum class Key
		{
			NONE,
			Enter
		};

	private:
		struct MouseState
		{
			void clear()
			{
				_is_L_button_pressed = false;
				_is_L_button_released = false;
				_is_R_button_released = false;
			}
			void update_position(const MSG& msg)
			{
				_position.x = static_cast<float>(GET_X_LPARAM(msg.lParam));
				_position.y = static_cast<float>(GET_Y_LPARAM(msg.lParam));
			}
			bool _is_L_button_pressed = false;
			bool _is_L_button_released = false;
			bool _is_R_button_released = false;
			float2 _L_pressed_position;
			float2 _position;

			bool _is_L_button_down = false;
		};
		struct KeyboardState
		{
			void clear()
			{
				_char = 0;
				_up_key = Key::NONE;
			}
			char _char = 0;
			Key _up_key = Key::NONE;
		};

	public:
		Renderer(const float2& windowSize, const Color& clearColor) : _windowSize{ windowSize }, _clearColor{ clearColor } { if (create_window()) create_device(); }
		~Renderer() { destroy_window(); }

	public:
		bool is_running();

	public:
		void bind_ShaderInputLayout(ShaderInputLayout& shaderInputLayout);
		void bind_Shader(Shader& shader);
		void bind_input(Resource& resource, const uint32 slot);
		void bind_ShaderResource(const ShaderType shaderType, Resource& resource, const uint32 slot);
		void use_triangle_primitive();

	public:
		void begin_rendering();
		void draw(const uint32 vertexCount);
		void draw_indexed(const uint32 indexCount);
		void draw_text(const Color& color, const std::string& text, const float2& position);
		void end_rendering();

	public:
		ID3D11Device* get_device() const { return _device.Get(); }
		ID3D11DeviceContext* get_device_context() const { return _deviceContext.Get(); }

	public:
		bool is_mouse_L_button_down() const { return _mouseState._is_L_button_down; }
		bool is_mouse_L_button_pressed() const { return _mouseState._is_L_button_pressed; }
		bool is_mouse_L_button_released() const { return _mouseState._is_L_button_released; }
		bool is_mouse_R_button_released() const { return _mouseState._is_R_button_released; }
		float2 get_mouse_move_delta() const { return _mouseState._position - _mouseState._L_pressed_position; }
		char get_keyboard_char() const { return _keyboardState._char; }
		Key get_keyboard_up_key() const { return _keyboardState._up_key; }

	private:
		bool create_window();
		void destroy_window();
		void create_device();
		void create_device_create_default_FontData();
		void create_device_create_default_FontData_push_glyphRow(const uint32 rowIndex, const byte(&ch)[kFontTextureGlyphCountInRow]);
		void bind_default_FontData();

	private:
		HINSTANCE _hInstance = nullptr;
		HWND _hWnd = nullptr;
		float2 _windowSize;
		Color _clearColor;

	private:
		ComPtr<IDXGISwapChain> _swapChain;
		ComPtr<ID3D11Device> _device;
		ComPtr<ID3D11DeviceContext> _deviceContext;
		ComPtr<ID3D11RenderTargetView> _backBufferRtv;
		ComPtr<ID3D11Texture2D> _depthStencilResource;
		ComPtr<ID3D11DepthStencilView> _depthStencilView;
		ComPtr<ID3D11RasterizerState> _defaultRasterizerState;
		ComPtr<ID3D11DepthStencilState> _defaultDepthStencilState;
		ComPtr<ID3D11SamplerState> _defaultSamplerState;
		ComPtr<ID3D11BlendState> _defaultBlendState;

	private:
		bool _is_InputLayout_bound = false;
		bool _is_VS_bound = false;
		bool _is_PS_bound = false;
		bool _is_VertexBuffer_bound = false;
		bool _is_IndexBuffer_bound = false;

	private:
		ShaderHeaderSet _defaultFontShaderHeaderSet;
		Shader _defaultFontVertexShader;
		ShaderInputLayout _defaultFontShaderInputLayout;
		Shader _defaultFontPixelShader;
		Resource _defaultFontCBMatrices;
		Resource _defaultFontTexture;
		Resource _defaultFontVertexBuffer;
		Resource _defaultFontIndexBuffer;
		DefaultFontData _defaultFontData;
		std::vector<DEFAULT_FONT_VS_INPUT> _defaultFontVertices;
		std::vector<uint32> _defaultFontIndices;
		float2 _defaultFontScale = float2(1.25f, 2.25f);

	private:
		MouseState _mouseState;
		KeyboardState _keyboardState;
	};


#pragma region Function Definitions
	void DefaultFontData::push_glyph(const DefaultFontGlyphMeta& glyphMeta)
	{
		auto found = _glyphMap.find(glyphMeta._ch);
		if (found != _glyphMap.end())
		{
			return;
		}

		_glyphMetas.push_back(glyphMeta);
		_glyphMap.insert(std::pair<byte, uint64>(glyphMeta._ch, _glyphMetas.size() - 1));
	}

	const DefaultFontGlyphMeta& DefaultFontData::get_GlyphMeta(const byte& ch) const
	{
		auto found = _glyphMap.find(ch);
		if (found == _glyphMap.end())
		{
			return _glyphMetas[_glyphMap.find(0)->second];
		}
		return _glyphMetas[found->second];
	}

	void ShaderHeaderSet::push_shader_header(const std::string& headerName, const std::string& headerCode)
	{
		_headerNames.push_back(headerName);
		_headerCodes.push_back(headerCode);
	}

	HRESULT ShaderHeaderSet::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
	{
		const uint32 headerCount = (uint32)_headerNames.size();
		for (uint32 headerIndex = 0; headerIndex < headerCount; ++headerIndex)
		{
			if (_headerNames[headerIndex] == pFileName)
			{
				*ppData = _headerCodes[headerIndex].c_str();
				*pBytes = static_cast<UINT>(_headerCodes[headerIndex].length());
				return S_OK;
			}
		}
		return E_FAIL;
	}

	void ShaderInputLayout::clear_InputElements()
	{
		_inputElements.clear();
		_inputTotalByteSize = 0;
	}

	void ShaderInputLayout::push_InputElement(const InputElement& newInputElement)
	{
		D3D11_INPUT_ELEMENT_DESC inputElementDesc{};
		inputElementDesc.AlignedByteOffset = _inputTotalByteSize;
		inputElementDesc.Format = newInputElement._format;
		inputElementDesc.InputSlot = newInputElement._inputSlot;
		inputElementDesc.InputSlotClass = newInputElement._inputSlotClass;
		inputElementDesc.SemanticName = newInputElement._semanticName;
		inputElementDesc.SemanticIndex = newInputElement._semanticIndex;
		inputElementDesc.InstanceDataStepRate = newInputElement._instanceStepRate;
		_inputElements.push_back(inputElementDesc);

		_inputTotalByteSize += compute_InputElement_byte_size(inputElementDesc);
	}

	bool ShaderInputLayout::create(Renderer& renderer, const Shader& vertexShader)
	{
		if (_inputElements.empty())
		{
			MINT_LOG_ERROR("Push input elements before creating ShaderInputLayout!");
			return false;
		}

		if (FAILED(renderer.get_device()->CreateInputLayout(&_inputElements[0], static_cast<UINT>(_inputElements.size()),
			vertexShader._shaderBlob->GetBufferPointer(), vertexShader._shaderBlob->GetBufferSize(), _inputLayout.ReleaseAndGetAddressOf())))
		{
			MINT_LOG_ERROR("Failed to create ShaderInputLayout");
			return false;
		}
		return true;
	}

	bool Shader::create(Renderer& renderer, const char* sourceCode, const ShaderType& shaderType, const char* shaderIdentifier, const char* entryPoint, const char* target, ShaderHeaderSet* const shaderHeaderSet)
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
		HRESULT result = D3DCompile(sourceCode, ::strlen(sourceCode), shaderIdentifier, nullptr, shaderHeaderSet, entryPoint, target, debugFlag, 0, _shaderBlob.ReleaseAndGetAddressOf(), _errorMessageBlob.ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			std::string errorMessages(reinterpret_cast<char*>(_errorMessageBlob->GetBufferPointer()));
			MINT_LOG_ERROR("Shader compile failed.");
			return false;
		}

		if (shaderType == ShaderType::VertexShader)
		{
			if (FAILED(renderer.get_device()->CreateVertexShader(_shaderBlob->GetBufferPointer(), _shaderBlob->GetBufferSize(), NULL, reinterpret_cast<ID3D11VertexShader**>(_shader.ReleaseAndGetAddressOf()))))
			{
				return false;
			}
			return true;
		}
		else if (shaderType == ShaderType::PixelShader)
		{
			if (FAILED(renderer.get_device()->CreatePixelShader(_shaderBlob->GetBufferPointer(), _shaderBlob->GetBufferSize(), NULL, reinterpret_cast<ID3D11PixelShader**>(_shader.ReleaseAndGetAddressOf()))))
			{
				return false;
			}
			return true;
		}
		return false;
	}

	bool Resource::create_texture2D(Renderer& renderer, const TextureFormat& format, const void* const resourceContent, const uint32 width, const uint32 height)
	{
		ComPtr<ID3D11Resource> newResource;
		D3D11_TEXTURE2D_DESC texture2DDescriptor{};
		texture2DDescriptor.Width = width;
		texture2DDescriptor.Height = height;
		texture2DDescriptor.MipLevels = 1;
		texture2DDescriptor.ArraySize = 1;
		texture2DDescriptor.Format = __convert_to_DXGI_FORMAT(format);
		texture2DDescriptor.SampleDesc.Count = 1;
		texture2DDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		texture2DDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
		texture2DDescriptor.CPUAccessFlags = 0;
		const uint32 elementStride = __compute_element_stride(format);
		D3D11_SUBRESOURCE_DATA subResource{};
		subResource.pSysMem = resourceContent;
		subResource.SysMemPitch = texture2DDescriptor.Width * elementStride;
		subResource.SysMemSlicePitch = 0;
		if (SUCCEEDED(renderer.get_device()->CreateTexture2D(&texture2DDescriptor, &subResource, reinterpret_cast<ID3D11Texture2D**>(newResource.ReleaseAndGetAddressOf()))))
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor{};
			shaderResourceViewDescriptor.Format = texture2DDescriptor.Format;
			shaderResourceViewDescriptor.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDescriptor.Texture2D.MipLevels = texture2DDescriptor.MipLevels;
			shaderResourceViewDescriptor.Texture2D.MostDetailedMip = 0;
			if (SUCCEEDED(renderer.get_device()->CreateShaderResourceView(newResource.Get(), &shaderResourceViewDescriptor, reinterpret_cast<ID3D11ShaderResourceView**>(_view.ReleaseAndGetAddressOf()))))
			{
				_type = ResourceType::Teture2D;
				_format = format;

				_elementStride = elementStride;
				_elementMaxCount = texture2DDescriptor.Width * texture2DDescriptor.Height;
				//_resourceCapacity = _elementStride * _elementMaxCount;

				_width = width;
				//_height = _width / _elementMaxCount;

				std::swap(_resource, newResource);
				return true;
			}
		}
		return false;
	}

	bool Resource::create_buffer(Renderer& renderer, const ResourceType& type, const void* const content, const uint32 elementStride, const uint32 elementCount)
	{
		if (type == ResourceType::Teture2D)
		{
			MINT_ASSERT(false, "Use create_texture2D() instead!");
			return false;
		}

		ComPtr<ID3D11Resource> newResource;
		D3D11_BUFFER_DESC bufferDescriptor{};
		bufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		bufferDescriptor.ByteWidth = elementStride * elementCount;
		bufferDescriptor.BindFlags = D3D11_BIND_FLAG(1 << (uint32)type); // !!! CAUTION !!!
		bufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
		bufferDescriptor.MiscFlags = 0;
		bufferDescriptor.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA subresourceData{};
		subresourceData.pSysMem = content;
		if (SUCCEEDED(renderer.get_device()->CreateBuffer(&bufferDescriptor, (content != nullptr) ? &subresourceData : nullptr, reinterpret_cast<ID3D11Buffer**>(newResource.ReleaseAndGetAddressOf()))))
		{
			_type = type;
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
		if (elementCount > _elementMaxCount)
		{
			return create_buffer(renderer, _type, content, elementStride, elementCount);
		}

		class SafeResourceMapper
		{
		public:
			SafeResourceMapper(Renderer& renderer, ID3D11Resource* const resource, const uint32 subresource)
				: _renderer{ renderer }
				, _resource{ resource }
				, _subresource{ subresource }
				, _mappedSubresource{}
			{
				if (FAILED(_renderer.get_device_context()->Map(_resource, _subresource, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &_mappedSubresource)))
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
					_renderer.get_device_context()->Unmap(_resource, _subresource);
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

	DXGI_FORMAT Resource::__convert_to_DXGI_FORMAT(const TextureFormat& format)
	{
		switch (format)
		{
		case TextureFormat::R8_UNORM:
			return DXGI_FORMAT::DXGI_FORMAT_R8_UNORM;
		case TextureFormat::R8G8B8A8_UNORM:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		default:
			break;
		}
		MINT_ASSERT(false, "This texture format is not supported yet!");
		return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	uint32 Resource::__compute_element_stride(const TextureFormat& format)
	{
		switch (format)
		{
		case TextureFormat::R8_UNORM:
			return 1;
		case TextureFormat::R8G8B8A8_UNORM:
			return 4;
		default:
			break;
		}
		MINT_ASSERT(false, "This texture format is not supported yet!");
		return 4;
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

	bool Renderer::is_running()
	{
		if (!_hWnd) return false;

		_keyboardState.clear();
		_mouseState.clear();

		MSG msg{};
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) == false) { return true; }
		switch (msg.message)
		{
		case WM_KEYUP:
		{
			if (msg.wParam == VK_RETURN)
			{
				_keyboardState._up_key = Key::Enter;
			}
			break;
		}
		case WM_CHAR:
			_keyboardState._char = (char)msg.wParam;
			break;
		case WM_MOUSEMOVE:
		{
			_mouseState.update_position(msg);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			_mouseState._is_L_button_pressed = true;
			_mouseState._is_L_button_down = true;
			_mouseState.update_position(msg);
			_mouseState._L_pressed_position = _mouseState._position;
			break;
		}
		case WM_LBUTTONUP:
		{
			_mouseState._is_L_button_released = true;
			_mouseState._is_L_button_down = false;
			_mouseState.update_position(msg);
			break;
		}
		case WM_RBUTTONUP:
		{
			_mouseState._is_R_button_released = true;
			_mouseState.update_position(msg);
			break;
		}
		case WM_QUIT:
			destroy_window();
			return false;
		default:
			break;
		}
		::DispatchMessage(&msg);
		::TranslateMessage(&msg);
		return true;
	}

	void Renderer::bind_ShaderInputLayout(ShaderInputLayout& shaderInputLayout)
	{
		_is_InputLayout_bound = true;

		if (shaderInputLayout._inputLayout.Get() != nullptr)
			_deviceContext->IASetInputLayout(shaderInputLayout._inputLayout.Get());
	}

	void Renderer::bind_Shader(Shader& shader)
	{
		if (shader._type == ShaderType::VertexShader)
		{
			_is_VS_bound = true;
			_deviceContext->VSSetShader(static_cast<ID3D11VertexShader*>(shader._shader.Get()), nullptr, 0);
		}
		else if (shader._type == ShaderType::PixelShader)
		{
			_is_PS_bound = true;
			_deviceContext->PSSetShader(static_cast<ID3D11PixelShader*>(shader._shader.Get()), nullptr, 0);
		}
	}

	void Renderer::bind_input(Resource& resource, const uint32 slot)
	{
		if (resource._type == ResourceType::VertexBuffer)
		{
			_is_VertexBuffer_bound = true;

			ID3D11Buffer* buffers[1]{ static_cast<ID3D11Buffer*>(resource.get_resource()) };
			uint32 strides[1]{ resource._elementStride };
			uint32 offsets[1]{ 0 };
			_deviceContext->IASetVertexBuffers(slot, 1, buffers, strides, offsets);
		}
		else if (resource._type == ResourceType::IndexBuffer)
		{
			_is_IndexBuffer_bound = true;

			_deviceContext->IASetIndexBuffer(static_cast<ID3D11Buffer*>(resource.get_resource()), Resource::kIndexBufferFormat, 0);
		}
		else
		{
			MINT_LOG_ERROR("!!!");
		}
	}

	void Renderer::bind_ShaderResource(const ShaderType shaderType, Resource& resource, const uint32 slot)
	{
		if (resource._type == ResourceType::ConstantBuffer)
		{
			ID3D11Buffer* buffers[1]{ static_cast<ID3D11Buffer*>(resource.get_resource()) };
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
				MINT_LOG_ERROR("This shader type is not supported yet!");
			}
		}
		else if (resource._type == ResourceType::Teture2D)
		{
			ID3D11ShaderResourceView* views[1]{ static_cast<ID3D11ShaderResourceView*>(resource.get_view()) };
			if (shaderType == ShaderType::VertexShader)
			{
				_deviceContext->VSSetShaderResources(slot, 1, views);
			}
			else if (shaderType == ShaderType::PixelShader)
			{
				_deviceContext->PSSetShaderResources(slot, 1, views);
			}
			else
			{
				MINT_LOG_ERROR("This shader type is not supported yet!");
			}
		}
		else
		{
			MINT_LOG_ERROR("This resource type is not supported yet!");
		}
	}

	void Renderer::use_triangle_primitive()
	{
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void Renderer::begin_rendering()
	{
		_deviceContext->ClearRenderTargetView(_backBufferRtv.Get(), _clearColor.f);
		_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		use_triangle_primitive();
	}

	void Renderer::draw_indexed(const uint32 indexCount)
	{
		if (_is_InputLayout_bound == false)
		{
			MINT_LOG_ERROR("You must bind ShaderInputLayout first!");
			return;
		}
		if (_is_VS_bound == false || _is_PS_bound == false)
		{
			MINT_LOG_ERROR("You must at least bind VertexShader and PixelShader first!");
			return;
		}
		if (_is_VertexBuffer_bound == false)
		{
			MINT_LOG_ERROR("You must bind VertexBuffer first!");
			return;
		}
		if (_is_IndexBuffer_bound == false)
		{
			MINT_LOG_ERROR("You must bind IndexBuffer first!");
			return;
		}

		_deviceContext->DrawIndexed(indexCount, 0, 0);
	}

	void Renderer::draw_text(const Color& color, const std::string& text, const float2& position)
	{
		if (text.empty() == true)
		{
			return;
		}

		const float unit_x = _defaultFontScale.x * kFontTextureGlyphWidth;
		const float unit_y = _defaultFontScale.y * kFontTextureGlyphHeight;
		const float2 sizeUnit = float2(unit_x, unit_y);
		const float2 positionUnit = float2(unit_x, 0);
		uint32 chCount = 0;
		for (const char& ch : text)
		{
			const DefaultFontGlyphMeta& glyphMeta = _defaultFontData.get_GlyphMeta(ch);
			const float u0 = glyphMeta._u0;
			const float u1 = glyphMeta._u1;
			const float v0 = glyphMeta._v0;
			const float v1 = glyphMeta._v1;
			MeshGenerator<DEFAULT_FONT_VS_INPUT>::push_2D_rectangle(color, sizeUnit, position + sizeUnit * 0.5f + positionUnit * (float)chCount, 0.0f, _defaultFontVertices, _defaultFontIndices);
			_defaultFontVertices[_defaultFontVertices.size() - 4]._texcoord = float2(u0, v0);
			_defaultFontVertices[_defaultFontVertices.size() - 3]._texcoord = float2(u0, v1);
			_defaultFontVertices[_defaultFontVertices.size() - 2]._texcoord = float2(u1, v1);
			_defaultFontVertices[_defaultFontVertices.size() - 1]._texcoord = float2(u1, v0);
			++chCount;
		}
	}

	void Renderer::draw(const uint32 vertexCount)
	{
		if (_is_VertexBuffer_bound == false)
		{
			MINT_LOG_ERROR("You must bind VertexBuffer first!");
			return;
		}

		_deviceContext->Draw(vertexCount, 0);
	}

	void Renderer::end_rendering()
	{
		if (_defaultFontVertices.empty() == false)
		{
			_defaultFontVertexBuffer.update(*this, &_defaultFontVertices[0], sizeof(DEFAULT_FONT_VS_INPUT), (uint32)_defaultFontVertices.size());
			_defaultFontIndexBuffer.update(*this, &_defaultFontIndices[0], sizeof(uint32), (uint32)_defaultFontIndices.size());

			bind_default_FontData();

			draw_indexed((uint32)_defaultFontIndices.size());
			_defaultFontVertices.clear();
			_defaultFontIndices.clear();
		}

		_swapChain->Present(0, 0);
	}

	bool Renderer::create_window()
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

	void Renderer::destroy_window()
	{
		if (!_hWnd) return;
		::DestroyWindow(_hWnd);
		_hWnd = nullptr;
	}

	void Renderer::create_device()
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
		depthStencilResourceDescriptor.SampleDesc.Count = swapChainDescriptor.SampleDesc.Count;
		depthStencilResourceDescriptor.SampleDesc.Quality = swapChainDescriptor.SampleDesc.Quality;
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

		{
			D3D11_RASTERIZER_DESC rasterizerDescriptor{};
			rasterizerDescriptor.AntialiasedLineEnable = FALSE;
			rasterizerDescriptor.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
			rasterizerDescriptor.DepthBias = 0;
			rasterizerDescriptor.DepthBiasClamp = 0.0f;
			rasterizerDescriptor.DepthClipEnable = TRUE;
			rasterizerDescriptor.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
			rasterizerDescriptor.FrontCounterClockwise = TRUE;
			rasterizerDescriptor.MultisampleEnable = TRUE;
			rasterizerDescriptor.ScissorEnable = FALSE;
			rasterizerDescriptor.SlopeScaledDepthBias = 0.0f;
			_device->CreateRasterizerState(&rasterizerDescriptor, _defaultRasterizerState.ReleaseAndGetAddressOf());
			_deviceContext->RSSetState(_defaultRasterizerState.Get());
		}

		{
			D3D11_DEPTH_STENCIL_DESC depthStencilDescriptor{};
			depthStencilDescriptor.DepthEnable = TRUE;
			depthStencilDescriptor.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
			depthStencilDescriptor.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDescriptor.StencilEnable = FALSE;
			if (FAILED(_device->CreateDepthStencilState(&depthStencilDescriptor, _defaultDepthStencilState.ReleaseAndGetAddressOf())))
			{
				MINT_LOG_ERROR("Failed to create Depth-Stencil state.");
				return;
			}
		}

		{
			D3D11_VIEWPORT viewport{};
			viewport.Width = _windowSize.x;
			viewport.Height = _windowSize.y;
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			_deviceContext->RSSetViewports(1, &viewport);
		}

		{
			D3D11_SAMPLER_DESC samplerDescriptor{};
			samplerDescriptor.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDescriptor.MipLODBias = 0.0f;
			samplerDescriptor.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
			samplerDescriptor.MinLOD = 0.0f;
			samplerDescriptor.MaxLOD = 0.0f;
			_device->CreateSamplerState(&samplerDescriptor, _defaultSamplerState.ReleaseAndGetAddressOf());
			_deviceContext->PSSetSamplers(0, 1, _defaultSamplerState.GetAddressOf());
		}

		{
			D3D11_BLEND_DESC blendDescriptor{};
			blendDescriptor.AlphaToCoverageEnable = false;
			blendDescriptor.RenderTarget[0].BlendEnable = true;
			blendDescriptor.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
			blendDescriptor.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
			blendDescriptor.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			blendDescriptor.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
			blendDescriptor.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
			blendDescriptor.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			blendDescriptor.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
			_device->CreateBlendState(&blendDescriptor, _defaultBlendState.ReleaseAndGetAddressOf());

			const float kBlendFactor[4]{ 0, 0, 0, 0 };
			_deviceContext->OMSetBlendState(_defaultBlendState.Get(), kBlendFactor, 0xFFFFFFFF);
		}

		_deviceContext->OMSetRenderTargets(1, _backBufferRtv.GetAddressOf(), _depthStencilView.Get());
		_deviceContext->OMSetDepthStencilState(_defaultDepthStencilState.Get(), 0);

		create_device_create_default_FontData();
	}

	void Renderer::create_device_create_default_FontData_push_glyphRow(const uint32 rowIndex, const byte(&ch)[kFontTextureGlyphCountInRow])
	{
		const float glyphTextureWidth = (float)kFontTextureGlyphWidth;
		const float glyphTextureHeight = (float)kFontTextureGlyphHeight;
		const float glyphTextureUnit_U = glyphTextureWidth / kFontTextureWidth;
		const float glyphTextureUnit_V = glyphTextureHeight / kFontTextureHeight;
		const float v0 = glyphTextureUnit_V * rowIndex;
		const float v1 = v0 + glyphTextureUnit_V;
		for (uint32 iter = 0; iter < kFontTextureGlyphCountInRow; ++iter)
		{
			_defaultFontData.push_glyph(DefaultFontGlyphMeta(ch[iter], glyphTextureUnit_U * iter, v0, glyphTextureUnit_U * (iter + 1), v1));
		}
	}

	void Renderer::create_device_create_default_FontData()
	{
		Renderer& renderer = *this;

		_defaultFontShaderHeaderSet.push_shader_header("DefaultFontShaderHeader", kDefaultFontShaderHeaderCode);

		_defaultFontVertexShader.create(renderer, kDefaultFontVertexShaderCode, ShaderType::VertexShader, "DefaultFontVertexShader", "main", "vs_5_0", &_defaultFontShaderHeaderSet);

		_defaultFontShaderInputLayout.push_InputElement(ShaderInputLayout::create_InputElement_float4("POSITION", 0));
		_defaultFontShaderInputLayout.push_InputElement(ShaderInputLayout::create_InputElement_float4("COLOR", 0));
		_defaultFontShaderInputLayout.push_InputElement(ShaderInputLayout::create_InputElement_float2("TEXCOORD", 0));
		_defaultFontShaderInputLayout.create(renderer, _defaultFontVertexShader);

		_defaultFontPixelShader.create(renderer, kDefaultFontPixelShaderCode, ShaderType::PixelShader, "DefaultFontPixelShader", "main", "ps_5_0", &_defaultFontShaderHeaderSet);

		DEFAULT_FONT_CB_MATRICES default_font_cb_matrices;
		default_font_cb_matrices._projectionMatrix.make_pixel_coordinates_projection_matrix(_windowSize);
		_defaultFontCBMatrices.create_buffer(renderer, ResourceType::ConstantBuffer, &default_font_cb_matrices, sizeof(default_font_cb_matrices), 1);

		byte bytes[kFontTextureByteCount]{};
		for (uint32 iter = 0; iter < kFontTextureByteCount; ++iter)
		{
			const uint32 bitAt = iter % 8;
			const uint32 byteAt = iter / 8;
			const byte byte_ = (kFontTextureRawBitData[byteAt] >> (7 - bitAt)) & 1;
			bytes[iter] = byte_ * 255;
		}
		_defaultFontTexture.create_texture2D(renderer, TextureFormat::R8_UNORM, bytes, kFontTextureWidth, kFontTextureHeight);

		MeshGenerator<DEFAULT_FONT_VS_INPUT>::push_2D_rectangle(Color(), float2(512, 480), float2(256, 240), 0.0f, _defaultFontVertices, _defaultFontIndices);
		_defaultFontVertices[0]._texcoord = float2(0, 0);
		_defaultFontVertices[1]._texcoord = float2(1, 0);
		_defaultFontVertices[2]._texcoord = float2(0, 1);
		_defaultFontVertices[3]._texcoord = float2(1, 1);
		_defaultFontVertexBuffer.create_buffer(renderer, ResourceType::VertexBuffer, &_defaultFontVertices[0], sizeof(DEFAULT_FONT_VS_INPUT), (uint32)_defaultFontVertices.size());
		_defaultFontIndexBuffer.create_buffer(renderer, ResourceType::IndexBuffer, &_defaultFontIndices[0], sizeof(uint32), (uint32)_defaultFontIndices.size());

		byte row0[kFontTextureGlyphCountInRow]{ ' ','!','\"','$','#','%','&','\'','(',')','*','+',',','-','.','/' };
		create_device_create_default_FontData_push_glyphRow(0, row0);

		byte row1[kFontTextureGlyphCountInRow]{ '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?' };
		create_device_create_default_FontData_push_glyphRow(1, row1);

		byte row2[kFontTextureGlyphCountInRow]{ '@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O' };
		create_device_create_default_FontData_push_glyphRow(2, row2);

		byte row3[kFontTextureGlyphCountInRow]{ 'P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_' };
		create_device_create_default_FontData_push_glyphRow(3, row3);

		byte row4[kFontTextureGlyphCountInRow]{ '`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o' };
		create_device_create_default_FontData_push_glyphRow(4, row4);

		byte row5[kFontTextureGlyphCountInRow]{ 'p','q','r','s','t','u','v','w','x','y','z','(','|',')','~', 0 };
		create_device_create_default_FontData_push_glyphRow(5, row5);
	}

	void Renderer::bind_default_FontData()
	{
		bind_Shader(_defaultFontVertexShader);
		bind_Shader(_defaultFontPixelShader);
		bind_ShaderInputLayout(_defaultFontShaderInputLayout);
		bind_ShaderResource(ShaderType::VertexShader, _defaultFontCBMatrices, 0);
		bind_ShaderResource(ShaderType::PixelShader, _defaultFontTexture, 0);

		bind_input(_defaultFontVertexBuffer, 0);
		bind_input(_defaultFontIndexBuffer, 0);
	}

	bool read_file(const std::string& file_name, std::string& out_content)
	{
		out_content.clear();

		std::ifstream ifs;
		ifs.open(file_name, std::ios_base::ate);
		if (ifs.is_open() == false)
		{
			return false;
		}

		const std::streampos length = ifs.tellg();
		ifs.seekg(std::ios_base::beg);
		out_content.resize(length);
		ifs.read(&out_content[0], length);
		return true;
	}

	struct XML
	{
		struct Attribute;
		struct Node;
		static constexpr size_t INVALID_ID = size_t(-1);

		struct Attribute
		{
			friend XML;

			size_t _ID = 0;
			size_t _node_ID = 0;
			size_t _index_in_node = 0;
			size_t _name_at = 0;
			size_t _name_length = 0;
			size_t _value_at = 0;
			size_t _value_length = 0;

			//std::string _debug_name;
			//std::string _debug_value;

			bool is_valid() const { return _name_length > 0; }
			std::string get_name() const { return _XML->_text.substr(_name_at, _name_length); }
			std::string get_value() const { return _XML->_text.substr(_value_at, _value_length); }
			Attribute get_next_attribute() const { return get_node().get_attribute(_index_in_node + 1); }

		private:
			const Node& get_node() const { return _XML->get_node(_node_ID); }
			const XML* _XML = nullptr;
		};
		struct Node
		{
			friend XML;

			size_t _ID = 0;
			size_t _parent_ID = 0;
			size_t _index_in_parent_node = 0;
			size_t _name_at = 0;
			size_t _name_length = 0;
			std::vector<size_t> _attribute_IDs;
			std::vector<size_t> _child_node_IDs;

			//std::string _debug_name;

			bool is_valid() const { return _name_length > 0; }
			std::string get_name() const { return _XML->_text.substr(_name_at, _name_length); }
			const Attribute& get_attribute(const size_t index) const { return _XML->get_attribute((index >= _attribute_IDs.size() ? INVALID_ID : _attribute_IDs[index])); }
			const Node& get_child_node(const size_t index) const { return _XML->get_node((index >= _child_node_IDs.size() ? INVALID_ID : _child_node_IDs[index])); }
			const Node& get_next_sibling() const { return _XML->get_node(_parent_ID).get_child_node(_index_in_parent_node + 1); }

		private:
			bool has_name() const { return _name_length > 0; }
			const XML* _XML = nullptr;
		};
		bool parse(const std::string& text)
		{
			_text = text;
			if (check_validity() == false)
			{
				return false;
			}

			_at = 0;
			_line = 1;
			return parse_node(INVALID_ID);
		}
		const Node& get_root_node() const { return _nodes[0]; }
		const Node& get_node(const size_t ID) const { return (ID >= _nodes.size() ? INVALID_NODE : _nodes[ID]); }
		const Attribute& get_attribute(const size_t ID) const { return (ID >= _attributes.size() ? INVALID_ATTRIBUTE : _attributes[ID]); }

	private:
		bool advance_to_find(const char ch)
		{
			const size_t length = _text.length();
			while (_at < length && _text[_at] != ch)
			{
				++_at;
			}
			return _text[_at] == ch;
		}
		bool parse_node(const size_t parent_node_ID)
		{
			const size_t length = _text.length();
			if (advance_to_find('<') == false)
			{
				return (_at == length);
			}

			if (_at + 1 < length && _text[_at + 1] == '/')
			{
				if (advance_to_find('>') == false)
				{
					return false;
				}

				return parse_node(_nodes[parent_node_ID]._parent_ID);
			}

			const size_t node_name_at = _at + 1;
			if (advance_to_find('>') == false)
			{
				return false;
			}

			const bool is_open_close_node = (_text[_at - 1] == '/');
			const size_t node_ID = _nodes.size();
			_nodes.push_back(Node());
			_nodes.back()._XML = this;
			_nodes.back()._ID = node_ID;
			if (parent_node_ID != INVALID_ID)
			{
				_nodes[parent_node_ID]._child_node_IDs.push_back(_nodes[node_ID]._ID);
				_nodes[node_ID]._index_in_parent_node = _nodes[parent_node_ID]._child_node_IDs.size() - 1;
			}
			_nodes[node_ID]._parent_ID = parent_node_ID;
			_nodes[node_ID]._name_at = node_name_at;

			for (size_t at = node_name_at; at < length; ++at)
			{
				if (_text[at] != ' ' && _text[at] != '/' && _text[at] != '>')
				{
					continue;
				}

				if (_nodes[node_ID].has_name() == false)
				{
					_nodes[node_ID]._name_length = at - node_name_at;

					//_nodes[node_ID]._debug_name = _text.substr(node_name_at, _nodes[node_ID]._name_length);
				}

				if ((_text[at] == '/' && at + 1 < length && _text[at + 1] == '>') || _text[at] == '>')
				{
					// end
					_at = at + 1;
					return parse_node((is_open_close_node ? parent_node_ID : _nodes[node_ID]._ID));
				}
				else
				{
					// attributes
					_at = at + 1;
					parse_attribute(_nodes[node_ID]);
				}
			}
			return true;
		}
		bool parse_attribute(Node& node)
		{
			_attributes.push_back(Attribute());

			Attribute& attribute = _attributes.back();
			attribute._XML = this;
			attribute._ID = _attributes.size() - 1;
			attribute._name_at = _at;

			node._attribute_IDs.push_back(attribute._ID);
			attribute._node_ID = node._ID;
			attribute._index_in_node = node._attribute_IDs.size() - 1;

			if (advance_to_find('=') == false)
			{
				report_error("invalid attribute! '=' is needed.");
				return false;
			}

			attribute._name_length = _at - attribute._name_at;
			//attribute._debug_name = _text.substr(attribute._name_at, attribute._name_length);

			if (_text[_at + 1] != '\"')
			{
				report_error("'\"' must be followed by '='.");
				return false;
			}
			_at += 2;
			attribute._value_at = _at;

			if (advance_to_find('\"') == false)
			{
				report_error("closing '\"' is missing.");
				return false;
			}
			attribute._value_length = _at - attribute._value_at;
			//attribute._debug_value = _text.substr(attribute._value_at, attribute._value_length);
			return true;
		}

	private:
		bool check_validity() const
		{
			constexpr size_t CMP_COUNT = 4;
			const char cmps[CMP_COUNT] = { ' ', '<', '>', '/' };
			const size_t length = _text.length();
			char message[] = "character[ ] is repeated!";
			for (size_t at = 0; at < length; ++at)
			{
				for (size_t cmp_index = 0; cmp_index < CMP_COUNT; cmp_index++)
				{
					if (_text[at] == cmps[cmp_index])
					{
						if (at + 1 < length && _text[at + 1] == cmps[cmp_index])
						{
							message[10] = cmps[cmp_index];
							report_error(message, at);
							return false;
						}
					}
				}
			}
			return true;
		}
		void report_error(const std::string& error) const { _error = error; __report_where(_at); }
		void report_error(const std::string& error, const size_t at) const { _error = error; __report_where(at); }
		void __report_where(const size_t at) const { _error += " at["; _error += std::to_string(at); _error += "] line["; _error += std::to_string(_line); _error += "]"; }

	private:
		std::string _text;
		std::vector<Node> _nodes;
		std::vector<Attribute> _attributes;
		mutable std::string _error;
		size_t _at = 0;
		size_t _line = 0;

	private:
		const Node INVALID_NODE;
		const Attribute INVALID_ATTRIBUTE;
	};
#pragma endregion


#pragma region Sample Code
	const char kSampleShaderHeaderCode_StreamData[] =
		R"(
		struct SAMPLE_VS_INPUT
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

	const char kSampleVertexShaderCode[] =
		R"(
		#include "StreamData"
    
		cbuffer SAMPLE_CB_MATRICES
		{
			float4x4 _cbProjectionMatrix;
		};
    
		VS_OUTPUT main(SAMPLE_VS_INPUT input)
		{
			VS_OUTPUT output;
			output.screenPosition = mul(input.position, _cbProjectionMatrix);
			output.screenPosition /= output.screenPosition.w;
			output.color = input.color;
			output.texcoord = input.texcoord;
			return output;
		}
	)";

	const char kSamplePixelShaderCode[] =
		R"(
		#include "StreamData"
    
		float4 main(VS_OUTPUT input) : SV_Target
		{
			return input.color;
		}
	)";

	struct alignas(float) SAMPLE_VS_INPUT
	{
		SimpleRenderer::float4 _position;
		SimpleRenderer::float4 _color;
		SimpleRenderer::float2 _texcoord;
	};

	struct SAMPLE_CB_MATRICES
	{
		SimpleRenderer::float4x4 _projectionMatrix;
	};

	int SampleMain()
	{
		using namespace SimpleRenderer;
		constexpr float2 kScreenSize = float2(800, 600);
		Renderer renderer{ Renderer(kScreenSize, Color(0, 0.5f, 1, 1)) };
		ShaderHeaderSet shaderHeaderSet;
		shaderHeaderSet.push_shader_header("StreamData", kSampleShaderHeaderCode_StreamData);
		Shader vertexShader;
		vertexShader.create(renderer, kSampleVertexShaderCode, ShaderType::VertexShader, "SampleVertexShader", "main", "vs_5_0", &shaderHeaderSet);
		ShaderInputLayout shaderInputLayout;
		shaderInputLayout.push_InputElement(ShaderInputLayout::create_InputElement_float4("POSITION", 0));
		shaderInputLayout.push_InputElement(ShaderInputLayout::create_InputElement_float4("COLOR", 0));
		shaderInputLayout.push_InputElement(ShaderInputLayout::create_InputElement_float2("TEXCOORD", 0));
		shaderInputLayout.create(renderer, vertexShader);
		Shader pixelShader;
		pixelShader.create(renderer, kSamplePixelShaderCode, ShaderType::PixelShader, "SamplePixelShader", "main", "ps_5_0", &shaderHeaderSet);
		Resource vscbMatrices;
		SAMPLE_CB_MATRICES cb_matrices;
		cb_matrices._projectionMatrix.make_pixel_coordinates_projection_matrix(kScreenSize);
		vscbMatrices.create_buffer(renderer, ResourceType::ConstantBuffer, &cb_matrices, sizeof(SAMPLE_CB_MATRICES), 1);

		std::vector<SAMPLE_VS_INPUT> vertices;
		std::vector<uint32> indices;
		Resource vertexBuffer;
		vertexBuffer._type = ResourceType::VertexBuffer;
		Resource indexBuffer;
		indexBuffer._type = ResourceType::IndexBuffer;

		while (renderer.is_running())
		{
			renderer.begin_rendering();
			{
				vertices.clear();
				indices.clear();

				MeshGenerator<SAMPLE_VS_INPUT>::push_2D_circle(Color(1, 1, 0, 1), float2(100, 100), 32.0f, 16, vertices, indices);
				renderer.bind_ShaderInputLayout(shaderInputLayout);
				renderer.bind_Shader(vertexShader);
				renderer.bind_Shader(pixelShader);
				renderer.bind_ShaderResource(ShaderType::VertexShader, vscbMatrices, 0);
				renderer.bind_input(vertexBuffer, 0);
				renderer.bind_input(indexBuffer, 0);
				if (vertices.empty() == false)
				{
					vertexBuffer.update(renderer, &vertices[0], sizeof(SAMPLE_VS_INPUT), (uint32)vertices.size());
					indexBuffer.update(renderer, &indices[0], sizeof(uint32), (uint32)indices.size());
					renderer.draw_indexed((uint32)indices.size());
				}
				renderer.draw_text(Color(1, 1, 1, 1), "Sample Window", float2(10, 10));
			}
			renderer.end_rendering();
		}
		return 0;
	}
#pragma endregion
}
