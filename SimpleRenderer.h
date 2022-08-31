﻿#pragma once

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

#pragma region Constants
    constexpr float kPi = 3.14159265f;
    constexpr float k2Pi = kPi * 2.0f;

    // ### Copy and paste to see this better ###
    // Replace ', ' with '_'
    // Replace '0b' with '.'
    // Replace  '0' with ' '
    // Size: 16 x 60
    static constexpr uint32 kFontTextureWidth = 128;
    static constexpr uint32 kFontTextureHeight = 60;
    static constexpr uint32 kFontTextureByteCount = kFontTextureWidth * kFontTextureHeight;
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
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00011000, 0b00010000, 0b00110000, 0b00000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100000, 0b00010000, 0b00001000, 0b00000000, 0b00000000,
        0b01011100, 0b01110100, 0b01011000, 0b00111100, 0b01111100, 0b01000100, 0b01000100, 0b01000100, 0b01000100, 0b10001000, 0b11111000, 0b00100000, 0b00010000, 0b00001000, 0b11100000, 0b00000000,
        0b01100010, 0b10001100, 0b01100100, 0b01000010, 0b00010000, 0b01000100, 0b01000100, 0b01010100, 0b00101000, 0b10001000, 0b00010000, 0b01100000, 0b00010000, 0b00001100, 0b10010010, 0b00000000,
        0b01000010, 0b10000100, 0b01000000, 0b00111000, 0b00010000, 0b01000100, 0b01000100, 0b01010100, 0b00010000, 0b10001000, 0b00100000, 0b00100000, 0b00010000, 0b00001000, 0b00001110, 0b00000000,
        0b01111100, 0b01111100, 0b01000000, 0b00000110, 0b00010010, 0b01000100, 0b00101000, 0b01010100, 0b00101000, 0b01111000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000000, 0b00000000,
        0b01000000, 0b00000100, 0b01000000, 0b01111100, 0b00001100, 0b00111010, 0b00010000, 0b00101000, 0b01000100, 0b00001000, 0b11111000, 0b00011000, 0b00010000, 0b00110000, 0b00000000, 0b00000000,
        0b01000000, 0b00000100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    };
