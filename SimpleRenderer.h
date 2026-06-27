#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>

#if defined(_WIN32)
#define SR_WINDOWS
#endif // defined(_WIN32)

#if defined(SR_WINDOWS)
#define SR_DIRECTX
#include <wrl.h>
#include <Windows.h>
#include <windowsx.h>
#endif // defined(SR_WINDOWS)

#if defined(SR_DIRECTX)
#include <d3d11.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#endif // defined(SR_DIRECTX)

namespace SimpleRenderer
{
#define SR_LOG_ERROR(content) { std::cout << content; ::DebugBreak(); }
#define SR_ASSERT(condition, content) if (!(condition)) { SR_LOG_ERROR(content); }
#define SR_STATIC_ASSERT(condition, content) static_assert(condition, content)
#define SR_ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

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

	using i8 = int8;
	using u8 = uint8;
	using i16 = int16;
	using u16 = uint16;
	using i32 = int32;
	using u32 = uint32;
	using i64 = int64;
	using u64 = uint64;

	using String = std::string;
	template<typename T>
	String ToString(const T& value) { return std::to_string(value); }
	template<typename T>
	void Swap(T&& a, T&& b) { std::swap(a, b); }
	template<typename T>
	using vector = std::vector<T>;
	template<typename Key, typename Value>
	using HashMap = std::unordered_map<Key, Value>;
	template<typename T1, typename T2>
	using Pair = std::pair<T1, T2>;
#pragma endregion

#pragma region Forward Declaration
	class App;
	class RenderDevice;
	class Shader;
	struct float4;
	struct quaternion;
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

#pragma region Math
	struct i32v2
	{
		constexpr i32v2() : i32v2(0, 0) { __noop; }
		constexpr explicit i32v2(i32 s) : x{ s }, y{ s } { __noop; }
		constexpr i32v2(i32 x_, i32 y_) : x{ x_ }, y{ y_ } { __noop; }
		i32& operator[](const uint32 index) { return f[index]; }
		const i32& operator[](const uint32 index) const { return f[index]; }
		constexpr i32v2 operator+() const { return *this; }
		constexpr i32v2 operator-() const { return i32v2(-x, -y); }
		i32v2& operator+=(const i32v2& rhs) { x += rhs.x; y += rhs.y; return *this; }
		i32v2& operator-=(const i32v2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
		constexpr i32v2 operator+(const i32v2& rhs) const { return i32v2(x + rhs.x, y + rhs.y); }
		constexpr i32v2 operator-(const i32v2& rhs) const { return i32v2(x - rhs.x, y - rhs.y); }
		i32v2& operator*=(const i32 s) { x *= s; y *= s; return *this; }
		i32v2& operator/=(const i32 s) { x /= s; y /= s; return *this; }
		constexpr i32v2 operator*(const i32 s) const { return i32v2(x * s, y * s); }
		constexpr i32v2 operator/(const i32 s) const { return i32v2(x / s, y / s); }
		union { struct { i32 x; i32 y; }; i32 f[2]; };
		i32v2 operator*(const i32v2& rhs) { return i32v2(x * rhs.x, y * rhs.y); }
	};
	using int2 = i32v2;

	struct u32v2
	{
		constexpr u32v2() : u32v2(0, 0) { __noop; }
		constexpr explicit u32v2(u32 s) : x{ s }, y{ s } { __noop; }
		constexpr u32v2(u32 x_, u32 y_) : x{ x_ }, y{ y_ } { __noop; }
		u32& operator[](const uint32 index) { return f[index]; }
		const u32& operator[](const uint32 index) const { return f[index]; }
		constexpr u32v2 operator+() const { return *this; }
		//constexpr u32v2 operator-() const { return u32v2(-x, -y); }
		u32v2& operator+=(const u32v2& rhs) { x += rhs.x; y += rhs.y; return *this; }
		u32v2& operator-=(const u32v2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
		constexpr u32v2 operator+(const u32v2& rhs) const { return u32v2(x + rhs.x, y + rhs.y); }
		constexpr u32v2 operator-(const u32v2& rhs) const { return u32v2(x - rhs.x, y - rhs.y); }
		u32v2& operator*=(const u32 s) { x *= s; y *= s; return *this; }
		u32v2& operator/=(const u32 s) { x /= s; y /= s; return *this; }
		constexpr u32v2 operator*(const u32 s) const { return u32v2(x * s, y * s); }
		constexpr u32v2 operator/(const u32 s) const { return u32v2(x / s, y / s); }
		union { struct { u32 x; u32 y; }; u32 f[2]; };
		u32v2 operator*(const u32v2& rhs) { return u32v2(x * rhs.x, y * rhs.y); }
	};
	using uint2 = u32v2;