#pragma endregion

    struct float2
    {
        constexpr float2() : float2(0, 0) { __noop; }
        constexpr float2(float x_, float y_) : x{ x_ }, y{ y_ } { __noop; }
        float& operator[](const uint32 index) { return f[index]; }
        const float& operator[](const uint32 index) const { return f[index]; }
        float2 operator+(const float2& rhs) const { return float2(x + rhs.x, y + rhs.y); }
        float2 operator-(const float2& rhs) const { return float2(x - rhs.x, y - rhs.y); }
        float2 operator*(const float s) const { return float2(x * s, y * s); }
        float2 operator/(const float s) const { return float2(x / s, y / s); }
        constexpr float dot(const float2& rhs) const { return x * rhs.x + y * rhs.y; }
        constexpr float lengthSq() const { return dot(*this); }
        float length() const { return ::sqrt(lengthSq()); }
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
        void setPoint(const float2& position) { x = position.x; y = position.y; z = 0; w = 1; }
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
            float4 sampled = g_texture0.Sample(g_sampler0, input.texcoord.xy);
            float4 sampledSqrt = sqrt(sampled.r);
            return float4(sampledSqrt);
        }
    )";

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
        std::string _headerCode;
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

        static InputElement createInputElementFloat4(const char* const semanticName, const uint32 semanticIndex) { return __createInputElement_common(DXGI_FORMAT_R32G32B32A32_FLOAT, semanticName, semanticIndex); }
        static InputElement createInputElementFloat3(const char* const semanticName, const uint32 semanticIndex) { return __createInputElement_common(DXGI_FORMAT_R32G32B32_FLOAT, semanticName, semanticIndex); }
        static InputElement createInputElementFloat2(const char* const semanticName, const uint32 semanticIndex) { return __createInputElement_common(DXGI_FORMAT_R32G32_FLOAT, semanticName, semanticIndex); }
        static InputElement createInputElementFloat(const char* const semanticName, const uint32 semanticIndex) { return __createInputElement_common(DXGI_FORMAT_R32_FLOAT, semanticName, semanticIndex); }

        void clearInputElements();
        void pushInputElement(const InputElement& newInputElement);

        bool create(Renderer& renderer, const Shader& vertexShader);

    private:
        static InputElement __createInputElement_common(const DXGI_FORMAT format, const char* const semanticName, const uint32 semanticIndex)
        {
            InputElement inputElement;
            inputElement._format = format;
            inputElement._semanticName = semanticName;
            inputElement._semanticIndex = semanticIndex;
            return inputElement;
        }

        uint32 computeInputElementByteSize(const D3D11_INPUT_ELEMENT_DESC& inputElementDesc)
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
        bool create(Renderer& renderer, const char* sourceCode, const ShaderType& shaderType, const char* shaderIdentifier, const char* entryPoint, const char* target, ShaderHeader* const shaderHeader = nullptr);

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
        bool createTexture2D(Renderer& renderer, const TextureFormat& format, const void* const resourceContent, const uint32 width, const uint32 height);
        bool createBuffer(Renderer& renderer, const ResourceType& type, const void* const content, const uint32 elementStride, const uint32 elementCount);
        bool update(Renderer& renderer, const void* const content, const uint32 elementStride, const uint32 elementCount);

    private:
        static DXGI_FORMAT __convert_to_DXGI_FORMAT(const TextureFormat& format);
        static uint32 __computeElementStride(const TextureFormat& format);

    public:
        ID3D11Resource* getResource() const { return _resource.Get(); }
        ID3D11View* getView() const { return _view.Get(); }

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
        static void push_2D_triangle(const float2& a, const float2& b, const float2& c, std::vector<Vertex>& vertices, std::vector<uint32>& indices)
        {
            const uint64 vertexBase = vertices.size();
            vertices.resize(vertexBase + 3);

            const uint64 indexBase = indices.size();
            indices.reserve(indexBase + 3);

            vertices[vertexBase + 0]._position = float4(a.x, a.y, 0, 1);
            vertices[vertexBase + 1]._position = float4(b.x, b.y, 0, 1);
            vertices[vertexBase + 2]._position = float4(c.x, c.y, 0, 1);

            pushIndex(indices, vertexBase + 0);
            pushIndex(indices, vertexBase + 1);
            pushIndex(indices, vertexBase + 2);
        }

        static void push_2D_rectangle(const float2& centerPosition, const float2& size, const float rotationAngle, std::vector<Vertex>& vertices, std::vector<uint32>& indices)
        {
            const uint64 vertexBase = vertices.size();
            vertices.resize(vertexBase + 4);

            const uint64 indexBase = indices.size();
            indices.reserve(indexBase + 6);

            const float2& halfSize = size * 0.5f;
            const float cosTheta = ::cos(rotationAngle);
            const float sinTheta = ::sin(rotationAngle);
            const float2 rotatedX = float2(cosTheta, sinTheta);
            const float2 rotatedY = float2(-sinTheta, cosTheta);
            vertices[vertexBase + 0]._position.setPoint(centerPosition - rotatedX * halfSize.x - rotatedY * halfSize.y);
            vertices[vertexBase + 1]._position.setPoint(centerPosition + rotatedX * halfSize.x - rotatedY * halfSize.y);
            vertices[vertexBase + 2]._position.setPoint(centerPosition - rotatedX * halfSize.x + rotatedY * halfSize.y);
            vertices[vertexBase + 3]._position.setPoint(centerPosition + rotatedX * halfSize.x + rotatedY * halfSize.y);

            pushIndex(indices, vertexBase + 0);
            pushIndex(indices, vertexBase + 1);
            pushIndex(indices, vertexBase + 2);

            pushIndex(indices, vertexBase + 1);
            pushIndex(indices, vertexBase + 3);
            pushIndex(indices, vertexBase + 2);
        }

        static void push_2D_circle(const float2& centerPosition, float radius, uint32 sideCount, std::vector<Vertex>& vertices, std::vector<uint32>& indices)
        {
            radius = max(radius, 1.0f);
            sideCount = max(sideCount, 4);

            const uint64 vertexBase = vertices.size();
            vertices.resize(vertexBase + sideCount + 1);
            const uint64 indexBase = indices.size();
            indices.reserve(indexBase + static_cast<uint64>(sideCount) * 3);

            vertices[vertexBase]._position = float4(centerPosition.x, centerPosition.y, 0, 1);
            for (uint32 sideIndex = 0; sideIndex < sideCount; ++sideIndex)
            {
                const float theta = (k2Pi * sideIndex) / sideCount;
                const float x = radius * ::cos(theta);
                const float y = -radius * ::sin(theta);
                vertices[vertexBase + sideIndex + 1]._position = float4(centerPosition.x + x, centerPosition.y + y, 0, 1);

                pushIndex(indices, vertexBase + 0);
                pushIndex(indices, vertexBase + sideIndex + 2);
                pushIndex(indices, vertexBase + sideIndex + 1);
            }
            indices[indices.size() - 2] = static_cast<uint32>(vertexBase + 1);
        }

        static void push_2D_lineSegment(const float2& a, const float2& b, float thickness, std::vector<Vertex>& vertices, std::vector<uint32>& indices)
        {
            thickness = max(thickness, 1.0f);

            const float2 ab = b - a;
            const float l = ab.length();
            if (l == 0.0f)
            {
                return;
            }

            const float2 direction = ab / l;
            const float rotationAngle = ::atan2f(direction.y, direction.x);
            const float2 m = (a + b) * 0.5f;
            push_2D_rectangle(m, float2(l, thickness), rotationAngle, vertices, indices);
        }

        static void fillVertexColor(std::vector<Vertex>& vertices, const Color& color)
        {
            for (auto& vertex : vertices)
            {
                vertex._color = color;
            }
        }

    private:
        static void pushIndex(std::vector<uint32>& indices, const uint64 index)
        {
            indices.push_back(static_cast<uint32>(index));
        }
    };

    class Renderer final
    {
    public:
        Renderer(const float2& windowSize, const Color& clearColor) : _windowSize{ windowSize }, _clearColor{ clearColor } { if (createWindow()) createDevice(); }
        ~Renderer() { destroyWindow(); }

    public:
        bool isRunning();

    public:
        void bindShaderInputLayout(ShaderInputLayout& shaderInputLayout);
        void bindShader(Shader& shader);
        void bindInput(Resource& resource, const uint32 slot);
        void bindShaderResource(const ShaderType shaderType, Resource& resource, const uint32 slot);
        void useTrianglePrimitive();

    public:
        bool beginRendering();
        void draw(const uint32 vertexCount);
        void drawIndexed(const uint32 indexCount);
        void drawText(const std::string& text);
        void endRendering();

    public:
        ID3D11Device* getDevice() const { return _device.Get(); }
        ID3D11DeviceContext* getDeviceContext() const { return _deviceContext.Get(); }

    private:
        bool createWindow();
        void destroyWindow();
        void createDevice();
        void createDevice_createDefaultFontData();
        void bindDefaultFontData();

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
        ComPtr<ID3D11SamplerState> _defaultSamplerState;
        ComPtr<ID3D11BlendState> _defaultBlendState;

    private:
        bool _is_InputLayout_bound = false;
        bool _is_VS_bound = false;
        bool _is_PS_bound = false;
        bool _is_VertexBuffer_bound = false;
        bool _is_IndexBuffer_bound = false;

    private:
        ShaderHeader _defaultFontShaderHeader;
        Shader _defaultFontVertexShader;
        ShaderInputLayout _defaultFontShaderInputLayout;
        Shader _defaultFontPixelShader;
        Resource _defaultFontCBMatrices;
        Resource _defaultFontTexture;
        Resource _defaultFontVertexBuffer;
        Resource _defaultFontIndexBuffer;
        std::vector<DEFAULT_FONT_VS_INPUT> _defaultFontVertices;
        std::vector<uint32> _defaultFontIndices;
    };