	struct float2
	{
		constexpr float2() : float2(0, 0) { __noop; }
		constexpr explicit float2(float s) : x{ s }, y{ s } { __noop; }
		constexpr float2(float x_, float y_) : x{ x_ }, y{ y_ } { __noop; }
		constexpr float2(const uint2 uv) : x{ static_cast<float>(uv.x) }, y{ static_cast<float>(uv.y) } { __noop; }
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
		constexpr float Dot(const float2& rhs) const { return x * rhs.x + y * rhs.y; }
		constexpr float LengthSq() const { return Dot(*this); }
		float Length() const { return ::sqrt(LengthSq()); }
		void Normalize() { *this /= Length(); }
		union { struct { float x; float y; }; float f[2]; };
		float2 operator*(const float2& rhs) { return float2(x * rhs.x, y * rhs.y); }
	};
	using f32v2 = float2;

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
		constexpr float Dot(const float3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
		constexpr float3 Cross(const float3& rhs) const { return float3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x); }
		constexpr float LengthSq() const { return Dot(*this); }
		float Length() const { return ::sqrt(LengthSq()); }
		void Normalize() { *this /= Length(); }
		float3 ComputeNormalized() const { float3 result = *this; result.Normalize(); return result; }
		void SetPoint(const float2& position) { x = position.x; y = position.y; z = 0; }
		union { struct { float x; float y; float z; }; float f[3]; };
	};
	using f32v3 = float3;

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
		constexpr float Dot(const float4& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w; }
		constexpr float LengthSq() const { return Dot(*this); }
		float Length() const { return ::sqrt(LengthSq()); }
		void Normalize() { *this /= Length(); }
		float4 ComputeNormalized() const { float4 result = *this; result.Normalize(); return result; }
		void SetPoint(const float2& position) { x = position.x; y = position.y; z = 0; w = 1; }
		union { struct { float x; float y; float z; float w; }; float f[4]; };
	};
	using f32v4 = float4;

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
		float4 Rotate(const float4& v) const noexcept
		{
			quaternion q = *this;
			q *= quaternion(v.x, v.y, v.z, 0);
			q *= quaternion::Conjugate(*this);
			return float4(q.x, q.y, q.z, v.w);
		}
		static quaternion MakeByAxisAngle(float3 axis, const float angle) noexcept
		{
			axis.Normalize();
			const float half_angle = angle * 0.5f;
			const float cos_half = ::cos(half_angle);
			const float sin_half = ::sin(half_angle);
			return quaternion(sin_half * axis.x, sin_half * axis.y, sin_half * axis.z, cos_half);
		}
		void GetAxisAngle(float3& axis, float& angle) const noexcept
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
		static quaternion Conjugate(const quaternion& q) noexcept { return quaternion(-q.x, -q.y, -q.z, q.w); }
		float x; float y; float z; float w;
	};

	struct float2x2
	{
		constexpr float2x2() : float2x2(1, 0, 0, 1) { __noop; }
		constexpr float2x2(float _11_, float _12_, float _21_, float _22_) : _m{ _11_, _12_, _21_, _22_ } { __noop; }
		constexpr float2x2(const float(&m)[4]) : _m{ m[0],m[1],m[2],m[3] } { __noop; }
		void Set(float _11_, float _12_, float _21_, float _22_)
		{
			_11 = _11_; _12 = _12_; _21 = _21_; _22 = _22_;
		}
		void MakeIdentity()
		{
			Set(1.0f, 0.0f, 0.0f, 1.0f);
		}
		void MakeZero()
		{
			Set(0.0f, 0.0f, 0.0f, 0.0f);
		}
		void MakeRotationMatrix(const float theta)
		{
			const float cosTheta = ::cos(theta);
			const float sinTheta = ::sin(theta);
			Set(cosTheta, sinTheta, -sinTheta, cosTheta);
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
	using f32m2x2 = float2x2;

	struct float4x4
	{
		constexpr float4x4() : float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) { __noop; }
		constexpr float4x4(const float4(&rows)[4]) : _rows{ rows[0], rows[1], rows[2], rows[3] } { __noop; }
		constexpr float4x4(float _11_, float _12_, float _13_, float _14_, float _21_, float _22_, float _23_, float _24_, float _31_, float _32_, float _33_, float _34_, float _41_, float _42_, float _43_, float _44_) : _m{ _11_, _12_, _13_, _14_, _21_, _22_, _23_, _24_, _31_, _32_, _33_, _34_, _41_, _42_, _43_, _44_ } { __noop; }
		constexpr float4x4(const float(&m)[16]) : _m{ m[0],m[1],m[2],m[3],m[4],m[5],m[6],m[7],m[8],m[9],m[10],m[11],m[12],m[13],m[14],m[15] } { __noop; }
		void Set(float _11_, float _12_, float _13_, float _14_, float _21_, float _22_, float _23_, float _24_, float _31_, float _32_, float _33_, float _34_, float _41_, float _42_, float _43_, float _44_)
		{
			_11 = _11_; _12 = _12_; _13 = _13_; _14 = _14_; _21 = _21_; _22 = _22_; _23 = _23_; _24 = _24_; _31 = _31_; _32 = _32_; _33 = _33_; _34 = _34_; _41 = _41_; _42 = _42_; _43 = _43_; _44 = _44_;
		}
		void MakeIdentity()
		{
			_11 = 1; _12 = 0; _13 = 0; _14 = 0; _21 = 0; _22 = 1; _23 = 0; _24 = 0; _31 = 0; _32 = 0; _33 = 1; _34 = 0; _41 = 0; _42 = 0; _43 = 0; _44 = 1;
		}
		void MakeZero()
		{
			_11 = 0; _12 = 0; _13 = 0; _14 = 0; _21 = 0; _22 = 0; _23 = 0; _24 = 0; _31 = 0; _32 = 0; _33 = 0; _34 = 0; _41 = 0; _42 = 0; _43 = 0; _44 = 0;
		}
		void MakePixelCoordsProjectionMatrix(const uint2& screenSize)
		{
			MakeIdentity();
			_11 = 2.0f / static_cast<float>(screenSize.x); _14 = -1.0f; _22 = -2.0f / static_cast<float>(screenSize.y); _24 = 1.0f;
		}
		void MakePerspectiveProjectionMatrix(const float FOVAngle, const float nearDepthAbs, const float farDepthAbs, const float screenWidthOverHeight)
		{
			MakeZero();
			bool isRightHanded = true;
			const float halfFOVAngle = FOVAngle * 0.5f;
			const float a = 1.0f / (::tanf(halfFOVAngle) * screenWidthOverHeight);
			const float b = 1.0f / (::tanf(halfFOVAngle));
			const float c = (farDepthAbs / (nearDepthAbs - farDepthAbs)) * (isRightHanded ? +1.0f : -1.0f);
			const float d = (farDepthAbs * nearDepthAbs) / (nearDepthAbs - farDepthAbs);
			const float e = (isRightHanded ? -1.0f : +1.0f);
			_11 = a;
			_22 = b;
			_33 = c;
			_34 = d;
			_43 = e;
		}
		void PreTranslate(const float x, const float y, const float z) noexcept
		{
			_14 += x; _24 += y; _34 += z;
		}
		void PostScale(const float x, const float y, const float z) noexcept
		{
			_11 *= x; _12 *= y; _13 *= z;
			_21 *= x; _22 *= y; _23 *= z;
			_31 *= x; _32 *= y; _33 *= z;
			_41 *= x; _42 *= y; _43 *= z;
		}
		static float4x4 CreateRotationMatrix(const quaternion& q)
		{
			float3 axis;
			float angle;
			q.GetAxisAngle(axis, angle);
			return CreateRotationMatrix(axis, angle);
		}
		static float4x4 CreateRotationMatrix(const float3& axis, const float angle)
		{
			// (v * r)r(1 - cosθ) + vcosθ + (r X v)sinθ
			const float3 r = axis.ComputeNormalized();
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
	using f32m4x4 = float4x4;

	struct Transform2D
	{
		Transform2D() : Transform2D(0.0f) { __noop; }
		Transform2D(const float rotation) : Transform2D(rotation, float2(0.0f, 0.0f)) { __noop; }
		Transform2D(const float rotation, const float2& translation) : _scale{ 1.0f, 1.0f }, _rotation{ rotation }, _translation{ translation } { __noop; }
		Transform2D& operator*=(const Transform2D& rhs)
		{
			SR_ASSERT(_scale.x == _scale.y, "Shear is not supported for Transform2D!!!");

			float2x2 rotationMatrix;
			rotationMatrix.MakeRotationMatrix(_rotation);

			float2 t = rotationMatrix * rhs._translation;
			_translation += (rotationMatrix * rhs._translation) * _scale;

			_rotation += rhs._rotation;

			return *this;
		}
		static Transform2D Interpolate(const Transform2D& a, const Transform2D& b, const float t)
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
		float4x4 CreateFloat4x4() const
		{
			// SRT matrix for column vector is like below:
			// SRT = T * R * S
			// which is the same as below..
			float4x4 matrix = float4x4::CreateRotationMatrix(_rotation);
			matrix.PreTranslate(_translation.x, _translation.y, _translation.z);
			matrix.PostScale(_scale.x, _scale.y, _scale.z);
			return matrix;
		}
		void MakeByFloat4x4(const float4x4 m) noexcept
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
#pragma endregion

	enum class ShaderType
	{
		VertexShader,
		PixelShader,
	};
	enum class TextureFormat
	{
		R8_UNORM,
		R8G8B8A8_UNORM,
		Count
	};
	enum class ResourceType
	{
		VertexBuffer,
		IndexBuffer,
		ConstantBuffer,
		//StructuredBuffer,
		Teture2D,
	};

	struct alignas(float) FONT_VS_INPUT
	{
		float4 _position;
		float4 _color;
		float2 _texcoord;
	};

	struct FONT_CB_MATRICES
	{
		float4x4 _projectionMatrix;
	};

	const char kFontShaderHeaderCode[] =
		R"(
        struct FONT_VS_INPUT
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

	const char kFontVertexShaderCode[] =
		R"(
        #include "FontShaderHeader"
    
        cbuffer DEFAULT_CB_MATRICES
        {
            float4x4 g_cbProjectionMatrix;
        };
    
        VS_OUTPUT main(FONT_VS_INPUT input)
        {
            VS_OUTPUT output;
            output.screenPosition = mul(input.position, g_cbProjectionMatrix);
            output.screenPosition /= output.screenPosition.w;
            output.color = input.color;
            output.texcoord = input.texcoord;
            return output;
        }
    )";

	const char kFontPixelShaderCode[] =
		R"(
        #include "FontShaderHeader"
    
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

	struct FontGlyphMeta
	{
		FontGlyphMeta() :FontGlyphMeta(0, 0, 0, 1, 1) { __noop; }
		FontGlyphMeta(byte ch, float u0, float v0, float u1, float v1) : _ch{ ch }, _u0{ u0 }, _v0{ v0 }, _u1{ u1 }, _v1{ v1 } { __noop; }

		byte _ch;
		float _u0;
		float _v0;
		float _u1;
		float _v1;
	};

	class FontData
	{
	public:
		void PushGlyph(const FontGlyphMeta& glyphMeta);
		const FontGlyphMeta& GetGlyphMeta(const byte& ch) const;

	private:
		vector<FontGlyphMeta> _glyphMetas;
		HashMap<byte, uint64> _glyphMap;
	};

	struct ShaderHeaderSet : public ID3DInclude
	{
	public:
		ShaderHeaderSet() = default;
		virtual ~ShaderHeaderSet() = default;

	public:
		void PushShaderHeader(const String& headerName, const String& headerCode);

	public:
		virtual HRESULT WINAPI Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override final;
		virtual HRESULT WINAPI Close(LPCVOID pData) override final { return S_OK; }

	public:
		vector<String> _headerNames;
		vector<String> _headerCodes;
	};

	enum class GraphicsFormat
	{
		R32G32B32A32_FLOAT,
		R32G32B32_FLOAT,
		R32G32_FLOAT,
		R32_FLOAT,
		R32_UINT,
		R16_UINT,
		R8G8B8A8_UNORM,
		Count
	};

#if defined(SR_DIRECTX)
	DXGI_FORMAT DX_ConvertGraphicsFormat(const GraphicsFormat& inputFormat)
	{
		static constexpr DXGI_FORMAT kFormats[]
		{
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R32G32B32_FLOAT,
			DXGI_FORMAT_R32G32_FLOAT,
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_R32_UINT,
			DXGI_FORMAT_R16_UINT,
			DXGI_FORMAT_R8G8B8A8_UNORM,
		};
		SR_STATIC_ASSERT(SR_ARRAY_SIZE(kFormats) == static_cast<size_t>(GraphicsFormat::Count), "The sizes must match each other!");
		return kFormats[static_cast<size_t>(inputFormat)];
	}

	DXGI_FORMAT DX_ConvertTextureFormat(const TextureFormat& format)
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
		SR_ASSERT(false, "This texture format is not supported yet!");
		return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	}
#endif // defined(SR_DIRECTX)

	enum class ShaderInputSlotClass
	{
		PerVertexData,
		PerInstanceData,
		Count
	};

	struct ShaderInputElement
	{
	public:
		static ShaderInputElement CreateInputelement_float4(const char* const semanticName, const uint32 semanticIndex) { return __CreateInputelementCommon(GraphicsFormat::R32G32B32A32_FLOAT, semanticName, semanticIndex); }
		static ShaderInputElement CreateInputelement_float3(const char* const semanticName, const uint32 semanticIndex) { return __CreateInputelementCommon(GraphicsFormat::R32G32B32_FLOAT, semanticName, semanticIndex); }
		static ShaderInputElement CreateInputelement_float2(const char* const semanticName, const uint32 semanticIndex) { return __CreateInputelementCommon(GraphicsFormat::R32G32_FLOAT, semanticName, semanticIndex); }
		static ShaderInputElement CreateInputelement_float(const char* const semanticName, const uint32 semanticIndex) { return __CreateInputelementCommon(GraphicsFormat::R32_FLOAT, semanticName, semanticIndex); }

	private:
		static ShaderInputElement __CreateInputelementCommon(const GraphicsFormat format, const char* const semanticName, const uint32 semanticIndex)
		{
			ShaderInputElement shaderInputElement;
			shaderInputElement._format = format;
			shaderInputElement._semanticName = semanticName;
			shaderInputElement._semanticIndex = semanticIndex;
			return shaderInputElement;
		}

	public:
		GraphicsFormat _format = GraphicsFormat::R32G32B32A32_FLOAT;
		uint32 _inputSlot = 0;
		ShaderInputSlotClass _inputSlotClass = ShaderInputSlotClass::PerVertexData;
		const char* _semanticName = nullptr;
		uint32 _semanticIndex = 0;
		uint32 _instanceStepRate = 0;
	};

	class ShaderInputLayout
	{
		friend RenderDevice;

	public:
		ShaderInputLayout() = default;
		~ShaderInputLayout() = default;

	private:
		static uint32 ComputeInputElementByteSize(const ShaderInputElement& shaderInputElement)
		{
			switch (shaderInputElement._format)
			{
			case GraphicsFormat::R32G32B32A32_FLOAT:
				return 16;
			case GraphicsFormat::R32G32B32_FLOAT:
				return 12;
			case GraphicsFormat::R32G32_FLOAT:
				return 8;
			case GraphicsFormat::R32_FLOAT:
				return 4;
			default:
				break;
			}
			SR_LOG_ERROR("!!!");
			return 0;
		}

#if defined(SR_DIRECTX)
		static D3D11_INPUT_CLASSIFICATION DX_ConvertInputSlotClass(const ShaderInputSlotClass& inputSlotClass)
		{
			static constexpr D3D11_INPUT_CLASSIFICATION kSlotClasses[]
			{
				D3D11_INPUT_PER_VERTEX_DATA,
				D3D11_INPUT_PER_INSTANCE_DATA
			};
			SR_STATIC_ASSERT(SR_ARRAY_SIZE(kSlotClasses) == static_cast<size_t>(ShaderInputSlotClass::Count), "The sizes must match each other!");
			return kSlotClasses[static_cast<size_t>(inputSlotClass)];
		}
#endif // defined(SR_DIRECTX)

	private:
#if defined(SR_DIRECTX)
		ComPtr<ID3D11InputLayout> _inputLayout;
#endif // defined(SR_DIRECTX)
	};

	class Shader
	{
		friend RenderDevice;

	public:
		Shader() = default;
		~Shader() = default;

	private:
		ShaderType _type = ShaderType::VertexShader;
#if defined(SR_DIRECTX)
		ComPtr<ID3D10Blob> _shaderBlob;
		ComPtr<ID3D10Blob> _errorMessageBlob;
		ComPtr<ID3D11DeviceChild> _shader;
#endif // defined(SR_DIRECTX)
	};

	// Buffer or Texture
	class Resource
	{
		friend RenderDevice;

	public:
		static constexpr GraphicsFormat kIndexBufferFormat = GraphicsFormat::R32_UINT;

	public:
		Resource() : _type{ ResourceType::VertexBuffer }, _format{ TextureFormat::R8G8B8A8_UNORM }, _byteSize{ 0 }, _elementStride{ 0 }, _elementMaxCount{ 0 }, _width{ 0 } { __noop; }
		~Resource() = default;

	private:
		static uint32 __ComputeElementStride(const TextureFormat& format);

	public:
		ResourceType _type;
		TextureFormat _format;
		uint32 _byteSize;
		uint32 _elementStride;
		uint32 _elementMaxCount;
		uint32 _width;

	private:
#if defined(SR_DIRECTX)
		ComPtr<ID3D11Resource> _resource;
		ComPtr<ID3D11View> _view; // Only used for Texture and StructuredBuffer
#endif // defined(SR_DIRECTX)
	};

	template<typename Vertex>
	class MeshGenerator
	{
	public:
		static void Push3DTriangle(const Color& color, const float4& a, const float4& b, const float4& c, vector<Vertex>& vertices, vector<uint32>& indices)
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

			PushIndex(indices, vertexBase + 0);
			PushIndex(indices, vertexBase + 1);
			PushIndex(indices, vertexBase + 2);
		}

		static void Push2DTriangle(const Color& color, const float2& a, const float2& b, const float2& c, vector<Vertex>& vertices, vector<uint32>& indices)
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

			PushIndex(indices, vertexBase + 0);
			PushIndex(indices, vertexBase + 1);
			PushIndex(indices, vertexBase + 2);
		}

		static void Push2DRectangle(const Color& color, const float2& size, const float2& centerPosition, const float rotationAngle, vector<Vertex>& vertices, vector<uint32>& indices)
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
			vertices[vertexBase + 0]._position.SetPoint(centerPosition - rotatedX * halfSize.x - rotatedY * halfSize.y);
			vertices[vertexBase + 0]._color = color;
			vertices[vertexBase + 1]._position.SetPoint(centerPosition - rotatedX * halfSize.x + rotatedY * halfSize.y);
			vertices[vertexBase + 1]._color = color;
			vertices[vertexBase + 2]._position.SetPoint(centerPosition + rotatedX * halfSize.x + rotatedY * halfSize.y);
			vertices[vertexBase + 2]._color = color;
			vertices[vertexBase + 3]._position.SetPoint(centerPosition + rotatedX * halfSize.x - rotatedY * halfSize.y);
			vertices[vertexBase + 3]._color = color;

			PushIndex(indices, vertexBase + 0);
			PushIndex(indices, vertexBase + 1);
			PushIndex(indices, vertexBase + 2);

			PushIndex(indices, vertexBase + 0);
			PushIndex(indices, vertexBase + 2);
			PushIndex(indices, vertexBase + 3);
		}

		static void Push2DCircle(const Color& color, const float2& centerPosition, float radius, uint32 sideCount, vector<Vertex>& vertices, vector<uint32>& indices)
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

				PushIndex(indices, vertexBase + 0);
				PushIndex(indices, vertexBase + sideIndex + 1);
				PushIndex(indices, vertexBase + sideIndex + 2);
			}
			indices[indices.size() - 1] = static_cast<uint32>(vertexBase + 1);
		}

		static void Push2DLineSegment(const Color& color, const float2& a, const float2& b, float thickness, vector<Vertex>& vertices, vector<uint32>& indices)
		{
			thickness = max(thickness, 1.0f);

			const float2 ab = b - a;
			const float l = ab.Length();
			if (l == 0.0f)
			{
				return;
			}

			const float2 direction = ab / l;
			const float rotationAngle = ::atan2f(-direction.y, direction.x);
			const float2 m = (a + b) * 0.5f;
			Push2DRectangle(color, float2(l, thickness), m, rotationAngle, vertices, indices);
		}

		static void Push2DArrow(const Color& color, const float2& a, const float2& b, float thickness, float head_length_ratio, float head_width_scale, vector<Vertex>& vertices, vector<uint32>& indices)
		{
			thickness = max(thickness, 1.0f);

			const float2 ab = b - a;
			const float l = ab.Length();
			if (l == 0.0f)
			{
				return;
			}

			const float2 direction = ab / l;
			const float rotationAngle = ::atan2f(-direction.y, direction.x);
			const float2 m = (a + b) * 0.5f;
			Push2DRectangle(color, float2(l, thickness), m, rotationAngle, vertices, indices);

			const float2 head_base = a + direction * l * (1.0f - head_length_ratio);
			const quaternion rotation = quaternion::MakeByAxisAngle(float3(0, 0, -1), kPi * 0.5f);
			const float2 head_left_direction = rotation.Rotate(direction);
			const float2 head_left = head_base + head_left_direction * thickness * head_width_scale;
			const float2 head_right = head_base - head_left_direction * thickness * head_width_scale;
			const float2& head_top = b;
			Push2DTriangle(color, head_right, head_top, head_left, vertices, indices);
		}

		static void FillVertexColor(vector<Vertex>& vertices, const Color& color)
		{
			for (auto& vertex : vertices)
			{
				vertex._color = color;
			}
		}

		static void FillVertexColor(const size_t vertexOffset, vector<Vertex>& vertices, const Color& color)
		{
			for (size_t i = vertexOffset; i < vertices.size(); i++)
			{
				vertices[i]._color = color;
			}
		}

	private:
		static void PushIndex(vector<uint32>& indices, const uint64 index)
		{
			indices.push_back(static_cast<uint32>(index));
		}
	};

	class Window
	{
	public:
		struct CreateDesc
		{
			CreateDesc() = default;
			CreateDesc(const char* title, const uint2 size)
				: _title(title)
				, _width(size.x)
				, _height(size.y)
			{
				__noop;
			}
			const char* _title = "SimpleRenderer";
			uint32 _width = 800;
			uint32 _height = 600;
		};
		enum class Key
		{
			NONE,
			Enter
		};
		struct MouseState
		{
			void ClearMouseState()
			{
				_is_L_button_pressed = false;
				_is_L_button_released = false;
				_is_R_button_released = false;
			}
			void UpdatePosition(const MSG& msg)
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
			void ClearKeyboardState()
			{
				_char = 0;
				_up_key = Key::NONE;
			}
			char _char = 0;
			Key _up_key = Key::NONE;
		};

	public:
		Window() = default;
		~Window() = default;

	public:
		bool Create(const CreateDesc& createDesc);
		void Destroy();
		int32 ProcessMessages();

	public:
		const uint2& GetSize() const { return _windowSize; }
		uint64 GetWindowHandle() const { return _windowHandle; }
		const MouseState& GetMouseState() const { return _mouseState; }
		const KeyboardState& GetKeyboardState() const { return _keyboardState; }

	private:
		uint64 _instanceHandle = 0;
		uint64 _windowHandle = 0;
		uint2 _windowSize;

	private:
		MouseState _mouseState;
		KeyboardState _keyboardState;
	};

	class RenderDevice
	{
	public:
		RenderDevice() = default;
		~RenderDevice() = default;

	public:
		bool CreateDevice(const Window& window);
		void DestroyDevice() {}

	public:
		bool CreateShaderInputLayout(const Shader& vertexShader, const vector<ShaderInputElement>& shaderInputElements, ShaderInputLayout& shaderInputLayout);
		bool CreateShader(const char* sourceCode, const ShaderType& shaderType, const char* shaderIdentifier, const char* entryPoint, const char* target, ShaderHeaderSet* const shaderHeaderSet, Shader& shader);
		bool CreateTexture2D(const TextureFormat& format, const void* const resourceContent, const uint32 width, const uint32 height, Resource& resource);
		bool CreateBuffer(const ResourceType& type, const void* const content, const uint32 elementStride, const uint32 elementCount, Resource& resource);
		bool UpdateShaderResource(const void* const content, const uint32 elementStride, const uint32 elementCount, Resource& resource);

	public:
		void BindShaderInputLayout(ShaderInputLayout& shaderInputLayout);
		void BindShader(Shader& shader);
		void BindInput(Resource& resource, const uint32 slot);
		void BindShaderResource(const ShaderType shaderType, Resource& resource, const uint32 slot);
		void UseTrianglePrimitive();

	public:
		void BeginRendering(const Color& clearColor);
		void Draw(const uint32 vertexCount);
		void DrawIndexed(const uint32 indexCount);
		void EndRendering();

	private:
#if defined(SR_DIRECTX)
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
#endif // defined(SR_DIRECTX)

	private:
		bool _is_InputLayout_bound = false;
		bool _is_VS_bound = false;
		bool _is_PS_bound = false;
		bool _is_VertexBuffer_bound = false;
		bool _is_IndexBuffer_bound = false;
	};

	class App final
	{
	public:
		App(Window& window, const Color& clearColor) : _window{ window }, _clearColor{ clearColor } { _renderDevice.CreateDevice(_window); CreateDefaultFontData(); }
		~App() = default;

	public:
		bool IsRunning();

	public:
		void BeginRendering();
		void Draw(const uint32 vertexCount);
		void DrawIndexed(const uint32 indexCount);
		void DrawTextAt(const Color& color, const String& text, const float2& position);
		void EndRendering();
	public:
		bool IsMouseLButtonDown() const { return _window.GetMouseState()._is_L_button_down; }
		bool IsMouseLButtonPressed() const { return _window.GetMouseState()._is_L_button_pressed; }
		bool IsMouseLButtonReleased() const { return _window.GetMouseState()._is_L_button_released; }
		bool IsMouseRButtonReleased() const { return _window.GetMouseState()._is_R_button_released; }
		float2 GetMouseMoveDelta() const { return _window.GetMouseState()._position - _window.GetMouseState()._L_pressed_position; }
		char GetKeyboardChar() const { return _window.GetKeyboardState()._char; }
		Window::Key GetKeyboardUpKey() const { return _window.GetKeyboardState()._up_key; }
		RenderDevice& GetRenderDevice() { return _renderDevice; }
	private:
		void CreateDefaultFontData();
		void CreateDefaultFontDataPushGlyphRow(const uint32 rowIndex, const byte(&ch)[kFontTextureGlyphCountInRow]);
		void BindDefaultFontData();

	private:
		Window& _window;
		Color _clearColor;

	private:
		RenderDevice _renderDevice;

	private:
		ShaderHeaderSet _fontShaderHeaderSet;
		Shader _fontVertexShader;
		ShaderInputLayout _fontShaderInputLayout;
		Shader _fontPixelShader;
		Resource _fontCBMatrices;
		Resource _fontTexture;
		Resource _fontVertexBuffer;
		Resource _fontIndexBuffer;
		vector<FONT_VS_INPUT> _fontVertices;
		vector<uint32> _fontIndices;
		float2 _fontScale = float2(1.25f, 2.25f);
		FontData _defaultFontData;
	};


#pragma region Function Definitions
	void FontData::PushGlyph(const FontGlyphMeta& glyphMeta)
	{
		auto found = _glyphMap.find(glyphMeta._ch);
		if (found != _glyphMap.end())
		{
			return;
		}

		_glyphMetas.push_back(glyphMeta);
		_glyphMap.insert(Pair<byte, uint64>(glyphMeta._ch, _glyphMetas.size() - 1));
	}

	const FontGlyphMeta& FontData::GetGlyphMeta(const byte& ch) const
	{
		auto found = _glyphMap.find(ch);
		if (found == _glyphMap.end())
		{
			return _glyphMetas[_glyphMap.find(0)->second];
		}
		return _glyphMetas[found->second];
	}

	void ShaderHeaderSet::PushShaderHeader(const String& headerName, const String& headerCode)
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

	uint32 Resource::__ComputeElementStride(const TextureFormat& format)
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
		SR_ASSERT(false, "This texture format is not supported yet!");
		return 4;
	}

#if defined(SR_WINDOWS)
	static LRESULT WINAPI WIN_windowProcedure(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		switch (Msg)
		{
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		}
		return ::DefWindowProc(hWnd, Msg, wParam, lParam);
	}
#endif // defined(SR_WINDOWS)

	bool Window::Create(const Window::CreateDesc& createDesc)
	{
#if defined(SR_WINDOWS)
		const HINSTANCE typedInstanceHandle = ::GetModuleHandle(nullptr);
		_instanceHandle = reinterpret_cast<uint64>(typedInstanceHandle);
		WNDCLASSEX wndClassEx{};
		wndClassEx.cbSize = sizeof(wndClassEx);
		wndClassEx.hInstance = typedInstanceHandle;
		wndClassEx.lpszClassName = TEXT("SimpleRenderer Window");
		wndClassEx.hbrBackground = ::CreateSolidBrush(RGB(255, 255, 255));
		wndClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wndClassEx.lpfnWndProc = WIN_windowProcedure;
		::RegisterClassEx(&wndClassEx);

		wchar_t titleBuffer[256]{};
		const int titleBufferLength = ::MultiByteToWideChar(CP_ACP, 0, createDesc._title, -1, titleBuffer, 0);
		if (titleBufferLength >= 256) { _instanceHandle = 0; return false; }
		::MultiByteToWideChar(CP_ACP, 0, createDesc._title, -1, titleBuffer, titleBufferLength);

		const HWND typedWindowHandle = ::CreateWindowEx(0, wndClassEx.lpszClassName, titleBuffer, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, (int)createDesc._width, (int)createDesc._height, nullptr, nullptr, typedInstanceHandle, nullptr);
		_windowHandle = reinterpret_cast<uint64>(typedWindowHandle);
		if (_windowHandle == 0) { _instanceHandle = 0; return false; }
		_windowSize.x = createDesc._width;
		_windowSize.y = createDesc._height;
		::ShowWindow(typedWindowHandle, SW_SHOWDEFAULT);
		return true;
#else
		return false;
#endif // defined(SR_WINDOWS)
	}

	void Window::Destroy()
	{
#if defined(SR_WINDOWS)
		if (_windowHandle == 0) { return; }
		::DestroyWindow(reinterpret_cast<HWND>(_windowHandle));
		_windowHandle = 0;
		_instanceHandle = 0;
#endif // defined(SR_WINDOWS)
	}

	int32 Window::ProcessMessages()
	{
		_keyboardState.ClearKeyboardState();
		_mouseState.ClearMouseState();
#if defined(SR_WINDOWS)
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
			_mouseState.UpdatePosition(msg);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			_mouseState._is_L_button_pressed = true;
			_mouseState._is_L_button_down = true;
			_mouseState.UpdatePosition(msg);
			_mouseState._L_pressed_position = _mouseState._position;
			break;
		}
		case WM_LBUTTONUP:
		{
			_mouseState._is_L_button_released = true;
			_mouseState._is_L_button_down = false;
			_mouseState.UpdatePosition(msg);
			break;
		}
		case WM_RBUTTONUP:
		{
			_mouseState._is_R_button_released = true;
			_mouseState.UpdatePosition(msg);
			break;
		}
		case WM_QUIT:
			//destroy_window();
			return -1;
		default:
			break;
		}
		::DispatchMessage(&msg);
		::TranslateMessage(&msg);