#pragma region Function Definitions
    HRESULT ShaderHeader::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
    {
        pFileName = _headerName.c_str();
        *ppData = _headerCode.c_str();
        *pBytes = static_cast<UINT>(_headerCode.length());
        return S_OK;
    }

    void ShaderInputLayout::clearInputElements()
    {
        _inputElements.clear();
        _inputTotalByteSize = 0;
    }

    void ShaderInputLayout::pushInputElement(const InputElement& newInputElement)
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

        _inputTotalByteSize += computeInputElementByteSize(inputElementDesc);
    }

    bool ShaderInputLayout::create(Renderer& renderer, const Shader& vertexShader)
    {
        if (_inputElements.empty())
        {
            MINT_LOG_ERROR("Push input elements before creating ShaderInputLayout!");
            return false;
        }

        if (FAILED(renderer.getDevice()->CreateInputLayout(&_inputElements[0], static_cast<UINT>(_inputElements.size()),
            vertexShader._shaderBlob->GetBufferPointer(), vertexShader._shaderBlob->GetBufferSize(), _inputLayout.ReleaseAndGetAddressOf())))
        {
            MINT_LOG_ERROR("Failed to create ShaderInputLayout");
            return false;
        }
        return true;
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

    bool Resource::createTexture2D(Renderer& renderer, const TextureFormat& format, const void* const resourceContent, const uint32 width, const uint32 height)
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
        const uint32 elementStride = __computeElementStride(format);
        D3D11_SUBRESOURCE_DATA subResource{};
        subResource.pSysMem = resourceContent;
        subResource.SysMemPitch = texture2DDescriptor.Width * elementStride;
        subResource.SysMemSlicePitch = 0;
        if (SUCCEEDED(renderer.getDevice()->CreateTexture2D(&texture2DDescriptor, &subResource, reinterpret_cast<ID3D11Texture2D**>(newResource.ReleaseAndGetAddressOf()))))
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor;
            shaderResourceViewDescriptor.Format = texture2DDescriptor.Format;
            shaderResourceViewDescriptor.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDescriptor.Texture2D.MipLevels = texture2DDescriptor.MipLevels;
            shaderResourceViewDescriptor.Texture2D.MostDetailedMip = 0;
            if (SUCCEEDED(renderer.getDevice()->CreateShaderResourceView(newResource.Get(), &shaderResourceViewDescriptor, reinterpret_cast<ID3D11ShaderResourceView**>(_view.ReleaseAndGetAddressOf()))))
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

    bool Resource::createBuffer(Renderer& renderer, const ResourceType& type, const void* const content, const uint32 elementStride, const uint32 elementCount)
    {
        if (type == ResourceType::Teture2D)
        {
            MINT_ASSERT(false, "Use createTexture2D() instead!");
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
        if (SUCCEEDED(renderer.getDevice()->CreateBuffer(&bufferDescriptor, (content != nullptr) ? &subresourceData : nullptr, reinterpret_cast<ID3D11Buffer**>(newResource.ReleaseAndGetAddressOf()))))
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

    uint32 Resource::__computeElementStride(const TextureFormat& format)
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

    void Renderer::bindShaderInputLayout(ShaderInputLayout& shaderInputLayout)
    {
        _is_InputLayout_bound = true;

        if (shaderInputLayout._inputLayout.Get() != nullptr)
            _deviceContext->IASetInputLayout(shaderInputLayout._inputLayout.Get());
    }

    void Renderer::bindShader(Shader& shader)
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

    void Renderer::bindInput(Resource& resource, const uint32 slot)
    {
        if (resource._type == ResourceType::VertexBuffer)
        {
            _is_VertexBuffer_bound = true;

            ID3D11Buffer* buffers[1]{ static_cast<ID3D11Buffer*>(resource.getResource()) };
            uint32 strides[1]{ resource._elementStride };
            uint32 offsets[1]{ 0 };
            _deviceContext->IASetVertexBuffers(slot, 1, buffers, strides, offsets);
        }
        else if (resource._type == ResourceType::IndexBuffer)
        {
            _is_IndexBuffer_bound = true;

            _deviceContext->IASetIndexBuffer(static_cast<ID3D11Buffer*>(resource.getResource()), Resource::kIndexBufferFormat, 0);
        }
        else
        {
            MINT_LOG_ERROR("!!!");
        }
    }

    void Renderer::bindShaderResource(const ShaderType shaderType, Resource& resource, const uint32 slot)
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
                MINT_LOG_ERROR("This shader type is not supported yet!");
            }
        }
        else if (resource._type == ResourceType::Teture2D)
        {
            ID3D11ShaderResourceView* views[1]{ static_cast<ID3D11ShaderResourceView*>(resource.getView()) };
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

    void Renderer::useTrianglePrimitive()
    {
        _deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    bool Renderer::beginRendering()
    {
        if (_is_InputLayout_bound == false)
        {
            MINT_LOG_ERROR("You must bind ShaderInputLayout first!");
            return false;
        }
        if (_is_VS_bound == false || _is_PS_bound == false)
        {
            MINT_LOG_ERROR("You must at least bind VertexShader and PixelShader first!");
            return false;
        }

        _deviceContext->ClearRenderTargetView(_backBufferRtv.Get(), _clearColor.f);
        _deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        useTrianglePrimitive();
        return true;
    }

    void Renderer::drawIndexed(const uint32 indexCount)
    {
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

    void Renderer::drawText(const std::string& text)
    {
        // TODO
        __noop;
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

    void Renderer::endRendering()
    {
        bindDefaultFontData();

        drawIndexed((uint32)_defaultFontIndices.size());

        _swapChain->Present(0, 0);
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
            samplerDescriptor.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
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

        createDevice_createDefaultFontData();
    }

    void Renderer::createDevice_createDefaultFontData()
    {
        Renderer& renderer = *this;

        _defaultFontShaderHeader._headerName = "DefaultFontShaderHeader";
        _defaultFontShaderHeader._headerCode = kDefaultFontShaderHeaderCode;

        _defaultFontVertexShader.create(renderer, kDefaultFontVertexShaderCode, ShaderType::VertexShader, "DefaultFontVertexShader", "main", "vs_5_0", &_defaultFontShaderHeader);

        _defaultFontShaderInputLayout.pushInputElement(ShaderInputLayout::createInputElementFloat4("POSITION", 0));
        _defaultFontShaderInputLayout.pushInputElement(ShaderInputLayout::createInputElementFloat4("COLOR", 0));
        _defaultFontShaderInputLayout.pushInputElement(ShaderInputLayout::createInputElementFloat2("TEXCOORD", 0));
        _defaultFontShaderInputLayout.create(renderer, _defaultFontVertexShader);

        _defaultFontPixelShader.create(renderer, kDefaultFontPixelShaderCode, ShaderType::PixelShader, "DefaultFontPixelShader", "main", "ps_5_0", &_defaultFontShaderHeader);

        DEFAULT_FONT_CB_MATRICES default_font_cb_matrices;
        default_font_cb_matrices._projectionMatrix.makePixelCoordinatesProjectionMatrix(_windowSize);
        _defaultFontCBMatrices.createBuffer(renderer, ResourceType::ConstantBuffer, &default_font_cb_matrices, sizeof(default_font_cb_matrices), 1);

        byte bytes[kFontTextureByteCount]{};
        for (uint32 iter = 0; iter < kFontTextureByteCount; iter++)
        {
            const uint32 bitAt = iter % 8;
            const uint32 byteAt = iter / 8;
            const byte byte_ = (kFontTextureRawBitData[byteAt] >> (7 - bitAt)) & 1;
            bytes[iter] = byte_ * 255;
        }
        _defaultFontTexture.createTexture2D(renderer, TextureFormat::R8_UNORM, bytes, 128, 60);

        MeshGenerator<DEFAULT_FONT_VS_INPUT>::push_2D_rectangle(float2(256, 240), float2(512, 480), 0.0f, _defaultFontVertices, _defaultFontIndices);
        _defaultFontVertices[0]._texcoord = float2(0, 0);
        _defaultFontVertices[1]._texcoord = float2(1, 0);
        _defaultFontVertices[2]._texcoord = float2(0, 1);
        _defaultFontVertices[3]._texcoord = float2(1, 1);
        _defaultFontVertexBuffer.createBuffer(renderer, ResourceType::VertexBuffer, &_defaultFontVertices[0], sizeof(DEFAULT_FONT_VS_INPUT), (uint32)_defaultFontVertices.size());
        _defaultFontIndexBuffer.createBuffer(renderer, ResourceType::IndexBuffer, &_defaultFontIndices[0], sizeof(uint32), (uint32)_defaultFontIndices.size());
    }

    void Renderer::bindDefaultFontData()
    {
        bindShader(_defaultFontVertexShader);
        bindShader(_defaultFontPixelShader);
        bindShaderInputLayout(_defaultFontShaderInputLayout);
        bindShaderResource(ShaderType::VertexShader, _defaultFontCBMatrices, 0);
        bindShaderResource(ShaderType::PixelShader, _defaultFontTexture, 0);

        bindInput(_defaultFontVertexBuffer, 0);
        bindInput(_defaultFontIndexBuffer, 0);
    }
#pragma endregion
}