#endif // defined(SR_WINDOWS)
		return 0;
	}

	bool RenderDevice::CreateDevice(const Window& window)
	{
#if defined(SR_DIRECTX)
		DXGI_SWAP_CHAIN_DESC swapChainDescriptor{};
		swapChainDescriptor.BufferCount = 1;
		swapChainDescriptor.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDescriptor.BufferDesc.Width = static_cast<UINT>(window.GetSize().x);
		swapChainDescriptor.BufferDesc.Height = static_cast<UINT>(window.GetSize().y);
		swapChainDescriptor.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDescriptor.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDescriptor.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDescriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDescriptor.Flags = 0;
		swapChainDescriptor.OutputWindow = reinterpret_cast<HWND>(window.GetWindowHandle());
		swapChainDescriptor.SampleDesc.Count = 1;
		swapChainDescriptor.SampleDesc.Quality = 0;
		swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
		swapChainDescriptor.Windowed = TRUE;
		if (FAILED(::D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
			&swapChainDescriptor, _swapChain.ReleaseAndGetAddressOf(), _device.ReleaseAndGetAddressOf(), nullptr, _deviceContext.ReleaseAndGetAddressOf())))
		{
			SR_LOG_ERROR("Failed to create Device and SwapChain.");
			return false;
		}

		ComPtr<ID3D11Texture2D> backBuffer;
		_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.ReleaseAndGetAddressOf()));
		if (FAILED(_device->CreateRenderTargetView(backBuffer.Get(), nullptr, _backBufferRtv.ReleaseAndGetAddressOf())))
		{
			SR_LOG_ERROR("Failed to get BackBuffer.");
			return false;
		}

		D3D11_TEXTURE2D_DESC depthStencilResourceDescriptor{};
		depthStencilResourceDescriptor.Width = static_cast<UINT>(window.GetSize().x);
		depthStencilResourceDescriptor.Height = static_cast<UINT>(window.GetSize().y);
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
			SR_LOG_ERROR("Failed to create Depth-Stencil texture.");
			return false;
		}
		if (FAILED(_device->CreateDepthStencilView(_depthStencilResource.Get(), nullptr, _depthStencilView.ReleaseAndGetAddressOf())))
		{
			SR_LOG_ERROR("Failed to create Depth-Stencil view.");
			return false;
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
				SR_LOG_ERROR("Failed to create Depth-Stencil state.");
				return false;
			}
		}

		{
			D3D11_VIEWPORT viewport{};
			viewport.Width = static_cast<FLOAT>(window.GetSize().x);
			viewport.Height = static_cast<FLOAT>(window.GetSize().y);
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
		return true;
#endif // defined(SR_DIRECTX)
		return false;
	}

	bool RenderDevice::CreateShaderInputLayout(const Shader& vertexShader, const vector<ShaderInputElement>& shaderInputElements, ShaderInputLayout& shaderInputLayout)
	{
		if (shaderInputElements.empty())
		{
			SR_LOG_ERROR("Push input elements before creating ShaderInputLayout!");
			return false;
		}

#if defined(SR_DIRECTX)
		vector<D3D11_INPUT_ELEMENT_DESC> DX_inputElements;
		uint32 inputTotalByteSize = 0;
		for (const ShaderInputElement& shaderInputElement : shaderInputElements)
		{
			D3D11_INPUT_ELEMENT_DESC inputElementDesc{};
			inputElementDesc.AlignedByteOffset = inputTotalByteSize;
			inputElementDesc.Format = DX_ConvertGraphicsFormat(shaderInputElement._format);
			inputElementDesc.InputSlot = shaderInputElement._inputSlot;
			inputElementDesc.InputSlotClass = ShaderInputLayout::DX_ConvertInputSlotClass(shaderInputElement._inputSlotClass);
			inputElementDesc.SemanticName = shaderInputElement._semanticName;
			inputElementDesc.SemanticIndex = shaderInputElement._semanticIndex;
			inputElementDesc.InstanceDataStepRate = shaderInputElement._instanceStepRate;
			DX_inputElements.push_back(inputElementDesc);

			inputTotalByteSize += ShaderInputLayout::ComputeInputElementByteSize(shaderInputElement);
		}
		if (SUCCEEDED(_device->CreateInputLayout(&DX_inputElements[0], static_cast<UINT>(DX_inputElements.size()),
			vertexShader._shaderBlob->GetBufferPointer(), vertexShader._shaderBlob->GetBufferSize(), shaderInputLayout._inputLayout.ReleaseAndGetAddressOf())))
		{
			return true;
		}
#endif // defined(SR_DIRECTX)
		SR_LOG_ERROR("Failed to create ShaderInputLayout");
		return false;
	}

	bool RenderDevice::CreateShader(const char* sourceCode, const ShaderType& shaderType, const char* shaderIdentifier, const char* entryPoint, const char* target, ShaderHeaderSet* const shaderHeaderSet, Shader& shader)
	{
		if (sourceCode == nullptr)
		{
			SR_LOG_ERROR("Must exist source code!");
			return false;
		}

		if (entryPoint == nullptr)
		{
			SR_LOG_ERROR("Must specify entry point!");
			return false;
		}

		if (target == nullptr)
		{
			SR_LOG_ERROR("Must specify target!");
			return false;
		}

#if defined(SR_DIRECTX)
		const UINT debugFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		HRESULT result = ::D3DCompile(sourceCode, ::strlen(sourceCode), shaderIdentifier, nullptr, shaderHeaderSet, entryPoint, target, debugFlag, 0, shader._shaderBlob.ReleaseAndGetAddressOf(), shader._errorMessageBlob.ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			String errorMessages(reinterpret_cast<char*>(shader._errorMessageBlob->GetBufferPointer()));
			SR_LOG_ERROR("Shader compile failed.");
			return false;
		}

		if (shaderType == ShaderType::VertexShader)
		{
			if (FAILED(_device->CreateVertexShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), NULL, reinterpret_cast<ID3D11VertexShader**>(shader._shader.ReleaseAndGetAddressOf()))))
			{
				return false;
			}
		}
		else if (shaderType == ShaderType::PixelShader)
		{
			if (FAILED(_device->CreatePixelShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), NULL, reinterpret_cast<ID3D11PixelShader**>(shader._shader.ReleaseAndGetAddressOf()))))
			{
				return false;
			}
		}
		else
		{
			return false;
		}

		shader._type = shaderType;
		return true;
#else
		return false;
#endif // defined(SR_DIRECTX)
	}

	bool RenderDevice::CreateTexture2D(const TextureFormat& format, const void* const resourceContent, const uint32 width, const uint32 height, Resource& resource)
	{
#if defined(SR_DIRECTX)
		ComPtr<ID3D11Resource> newResource;
		D3D11_TEXTURE2D_DESC texture2DDescriptor{};
		texture2DDescriptor.Width = width;
		texture2DDescriptor.Height = height;
		texture2DDescriptor.MipLevels = 1;
		texture2DDescriptor.ArraySize = 1;
		texture2DDescriptor.Format = DX_ConvertTextureFormat(format);
		texture2DDescriptor.SampleDesc.Count = 1;
		texture2DDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		texture2DDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
		texture2DDescriptor.CPUAccessFlags = 0;
		const uint32 elementStride = Resource::__ComputeElementStride(format);
		D3D11_SUBRESOURCE_DATA subResource{};
		subResource.pSysMem = resourceContent;
		subResource.SysMemPitch = texture2DDescriptor.Width * elementStride;
		subResource.SysMemSlicePitch = 0;
		if (SUCCEEDED(_device->CreateTexture2D(&texture2DDescriptor, &subResource, reinterpret_cast<ID3D11Texture2D**>(newResource.ReleaseAndGetAddressOf()))))
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor{};
			shaderResourceViewDescriptor.Format = texture2DDescriptor.Format;
			shaderResourceViewDescriptor.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDescriptor.Texture2D.MipLevels = texture2DDescriptor.MipLevels;
			shaderResourceViewDescriptor.Texture2D.MostDetailedMip = 0;
			if (SUCCEEDED(_device->CreateShaderResourceView(newResource.Get(), &shaderResourceViewDescriptor, reinterpret_cast<ID3D11ShaderResourceView**>(resource._view.ReleaseAndGetAddressOf()))))
			{
				resource._type = ResourceType::Teture2D;
				resource._format = format;

				resource._elementStride = elementStride;
				resource._elementMaxCount = texture2DDescriptor.Width * texture2DDescriptor.Height;

				resource._width = width;

				Swap(resource._resource, newResource);
				return true;
			}
		}
#endif // defined(SR_DIRECTX)
		return false;
	}

	bool RenderDevice::CreateBuffer(const ResourceType& type, const void* const content, const uint32 elementStride, const uint32 elementCount, Resource& resource)
	{
		if (type == ResourceType::Teture2D)
		{
			SR_ASSERT(false, "Use CreateTexture2D() instead!");
			return false;
		}

#if defined(SR_DIRECTX)
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
		if (SUCCEEDED(_device->CreateBuffer(&bufferDescriptor, (content != nullptr) ? &subresourceData : nullptr, reinterpret_cast<ID3D11Buffer**>(newResource.ReleaseAndGetAddressOf()))))
		{
			resource._type = type;
			resource._byteSize = bufferDescriptor.ByteWidth;
			resource._elementStride = elementStride;
			resource._elementMaxCount = elementCount;

			Swap(resource._resource, newResource);
			return true;
		}
#endif // defined(SR_DIRECTX)
		return false;
	}

	bool RenderDevice::UpdateShaderResource(const void* const content, const uint32 elementStride, const uint32 elementCount, Resource& resource)
	{
		if (elementCount > resource._elementMaxCount)
		{
			return CreateBuffer(resource._type, content, elementStride, elementCount, resource);
		}

#if defined(SR_DIRECTX)
		class SafeResourceMapper
		{
		public:
			SafeResourceMapper(RenderDevice& renderDevice, ID3D11Resource* const resource, const uint32 subresource)
				: _renderDevice{ renderDevice }
				, _resource{ resource }
				, _subresource{ subresource }
				, _mappedSubresource{}
			{
				if (FAILED(_renderDevice._deviceContext->Map(_resource, _subresource, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &_mappedSubresource)))
				{
					_mappedSubresource.pData = nullptr;
					_mappedSubresource.DepthPitch = 0;
					_mappedSubresource.RowPitch = 0;
				}
			}
			~SafeResourceMapper()
			{
				if (IsValid() == true)
				{
					_renderDevice._deviceContext->Unmap(_resource, _subresource);
				}
			}
			bool IsValid() const noexcept
			{
				return _mappedSubresource.pData != nullptr;
			}
			void Set(const void* const data, const uint32 size) noexcept
			{
				::memcpy(_mappedSubresource.pData, data, size);
			}

		private:
			RenderDevice& _renderDevice;
			ID3D11Resource* const _resource;
			const uint32 _subresource;
			D3D11_MAPPED_SUBRESOURCE _mappedSubresource;
		};

		SafeResourceMapper safeResourceMapper(*this, resource._resource.Get(), 0);
		if (safeResourceMapper.IsValid())
		{
			safeResourceMapper.Set(content, elementStride * elementCount);
			return true;
		}
#endif // defined(SR_DIRECTX)
		return false;
	}

	void RenderDevice::BindShaderInputLayout(ShaderInputLayout& shaderInputLayout)
	{
		_is_InputLayout_bound = true;
#if defined(SR_DIRECTX)
		if (shaderInputLayout._inputLayout.Get() != nullptr)
			_deviceContext->IASetInputLayout(shaderInputLayout._inputLayout.Get());
#endif // defined(SR_DIRECTX)
	}

	void RenderDevice::BindShader(Shader& shader)
	{
		if (shader._type == ShaderType::VertexShader)
		{
			_is_VS_bound = true;
#if defined(SR_DIRECTX)
			_deviceContext->VSSetShader(static_cast<ID3D11VertexShader*>(shader._shader.Get()), nullptr, 0);
#endif // defined(SR_DIRECTX)
		}
		else if (shader._type == ShaderType::PixelShader)
		{
			_is_PS_bound = true;
#if defined(SR_DIRECTX)
			_deviceContext->PSSetShader(static_cast<ID3D11PixelShader*>(shader._shader.Get()), nullptr, 0);
#endif // defined(SR_DIRECTX)
		}
	}

	void RenderDevice::BindInput(Resource& resource, const uint32 slot)
	{
		if (resource._type == ResourceType::VertexBuffer)
		{
			_is_VertexBuffer_bound = true;
#if defined(SR_DIRECTX)
			ID3D11Buffer* buffers[1]{ static_cast<ID3D11Buffer*>(resource._resource.Get()) };
			uint32 strides[1]{ resource._elementStride };
			uint32 offsets[1]{ 0 };
			_deviceContext->IASetVertexBuffers(slot, 1, buffers, strides, offsets);
#endif // defined(SR_DIRECTX)
		}
		else if (resource._type == ResourceType::IndexBuffer)
		{
			_is_IndexBuffer_bound = true;
#if defined(SR_DIRECTX)
			_deviceContext->IASetIndexBuffer(static_cast<ID3D11Buffer*>(resource._resource.Get()), DX_ConvertGraphicsFormat(Resource::kIndexBufferFormat), 0);
#endif // defined(SR_DIRECTX)
		}
		else
		{
			SR_LOG_ERROR("!!!");
		}
	}

	void RenderDevice::BindShaderResource(const ShaderType shaderType, Resource& resource, const uint32 slot)
	{
		if (resource._type == ResourceType::ConstantBuffer)
		{
#if defined(SR_DIRECTX)
			ID3D11Buffer* buffers[1]{ static_cast<ID3D11Buffer*>(resource._resource.Get()) };
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
				SR_LOG_ERROR("This shader type is not supported yet!");
			}
#endif // defined(SR_DIRECTX)
		}
		else if (resource._type == ResourceType::Teture2D)
		{
#if defined(SR_DIRECTX)
			ID3D11ShaderResourceView* views[1]{ static_cast<ID3D11ShaderResourceView*>(resource._view.Get()) };
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
				SR_LOG_ERROR("This shader type is not supported yet!");
			}
#endif // defined(SR_DIRECTX)
		}
		else
		{
			SR_LOG_ERROR("This resource type is not supported yet!");
		}
	}

	void RenderDevice::UseTrianglePrimitive()
	{
#if defined(SR_DIRECTX)
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#endif // defined(SR_DIRECTX)
	}

	void RenderDevice::BeginRendering(const Color& clearColor)
	{
#if defined(SR_DIRECTX)
		_deviceContext->ClearRenderTargetView(_backBufferRtv.Get(), clearColor.f);
		_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
#endif // defined(SR_DIRECTX)
		UseTrianglePrimitive();
	}

	void RenderDevice::DrawIndexed(const uint32 indexCount)
	{
		if (_is_InputLayout_bound == false)
		{
			SR_LOG_ERROR("You must bind ShaderInputLayout first!");
			return;
		}
		if (_is_VS_bound == false || _is_PS_bound == false)
		{
			SR_LOG_ERROR("You must at least bind VertexShader and PixelShader first!");
			return;
		}
		if (_is_VertexBuffer_bound == false)
		{
			SR_LOG_ERROR("You must bind VertexBuffer first!");
			return;
		}
		if (_is_IndexBuffer_bound == false)
		{
			SR_LOG_ERROR("You must bind IndexBuffer first!");
			return;
		}

#if defined(SR_DIRECTX)
		_deviceContext->DrawIndexed(indexCount, 0, 0);
#endif // defined(SR_DIRECTX)
	}

	void RenderDevice::Draw(const uint32 vertexCount)
	{
		if (_is_VertexBuffer_bound == false)
		{
			SR_LOG_ERROR("You must bind VertexBuffer first!");
			return;
		}

#if defined(SR_DIRECTX)
		_deviceContext->Draw(vertexCount, 0);
#endif // defined(SR_DIRECTX)
	}

	void RenderDevice::EndRendering()
	{
#if defined(SR_DIRECTX)
		_swapChain->Present(0, 0);
#endif // defined(SR_DIRECTX)
	}

	bool App::IsRunning()
	{
		if (_window.ProcessMessages() < 0)
		{
			return false;
		}
		return true;
	}

	void App::BeginRendering()
	{
		_renderDevice.BeginRendering(_clearColor);
	}

	void App::DrawIndexed(const uint32 indexCount)
	{
		_renderDevice.DrawIndexed(indexCount);
	}

	void App::DrawTextAt(const Color& color, const String& text, const float2& position)
	{
		if (text.empty() == true)
		{
			return;
		}

		const float unit_x = _fontScale.x * kFontTextureGlyphWidth;
		const float unit_y = _fontScale.y * kFontTextureGlyphHeight;
		const float2 sizeUnit = float2(unit_x, unit_y);
		const float2 positionUnit = float2(unit_x, 0);
		uint32 chCount = 0;
		for (const char& ch : text)
		{
			const FontGlyphMeta& glyphMeta = _defaultFontData.GetGlyphMeta(ch);
			const float u0 = glyphMeta._u0;
			const float u1 = glyphMeta._u1;
			const float v0 = glyphMeta._v0;
			const float v1 = glyphMeta._v1;
			MeshGenerator<FONT_VS_INPUT>::Push2DRectangle(color, sizeUnit, position + sizeUnit * 0.5f + positionUnit * (float)chCount, 0.0f, _fontVertices, _fontIndices);
			_fontVertices[_fontVertices.size() - 4]._texcoord = float2(u0, v0);
			_fontVertices[_fontVertices.size() - 3]._texcoord = float2(u0, v1);
			_fontVertices[_fontVertices.size() - 2]._texcoord = float2(u1, v1);
			_fontVertices[_fontVertices.size() - 1]._texcoord = float2(u1, v0);
			++chCount;
		}
	}

	void App::Draw(const uint32 vertexCount)
	{
		_renderDevice.Draw(vertexCount);
	}

	void App::EndRendering()
	{
		if (_fontVertices.empty() == false)
		{
			_renderDevice.UpdateShaderResource(&_fontVertices[0], sizeof(FONT_VS_INPUT), (uint32)_fontVertices.size(), _fontVertexBuffer);
			_renderDevice.UpdateShaderResource(&_fontIndices[0], sizeof(uint32), (uint32)_fontIndices.size(), _fontIndexBuffer);

			BindDefaultFontData();

			DrawIndexed((uint32)_fontIndices.size());
			_fontVertices.clear();
			_fontIndices.clear();
		}

		_renderDevice.EndRendering();
	}

	void App::CreateDefaultFontDataPushGlyphRow(const uint32 rowIndex, const byte(&ch)[kFontTextureGlyphCountInRow])
	{
		const float glyphTextureWidth = (float)kFontTextureGlyphWidth;
		const float glyphTextureHeight = (float)kFontTextureGlyphHeight;
		const float glyphTextureUnit_U = glyphTextureWidth / kFontTextureWidth;
		const float glyphTextureUnit_V = glyphTextureHeight / kFontTextureHeight;
		const float v0 = glyphTextureUnit_V * rowIndex;
		const float v1 = v0 + glyphTextureUnit_V;
		for (uint32 iter = 0; iter < kFontTextureGlyphCountInRow; ++iter)
		{
			_defaultFontData.PushGlyph(FontGlyphMeta(ch[iter], glyphTextureUnit_U * iter, v0, glyphTextureUnit_U * (iter + 1), v1));
		}
	}

	void App::CreateDefaultFontData()
	{
		_fontShaderHeaderSet.PushShaderHeader("FontShaderHeader", kFontShaderHeaderCode);

		_renderDevice.CreateShader(kFontVertexShaderCode, ShaderType::VertexShader, "FontVertexShader", "main", "vs_5_0", &_fontShaderHeaderSet, _fontVertexShader);

		vector<ShaderInputElement> shaderInputElements;
		shaderInputElements.push_back(ShaderInputElement::CreateInputelement_float4("POSITION", 0));
		shaderInputElements.push_back(ShaderInputElement::CreateInputelement_float4("COLOR", 0));
		shaderInputElements.push_back(ShaderInputElement::CreateInputelement_float2("TEXCOORD", 0));
		_renderDevice.CreateShaderInputLayout(_fontVertexShader, shaderInputElements, _fontShaderInputLayout);

		_renderDevice.CreateShader(kFontPixelShaderCode, ShaderType::PixelShader, "FontPixelShader", "main", "ps_5_0", &_fontShaderHeaderSet, _fontPixelShader);

		const uint2& screenSize = _window.GetSize();
		FONT_CB_MATRICES font_cb_matrices;
		font_cb_matrices._projectionMatrix.MakePixelCoordsProjectionMatrix(screenSize);
		_renderDevice.CreateBuffer(ResourceType::ConstantBuffer, &font_cb_matrices, sizeof(font_cb_matrices), 1, _fontCBMatrices);

		byte bytes[kFontTextureByteCount]{};
		for (uint32 iter = 0; iter < kFontTextureByteCount; ++iter)
		{
			const uint32 bitAt = iter % 8;
			const uint32 byteAt = iter / 8;
			const byte byte_ = (kFontTextureRawBitData[byteAt] >> (7 - bitAt)) & 1;
			bytes[iter] = byte_ * 255;
		}
		_renderDevice.CreateTexture2D(TextureFormat::R8_UNORM, bytes, kFontTextureWidth, kFontTextureHeight, _fontTexture);

		MeshGenerator<FONT_VS_INPUT>::Push2DRectangle(Color(), float2(512, 480), float2(256, 240), 0.0f, _fontVertices, _fontIndices);
		_fontVertices[0]._texcoord = float2(0, 0);
		_fontVertices[1]._texcoord = float2(1, 0);
		_fontVertices[2]._texcoord = float2(0, 1);
		_fontVertices[3]._texcoord = float2(1, 1);
		_renderDevice.CreateBuffer(ResourceType::VertexBuffer, &_fontVertices[0], sizeof(FONT_VS_INPUT), (uint32)_fontVertices.size(), _fontVertexBuffer);
		_renderDevice.CreateBuffer(ResourceType::IndexBuffer, &_fontIndices[0], sizeof(uint32), (uint32)_fontIndices.size(), _fontIndexBuffer);

		byte row0[kFontTextureGlyphCountInRow]{ ' ','!','\"','$','#','%','&','\'','(',')','*','+',',','-','.','/' };
		CreateDefaultFontDataPushGlyphRow(0, row0);

		byte row1[kFontTextureGlyphCountInRow]{ '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?' };
		CreateDefaultFontDataPushGlyphRow(1, row1);

		byte row2[kFontTextureGlyphCountInRow]{ '@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O' };
		CreateDefaultFontDataPushGlyphRow(2, row2);

		byte row3[kFontTextureGlyphCountInRow]{ 'P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_' };
		CreateDefaultFontDataPushGlyphRow(3, row3);

		byte row4[kFontTextureGlyphCountInRow]{ '`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o' };
		CreateDefaultFontDataPushGlyphRow(4, row4);

		byte row5[kFontTextureGlyphCountInRow]{ 'p','q','r','s','t','u','v','w','x','y','z','(','|',')','~', 0 };
		CreateDefaultFontDataPushGlyphRow(5, row5);
	}

	void App::BindDefaultFontData()
	{
		_renderDevice.BindShader(_fontVertexShader);
		_renderDevice.BindShader(_fontPixelShader);
		_renderDevice.BindShaderInputLayout(_fontShaderInputLayout);
		_renderDevice.BindShaderResource(ShaderType::VertexShader, _fontCBMatrices, 0);
		_renderDevice.BindShaderResource(ShaderType::PixelShader, _fontTexture, 0);
		_renderDevice.BindInput(_fontVertexBuffer, 0);
		_renderDevice.BindInput(_fontIndexBuffer, 0);
	}

	bool ReadFile(const String& file_name, String& out_content)
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

			//String _debug_name;
			//String _debug_value;

			bool IsValid() const { return _name_length > 0; }
			String GetName() const { return _XML->_text.substr(_name_at, _name_length); }
			String GetValue() const { return _XML->_text.substr(_value_at, _value_length); }
			Attribute GetNextAttribute() const { return GetNode().GetAttribute(_index_in_node + 1); }

		private:
			const Node& GetNode() const { return _XML->GetNode(_node_ID); }
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
			vector<size_t> _attribute_IDs;
			vector<size_t> _child_node_IDs;

			//String _debug_name;

			bool IsValid() const { return _name_length > 0; }
			String GetName() const { return _XML->_text.substr(_name_at, _name_length); }
			const Attribute& GetAttribute(const size_t index) const { return _XML->GetAttribute((index >= _attribute_IDs.size() ? INVALID_ID : _attribute_IDs[index])); }
			const Node& GetChildNode(const size_t index) const { return _XML->GetNode((index >= _child_node_IDs.size() ? INVALID_ID : _child_node_IDs[index])); }
			const Node& GetNextSiblingNode() const { return _XML->GetNode(_parent_ID).GetChildNode(_index_in_parent_node + 1); }
		private:
			bool HasName() const { return _name_length > 0; }
			const XML* _XML = nullptr;
		};
		bool Parse(const String& text)
		{
			_text = text;
			if (CheckValidity() == false)
			{
				return false;
			}

			_at = 0;
			_line = 1;
			return ParseNode(INVALID_ID);
		}
		const Node& GetRootNode() const { return _nodes[0]; }
		const Node& GetNode(const size_t ID) const { return (ID >= _nodes.size() ? INVALID_NODE : _nodes[ID]); }
		const Attribute& GetAttribute(const size_t ID) const { return (ID >= _attributes.size() ? INVALID_ATTRIBUTE : _attributes[ID]); }
	private:
		bool AdvanceToFind(const char ch)
		{
			const size_t length = _text.length();
			while (_at < length && _text[_at] != ch)
			{
				++_at;
			}
			return _text[_at] == ch;
		}
		bool ParseNode(const size_t parent_node_ID)
		{
			const size_t length = _text.length();
			if (AdvanceToFind('<') == false)
			{
				return (_at == length);
			}

			if (_at + 1 < length && _text[_at + 1] == '/')
			{
				if (AdvanceToFind('>') == false)
				{
					return false;
				}

				return ParseNode(_nodes[parent_node_ID]._parent_ID);
			}

			const size_t node_name_at = _at + 1;
			if (AdvanceToFind('>') == false)
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

				if (_nodes[node_ID].HasName() == false)
				{
					_nodes[node_ID]._name_length = at - node_name_at;

					//_nodes[node_ID]._debug_name = _text.substr(node_name_at, _nodes[node_ID]._name_length);
				}

				if ((_text[at] == '/' && at + 1 < length && _text[at + 1] == '>') || _text[at] == '>')
				{
					// end
					_at = at + 1;
					return ParseNode((is_open_close_node ? parent_node_ID : _nodes[node_ID]._ID));
				}
				else
				{
					// attributes
					_at = at + 1;
					ParseAttribute(_nodes[node_ID]);
				}
			}
			return true;
		}
		bool ParseAttribute(Node& node)
		{
			_attributes.push_back(Attribute());

			Attribute& attribute = _attributes.back();
			attribute._XML = this;
			attribute._ID = _attributes.size() - 1;
			attribute._name_at = _at;

			node._attribute_IDs.push_back(attribute._ID);
			attribute._node_ID = node._ID;
			attribute._index_in_node = node._attribute_IDs.size() - 1;

			if (AdvanceToFind('=') == false)
			{
				ReportError("invalid attribute! '=' is needed.");
				return false;
			}

			attribute._name_length = _at - attribute._name_at;
			//attribute._debug_name = _text.substr(attribute._name_at, attribute._name_length);

			if (_text[_at + 1] != '\"')
			{
				ReportError("'\"' must be followed by '='.");
				return false;
			}
			_at += 2;
			attribute._value_at = _at;

			if (AdvanceToFind('\"') == false)
			{
				ReportError("closing '\"' is missing.");
				return false;
			}
			attribute._value_length = _at - attribute._value_at;
			//attribute._debug_value = _text.substr(attribute._value_at, attribute._value_length);
			return true;
		}

	private:
		bool CheckValidity() const
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
							ReportError(message, at);
							return false;
						}
					}
				}
			}
			return true;
		}
		void ReportError(const String& error) const { _error = error; __ReportWhere(_at); }
		void ReportError(const String& error, const size_t at) const { _error = error; __ReportWhere(at); }
		void __ReportWhere(const size_t at) const { _error += " at["; _error += ToString(at); _error += "] line["; _error += ToString(_line); _error += "]"; }

	private:
		String _text;
		vector<Node> _nodes;
		vector<Attribute> _attributes;
		mutable String _error;
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
		shaderHeaderSet.PushShaderHeader("StreamData", kSampleShaderHeaderCode_StreamData);
		Shader vertexShader;
		renderDevice.CreateShader(kSampleVertexShaderCode, ShaderType::VertexShader, "SampleVertexShader", "main", "vs_5_0", &shaderHeaderSet, vertexShader);
		vector<ShaderInputElement> shaderInputElements;
		shaderInputElements.push_back(ShaderInputElement::CreateInputelement_float4("POSITION", 0));
		shaderInputElements.push_back(ShaderInputElement::CreateInputelement_float4("COLOR", 0));
		shaderInputElements.push_back(ShaderInputElement::CreateInputelement_float2("TEXCOORD", 0));
		ShaderInputLayout shaderInputLayout;
		renderDevice.CreateShaderInputLayout(vertexShader, shaderInputElements, shaderInputLayout);
		Shader pixelShader;
		renderDevice.CreateShader(kSamplePixelShaderCode, ShaderType::PixelShader, "SamplePixelShader", "main", "ps_5_0", &shaderHeaderSet, pixelShader);
		Resource vscbMatrices;
		SAMPLE_CB_MATRICES cb_matrices;
		cb_matrices._projectionMatrix.MakePixelCoordsProjectionMatrix(kScreenSize);
		renderDevice.CreateBuffer(ResourceType::ConstantBuffer, &cb_matrices, sizeof(SAMPLE_CB_MATRICES), 1, vscbMatrices);
		vector<SAMPLE_VS_INPUT> vertices;
		vector<uint32> indices;
		Resource vertexBuffer;
		vertexBuffer._type = ResourceType::VertexBuffer;
		Resource indexBuffer;
		indexBuffer._type = ResourceType::IndexBuffer;

		while (app.IsRunning())
		{
			app.BeginRendering();
			{
				vertices.clear();
				indices.clear();

				MeshGenerator<SAMPLE_VS_INPUT>::Push2DCircle(Color(1, 1, 0, 1), float2(100, 100), 32.0f, 16, vertices, indices);
				renderDevice.BindShaderInputLayout(shaderInputLayout);
				renderDevice.BindShader(vertexShader);
				renderDevice.BindShader(pixelShader);
				renderDevice.BindShaderResource(ShaderType::VertexShader, vscbMatrices, 0);
				renderDevice.BindInput(vertexBuffer, 0);
				renderDevice.BindInput(indexBuffer, 0);
				if (vertices.empty() == false)
				{
					renderDevice.UpdateShaderResource(&vertices[0], sizeof(SAMPLE_VS_INPUT), (uint32)vertices.size(), vertexBuffer);
					renderDevice.UpdateShaderResource(&indices[0], sizeof(uint32), (uint32)indices.size(), indexBuffer);
					renderDevice.DrawIndexed((uint32)indices.size());
				}
				app.DrawTextAt(Color(1, 1, 1, 1), "Sample Window", float2(10, 10));
			}
			app.EndRendering();
		}
		return 0;
	}
#pragma endregion
}
