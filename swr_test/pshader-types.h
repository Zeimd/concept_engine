#pragma once

#ifndef CENG_SWR_SHADER_DATATYPE_H
#define CENG_SWR_SHADER_DATATYPE_H

#include <vector>

#include <ceng/datatypes/boolean.h>

#include "pshader-input.h"

#include "texture-unit.h"

#include "cr-shader-view.h"

namespace Ceng::Pshader
{
	void PS_MOV_FLOAT4_FLOAT4_X86_SSE(void* dest, void* source);

	void PS_MUL_FLOAT4_LOCAL_FLOAT_X86_SSE(void* dest, void* source);

	void PS_MUL_FLOAT4_FLOAT4_X86_SSE(void* dest, void* source);
	void PS_MUL_FLOAT4_FLOAT_X86_SSE(void* dest, void* source);

	struct alignas(16) SOAVecBool
	{
		union
		{
			Ceng::BOOL arr[4];
			struct
			{
				Ceng::BOOL a;
				Ceng::BOOL b;
				Ceng::BOOL c;
				Ceng::BOOL d;
			};
		};

		inline SOAVecBool()
		{

		}

		inline SOAVecBool(const SOAVecBool& source)
			: a(source.a), b(source.b), c(source.c), d(source.d)
		{

		}

		inline SOAVecBool(const bool x)
			: a(x), b(x), c(x), d(x)
		{

		}

		inline SOAVecBool(const Ceng::BOOL x)
			: a(x), b(x), c(x), d(x)
		{

		}

		inline SOAVecBool(Ceng::BOOL a, Ceng::BOOL b, Ceng::BOOL c, Ceng::BOOL d)
			: a(a), b(b), c(c), d(d)
		{

		}

		inline SOAVecBool(bool a, bool b, bool c, bool d)
			: a(a), b(b), c(c), d(d)
		{

		}

		inline SOAVecBool& operator = (const bool x)
		{
			a = x;
			b = x;
			c = x;
			d = x;

			return *this;
		}

		inline SOAVecBool& operator = (const Ceng::BOOL x)
		{
			a = x;
			b = x;
			c = x;
			d = x;

			return *this;
		}

		inline SOAVecBool operator == (const bool value)
		{
			return { a == value, b == value, c == value, d == value };
		}

		inline SOAVecBool operator != (const bool value)
		{
			return { a != value, b != value, c != value, d != value };
		}

		inline SOAVecBool operator == (const Ceng::BOOL value)
		{
			return { a == value, b == value, c == value, d == value };
		}

		inline SOAVecBool operator != (const Ceng::BOOL value)
		{
			return { a != value, b != value, c != value, d != value };
		}
	};

	inline SOAVecBool operator == (const SOAVecBool& left, const SOAVecBool& right)
	{
		return { left.a == right.a, left.b == right.b , left.c == right.c , left.d == right.d };
	}

	inline SOAVecBool operator != (const SOAVecBool& left, const SOAVecBool& right)
	{
		return { left.a != right.a, left.b != right.b , left.c != right.c , left.d != right.d };
	}

	inline SOAVecBool LogicalAnd(const SOAVecBool& left, const SOAVecBool& right)
	{
		return { (left.a & right.a), (left.b & right.b) , (left.c & right.c) , (left.d & right.d) };
	}

	inline SOAVecBool LogicalOr(const SOAVecBool& left, const SOAVecBool& right)
	{
		return { (left.a | right.a), (left.b | right.b) , (left.c | right.c) , (left.d | right.d) };
	}

	inline SOAVecBool LogicalNot(const SOAVecBool& x)
	{
		return { ~x.a, ~x.b, ~x.c, ~x.d };
	}

	class alignas(16) SOAVecFloat
	{
	public:
		union
		{
			Ceng::FLOAT32 arr[4];
			struct
			{
				Ceng::FLOAT32 a;
				Ceng::FLOAT32 b;
				Ceng::FLOAT32 c;
				Ceng::FLOAT32 d;
			};
		};

	public:

		inline SOAVecFloat()
		{

		}

		inline SOAVecFloat(Ceng::FLOAT32 a, Ceng::FLOAT32 b, Ceng::FLOAT32 c, Ceng::FLOAT32 d)
			: a(a), b(b), c(c), d(d)
		{

		}

		inline SOAVecFloat(Ceng::FLOAT32 a)
			: a(a), b(a), c(a), d(a)
		{

		}

		inline SOAVecFloat& operator += (const SOAVecFloat& other)
		{
			a += other.a;
			b += other.b;
			c += other.c;
			d += other.d;

			return *this;
		}

		inline SOAVecFloat& operator -= (const SOAVecFloat& other)
		{
			a -= other.a;
			b -= other.b;
			c -= other.c;
			d -= other.d;

			return *this;
		}

		inline SOAVecFloat& operator *= (const SOAVecFloat& other)
		{
			a *= other.a;
			b *= other.b;
			c *= other.c;
			d *= other.d;

			return *this;
		}

		inline SOAVecFloat& operator /= (const SOAVecFloat& other)
		{
			a /= other.a;
			b /= other.b;
			c /= other.c;
			d /= other.d;

			return *this;
		}

	};

	inline SOAVecFloat operator + (const SOAVecFloat& x, const SOAVecFloat& y)
	{
		return { x.a + y.a, x.b + y.b, x.c + y.c, x.d + y.d };
	}

	inline SOAVecFloat operator - (const SOAVecFloat& x, const SOAVecFloat& y)
	{
		return { x.a - y.a, x.b - y.b, x.c - y.c, x.d - y.d };
	}

	inline SOAVecFloat operator * (const SOAVecFloat& x, const SOAVecFloat& y)
	{
		return { x.a * y.a, x.b * y.b, x.c * y.c, x.d * y.d };
	}

	inline SOAVecFloat operator / (const SOAVecFloat& x, const SOAVecFloat& y)
	{
		return { x.a / y.a, x.b / y.b, x.c / y.c, x.d / y.d };
	}

	inline SOAVecBool operator == (const SOAVecFloat& x, const SOAVecFloat& y)
	{
		return { x.a == y.a, x.b == y.b, x.c == y.c, x.d == y.d };
	}

	inline SOAVecBool operator != (const SOAVecFloat& x, const SOAVecFloat& y)
	{
		return { x.a != y.a, x.b != y.b, x.c != y.c, x.d != y.d };
	}

	inline SOAVecBool operator < (const SOAVecFloat& x, const SOAVecFloat& y)
	{
		return { x.a < y.a, x.b < y.b, x.c < y.c, x.d < y.d };
	}

	inline SOAVecBool operator <= (const SOAVecFloat& x, const SOAVecFloat& y)
	{
		return { x.a <= y.a, x.b <= y.b, x.c <= y.c, x.d <= y.d };
	}

	inline SOAVecBool operator > (const SOAVecFloat& x, const SOAVecFloat& y)
	{
		return { x.a > y.a, x.b > y.b, x.c > y.c, x.d > y.d };
	}

	inline SOAVecBool operator >= (const SOAVecFloat& x, const SOAVecFloat& y)
	{
		return { x.a >= y.a, x.b >= y.b, x.c >= y.c, x.d >= y.d };
	}

	class alignas(16) Bool
	{
	public:
		SOAVecBool x;

		inline Bool()
		{

		}

		inline Bool(const SOAVecBool& source)
			: x(source)
		{

		}

		inline Bool(Ceng::BOOL a, Ceng::BOOL b, Ceng::BOOL c, Ceng::BOOL d)
			: x(a, b, c, d)
		{

		}

		inline Bool(bool a, bool b, bool c, bool d)
			: x(a, b, c, d)
		{

		}

		inline Bool(bool a)
			: x(a)
		{

		}

		inline Bool& operator = (const bool a)
		{
			x = a;
			return *this;
		}

		inline Bool& operator = (const Ceng::BOOL a)
		{
			x = a;
			return *this;
		}


	};

	inline Bool operator == (const Bool& left, const Bool& right)
	{
		return left.x == right.x;
	}

	inline Bool operator != (const Bool& left, const Bool& right)
	{
		return left.x != right.x;
	}

	class alignas(16) Bool2
	{
	public:
		SOAVecBool x;
		SOAVecBool y;

		inline Bool2()
		{

		}

		inline Bool2(const SOAVecBool& x, const SOAVecBool& y)
			: x(x), y(y)
		{

		}

		inline Bool2(bool a, bool b)
			: x(a), y(b)
		{

		}
	};

	inline Bool2 operator == (const Bool2& left, const Bool2& right)
	{
		return { left.x == right.x, left.y == right.y };
	}

	inline Bool2 operator != (const Bool2& left, const Bool2& right)
	{
		return { left.x != right.x, left.y != right.y };
	}

	class alignas(16) Bool3
	{
	public:
		SOAVecBool x;
		SOAVecBool y;
		SOAVecBool z;

		inline Bool3()
		{

		}

		inline Bool3(const SOAVecBool& x, const SOAVecBool& y, const SOAVecBool& z)
			: x(x), y(y), z(z)
		{

		}

		inline Bool3(bool a, bool b, bool c)
			: x(a), y(b), z(c)
		{

		}
	};

	class alignas(16) Bool4
	{
	public:
		SOAVecBool x;
		SOAVecBool y;
		SOAVecBool z;
		SOAVecBool w;

		inline Bool4()
		{

		}

		inline Bool4(const SOAVecBool& x, const SOAVecBool& y, const SOAVecBool& z, const SOAVecBool& w)
			: x(x), y(y), z(z), w(w)
		{

		}

		inline Bool4(bool a, bool b, bool c, bool d)
			: x(a), y(b), z(c), w(d)
		{

		}
	};

	inline Bool4 operator == (const Bool4& left, const Bool4& right)
	{
		return { left.x == right.x, left.y == right.y, left.z == right.z, left.w == right.w };
	}

	inline Bool4 operator != (const Bool4& left, const Bool4& right)
	{
		return { left.x != right.x, left.y != right.y, left.z != right.z, left.w != right.w };
	}


	class Float2;
	class Float3;
	class Float4;

	class SwizzledFloat;
	class SwizzledFloat2;
	class SwizzledFloat3;
	class SwizzledFloat4;

	class ConstSwizzledFloat;
	class ConstSwizzledFloat2;
	class ConstSwizzledFloat3;
	class ConstSwizzledFloat4;

	class SampleTexture2D;

	class alignas(16) Float
	{
	public:
		SOAVecFloat x;

	public:
		inline Float()
		{

		}

		inline Float(const SOAVecFloat& source)
			: x(source)
		{

		}

		inline Float(Ceng::FLOAT32 a)
			: x(a)
		{

		}

		inline Float& operator= (const Float& other)
		{
			x = other.x;
			return *this;
		}

		Float& operator= (const Float2& other);

		Float& operator= (const Float3& other);

		Float& operator= (const Float4& other);

		Float& operator= (const SwizzledFloat& other);

		inline Float& operator = (const CR_PixelShaderInput& source)
		{
			source.MoveToFloat(&x);
			return *this;
		}

		inline Float& operator += (const Float& other)
		{
			x += other.x;

			return *this;
		}

		inline Float& operator -= (const Float& other)
		{
			x -= other.x;

			return *this;
		}

		inline Float& operator *= (const Float& other)
		{
			x *= other.x;

			return *this;
		}

		inline Float& operator /= (const Float& other)
		{
			x /= other.x;

			return *this;
		}


	};

	inline Float operator + (const Float& a, const Float& b)
	{
		return a.x + b.x;
	}

	inline Float operator - (const Float& a, const Float& b)
	{
		return a.x - b.x;
	}

	inline Float operator * (const Float& a, const Float& b)
	{
		return a.x * b.x;
	}

	inline Float operator / (const Float& a, const Float& b)
	{
		return a.x / b.x;
	}

	inline Bool operator == (const Float& a, const Float& b)
	{
		return a.x == b.x;
	}

	inline Bool operator != (const Float& a, const Float& b)
	{
		return a.x != b.x;
	}

	inline Bool operator < (const Float& a, const Float& b)
	{
		return a.x < b.x;
	}

	inline Bool operator > (const Float& a, const Float& b)
	{
		return a.x > b.x;
	}

	inline Bool operator <= (const Float& a, const Float& b)
	{
		return a.x <= b.x;
	}

	inline Bool operator >= (const Float& a, const Float& b)
	{
		return a.x >= b.x;
	}

	class alignas(16) Float2
	{
	public:
		SOAVecFloat _x;
		SOAVecFloat _y;

	public:
		inline Float2()
		{

		}

		inline Float2(const SOAVecFloat& a, const SOAVecFloat& b)
			: _x(a), _y(b)
		{

		}

		inline Float2(Ceng::FLOAT32 a, Ceng::FLOAT32 b)
			: _x(a), _y(b)
		{

		}

		inline Float2& operator= (const Float& other)
		{
			_x = other.x;
			_y = 0.0f;

			return *this;
		}

		inline Float2& operator= (const Float2& other)
		{
			_x = other._x;
			_y = other._y;

			return *this;
		}

		Float2& operator= (const Float3& other);

		Float2& operator= (const Float4& other);

		Float2& operator= (const SwizzledFloat2& other);

		Float2& operator= (const ConstSwizzledFloat2& other);

		inline Float2& operator = (const CR_PixelShaderInput& source)
		{
			source.MoveToFloat2(&_x);
			return *this;
		}

		inline Float2& operator += (const Float2& other)
		{
			_x += other._x;
			_y += other._y;

			return *this;
		}

		inline Float2& operator -= (const Float2& other)
		{
			_x -= other._x;
			_y -= other._y;

			return *this;
		}

		inline Float2& operator *= (const Float2& other)
		{
			_x *= other._x;
			_y *= other._y;

			return *this;
		}

		inline Float2& operator /= (const Float2& other)
		{
			_x /= other._x;
			_y /= other._y;

			return *this;
		}

		SwizzledFloat x();
		SwizzledFloat y();

		ConstSwizzledFloat2 xx();
		SwizzledFloat2 xy();
		SwizzledFloat2 yx();
		ConstSwizzledFloat2 yy();
	};

	inline Float2 operator + (const Float2& a, const Float2& b)
	{
		return { a._x + b._x, a._y + b._y };
	}

	inline Float2 operator + (const Float2& a, const Float& b)
	{
		return { a._x + b.x, a._y + b.x };
	}

	inline Float2 operator - (const Float2& a, const Float2& b)
	{
		return { a._x - b._x, a._y - b._y };
	}

	inline Float2 operator - (const Float2& a, const Float& b)
	{
		return { a._x - b.x, a._y - b.x };
	}

	inline Float2 operator * (const Float2& a, const Float2& b)
	{
		return  { a._x * b._x, a._y * b._y };
	}

	inline Float2 operator * (const Float2& a, const Float& b)
	{
		return  { a._x * b.x, a._y * b.x };
	}

	inline Float2 operator / (const Float2& a, const Float& b)
	{
		return  { a._x / b.x, a._y / b.x };
	}

	inline Float2 operator / (const Float2& a, const Float2& b)
	{
		return  { a._x / b._x, a._y / b._y };
	}

	inline Bool operator == (const Float2& a, const Float2& b)
	{
		return LogicalAnd(a._x == b._x, a._y == b._y);
	}

	inline Bool operator != (const Float2& a, const Float2& b)
	{
		return LogicalOr(a._x != b._x, a._y != b._y);
	}

	class alignas(16) Float3
	{
	public:
		SOAVecFloat _x;
		SOAVecFloat _y;
		SOAVecFloat _z;

	public:

		inline Float3()
		{

		}

		inline Float3(const SOAVecFloat& x, const SOAVecFloat& y, const SOAVecFloat& z)
			: _x(x), _y(y), _z(z)
		{

		}

		inline Float3(Ceng::FLOAT32 x, Ceng::FLOAT32 y, Ceng::FLOAT32 z)
			: _x(x), _y(y), _z(z)
		{

		}

		inline Float3& operator= (const Float& other)
		{
			_x = other.x;
			_y = 0.0f;
			_z = 0.0f;

			return *this;
		}

		inline Float3& operator= (const Float2& other)
		{
			_x = other._x;
			_y = other._y;
			_z = 0.0f;

			return *this;
		}

		inline Float3& operator= (const Float3& other)
		{
			_x = other._x;
			_y = other._y;
			_z = other._z;

			return *this;
		}

		Float3& operator= (const Float4& other);

		inline Float3& operator = (const CR_PixelShaderInput& source)
		{
			source.MoveToFloat3(&_x);
			return *this;
		}

		inline Float3& operator += (const Float3& other)
		{
			_x += other._x;
			_y += other._y;
			_z += other._z;

			return *this;
		}

		inline Float3& operator -= (const Float3& other)
		{
			_x -= other._x;
			_y -= other._y;
			_z -= other._z;

			return *this;
		}

		inline Float3& operator *= (const Float3& other)
		{
			_x *= other._x;
			_y *= other._y;
			_z *= other._z;

			return *this;
		}

		inline Float3& operator /= (const Float3& other)
		{
			_x /= other._x;
			_y /= other._y;
			_z /= other._z;

			return *this;
		}

		Float3& operator= (const SwizzledFloat3& other);

		Float3& operator= (const ConstSwizzledFloat3& other);

		SwizzledFloat x();
		SwizzledFloat y();
		SwizzledFloat z();

		ConstSwizzledFloat2 xx();
		SwizzledFloat2 xy();
		SwizzledFloat2 xz();

		SwizzledFloat2 yx();
		ConstSwizzledFloat2 yy();
		SwizzledFloat2 yz();

		SwizzledFloat2 zx();
		SwizzledFloat2 zy();
		ConstSwizzledFloat2 zz();

		ConstSwizzledFloat3 xxx();
		ConstSwizzledFloat3 xxy();
		ConstSwizzledFloat3 xxz();

		ConstSwizzledFloat3 xyx();
		ConstSwizzledFloat3 xyy();
		SwizzledFloat3 xyz();

		ConstSwizzledFloat3 xzx();
		SwizzledFloat3 xzy();
		ConstSwizzledFloat3 xzz();

		ConstSwizzledFloat3 yxx();
		ConstSwizzledFloat3 yxy();
		SwizzledFloat3 yxz();

		ConstSwizzledFloat3 yyx();
		ConstSwizzledFloat3 yyy();
		ConstSwizzledFloat3 yyz();

		SwizzledFloat3 yzx();
		ConstSwizzledFloat3 yzy();
		ConstSwizzledFloat3 yzz();

		ConstSwizzledFloat3 zxx();
		SwizzledFloat3 zxy();
		ConstSwizzledFloat3 zxz();

		SwizzledFloat3 zyx();
		ConstSwizzledFloat3 zyy();
		ConstSwizzledFloat3 zyz();

		ConstSwizzledFloat3 zzx();
		ConstSwizzledFloat3 zzy();
		ConstSwizzledFloat3 zzz();

	};

	inline Float3 operator + (const Float3& a, const Float3& b)
	{
		return { a._x + b._x, a._y + b._y, a._z + b._z };
	}

	inline Float3 operator + (const Float3& a, const Float& b)
	{
		return { a._x + b.x, a._y + b.x, a._z + b.x };
	}

	inline Float3 operator - (const Float3& a, const Float3& b)
	{
		return { a._x - b._x, a._y - b._y, a._z - b._z };
	}

	inline Float3 operator - (const Float3& a, const Float& b)
	{
		return { a._x - b.x, a._y - b.x, a._z - b.x };
	}

	inline Float3 operator * (const Float3& a, const Float3& b)
	{
		return  { a._x * b._x, a._y * b._y, a._z * b._z };
	}

	inline Float3 operator * (const Float3& a, const Float& b)
	{
		return  { a._x * b.x, a._y * b.x, a._z * b.x };
	}

	inline Float3 operator / (const Float3& a, const Float3& b)
	{
		return  { a._x / b._x, a._y / b._y, a._z / b._z };
	}

	inline Float3 operator / (const Float3& a, const Float& b)
	{
		return  { a._x / b.x, a._y / b.x, a._z / b.x };
	}

	inline Bool operator == (const Float3& a, const Float3& b)
	{
		return LogicalAnd(LogicalAnd(a._x == b._x, a._y == b._y), a._z == b._z);
	}

	inline Bool operator != (const Float3& a, const Float3& b)
	{
		return LogicalOr(LogicalOr(a._x != b._x, a._y != b._y), a._z != b._z);
	}

	class alignas(16) Float4
	{
	public:
		SOAVecFloat _x;
		SOAVecFloat _y;
		SOAVecFloat _z;
		SOAVecFloat _w;

	public:
		inline Float4()
		{

		}

		inline Float4(const SOAVecFloat& x, const SOAVecFloat& y, const SOAVecFloat& z, const SOAVecFloat& w)
			: _x(x), _y(y), _z(z), _w(w)
		{

		}

		inline Float4(Ceng::FLOAT32 x, Ceng::FLOAT32 y, Ceng::FLOAT32 z, Ceng::FLOAT32 w)
			: _x(x), _y(y), _z(z), _w(w)
		{

		}

		inline Float4& operator= (const Float& other)
		{
			_x = other.x;
			_y = 0.0f;
			_z = 0.0f;
			_w = 1.0f;

			return *this;
		}

		inline Float4& operator= (const Float2& other)
		{
			_x = other._x;
			_y = other._y;
			_z = 0.0f;
			_w = 1.0f;

			return *this;
		}

		inline Float4& operator= (const Float3& other)
		{
			_x = other._x;
			_y = other._y;
			_z = other._z;
			_w = 1.0f;

			return *this;
		}

		inline Float4& operator= (const Float4& other)
		{
			_x = other._x;
			_y = other._y;
			_z = other._z;
			_w = other._w;

			return *this;
		}


		inline Float4& operator = (const CR_PixelShaderInput& source)
		{
			source.MoveToFloat4(&_x);
			return *this;
		}

		inline Float4& operator += (const Float4& other)
		{
			_x += other._x;
			_y += other._y;
			_z += other._z;
			_w += other._w;

			return *this;
		}

		inline Float4& operator -= (const Float4& other)
		{
			_x -= other._x;
			_y -= other._y;
			_z -= other._z;
			_w -= other._w;

			return *this;
		}

		inline Float4& operator *= (const Float4& other)
		{
			_x *= other._x;
			_y *= other._y;
			_z *= other._z;
			_w *= other._w;

			return *this;
		}

		inline Float4& operator /= (const Float4& other)
		{
			_x /= other._x;
			_y /= other._y;
			_z /= other._z;
			_w /= other._w;

			return *this;
		}

		Float4& operator = (const SampleTexture2D& source);

		Float4& operator = (const SwizzledFloat4& source);

		Float4& operator = (const ConstSwizzledFloat4& source);

		SwizzledFloat x();
		SwizzledFloat y();
		SwizzledFloat z();
		SwizzledFloat w();

		ConstSwizzledFloat2 xx();
		SwizzledFloat2 xy();
		SwizzledFloat2 xz();
		SwizzledFloat2 xw();

		SwizzledFloat2 yx();
		ConstSwizzledFloat2 yy();
		SwizzledFloat2 yz();
		SwizzledFloat2 yw();

		SwizzledFloat2 zx();
		SwizzledFloat2 zy();
		ConstSwizzledFloat2 zz();
		SwizzledFloat2 zw();

		SwizzledFloat2 wx();
		SwizzledFloat2 wy();
		SwizzledFloat2 wz();
		ConstSwizzledFloat2 ww();

		ConstSwizzledFloat3 xxx();
		ConstSwizzledFloat3 xxy();
		ConstSwizzledFloat3 xxz();
		ConstSwizzledFloat3 xxw();

		ConstSwizzledFloat3 xyx();
		ConstSwizzledFloat3 xyy();
		SwizzledFloat3 xyz();
		SwizzledFloat3 xyw();

		ConstSwizzledFloat3 xzx();
		SwizzledFloat3 xzy();
		ConstSwizzledFloat3 xzz();
		SwizzledFloat3 xzw();

		ConstSwizzledFloat3 xwx();
		SwizzledFloat3 xwy();
		SwizzledFloat3 xwz();
		ConstSwizzledFloat3 xww();

		ConstSwizzledFloat3 yxx();
		ConstSwizzledFloat3 yxy();
		SwizzledFloat3 yxz();
		SwizzledFloat3 yxw();

		ConstSwizzledFloat3 yyx();
		ConstSwizzledFloat3 yyy();
		ConstSwizzledFloat3 yyz();
		ConstSwizzledFloat3 yyw();

		SwizzledFloat3 yzx();
		ConstSwizzledFloat3 yzy();
		ConstSwizzledFloat3 yzz();
		SwizzledFloat3 yzw();

		SwizzledFloat3 ywx();
		ConstSwizzledFloat3 ywy();
		SwizzledFloat3 ywz();
		ConstSwizzledFloat3 yww();

		ConstSwizzledFloat3 zxx();
		SwizzledFloat3 zxy();
		ConstSwizzledFloat3 zxz();
		SwizzledFloat3 zxw();

		SwizzledFloat3 zyx();
		ConstSwizzledFloat3 zyy();
		ConstSwizzledFloat3 zyz();
		SwizzledFloat3 zyw();

		ConstSwizzledFloat3 zzx();
		ConstSwizzledFloat3 zzy();
		ConstSwizzledFloat3 zzz();
		ConstSwizzledFloat3 zzw();

		SwizzledFloat3 zwx();
		SwizzledFloat3 zwy();
		ConstSwizzledFloat3 zwz();
		ConstSwizzledFloat3 zww();

		ConstSwizzledFloat3 wxx();
		SwizzledFloat3 wxy();
		SwizzledFloat3 wxz();
		ConstSwizzledFloat3 wxw();

		SwizzledFloat3 wyx();
		ConstSwizzledFloat3 wyy();
		SwizzledFloat3 wyz();
		ConstSwizzledFloat3 wyw();

		SwizzledFloat3 wzx();
		SwizzledFloat3 wzy();
		ConstSwizzledFloat3 wzz();
		ConstSwizzledFloat3 wzw();

		ConstSwizzledFloat3 wwx();
		ConstSwizzledFloat3 wwy();
		ConstSwizzledFloat3 wwz();
		ConstSwizzledFloat3 www();

		// swizzles x---

		ConstSwizzledFloat4 xxxx();
		ConstSwizzledFloat4 xxxy();
		ConstSwizzledFloat4 xxxz();
		ConstSwizzledFloat4 xxxw();

		ConstSwizzledFloat4 xxyx();
		ConstSwizzledFloat4 xxyy();
		ConstSwizzledFloat4 xxyz();
		ConstSwizzledFloat4 xxyw();

		ConstSwizzledFloat4 xxzx();
		ConstSwizzledFloat4 xxzy();
		ConstSwizzledFloat4 xxzz();
		ConstSwizzledFloat4 xxzw();

		ConstSwizzledFloat4 xxwx();
		ConstSwizzledFloat4 xxwy();
		ConstSwizzledFloat4 xxwz();
		ConstSwizzledFloat4 xxww();

		ConstSwizzledFloat4 xyxx();
		ConstSwizzledFloat4 xyxy();
		ConstSwizzledFloat4 xyxz();
		ConstSwizzledFloat4 xyxw();

		ConstSwizzledFloat4 xyyx();
		ConstSwizzledFloat4 xyyy();
		ConstSwizzledFloat4 xyyz();
		ConstSwizzledFloat4 xyyw();

		ConstSwizzledFloat4 xyzx();
		ConstSwizzledFloat4 xyzy();
		ConstSwizzledFloat4 xyzz();
		SwizzledFloat4 xyzw();

		ConstSwizzledFloat4 xywx();
		ConstSwizzledFloat4 xywy();
		SwizzledFloat4 xywz();
		ConstSwizzledFloat4 xyww();

		ConstSwizzledFloat4 xzxx();
		ConstSwizzledFloat4 xzxy();
		ConstSwizzledFloat4 xzxz();
		ConstSwizzledFloat4 xzxw();

		ConstSwizzledFloat4 xzyx();
		ConstSwizzledFloat4 xzyy();
		ConstSwizzledFloat4 xzyz();
		SwizzledFloat4 xzyw();

		ConstSwizzledFloat4 xzzx();
		ConstSwizzledFloat4 xzzy();
		ConstSwizzledFloat4 xzzz();
		ConstSwizzledFloat4 xzzw();

		ConstSwizzledFloat4 xzwx();
		SwizzledFloat4 xzwy();
		ConstSwizzledFloat4 xzwz();
		ConstSwizzledFloat4 xzww();

		ConstSwizzledFloat4 xwxx();
		ConstSwizzledFloat4 xwxy();
		ConstSwizzledFloat4 xwxz();
		ConstSwizzledFloat4 xwxw();

		ConstSwizzledFloat4 xwyx();
		ConstSwizzledFloat4 xwyy();
		SwizzledFloat4 xwyz();
		ConstSwizzledFloat4 xwyw();

		ConstSwizzledFloat4 xwzx();
		SwizzledFloat4 xwzy();
		ConstSwizzledFloat4 xwzz();
		ConstSwizzledFloat4 xwzw();

		ConstSwizzledFloat4 xwwx();
		ConstSwizzledFloat4 xwwy();
		ConstSwizzledFloat4 xwwz();
		ConstSwizzledFloat4 xwww();

		// swizzles y---

		ConstSwizzledFloat4 yxxx();
		ConstSwizzledFloat4 yxxy();
		ConstSwizzledFloat4 yxxz();
		ConstSwizzledFloat4 yxxw();

		ConstSwizzledFloat4 yxyx();
		ConstSwizzledFloat4 yxyy();
		ConstSwizzledFloat4 yxyz();
		ConstSwizzledFloat4 yxyw();

		ConstSwizzledFloat4 yxzx();
		ConstSwizzledFloat4 yxzy();
		ConstSwizzledFloat4 yxzz();
		SwizzledFloat4 yxzw();

		ConstSwizzledFloat4 yxwx();
		ConstSwizzledFloat4 yxwy();
		SwizzledFloat4 yxwz();
		ConstSwizzledFloat4 yxww();

		ConstSwizzledFloat4 yyxx();
		ConstSwizzledFloat4 yyxy();
		ConstSwizzledFloat4 yyxz();
		ConstSwizzledFloat4 yyxw();

		ConstSwizzledFloat4 yyyx();
		ConstSwizzledFloat4 yyyy();
		ConstSwizzledFloat4 yyyz();
		ConstSwizzledFloat4 yyyw();

		ConstSwizzledFloat4 yyzx();
		ConstSwizzledFloat4 yyzy();
		ConstSwizzledFloat4 yyzz();
		ConstSwizzledFloat4 yyzw();

		ConstSwizzledFloat4 yywx();
		ConstSwizzledFloat4 yywy();
		ConstSwizzledFloat4 yywz();
		ConstSwizzledFloat4 yyww();

		ConstSwizzledFloat4 yzxx();
		ConstSwizzledFloat4 yzxy();
		ConstSwizzledFloat4 yzxz();
		SwizzledFloat4 yzxw();

		ConstSwizzledFloat4 yzyx();
		ConstSwizzledFloat4 yzyy();
		ConstSwizzledFloat4 yzyz();
		ConstSwizzledFloat4 yzyw();

		ConstSwizzledFloat4 yzzx();
		ConstSwizzledFloat4 yzzy();
		ConstSwizzledFloat4 yzzz();
		ConstSwizzledFloat4 yzzw();

		SwizzledFloat4 yzwx();
		ConstSwizzledFloat4 yzwy();
		ConstSwizzledFloat4 yzwz();
		ConstSwizzledFloat4 yzww();

		ConstSwizzledFloat4 ywxx();
		ConstSwizzledFloat4 ywxy();
		SwizzledFloat4 ywxz();
		ConstSwizzledFloat4 ywxw();

		ConstSwizzledFloat4 ywyx();
		ConstSwizzledFloat4 ywyy();
		ConstSwizzledFloat4 ywyz();
		ConstSwizzledFloat4 ywyw();

		SwizzledFloat4 ywzx();
		ConstSwizzledFloat4 ywzy();
		ConstSwizzledFloat4 ywzz();
		ConstSwizzledFloat4 ywzw();

		ConstSwizzledFloat4 ywwx();
		ConstSwizzledFloat4 ywwy();
		ConstSwizzledFloat4 ywwz();
		ConstSwizzledFloat4 ywww();

		// swizzles z---

		ConstSwizzledFloat4 zxxx();
		ConstSwizzledFloat4 zxxy();
		ConstSwizzledFloat4 zxxz();
		ConstSwizzledFloat4 zxxw();

		ConstSwizzledFloat4 zxyx();
		ConstSwizzledFloat4 zxyy();
		ConstSwizzledFloat4 zxyz();
		SwizzledFloat4 zxyw();

		ConstSwizzledFloat4 zxzx();
		ConstSwizzledFloat4 zxzy();
		ConstSwizzledFloat4 zxzz();
		ConstSwizzledFloat4 zxzw();

		ConstSwizzledFloat4 zxwx();
		SwizzledFloat4 zxwy();
		ConstSwizzledFloat4 zxwz();
		ConstSwizzledFloat4 zxww();

		ConstSwizzledFloat4 zyxx();
		ConstSwizzledFloat4 zyxy();
		ConstSwizzledFloat4 zyxz();
		SwizzledFloat4 zyxw();

		ConstSwizzledFloat4 zyyx();
		ConstSwizzledFloat4 zyyy();
		ConstSwizzledFloat4 zyyz();
		ConstSwizzledFloat4 zyyw();

		ConstSwizzledFloat4 zyzx();
		ConstSwizzledFloat4 zyzy();
		ConstSwizzledFloat4 zyzz();
		ConstSwizzledFloat4 zyzw();

		SwizzledFloat4 zywx();
		ConstSwizzledFloat4 zywy();
		ConstSwizzledFloat4 zywz();
		ConstSwizzledFloat4 zyww();

		ConstSwizzledFloat4 zzxx();
		ConstSwizzledFloat4 zzxy();
		ConstSwizzledFloat4 zzxz();
		ConstSwizzledFloat4 zzxw();

		ConstSwizzledFloat4 zzyx();
		ConstSwizzledFloat4 zzyy();
		ConstSwizzledFloat4 zzyz();
		ConstSwizzledFloat4 zzyw();

		ConstSwizzledFloat4 zzzx();
		ConstSwizzledFloat4 zzzy();
		ConstSwizzledFloat4 zzzz();
		ConstSwizzledFloat4 zzzw();

		ConstSwizzledFloat4 zzwx();
		ConstSwizzledFloat4 zzwy();
		ConstSwizzledFloat4 zzwz();
		ConstSwizzledFloat4 zzww();

		ConstSwizzledFloat4 zwxx();
		SwizzledFloat4 zwxy();
		ConstSwizzledFloat4 zwxz();
		ConstSwizzledFloat4 zwxw();

		SwizzledFloat4 zwyx();
		ConstSwizzledFloat4 zwyy();
		ConstSwizzledFloat4 zwyz();
		ConstSwizzledFloat4 zwyw();

		ConstSwizzledFloat4 zwzx();
		ConstSwizzledFloat4 zwzy();
		ConstSwizzledFloat4 zwzz();
		ConstSwizzledFloat4 zwzw();

		ConstSwizzledFloat4 zwwx();
		ConstSwizzledFloat4 zwwy();
		ConstSwizzledFloat4 zwwz();
		ConstSwizzledFloat4 zwww();

		// swizzles w---

		ConstSwizzledFloat4 wxxx();
		ConstSwizzledFloat4 wxxy();
		ConstSwizzledFloat4 wxxz();
		ConstSwizzledFloat4 wxxw();

		ConstSwizzledFloat4 wxyx();
		ConstSwizzledFloat4 wxyy();
		SwizzledFloat4 wxyz();
		ConstSwizzledFloat4 wxyw();

		ConstSwizzledFloat4 wxzx();
		SwizzledFloat4 wxzy();
		ConstSwizzledFloat4 wxzz();
		ConstSwizzledFloat4 wxzw();

		ConstSwizzledFloat4 wxwx();
		ConstSwizzledFloat4 wxwy();
		ConstSwizzledFloat4 wxwz();
		ConstSwizzledFloat4 wxww();

		ConstSwizzledFloat4 wyxx();
		ConstSwizzledFloat4 wyxy();
		SwizzledFloat4 wyxz();
		ConstSwizzledFloat4 wyxw();

		ConstSwizzledFloat4 wyyx();
		ConstSwizzledFloat4 wyyy();
		ConstSwizzledFloat4 wyyz();
		ConstSwizzledFloat4 wyyw();

		SwizzledFloat4 wyzx();
		ConstSwizzledFloat4 wyzy();
		ConstSwizzledFloat4 wyzz();
		ConstSwizzledFloat4 wyzw();

		ConstSwizzledFloat4 wywx();
		ConstSwizzledFloat4 wywy();
		ConstSwizzledFloat4 wywz();
		ConstSwizzledFloat4 wyww();

		ConstSwizzledFloat4 wzxx();
		SwizzledFloat4 wzxy();
		ConstSwizzledFloat4 wzxz();
		ConstSwizzledFloat4 wzxw();

		SwizzledFloat4 wzyx();
		ConstSwizzledFloat4 wzyy();
		ConstSwizzledFloat4 wzyz();
		ConstSwizzledFloat4 wzyw();

		ConstSwizzledFloat4 wzzx();
		ConstSwizzledFloat4 wzzy();
		ConstSwizzledFloat4 wzzz();
		ConstSwizzledFloat4 wzzw();

		ConstSwizzledFloat4 wzwx();
		ConstSwizzledFloat4 wzwy();
		ConstSwizzledFloat4 wzwz();
		ConstSwizzledFloat4 wzww();

		ConstSwizzledFloat4 wwxx();
		ConstSwizzledFloat4 wwxy();
		ConstSwizzledFloat4 wwxz();
		ConstSwizzledFloat4 wwxw();

		ConstSwizzledFloat4 wwyx();
		ConstSwizzledFloat4 wwyy();
		ConstSwizzledFloat4 wwyz();
		ConstSwizzledFloat4 wwyw();

		ConstSwizzledFloat4 wwzx();
		ConstSwizzledFloat4 wwzy();
		ConstSwizzledFloat4 wwzz();
		ConstSwizzledFloat4 wwzw();

		ConstSwizzledFloat4 wwwx();
		ConstSwizzledFloat4 wwwy();
		ConstSwizzledFloat4 wwwz();
		ConstSwizzledFloat4 wwww();

		/*
		inline Float4& Float4::operator *= (const Float &other)
		{
			(*call_mul_varying_float)((void*)dataAddress,(void*)other.dataAddress);
			return *this;
		}

		inline Float4& Float4::operator *= (const Float4 &other)
		{
			(*call_mul_varying_float4)((void*)dataAddress,(void*)other.dataAddress);
			return *this;
		}

		inline Float4& Float4::operator *= (const FLOAT32 &other)
		{
			(*call_mul_local_float)((void*)dataAddress,(void*)&other);
			return *this;
		}

		inline Float4& Float4::operator /= (const Float4 &other)
		{
			return *this;
		}
		*/

	public:

		static void (*call_mov_float4)(void* dest, void* source);

		static void (*call_mul_varying_float4)(void* dest, void* source);
		static void (*call_mul_local_float)(void* dest, void* source);

		static void (*call_mul_varying_float)(void* dest, void* source);


	};

	inline Float4 operator + (const Float4& a, const Float4& b)
	{
		return { a._x + b._x, a._y + b._y, a._z + b._z, a._w + b._w };
	}

	inline Float4 operator + (const Float4& a, const Float& b)
	{
		return { a._x + b.x, a._y + b.x, a._z + b.x, a._w + b.x };
	}

	inline Float4 operator - (const Float4& a, const Float4& b)
	{
		return { a._x - b._x, a._y - b._y, a._z - b._z, a._w - b._w };
	}

	inline Float4 operator - (const Float4& a, const Float& b)
	{
		return { a._x - b.x, a._y - b.x, a._z - b.x, a._w - b.x };
	}

	inline Float4 operator * (const Float4& a, const Float4& b)
	{
		return  { a._x * b._x, a._y * b._y, a._z * b._z, a._w * b._w };
	}

	inline Float4 operator * (const Float4& a, const Float& b)
	{
		return  { a._x * b.x, a._y * b.x, a._z * b.x, a._w * b.x };
	}

	inline Float4 operator / (const Float4& a, const Float4& b)
	{
		return  { a._x / b._x, a._y / b._y, a._z / b._z, a._w / b._w };
	}

	inline Float4 operator / (const Float4& a, const Float& b)
	{
		return  { a._x / b.x, a._y / b.x, a._z / b.x, a._w / b.x };
	}

	inline Bool operator == (const Float4& a, const Float4& b)
	{
		return LogicalAnd(LogicalAnd(LogicalAnd(a._x == b._x, a._y == b._y), a._z == b._z), a._w == b._w);
	}

	inline Bool operator != (const Float4& a, const Float4& b)
	{
		return LogicalOr(LogicalOr(LogicalOr(a._x != b._x, a._y != b._y), a._z != b._z), a._w != b._w);
	}

	class SampleTexture2D
	{
	public:

		Ceng::TextureUnit* unit;
		Float2* uv;

		void* dataAddress;

		//std::vector<TextureUnit> *textureUnits;

	public:

		SampleTexture2D() : unit(nullptr), uv(nullptr), dataAddress(nullptr)
		{
		}

		//SampleTexture2D& operator()(const Ceng::UINT32 index, Shader::Float2 &uv);
		SampleTexture2D& operator()(TextureUnit& texture, Float2& uv);

		void SampleToFloat4(void* destBuffer) const;
	};

	class SwizzledFloat
	{
	public:
		SOAVecFloat* base;
		Ceng::UINT32 a;

	public:

		inline SwizzledFloat(SOAVecFloat* base, Ceng::UINT32 a)
			: base(base), a(a)
		{

		}

		inline SwizzledFloat& operator = (Ceng::FLOAT32 x)
		{
			base[a] = x;
			return *this;
		}

		inline SwizzledFloat& operator = (const Float& source)
		{
			base[a] = source.x;
			return *this;
		}

		inline SwizzledFloat& operator = (const SwizzledFloat& source)
		{
			base[a] = source.base[source.a];
			return *this;
		}

		inline SwizzledFloat& operator += (const Float& source)
		{
			base[a] += source.x;
			return *this;
		}

		inline SwizzledFloat& operator += (const SwizzledFloat& source)
		{
			base[a] += source.base[source.a];
			return *this;
		}

		inline SwizzledFloat& operator -= (const Float& source)
		{
			base[a] -= source.x;
			return *this;
		}

		inline SwizzledFloat& operator -= (const SwizzledFloat& source)
		{
			base[a] -= source.base[source.a];
			return *this;
		}

		inline SwizzledFloat& operator *= (const Float& source)
		{
			base[a] *= source.x;
			return *this;
		}

		inline SwizzledFloat& operator *= (const SwizzledFloat& source)
		{
			base[a] *= source.base[source.a];
			return *this;
		}

		inline SwizzledFloat& operator /= (const Float& source)
		{
			base[a] /= source.x;
			return *this;
		}

		inline SwizzledFloat& operator /= (const SwizzledFloat& source)
		{
			base[a] /= source.base[source.a];
			return *this;
		}

		inline operator Float() const
		{
			return *base;
		}
	};

	class ConstSwizzledFloat
	{
	public:
		SOAVecFloat* base;
		Ceng::UINT32 a;

	public:

		inline ConstSwizzledFloat(SOAVecFloat* base, Ceng::UINT32 a)
			: base(base), a(a)
		{

		}

		inline operator Float() const
		{
			return *base;
		}
	};

	class SwizzledFloat2
	{
	public:
		SOAVecFloat* base;
		Ceng::UINT32 a;
		Ceng::UINT32 b;

	public:

		inline SwizzledFloat2(SOAVecFloat* base, Ceng::UINT32 a, Ceng::UINT32 b)
			: base(base), a(a), b(b)
		{

		}

		inline SwizzledFloat2& operator = (const Float2& source)
		{
			if (base == &source._x)
			{
				SOAVecFloat temp[2];

				temp[0] = source._x;
				temp[1] = source._y;

				base[a] = temp[0];
				base[b] = temp[1];
			}
			else
			{
				base[a] = source._x;
				base[b] = source._y;
			}

			return *this;
		}

		inline SwizzledFloat2& operator = (const SwizzledFloat2& source)
		{
			if (base == source.base)
			{
				SOAVecFloat temp[2];

				temp[0] = base[0];
				temp[1] = base[1];

				base[a] = temp[source.a];
				base[b] = temp[source.b];
			}
			else
			{
				base[a] = source.base[source.a];
				base[b] = source.base[source.b];
			}

			return *this;
		}

		inline SwizzledFloat2& operator += (const Float2& source)
		{
			if (base == &source._x)
			{
				SOAVecFloat temp[2];

				temp[0] = source._x;
				temp[1] = source._y;

				base[a] += temp[0];
				base[b] += temp[1];
			}
			else
			{
				base[a] += source._x;
				base[b] += source._y;
			}

			return *this;
		}

		inline SwizzledFloat2& operator += (const SwizzledFloat2& source)
		{
			if (base == source.base)
			{
				SOAVecFloat temp[2];

				temp[0] = base[0];
				temp[1] = base[1];

				base[a] += temp[source.a];
				base[b] += temp[source.b];
			}
			else
			{
				base[a] += source.base[source.a];
				base[b] += source.base[source.b];
			}

			return *this;
		}

		inline SwizzledFloat2& operator -= (const Float2& source)
		{
			if (base == &source._x)
			{
				SOAVecFloat temp[2];

				temp[0] = source._x;
				temp[1] = source._y;

				base[a] -= temp[0];
				base[b] -= temp[1];
			}
			else
			{
				base[a] -= source._x;
				base[b] -= source._y;
			}

			return *this;
		}

		inline SwizzledFloat2& operator -= (const SwizzledFloat2& source)
		{
			if (base == source.base)
			{
				SOAVecFloat temp[2];

				temp[0] = base[0];
				temp[1] = base[1];

				base[a] -= temp[source.a];
				base[b] -= temp[source.b];
			}
			else
			{
				base[a] -= source.base[source.a];
				base[b] -= source.base[source.b];
			}

			return *this;
		}

		inline SwizzledFloat2& operator *= (const Float2& source)
		{
			if (base == &source._x)
			{
				SOAVecFloat temp[2];

				temp[0] = source._x;
				temp[1] = source._y;

				base[a] *= temp[0];
				base[b] *= temp[1];
			}
			else
			{
				base[a] *= source._x;
				base[b] *= source._y;
			}

			return *this;
		}

		inline SwizzledFloat2& operator *= (const SwizzledFloat2& source)
		{
			if (base == source.base)
			{
				SOAVecFloat temp[2];

				temp[0] = base[0];
				temp[1] = base[1];

				base[a] *= temp[source.a];
				base[b] *= temp[source.b];
			}
			else
			{
				base[a] *= source.base[source.a];
				base[b] *= source.base[source.b];
			}

			return *this;
		}

		inline SwizzledFloat2& operator /= (const Float2& source)
		{
			if (base == &source._x)
			{
				SOAVecFloat temp[2];

				temp[0] = source._x;
				temp[1] = source._y;

				base[a] /= temp[0];
				base[b] /= temp[1];
			}
			else
			{
				base[a] /= source._x;
				base[b] /= source._y;
			}

			return *this;
		}

		inline SwizzledFloat2& operator /= (const SwizzledFloat2& source)
		{
			if (base == source.base)
			{
				SOAVecFloat temp[2];

				temp[0] = base[0];
				temp[1] = base[1];

				base[a] /= temp[source.a];
				base[b] /= temp[source.b];
			}
			else
			{
				base[a] /= source.base[source.a];
				base[b] /= source.base[source.b];
			}

			return *this;
		}

		inline operator Float() const
		{
			return *base;
		}

		inline operator Float2() const
		{
			return { base[a],base[b] };
		}
	};

	class ConstSwizzledFloat2
	{
	public:
		SOAVecFloat* base;
		Ceng::UINT32 a;
		Ceng::UINT32 b;

	public:

		inline ConstSwizzledFloat2(SOAVecFloat* base, Ceng::UINT32 a, Ceng::UINT32 b)
			: base(base), a(a), b(b)
		{

		}

		inline operator Float() const
		{
			return *base;
		}

		inline operator Float2() const
		{
			return { base[a],base[b] };
		}
	};

	class SwizzledFloat3
	{
	public:
		SOAVecFloat* base;
		Ceng::UINT32 a;
		Ceng::UINT32 b;
		Ceng::UINT32 c;

	public:

		inline SwizzledFloat3(SOAVecFloat* base, Ceng::UINT32 a, Ceng::UINT32 b, Ceng::UINT32 c)
			: base(base), a(a), b(b), c(c)
		{

		}

		inline SwizzledFloat3& operator = (const Float3& source)
		{
			if (base == &source._x)
			{
				SOAVecFloat temp[3];

				temp[0] = source._x;
				temp[1] = source._y;
				temp[2] = source._z;

				base[a] = temp[0];
				base[b] = temp[1];
				base[c] = temp[2];
			}
			else
			{
				base[a] = source._x;
				base[b] = source._y;
				base[c] = source._z;
			}

			return *this;
		}

		inline SwizzledFloat3& operator = (const SwizzledFloat3& source)
		{
			if (base == source.base)
			{
				SOAVecFloat temp[3];

				temp[0] = base[0];
				temp[1] = base[1];
				temp[2] = base[2];

				base[a] = temp[source.a];
				base[b] = temp[source.b];
				base[c] = temp[source.c];
			}
			else
			{
				base[a] = source.base[source.a];
				base[b] = source.base[source.b];
				base[c] = source.base[source.c];
			}

			return *this;
		}

		inline SwizzledFloat3& operator += (const Float3& source)
		{
			if (base == &source._x)
			{
				SOAVecFloat temp[3];

				temp[0] = source._x;
				temp[1] = source._y;
				temp[2] = source._z;

				base[a] += temp[0];
				base[b] += temp[1];
				base[c] += temp[2];
			}
			else
			{
				base[a] += source._x;
				base[b] += source._y;
				base[c] += source._z;
			}

			return *this;
		}

		inline SwizzledFloat3& operator += (const SwizzledFloat3& source)
		{
			if (base == source.base)
			{
				SOAVecFloat temp[3];

				temp[0] = base[0];
				temp[1] = base[1];
				temp[2] = base[2];

				base[a] += temp[source.a];
				base[b] += temp[source.b];
				base[c] += temp[source.c];
			}
			else
			{
				base[a] += source.base[source.a];
				base[b] += source.base[source.b];
				base[c] += source.base[source.c];
			}

			return *this;
		}

		inline SwizzledFloat3& operator -= (const Float3& source)
		{
			if (base == &source._x)
			{
				SOAVecFloat temp[3];

				temp[0] = source._x;
				temp[1] = source._y;
				temp[2] = source._z;

				base[a] -= temp[0];
				base[b] -= temp[1];
				base[c] -= temp[2];
			}
			else
			{
				base[a] -= source._x;
				base[b] -= source._y;
				base[c] -= source._z;
			}

			return *this;
		}

		inline SwizzledFloat3& operator -= (const SwizzledFloat3& source)
		{
			if (base == source.base)
			{
				SOAVecFloat temp[3];

				temp[0] = base[0];
				temp[1] = base[1];
				temp[2] = base[2];

				base[a] -= temp[source.a];
				base[b] -= temp[source.b];
				base[c] -= temp[source.c];
			}
			else
			{
				base[a] -= source.base[source.a];
				base[b] -= source.base[source.b];
				base[c] -= source.base[source.c];
			}

			return *this;
		}

		inline SwizzledFloat3& operator *= (const Float3& source)
		{
			if (base == &source._x)
			{
				SOAVecFloat temp[3];

				temp[0] = source._x;
				temp[1] = source._y;
				temp[2] = source._z;

				base[a] *= temp[0];
				base[b] *= temp[1];
				base[c] *= temp[2];
			}
			else
			{
				base[a] *= source._x;
				base[b] *= source._y;
				base[c] *= source._z;
			}

			return *this;
		}

		inline SwizzledFloat3& operator *= (const SwizzledFloat3& source)
		{
			if (base == source.base)
			{
				SOAVecFloat temp[3];

				temp[0] = base[0];
				temp[1] = base[1];
				temp[2] = base[2];

				base[a] *= temp[source.a];
				base[b] *= temp[source.b];
				base[c] *= temp[source.c];
			}
			else
			{
				base[a] *= source.base[source.a];
				base[b] *= source.base[source.b];
				base[c] *= source.base[source.c];
			}

			return *this;
		}

		inline SwizzledFloat3& operator /= (const Float3& source)
		{
			if (base == &source._x)
			{
				SOAVecFloat temp[3];

				temp[0] = source._x;
				temp[1] = source._y;
				temp[2] = source._z;

				base[a] /= temp[0];
				base[b] /= temp[1];
				base[c] /= temp[2];
			}
			else
			{
				base[a] /= source._x;
				base[b] /= source._y;
				base[c] /= source._z;
			}

			return *this;
		}

		inline SwizzledFloat3& operator /= (const SwizzledFloat3& source)
		{
			if (base == source.base)
			{
				SOAVecFloat temp[3];

				temp[0] = base[0];
				temp[1] = base[1];
				temp[2] = base[2];

				base[a] /= temp[source.a];
				base[b] /= temp[source.b];
				base[c] /= temp[source.c];
			}
			else
			{
				base[a] /= source.base[source.a];
				base[b] /= source.base[source.b];
				base[c] /= source.base[source.c];
			}

			return *this;
		}

		inline operator Float() const
		{
			return *base;
		}

		inline operator Float2() const
		{
			return { base[a],base[b] };
		}

		inline operator Float3() const
		{
			return { base[a],base[b], base[c] };
		}
	};

	class ConstSwizzledFloat3
	{
	public:
		SOAVecFloat* base;
		Ceng::UINT32 a;
		Ceng::UINT32 b;
		Ceng::UINT32 c;

	public:

		inline ConstSwizzledFloat3(SOAVecFloat* base, Ceng::UINT32 a, Ceng::UINT32 b, Ceng::UINT32 c)
			: base(base), a(a), b(b), c(c)
		{

		}

		inline operator Float() const
		{
			return *base;
		}

		inline operator Float2() const
		{
			return { base[a],base[b] };
		}

		inline operator Float3() const
		{
			return { base[a],base[b],base[c] };
		}
	};

	class SwizzledFloat4
	{
	public:
		SOAVecFloat* base;
		Ceng::UINT32 a;
		Ceng::UINT32 b;
		Ceng::UINT32 c;
		Ceng::UINT32 d;

	public:

		inline SwizzledFloat4(SOAVecFloat* base, Ceng::UINT32 a, Ceng::UINT32 b, Ceng::UINT32 c, Ceng::UINT32 d)
			: base(base), a(a), b(b), c(c), d(d)
		{

		}

		inline SwizzledFloat4& operator = (const Float4& source)
		{
			if (base == &source._x)
			{
				SOAVecFloat temp[4];

				temp[0] = source._x;
				temp[1] = source._y;
				temp[2] = source._z;
				temp[3] = source._w;

				base[a] = temp[0];
				base[b] = temp[1];
				base[c] = temp[2];
				base[d] = temp[3];
			}
			else
			{
				base[a] = source._x;
				base[b] = source._y;
				base[c] = source._z;
				base[d] = source._w;
			}

			return *this;
		}

		inline SwizzledFloat4& operator = (const SwizzledFloat4& source)
		{
			if (base == source.base)
			{
				SOAVecFloat temp[4];

				temp[0] = base[0];
				temp[1] = base[1];
				temp[2] = base[2];
				temp[3] = base[3];

				base[a] = temp[source.a];
				base[b] = temp[source.b];
				base[c] = temp[source.c];
				base[d] = temp[source.d];
			}
			else
			{
				base[a] = source.base[source.a];
				base[b] = source.base[source.b];
				base[c] = source.base[source.c];
				base[d] = source.base[source.d];
			}

			return *this;
		}

		inline SwizzledFloat4& operator += (const Float4& source)
		{
			if (base == &source._x)
			{
				SOAVecFloat temp[4];

				temp[0] = source._x;
				temp[1] = source._y;
				temp[2] = source._z;
				temp[3] = source._w;

				base[a] += temp[0];
				base[b] += temp[1];
				base[c] += temp[2];
				base[d] += temp[3];
			}
			else
			{
				base[a] += source._x;
				base[b] += source._y;
				base[c] += source._z;
				base[d] += source._w;
			}

			return *this;
		}

		inline SwizzledFloat4& operator += (const SwizzledFloat4& source)
		{
			if (base == source.base)
			{
				SOAVecFloat temp[4];

				temp[0] = base[0];
				temp[1] = base[1];
				temp[2] = base[2];
				temp[3] = base[3];

				base[a] += temp[source.a];
				base[b] += temp[source.b];
				base[c] += temp[source.c];
				base[d] += temp[source.d];
			}
			else
			{
				base[a] += source.base[source.a];
				base[b] += source.base[source.b];
				base[c] += source.base[source.c];
				base[d] += source.base[source.d];
			}

			return *this;
		}

		inline SwizzledFloat4& operator -= (const Float4& source)
		{
			if (base == &source._x)
			{
				SOAVecFloat temp[4];

				temp[0] = source._x;
				temp[1] = source._y;
				temp[2] = source._z;
				temp[3] = source._w;

				base[a] -= temp[0];
				base[b] -= temp[1];
				base[c] -= temp[2];
				base[d] -= temp[3];
			}
			else
			{
				base[a] -= source._x;
				base[b] -= source._y;
				base[c] -= source._z;
				base[d] -= source._w;
			}

			return *this;
		}

		inline SwizzledFloat4& operator -= (const SwizzledFloat4& source)
		{
			if (base == source.base)
			{
				SOAVecFloat temp[4];

				temp[0] = base[0];
				temp[1] = base[1];
				temp[2] = base[2];
				temp[3] = base[3];

				base[a] -= temp[source.a];
				base[b] -= temp[source.b];
				base[c] -= temp[source.c];
				base[d] -= temp[source.d];
			}
			else
			{
				base[a] -= source.base[source.a];
				base[b] -= source.base[source.b];
				base[c] -= source.base[source.c];
				base[d] -= source.base[source.d];
			}

			return *this;
		}

		inline SwizzledFloat4& operator *= (const Float4& source)
		{
			if (base == &source._x)
			{
				SOAVecFloat temp[4];

				temp[0] = source._x;
				temp[1] = source._y;
				temp[2] = source._z;
				temp[3] = source._w;

				base[a] *= temp[0];
				base[b] *= temp[1];
				base[c] *= temp[2];
				base[d] *= temp[3];
			}
			else
			{
				base[a] *= source._x;
				base[b] *= source._y;
				base[c] *= source._z;
				base[d] *= source._w;
			}

			return *this;
		}

		inline SwizzledFloat4& operator *= (const SwizzledFloat4& source)
		{
			if (base == source.base)
			{
				SOAVecFloat temp[4];

				temp[0] = base[0];
				temp[1] = base[1];
				temp[2] = base[2];
				temp[3] = base[3];

				base[a] *= temp[source.a];
				base[b] *= temp[source.b];
				base[c] *= temp[source.c];
				base[d] *= temp[source.d];
			}
			else
			{
				base[a] *= source.base[source.a];
				base[b] *= source.base[source.b];
				base[c] *= source.base[source.c];
				base[d] *= source.base[source.d];
			}

			return *this;
		}

		inline SwizzledFloat4& operator /= (const Float4& source)
		{
			if (base == &source._x)
			{
				SOAVecFloat temp[4];

				temp[0] = source._x;
				temp[1] = source._y;
				temp[2] = source._z;
				temp[3] = source._w;

				base[a] /= temp[0];
				base[b] /= temp[1];
				base[c] /= temp[2];
				base[d] /= temp[3];
			}
			else
			{
				base[a] /= source._x;
				base[b] /= source._y;
				base[c] /= source._z;
				base[d] /= source._w;
			}

			return *this;
		}

		inline SwizzledFloat4& operator /= (const SwizzledFloat4& source)
		{
			if (base == source.base)
			{
				SOAVecFloat temp[4];

				temp[0] = base[0];
				temp[1] = base[1];
				temp[2] = base[2];
				temp[3] = base[3];

				base[a] /= temp[source.a];
				base[b] /= temp[source.b];
				base[c] /= temp[source.c];
				base[d] /= temp[source.d];
			}
			else
			{
				base[a] /= source.base[source.a];
				base[b] /= source.base[source.b];
				base[c] /= source.base[source.c];
				base[d] /= source.base[source.d];
			}

			return *this;
		}

		inline operator Float() const
		{
			return *base;
		}

		inline operator Float2() const
		{
			return { base[a],base[b] };
		}

		inline operator Float3() const
		{
			return { base[a],base[b], base[c] };
		}

		inline operator Float4() const
		{
			return { base[a],base[b], base[c], base[d] };
		}
	};

	class ConstSwizzledFloat4
	{
	public:
		SOAVecFloat* base;
		Ceng::UINT32 a;
		Ceng::UINT32 b;
		Ceng::UINT32 c;
		Ceng::UINT32 d;

	public:

		inline ConstSwizzledFloat4(SOAVecFloat* base, Ceng::UINT32 a, Ceng::UINT32 b, Ceng::UINT32 c, Ceng::UINT32 d)
			: base(base), a(a), b(b), c(c), d(d)
		{

		}

		inline operator Float() const
		{
			return *base;
		}

		inline operator Float2() const
		{
			return { base[a],base[b] };
		}

		inline operator Float3() const
		{
			return { base[a],base[b],base[c] };
		}

		inline operator Float4() const
		{
			return { base[a],base[b],base[c],base[d] };
		}
	};

	//**********************************************************
	// Shader::Float methods

	inline Float& Float::operator= (const Float2& other)
	{
		x = other._x;
		return *this;
	}

	inline Float& Float::operator= (const Float3& other)
	{
		x = other._x;
		return *this;
	}

	inline Float& Float::operator= (const Float4& other)
	{
		x = other._x;
		return *this;
	}

	inline Float& Float::operator= (const SwizzledFloat& other)
	{
		x = *other.base;
		return *this;
	}

	//**********************************************************
	// Shader::Float2 methods

	inline Float2& Float2::operator= (const Float3& other)
	{
		_x = other._x;
		_y = other._y;

		return *this;
	}

	inline Float2& Float2::operator= (const Float4& other)
	{
		_x = other._x;
		_y = other._y;

		return *this;
	}

	inline Float2& Float2::operator= (const SwizzledFloat2& other)
	{
		if (other.base == &_x)
		{
			SOAVecFloat temp[2];

			temp[0] = _x;
			temp[1] = _y;

			_x = temp[other.a];
			_y = temp[other.b];
		}
		else
		{
			_x = other.base[other.a];
			_y = other.base[other.b];
		}

		return *this;
	}

	inline Float2& Float2::operator= (const ConstSwizzledFloat2& other)
	{
		if (other.base == &_x)
		{
			SOAVecFloat temp[2];

			temp[0] = _x;
			temp[1] = _y;

			_x = temp[other.a];
			_y = temp[other.b];
		}
		else
		{
			_x = other.base[other.a];
			_y = other.base[other.b];
		}

		return *this;
	}

	inline SwizzledFloat Float2::x()
	{
		return SwizzledFloat(&_x, 0);
	}

	inline SwizzledFloat Float2::y()
	{
		return SwizzledFloat(&_y, 1);
	}

	inline ConstSwizzledFloat2 Float2::xx()
	{
		return ConstSwizzledFloat2(&_x, 0, 0);
	}

	inline SwizzledFloat2 Float2::xy()
	{
		return SwizzledFloat2(&_x, 0, 1);
	}

	inline SwizzledFloat2 Float2::yx()
	{
		return SwizzledFloat2(&_x, 1, 0);
	}

	inline ConstSwizzledFloat2 Float2::yy()
	{
		return ConstSwizzledFloat2(&_x, 1, 1);
	}

	//**********************************************************
	// Shader::Float3 methods

	inline Float3& Float3::operator= (const Float4& other)
	{
		_x = other._x;
		_y = other._y;
		_z = other._z;

		return *this;
	}

	inline Float3& Float3::operator= (const SwizzledFloat3& other)
	{
		if (other.base == &_x)
		{
			SOAVecFloat temp[3];

			temp[0] = _x;
			temp[1] = _y;
			temp[2] = _z;

			_x = temp[other.a];
			_y = temp[other.b];
			_z = temp[other.c];
		}
		else
		{
			_x = other.base[other.a];
			_y = other.base[other.b];
			_z = other.base[other.c];
		}

		return *this;
	}

	inline Float3& Float3::operator= (const ConstSwizzledFloat3& other)
	{
		if (other.base == &_x)
		{
			SOAVecFloat temp[3];

			temp[0] = _x;
			temp[1] = _y;
			temp[2] = _z;

			_x = temp[other.a];
			_y = temp[other.b];
			_z = temp[other.c];
		}
		else
		{
			_x = other.base[other.a];
			_y = other.base[other.b];
			_z = other.base[other.c];
		}

		return *this;
	}

	inline SwizzledFloat Float3::x()
	{
		return SwizzledFloat(&_x, 0);
	}

	inline SwizzledFloat Float3::y()
	{
		return SwizzledFloat(&_y, 1);
	}

	inline SwizzledFloat Float3::z()
	{
		return SwizzledFloat(&_z, 2);
	}

	inline ConstSwizzledFloat2 Float3::xx()
	{
		return ConstSwizzledFloat2(&_x, 0, 0);
	}

	inline SwizzledFloat2 Float3::xy()
	{
		return SwizzledFloat2(&_x, 0, 1);
	}

	inline SwizzledFloat2 Float3::xz()
	{
		return SwizzledFloat2(&_x, 0, 2);
	}

	inline SwizzledFloat2 Float3::yx()
	{
		return SwizzledFloat2(&_x, 1, 0);
	}

	inline ConstSwizzledFloat2 Float3::yy()
	{
		return ConstSwizzledFloat2(&_x, 1, 1);
	}

	inline SwizzledFloat2 Float3::yz()
	{
		return SwizzledFloat2(&_x, 1, 2);
	}

	inline SwizzledFloat2 Float3::zx()
	{
		return SwizzledFloat2(&_x, 2, 0);
	}

	inline SwizzledFloat2 Float3::zy()
	{
		return SwizzledFloat2(&_x, 2, 1);
	}

	inline ConstSwizzledFloat2 Float3::zz()
	{
		return ConstSwizzledFloat2(&_x, 2, 2);
	}

	inline ConstSwizzledFloat3 Float3::xxx()
	{
		return ConstSwizzledFloat3(&_x, 0, 0, 0);
	}

	inline ConstSwizzledFloat3 Float3::xxy()
	{
		return ConstSwizzledFloat3(&_x, 0, 0, 1);
	}

	inline ConstSwizzledFloat3 Float3::xxz()
	{
		return ConstSwizzledFloat3(&_x, 0, 0, 2);
	}

	inline ConstSwizzledFloat3 Float3::xyx()
	{
		return ConstSwizzledFloat3(&_x, 0, 1, 0);
	}

	inline ConstSwizzledFloat3 Float3::xyy()
	{
		return ConstSwizzledFloat3(&_x, 0, 1, 1);
	}

	inline SwizzledFloat3 Float3::xyz()
	{
		return SwizzledFloat3(&_x, 0, 1, 2);
	}

	inline ConstSwizzledFloat3 Float3::xzx()
	{
		return ConstSwizzledFloat3(&_x, 0, 2, 0);
	}

	inline SwizzledFloat3 Float3::xzy()
	{
		return SwizzledFloat3(&_x, 0, 2, 1);
	}

	inline ConstSwizzledFloat3 Float3::xzz()
	{
		return ConstSwizzledFloat3(&_x, 0, 2, 2);
	}

	inline ConstSwizzledFloat3 Float3::yxx()
	{
		return ConstSwizzledFloat3(&_x, 1, 0, 0);
	}

	inline ConstSwizzledFloat3 Float3::yxy()
	{
		return ConstSwizzledFloat3(&_x, 1, 0, 1);
	}

	inline SwizzledFloat3 Float3::yxz()
	{
		return SwizzledFloat3(&_x, 1, 0, 2);
	}

	inline ConstSwizzledFloat3 Float3::yyx()
	{
		return ConstSwizzledFloat3(&_x, 1, 1, 0);
	}

	inline ConstSwizzledFloat3 Float3::yyy()
	{
		return ConstSwizzledFloat3(&_x, 1, 1, 1);
	}

	inline ConstSwizzledFloat3 Float3::yyz()
	{
		return ConstSwizzledFloat3(&_x, 1, 1, 2);
	}

	inline SwizzledFloat3 Float3::yzx()
	{
		return SwizzledFloat3(&_x, 1, 2, 0);
	}

	inline ConstSwizzledFloat3 Float3::yzy()
	{
		return ConstSwizzledFloat3(&_x, 1, 2, 1);
	}

	inline ConstSwizzledFloat3 Float3::yzz()
	{
		return ConstSwizzledFloat3(&_x, 1, 2, 2);
	}

	inline ConstSwizzledFloat3 Float3::zxx()
	{
		return ConstSwizzledFloat3(&_x, 2, 0, 0);
	}

	inline SwizzledFloat3 Float3::zxy()
	{
		return SwizzledFloat3(&_x, 2, 0, 1);
	}

	inline ConstSwizzledFloat3 Float3::zxz()
	{
		return ConstSwizzledFloat3(&_x, 2, 0, 2);
	}

	inline SwizzledFloat3 Float3::zyx()
	{
		return SwizzledFloat3(&_x, 2, 1, 0);
	}

	inline ConstSwizzledFloat3 Float3::zyy()
	{
		return ConstSwizzledFloat3(&_x, 2, 1, 1);
	}

	inline ConstSwizzledFloat3 Float3::zyz()
	{
		return ConstSwizzledFloat3(&_x, 2, 1, 2);
	}

	inline ConstSwizzledFloat3 Float3::zzx()
	{
		return ConstSwizzledFloat3(&_x, 2, 2, 0);
	}

	inline ConstSwizzledFloat3 Float3::zzy()
	{
		return ConstSwizzledFloat3(&_x, 2, 2, 1);
	}

	inline ConstSwizzledFloat3 Float3::zzz()
	{
		return ConstSwizzledFloat3(&_x, 2, 2, 2);
	}

	//**********************************************************
	// Shader::Float4 methods		

	inline Float4& Float4::operator = (const SampleTexture2D& source)
	{
		(*call_mov_float4)((void*)&_x, (void*)source.dataAddress);
		return *this;
	}

	inline Float4& Float4::operator = (const SwizzledFloat4& source)
	{
		if (source.base == &_x)
		{
			SOAVecFloat temp[4];

			temp[0] = _x;
			temp[1] = _y;
			temp[2] = _z;
			temp[3] = _w;

			_x = temp[source.a];
			_y = temp[source.b];
			_z = temp[source.c];
			_w = temp[source.d];
		}
		else
		{
			_x = source.base[source.a];
			_y = source.base[source.b];
			_z = source.base[source.c];
			_w = source.base[source.d];
		}

		return *this;
	}

	inline Float4& Float4::operator = (const ConstSwizzledFloat4& source)
	{
		if (source.base == &_x)
		{
			SOAVecFloat temp[4];

			temp[0] = _x;
			temp[1] = _y;
			temp[2] = _z;
			temp[3] = _w;

			_x = temp[source.a];
			_y = temp[source.b];
			_z = temp[source.c];
			_w = temp[source.d];
		}
		else
		{
			_x = source.base[source.a];
			_y = source.base[source.b];
			_z = source.base[source.c];
			_w = source.base[source.d];
		}

		return *this;
	}

	inline SwizzledFloat Float4::x()
	{
		return SwizzledFloat(&_x, 0);
	}

	inline SwizzledFloat Float4::y()
	{
		return SwizzledFloat(&_x, 1);
	}

	inline SwizzledFloat Float4::z()
	{
		return SwizzledFloat(&_x, 2);
	}

	inline SwizzledFloat Float4::w()
	{
		return SwizzledFloat(&_x, 3);
	}

	inline ConstSwizzledFloat2 Float4::xx()
	{
		return ConstSwizzledFloat2(&_x, 0, 0);
	}

	inline SwizzledFloat2 Float4::xy()
	{
		return SwizzledFloat2(&_x, 0, 1);
	}

	inline SwizzledFloat2 Float4::xz()
	{
		return SwizzledFloat2(&_x, 0, 2);
	}

	inline SwizzledFloat2 Float4::xw()
	{
		return SwizzledFloat2(&_x, 0, 3);
	}

	inline SwizzledFloat2 Float4::yx()
	{
		return SwizzledFloat2(&_x, 1, 0);
	}

	inline ConstSwizzledFloat2 Float4::yy()
	{
		return ConstSwizzledFloat2(&_x, 1, 1);
	}

	inline SwizzledFloat2 Float4::yz()
	{
		return SwizzledFloat2(&_x, 1, 2);
	}

	inline SwizzledFloat2 Float4::yw()
	{
		return SwizzledFloat2(&_x, 1, 3);
	}

	inline SwizzledFloat2 Float4::zx()
	{
		return SwizzledFloat2(&_x, 2, 0);
	}

	inline SwizzledFloat2 Float4::zy()
	{
		return SwizzledFloat2(&_x, 2, 1);
	}

	inline ConstSwizzledFloat2 Float4::zz()
	{
		return ConstSwizzledFloat2(&_x, 2, 2);
	}

	inline SwizzledFloat2 Float4::zw()
	{
		return SwizzledFloat2(&_x, 2, 3);
	}

	inline SwizzledFloat2 Float4::wx()
	{
		return SwizzledFloat2(&_x, 3, 0);
	}

	inline SwizzledFloat2 Float4::wy()
	{
		return SwizzledFloat2(&_x, 3, 1);
	}

	inline SwizzledFloat2 Float4::wz()
	{
		return SwizzledFloat2(&_x, 3, 2);
	}

	inline ConstSwizzledFloat2 Float4::ww()
	{
		return ConstSwizzledFloat2(&_x, 3, 3);
	}

	inline ConstSwizzledFloat3 Float4::xxx()
	{
		return ConstSwizzledFloat3(&_x, 0, 0, 0);
	}

	inline ConstSwizzledFloat3 Float4::xxy()
	{
		return ConstSwizzledFloat3(&_x, 0, 0, 1);
	}

	inline ConstSwizzledFloat3 Float4::xxz()
	{
		return ConstSwizzledFloat3(&_x, 0, 0, 2);
	}

	inline ConstSwizzledFloat3 Float4::xxw()
	{
		return ConstSwizzledFloat3(&_x, 0, 0, 3);
	}

	inline ConstSwizzledFloat3 Float4::xyx()
	{
		return ConstSwizzledFloat3(&_x, 0, 1, 0);
	}

	inline ConstSwizzledFloat3 Float4::xyy()
	{
		return ConstSwizzledFloat3(&_x, 0, 1, 1);
	}

	inline SwizzledFloat3 Float4::xyz()
	{
		return SwizzledFloat3(&_x, 0, 1, 2);
	}

	inline SwizzledFloat3 Float4::xyw()
	{
		return SwizzledFloat3(&_x, 0, 1, 3);
	}

	inline ConstSwizzledFloat3 Float4::xzx()
	{
		return ConstSwizzledFloat3(&_x, 0, 2, 0);
	}

	inline SwizzledFloat3 Float4::xzy()
	{
		return SwizzledFloat3(&_x, 0, 2, 1);
	}

	inline ConstSwizzledFloat3 Float4::xzz()
	{
		return ConstSwizzledFloat3(&_x, 0, 2, 2);
	}

	inline SwizzledFloat3 Float4::xzw()
	{
		return SwizzledFloat3(&_x, 0, 2, 3);
	}

	inline ConstSwizzledFloat3 Float4::xwx()
	{
		return ConstSwizzledFloat3(&_x, 0, 3, 0);
	}

	inline SwizzledFloat3 Float4::xwy()
	{
		return SwizzledFloat3(&_x, 0, 3, 1);
	}

	inline SwizzledFloat3 Float4::xwz()
	{
		return SwizzledFloat3(&_x, 0, 3, 2);
	}

	inline ConstSwizzledFloat3 Float4::xww()
	{
		return ConstSwizzledFloat3(&_x, 0, 3, 3);
	}

	inline ConstSwizzledFloat3 Float4::yxx()
	{
		return ConstSwizzledFloat3(&_x, 1, 0, 0);
	}

	inline ConstSwizzledFloat3 Float4::yxy()
	{
		return ConstSwizzledFloat3(&_x, 1, 0, 1);
	}

	inline SwizzledFloat3 Float4::yxz()
	{
		return SwizzledFloat3(&_x, 1, 0, 2);
	}

	inline SwizzledFloat3 Float4::yxw()
	{
		return SwizzledFloat3(&_x, 1, 0, 3);
	}

	inline ConstSwizzledFloat3 Float4::yyx()
	{
		return ConstSwizzledFloat3(&_x, 1, 1, 0);
	}

	inline ConstSwizzledFloat3 Float4::yyy()
	{
		return ConstSwizzledFloat3(&_x, 1, 1, 1);
	}

	inline ConstSwizzledFloat3 Float4::yyz()
	{
		return ConstSwizzledFloat3(&_x, 1, 1, 2);
	}

	inline ConstSwizzledFloat3 Float4::yyw()
	{
		return ConstSwizzledFloat3(&_x, 1, 1, 3);
	}

	inline SwizzledFloat3 Float4::yzx()
	{
		return SwizzledFloat3(&_x, 1, 2, 0);
	}

	inline ConstSwizzledFloat3 Float4::yzy()
	{
		return ConstSwizzledFloat3(&_x, 1, 2, 1);
	}

	inline ConstSwizzledFloat3 Float4::yzz()
	{
		return ConstSwizzledFloat3(&_x, 1, 2, 2);
	}

	inline SwizzledFloat3 Float4::yzw()
	{
		return SwizzledFloat3(&_x, 1, 2, 3);
	}

	inline SwizzledFloat3 Float4::ywx()
	{
		return SwizzledFloat3(&_x, 1, 3, 0);
	}

	inline ConstSwizzledFloat3 Float4::ywy()
	{
		return ConstSwizzledFloat3(&_x, 1, 3, 1);
	}

	inline SwizzledFloat3 Float4::ywz()
	{
		return SwizzledFloat3(&_x, 1, 3, 2);
	}

	inline ConstSwizzledFloat3 Float4::yww()
	{
		return ConstSwizzledFloat3(&_x, 1, 3, 3);
	}

	inline ConstSwizzledFloat3 Float4::zxx()
	{
		return ConstSwizzledFloat3(&_x, 2, 0, 0);
	}

	inline SwizzledFloat3 Float4::zxy()
	{
		return SwizzledFloat3(&_x, 2, 0, 1);
	}

	inline ConstSwizzledFloat3 Float4::zxz()
	{
		return ConstSwizzledFloat3(&_x, 2, 0, 2);
	}

	inline SwizzledFloat3 Float4::zxw()
	{
		return SwizzledFloat3(&_x, 2, 0, 3);
	}

	inline SwizzledFloat3 Float4::zyx()
	{
		return SwizzledFloat3(&_x, 2, 1, 0);
	}

	inline ConstSwizzledFloat3 Float4::zyy()
	{
		return ConstSwizzledFloat3(&_x, 2, 1, 1);
	}

	inline ConstSwizzledFloat3 Float4::zyz()
	{
		return ConstSwizzledFloat3(&_x, 2, 1, 2);
	}

	inline SwizzledFloat3 Float4::zyw()
	{
		return SwizzledFloat3(&_x, 2, 1, 3);
	}

	inline ConstSwizzledFloat3 Float4::zzx()
	{
		return ConstSwizzledFloat3(&_x, 2, 2, 0);
	}

	inline ConstSwizzledFloat3 Float4::zzy()
	{
		return ConstSwizzledFloat3(&_x, 2, 2, 1);
	}

	inline ConstSwizzledFloat3 Float4::zzz()
	{
		return ConstSwizzledFloat3(&_x, 2, 2, 2);
	}

	inline ConstSwizzledFloat3 Float4::zzw()
	{
		return ConstSwizzledFloat3(&_x, 2, 2, 3);
	}

	inline SwizzledFloat3 Float4::zwx()
	{
		return SwizzledFloat3(&_x, 2, 3, 0);
	}

	inline SwizzledFloat3 Float4::zwy()
	{
		return SwizzledFloat3(&_x, 2, 3, 1);
	}

	inline ConstSwizzledFloat3 Float4::zwz()
	{
		return ConstSwizzledFloat3(&_x, 2, 3, 2);
	}

	inline ConstSwizzledFloat3 Float4::zww()
	{
		return ConstSwizzledFloat3(&_x, 2, 3, 3);
	}

	inline ConstSwizzledFloat3 Float4::wxx()
	{
		return ConstSwizzledFloat3(&_x, 3, 0, 0);
	}

	inline SwizzledFloat3 Float4::wxy()
	{
		return SwizzledFloat3(&_x, 3, 0, 1);
	}

	inline SwizzledFloat3 Float4::wxz()
	{
		return SwizzledFloat3(&_x, 3, 0, 2);
	}

	inline ConstSwizzledFloat3 Float4::wxw()
	{
		return ConstSwizzledFloat3(&_x, 3, 0, 3);
	}

	inline SwizzledFloat3 Float4::wyx()
	{
		return SwizzledFloat3(&_x, 3, 1, 0);
	}

	inline ConstSwizzledFloat3 Float4::wyy()
	{
		return ConstSwizzledFloat3(&_x, 3, 1, 1);
	}

	inline SwizzledFloat3 Float4::wyz()
	{
		return SwizzledFloat3(&_x, 3, 1, 2);
	}

	inline ConstSwizzledFloat3 Float4::wyw()
	{
		return ConstSwizzledFloat3(&_x, 3, 1, 3);
	}

	inline SwizzledFloat3 Float4::wzx()
	{
		return SwizzledFloat3(&_x, 3, 2, 0);
	}

	inline SwizzledFloat3 Float4::wzy()
	{
		return SwizzledFloat3(&_x, 3, 2, 1);
	}

	inline ConstSwizzledFloat3 Float4::wzz()
	{
		return ConstSwizzledFloat3(&_x, 3, 2, 2);
	}

	inline ConstSwizzledFloat3 Float4::wzw()
	{
		return ConstSwizzledFloat3(&_x, 3, 2, 3);
	}

	inline ConstSwizzledFloat3 Float4::wwx()
	{
		return ConstSwizzledFloat3(&_x, 3, 3, 0);
	}

	inline ConstSwizzledFloat3 Float4::wwy()
	{
		return ConstSwizzledFloat3(&_x, 3, 3, 1);
	}

	inline ConstSwizzledFloat3 Float4::wwz()
	{
		return ConstSwizzledFloat3(&_x, 3, 3, 2);
	}

	inline ConstSwizzledFloat3 Float4::www()
	{
		return ConstSwizzledFloat3(&_x, 3, 3, 3);
	}

	// swizzles x---

	inline ConstSwizzledFloat4 Float4::xxxx()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 0, 0);
	}

	inline ConstSwizzledFloat4 Float4::xxxy()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 0, 1);
	}

	inline ConstSwizzledFloat4 Float4::xxxz()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 0, 2);
	}

	inline ConstSwizzledFloat4 Float4::xxxw()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 0, 3);
	}

	inline ConstSwizzledFloat4 Float4::xxyx()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::xxyy()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 1, 1);
	}

	inline ConstSwizzledFloat4 Float4::xxyz()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 1, 2);
	}

	inline ConstSwizzledFloat4 Float4::xxyw()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 1, 3);
	}

	inline ConstSwizzledFloat4 Float4::xxzx()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 2, 0);
	}

	inline ConstSwizzledFloat4 Float4::xxzy()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::xxzz()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 2, 2);
	}

	inline ConstSwizzledFloat4 Float4::xxzw()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 2, 3);
	}

	inline ConstSwizzledFloat4 Float4::xxwx()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 3, 0);
	}

	inline ConstSwizzledFloat4 Float4::xxwy()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 3, 1);
	}

	inline ConstSwizzledFloat4 Float4::xxwz()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::xxww()
	{
		return ConstSwizzledFloat4(&_x, 0, 0, 3, 3);
	}

	inline ConstSwizzledFloat4 Float4::xyxx()
	{
		return ConstSwizzledFloat4(&_x, 0, 1, 0, 0);
	}

	inline ConstSwizzledFloat4 Float4::xyxy()
	{
		return ConstSwizzledFloat4(&_x, 0, 1, 0, 1);
	}

	inline ConstSwizzledFloat4 Float4::xyxz()
	{
		return ConstSwizzledFloat4(&_x, 0, 1, 0, 2);
	}

	inline ConstSwizzledFloat4 Float4::xyxw()
	{
		return ConstSwizzledFloat4(&_x, 0, 1, 0, 3);
	}

	inline ConstSwizzledFloat4 Float4::xyyx()
	{
		return ConstSwizzledFloat4(&_x, 0, 1, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::xyyy()
	{
		return ConstSwizzledFloat4(&_x, 0, 1, 1, 1);
	}

	inline ConstSwizzledFloat4 Float4::xyyz()
	{
		return ConstSwizzledFloat4(&_x, 0, 1, 1, 2);
	}

	inline ConstSwizzledFloat4 Float4::xyyw()
	{
		return ConstSwizzledFloat4(&_x, 0, 1, 1, 3);
	}

	inline ConstSwizzledFloat4 Float4::xyzx()
	{
		return ConstSwizzledFloat4(&_x, 0, 1, 2, 0);
	}

	inline ConstSwizzledFloat4 Float4::xyzy()
	{
		return ConstSwizzledFloat4(&_x, 0, 1, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::xyzz()
	{
		return ConstSwizzledFloat4(&_x, 0, 1, 2, 2);
	}

	inline SwizzledFloat4 Float4::xyzw()
	{
		return SwizzledFloat4(&_x, 0, 1, 2, 3);
	}

	inline ConstSwizzledFloat4 Float4::xywx()
	{
		return ConstSwizzledFloat4(&_x, 0, 1, 3, 0);
	}

	inline ConstSwizzledFloat4 Float4::xywy()
	{
		return ConstSwizzledFloat4(&_x, 0, 1, 3, 1);
	}

	inline SwizzledFloat4 Float4::xywz()
	{
		return SwizzledFloat4(&_x, 0, 1, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::xyww()
	{
		return ConstSwizzledFloat4(&_x, 0, 1, 3, 3);
	}

	inline ConstSwizzledFloat4 Float4::xzxx()
	{
		return ConstSwizzledFloat4(&_x, 0, 2, 0, 0);
	}

	inline ConstSwizzledFloat4 Float4::xzxy()
	{
		return ConstSwizzledFloat4(&_x, 0, 2, 0, 1);
	}

	inline ConstSwizzledFloat4 Float4::xzxz()
	{
		return ConstSwizzledFloat4(&_x, 0, 2, 0, 2);
	}

	inline ConstSwizzledFloat4 Float4::xzxw()
	{
		return ConstSwizzledFloat4(&_x, 0, 2, 0, 3);
	}

	inline ConstSwizzledFloat4 Float4::xzyx()
	{
		return ConstSwizzledFloat4(&_x, 0, 2, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::xzyy()
	{
		return ConstSwizzledFloat4(&_x, 0, 2, 1, 1);
	}

	inline ConstSwizzledFloat4 Float4::xzyz()
	{
		return ConstSwizzledFloat4(&_x, 0, 2, 1, 2);
	}

	inline SwizzledFloat4 Float4::xzyw()
	{
		return SwizzledFloat4(&_x, 0, 2, 1, 3);
	}

	inline ConstSwizzledFloat4 Float4::xzzx()
	{
		return ConstSwizzledFloat4(&_x, 0, 2, 2, 0);
	}

	inline ConstSwizzledFloat4 Float4::xzzy()
	{
		return ConstSwizzledFloat4(&_x, 0, 2, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::xzzz()
	{
		return ConstSwizzledFloat4(&_x, 0, 2, 2, 2);
	}

	inline ConstSwizzledFloat4 Float4::xzzw()
	{
		return ConstSwizzledFloat4(&_x, 0, 2, 2, 3);
	}

	inline ConstSwizzledFloat4 Float4::xzwx()
	{
		return ConstSwizzledFloat4(&_x, 0, 2, 3, 0);
	}

	inline SwizzledFloat4 Float4::xzwy()
	{
		return SwizzledFloat4(&_x, 0, 2, 3, 1);
	}

	inline ConstSwizzledFloat4 Float4::xzwz()
	{
		return ConstSwizzledFloat4(&_x, 0, 2, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::xzww()
	{
		return ConstSwizzledFloat4(&_x, 0, 2, 3, 3);
	}

	inline ConstSwizzledFloat4 Float4::xwxx()
	{
		return ConstSwizzledFloat4(&_x, 0, 3, 0, 0);
	}

	inline ConstSwizzledFloat4 Float4::xwxy()
	{
		return ConstSwizzledFloat4(&_x, 0, 3, 0, 1);
	}

	inline ConstSwizzledFloat4 Float4::xwxz()
	{
		return ConstSwizzledFloat4(&_x, 0, 3, 0, 2);
	}

	inline ConstSwizzledFloat4 Float4::xwxw()
	{
		return ConstSwizzledFloat4(&_x, 0, 3, 0, 3);
	}

	inline ConstSwizzledFloat4 Float4::xwyx()
	{
		return ConstSwizzledFloat4(&_x, 0, 3, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::xwyy()
	{
		return ConstSwizzledFloat4(&_x, 0, 3, 1, 1);
	}

	inline SwizzledFloat4 Float4::xwyz()
	{
		return SwizzledFloat4(&_x, 0, 3, 1, 2);
	}

	inline ConstSwizzledFloat4 Float4::xwyw()
	{
		return ConstSwizzledFloat4(&_x, 0, 3, 1, 3);
	}

	inline ConstSwizzledFloat4 Float4::xwzx()
	{
		return ConstSwizzledFloat4(&_x, 0, 3, 2, 0);
	}

	inline SwizzledFloat4 Float4::xwzy()
	{
		return SwizzledFloat4(&_x, 0, 3, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::xwzz()
	{
		return ConstSwizzledFloat4(&_x, 0, 3, 2, 2);
	}

	inline ConstSwizzledFloat4 Float4::xwzw()
	{
		return ConstSwizzledFloat4(&_x, 0, 3, 2, 3);
	}

	inline ConstSwizzledFloat4 Float4::xwwx()
	{
		return ConstSwizzledFloat4(&_x, 0, 3, 3, 0);
	}

	inline ConstSwizzledFloat4 Float4::xwwy()
	{
		return ConstSwizzledFloat4(&_x, 0, 3, 3, 1);
	}

	inline ConstSwizzledFloat4 Float4::xwwz()
	{
		return ConstSwizzledFloat4(&_x, 0, 3, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::xwww()
	{
		return ConstSwizzledFloat4(&_x, 0, 3, 3, 3);
	}

	// swizzles y---

	inline ConstSwizzledFloat4 Float4::yxxx()
	{
		return ConstSwizzledFloat4(&_x, 1, 0, 0, 0);
	}

	inline ConstSwizzledFloat4 Float4::yxxy()
	{
		return ConstSwizzledFloat4(&_x, 1, 0, 0, 1);
	}

	inline ConstSwizzledFloat4 Float4::yxxz()
	{
		return ConstSwizzledFloat4(&_x, 1, 0, 0, 2);
	}

	inline ConstSwizzledFloat4 Float4::yxxw()
	{
		return ConstSwizzledFloat4(&_x, 1, 0, 0, 3);
	}

	inline ConstSwizzledFloat4 Float4::yxyx()
	{
		return ConstSwizzledFloat4(&_x, 1, 0, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::yxyy()
	{
		return ConstSwizzledFloat4(&_x, 1, 0, 1, 1);
	}

	inline ConstSwizzledFloat4 Float4::yxyz()
	{
		return ConstSwizzledFloat4(&_x, 1, 0, 1, 2);
	}

	inline ConstSwizzledFloat4 Float4::yxyw()
	{
		return ConstSwizzledFloat4(&_x, 1, 0, 1, 3);
	}

	inline ConstSwizzledFloat4 Float4::yxzx()
	{
		return ConstSwizzledFloat4(&_x, 1, 0, 2, 0);
	}

	inline ConstSwizzledFloat4 Float4::yxzy()
	{
		return ConstSwizzledFloat4(&_x, 1, 0, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::yxzz()
	{
		return ConstSwizzledFloat4(&_x, 1, 0, 2, 2);
	}

	inline SwizzledFloat4 Float4::yxzw()
	{
		return SwizzledFloat4(&_x, 1, 0, 2, 3);
	}

	inline ConstSwizzledFloat4 Float4::yxwx()
	{
		return ConstSwizzledFloat4(&_x, 1, 0, 3, 0);
	}

	inline ConstSwizzledFloat4 Float4::yxwy()
	{
		return ConstSwizzledFloat4(&_x, 1, 0, 3, 1);
	}

	inline SwizzledFloat4 Float4::yxwz()
	{
		return SwizzledFloat4(&_x, 1, 0, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::yxww()
	{
		return ConstSwizzledFloat4(&_x, 1, 0, 3, 3);
	}

	inline ConstSwizzledFloat4 Float4::yyxx()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 0, 0);
	}

	inline ConstSwizzledFloat4 Float4::yyxy()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 0, 1);
	}

	inline ConstSwizzledFloat4 Float4::yyxz()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 0, 2);
	}

	inline ConstSwizzledFloat4 Float4::yyxw()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 0, 3);
	}

	inline ConstSwizzledFloat4 Float4::yyyx()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::yyyy()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 1, 1);
	}

	inline ConstSwizzledFloat4 Float4::yyyz()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 1, 2);
	}

	inline ConstSwizzledFloat4 Float4::yyyw()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 1, 3);
	}

	inline ConstSwizzledFloat4 Float4::yyzx()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 2, 0);
	}

	inline ConstSwizzledFloat4 Float4::yyzy()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::yyzz()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 2, 2);
	}

	inline ConstSwizzledFloat4 Float4::yyzw()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 2, 3);
	}

	inline ConstSwizzledFloat4 Float4::yywx()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 3, 0);
	}

	inline ConstSwizzledFloat4 Float4::yywy()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 3, 1);
	}

	inline ConstSwizzledFloat4 Float4::yywz()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::yyww()
	{
		return ConstSwizzledFloat4(&_x, 1, 1, 3, 3);
	}

	inline ConstSwizzledFloat4 Float4::yzxx()
	{
		return ConstSwizzledFloat4(&_x, 1, 2, 0, 0);
	}

	inline ConstSwizzledFloat4 Float4::yzxy()
	{
		return ConstSwizzledFloat4(&_x, 1, 2, 0, 1);
	}

	inline ConstSwizzledFloat4 Float4::yzxz()
	{
		return ConstSwizzledFloat4(&_x, 1, 2, 0, 2);
	}

	inline SwizzledFloat4 Float4::yzxw()
	{
		return SwizzledFloat4(&_x, 1, 2, 0, 3);
	}

	inline ConstSwizzledFloat4 Float4::yzyx()
	{
		return ConstSwizzledFloat4(&_x, 1, 2, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::yzyy()
	{
		return ConstSwizzledFloat4(&_x, 1, 2, 1, 1);
	}

	inline ConstSwizzledFloat4 Float4::yzyz()
	{
		return ConstSwizzledFloat4(&_x, 1, 2, 1, 2);
	}

	inline ConstSwizzledFloat4 Float4::yzyw()
	{
		return ConstSwizzledFloat4(&_x, 1, 2, 1, 3);
	}

	inline ConstSwizzledFloat4 Float4::yzzx()
	{
		return ConstSwizzledFloat4(&_x, 1, 2, 2, 0);
	}

	inline ConstSwizzledFloat4 Float4::yzzy()
	{
		return ConstSwizzledFloat4(&_x, 1, 2, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::yzzz()
	{
		return ConstSwizzledFloat4(&_x, 1, 2, 2, 2);
	}

	inline ConstSwizzledFloat4 Float4::yzzw()
	{
		return ConstSwizzledFloat4(&_x, 1, 2, 2, 3);
	}

	inline SwizzledFloat4 Float4::yzwx()
	{
		return SwizzledFloat4(&_x, 1, 2, 3, 0);
	}

	inline ConstSwizzledFloat4 Float4::yzwy()
	{
		return ConstSwizzledFloat4(&_x, 1, 2, 3, 1);
	}

	inline ConstSwizzledFloat4 Float4::yzwz()
	{
		return ConstSwizzledFloat4(&_x, 1, 2, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::yzww()
	{
		return ConstSwizzledFloat4(&_x, 1, 2, 3, 3);
	}

	inline ConstSwizzledFloat4 Float4::ywxx()
	{
		return ConstSwizzledFloat4(&_x, 1, 3, 0, 0);
	}

	inline ConstSwizzledFloat4 Float4::ywxy()
	{
		return ConstSwizzledFloat4(&_x, 1, 3, 0, 1);
	}

	inline SwizzledFloat4 Float4::ywxz()
	{
		return SwizzledFloat4(&_x, 1, 3, 0, 2);
	}

	inline ConstSwizzledFloat4 Float4::ywxw()
	{
		return ConstSwizzledFloat4(&_x, 1, 3, 0, 3);
	}

	inline ConstSwizzledFloat4 Float4::ywyx()
	{
		return ConstSwizzledFloat4(&_x, 1, 3, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::ywyy()
	{
		return ConstSwizzledFloat4(&_x, 1, 3, 1, 1);
	}

	inline ConstSwizzledFloat4 Float4::ywyz()
	{
		return ConstSwizzledFloat4(&_x, 1, 3, 1, 2);
	}

	inline ConstSwizzledFloat4 Float4::ywyw()
	{
		return ConstSwizzledFloat4(&_x, 1, 3, 1, 3);
	}

	inline SwizzledFloat4 Float4::ywzx()
	{
		return SwizzledFloat4(&_x, 1, 3, 2, 0);
	}

	inline ConstSwizzledFloat4 Float4::ywzy()
	{
		return ConstSwizzledFloat4(&_x, 1, 3, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::ywzz()
	{
		return ConstSwizzledFloat4(&_x, 1, 3, 2, 2);
	}

	inline ConstSwizzledFloat4 Float4::ywzw()
	{
		return ConstSwizzledFloat4(&_x, 1, 3, 2, 3);
	}

	inline ConstSwizzledFloat4 Float4::ywwx()
	{
		return ConstSwizzledFloat4(&_x, 1, 3, 3, 0);
	}

	inline ConstSwizzledFloat4 Float4::ywwy()
	{
		return ConstSwizzledFloat4(&_x, 1, 3, 3, 1);
	}

	inline ConstSwizzledFloat4 Float4::ywwz()
	{
		return ConstSwizzledFloat4(&_x, 1, 3, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::ywww()
	{
		return ConstSwizzledFloat4(&_x, 1, 3, 3, 3);
	}

	// swizzles z---

	inline ConstSwizzledFloat4 Float4::zxxx()
	{
		return ConstSwizzledFloat4(&_x, 2, 0, 0, 0);
	}

	inline ConstSwizzledFloat4 Float4::zxxy()
	{
		return ConstSwizzledFloat4(&_x, 2, 0, 0, 1);
	}

	inline ConstSwizzledFloat4 Float4::zxxz()
	{
		return ConstSwizzledFloat4(&_x, 2, 0, 0, 2);
	}

	inline ConstSwizzledFloat4 Float4::zxxw()
	{
		return ConstSwizzledFloat4(&_x, 2, 0, 0, 3);
	}

	inline ConstSwizzledFloat4 Float4::zxyx()
	{
		return ConstSwizzledFloat4(&_x, 2, 0, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::zxyy()
	{
		return ConstSwizzledFloat4(&_x, 2, 0, 1, 1);
	}

	inline ConstSwizzledFloat4 Float4::zxyz()
	{
		return ConstSwizzledFloat4(&_x, 2, 0, 1, 2);
	}

	inline SwizzledFloat4 Float4::zxyw()
	{
		return SwizzledFloat4(&_x, 2, 0, 1, 3);
	}

	inline ConstSwizzledFloat4 Float4::zxzx()
	{
		return ConstSwizzledFloat4(&_x, 2, 0, 2, 0);
	}

	inline ConstSwizzledFloat4 Float4::zxzy()
	{
		return ConstSwizzledFloat4(&_x, 2, 0, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::zxzz()
	{
		return ConstSwizzledFloat4(&_x, 2, 0, 2, 2);
	}

	inline ConstSwizzledFloat4 Float4::zxzw()
	{
		return ConstSwizzledFloat4(&_x, 2, 0, 2, 3);
	}

	inline ConstSwizzledFloat4 Float4::zxwx()
	{
		return ConstSwizzledFloat4(&_x, 2, 0, 3, 0);
	}

	inline SwizzledFloat4 Float4::zxwy()
	{
		return SwizzledFloat4(&_x, 2, 0, 3, 1);
	}

	inline ConstSwizzledFloat4 Float4::zxwz()
	{
		return ConstSwizzledFloat4(&_x, 2, 0, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::zxww()
	{
		return ConstSwizzledFloat4(&_x, 2, 0, 3, 3);
	}

	inline ConstSwizzledFloat4 Float4::zyxx()
	{
		return ConstSwizzledFloat4(&_x, 2, 1, 0, 0);
	}

	inline ConstSwizzledFloat4 Float4::zyxy()
	{
		return ConstSwizzledFloat4(&_x, 2, 1, 0, 1);
	}

	inline ConstSwizzledFloat4 Float4::zyxz()
	{
		return ConstSwizzledFloat4(&_x, 2, 1, 0, 2);
	}

	inline SwizzledFloat4 Float4::zyxw()
	{
		return SwizzledFloat4(&_x, 2, 1, 0, 3);
	}

	inline ConstSwizzledFloat4 Float4::zyyx()
	{
		return ConstSwizzledFloat4(&_x, 2, 1, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::zyyy()
	{
		return ConstSwizzledFloat4(&_x, 2, 1, 1, 1);
	}

	inline ConstSwizzledFloat4 Float4::zyyz()
	{
		return ConstSwizzledFloat4(&_x, 2, 1, 1, 2);
	}

	inline ConstSwizzledFloat4 Float4::zyyw()
	{
		return ConstSwizzledFloat4(&_x, 2, 1, 1, 3);
	}

	inline ConstSwizzledFloat4 Float4::zyzx()
	{
		return ConstSwizzledFloat4(&_x, 2, 1, 2, 0);
	}

	inline ConstSwizzledFloat4 Float4::zyzy()
	{
		return ConstSwizzledFloat4(&_x, 2, 1, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::zyzz()
	{
		return ConstSwizzledFloat4(&_x, 2, 1, 2, 2);
	}

	inline ConstSwizzledFloat4 Float4::zyzw()
	{
		return ConstSwizzledFloat4(&_x, 2, 1, 2, 3);
	}

	inline SwizzledFloat4 Float4::zywx()
	{
		return SwizzledFloat4(&_x, 2, 1, 3, 0);
	}

	inline ConstSwizzledFloat4 Float4::zywy()
	{
		return ConstSwizzledFloat4(&_x, 2, 1, 3, 1);
	}

	inline ConstSwizzledFloat4 Float4::zywz()
	{
		return ConstSwizzledFloat4(&_x, 2, 1, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::zyww()
	{
		return ConstSwizzledFloat4(&_x, 2, 1, 3, 3);
	}

	inline ConstSwizzledFloat4 Float4::zzxx()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 0, 0);
	}

	inline ConstSwizzledFloat4 Float4::zzxy()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 0, 1);
	}

	inline ConstSwizzledFloat4 Float4::zzxz()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 0, 2);
	}

	inline ConstSwizzledFloat4 Float4::zzxw()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 0, 3);
	}

	inline ConstSwizzledFloat4 Float4::zzyx()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::zzyy()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 1, 1);
	}

	inline ConstSwizzledFloat4 Float4::zzyz()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 1, 2);
	}

	inline ConstSwizzledFloat4 Float4::zzyw()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 1, 3);
	}

	inline ConstSwizzledFloat4 Float4::zzzx()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 2, 0);
	}

	inline ConstSwizzledFloat4 Float4::zzzy()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::zzzz()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 2, 2);
	}

	inline ConstSwizzledFloat4 Float4::zzzw()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 2, 3);
	}

	inline ConstSwizzledFloat4 Float4::zzwx()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 3, 0);
	}

	inline ConstSwizzledFloat4 Float4::zzwy()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 3, 1);
	}

	inline ConstSwizzledFloat4 Float4::zzwz()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::zzww()
	{
		return ConstSwizzledFloat4(&_x, 2, 2, 3, 3);
	}

	inline ConstSwizzledFloat4 Float4::zwxx()
	{
		return ConstSwizzledFloat4(&_x, 2, 3, 0, 0);
	}

	inline SwizzledFloat4 Float4::zwxy()
	{
		return SwizzledFloat4(&_x, 2, 3, 0, 1);
	}

	inline ConstSwizzledFloat4 Float4::zwxz()
	{
		return ConstSwizzledFloat4(&_x, 2, 3, 0, 2);
	}

	inline ConstSwizzledFloat4 Float4::zwxw()
	{
		return ConstSwizzledFloat4(&_x, 2, 3, 0, 3);
	}

	inline SwizzledFloat4 Float4::zwyx()
	{
		return SwizzledFloat4(&_x, 2, 3, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::zwyy()
	{
		return ConstSwizzledFloat4(&_x, 2, 3, 1, 1);
	}

	inline ConstSwizzledFloat4 Float4::zwyz()
	{
		return ConstSwizzledFloat4(&_x, 2, 3, 1, 2);
	}

	inline ConstSwizzledFloat4 Float4::zwyw()
	{
		return ConstSwizzledFloat4(&_x, 2, 3, 1, 3);
	}

	inline ConstSwizzledFloat4 Float4::zwzx()
	{
		return ConstSwizzledFloat4(&_x, 2, 3, 2, 0);
	}

	inline ConstSwizzledFloat4 Float4::zwzy()
	{
		return ConstSwizzledFloat4(&_x, 2, 3, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::zwzz()
	{
		return ConstSwizzledFloat4(&_x, 2, 3, 2, 2);
	}

	inline ConstSwizzledFloat4 Float4::zwzw()
	{
		return ConstSwizzledFloat4(&_x, 2, 3, 2, 3);
	}

	inline ConstSwizzledFloat4 Float4::zwwx()
	{
		return ConstSwizzledFloat4(&_x, 2, 3, 3, 0);
	}

	inline ConstSwizzledFloat4 Float4::zwwy()
	{
		return ConstSwizzledFloat4(&_x, 2, 3, 3, 1);
	}

	inline ConstSwizzledFloat4 Float4::zwwz()
	{
		return ConstSwizzledFloat4(&_x, 2, 3, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::zwww()
	{
		return ConstSwizzledFloat4(&_x, 2, 3, 3, 3);
	}

	// swizzles w---

	inline ConstSwizzledFloat4 Float4::wxxx()
	{
		return ConstSwizzledFloat4(&_x, 3, 0, 0, 0);
	}

	inline ConstSwizzledFloat4 Float4::wxxy()
	{
		return ConstSwizzledFloat4(&_x, 3, 0, 0, 1);
	}

	inline ConstSwizzledFloat4 Float4::wxxz()
	{
		return ConstSwizzledFloat4(&_x, 3, 0, 0, 2);
	}

	inline ConstSwizzledFloat4 Float4::wxxw()
	{
		return ConstSwizzledFloat4(&_x, 3, 0, 0, 3);
	}

	inline ConstSwizzledFloat4 Float4::wxyx()
	{
		return ConstSwizzledFloat4(&_x, 3, 0, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::wxyy()
	{
		return ConstSwizzledFloat4(&_x, 3, 0, 1, 1);
	}

	inline SwizzledFloat4 Float4::wxyz()
	{
		return SwizzledFloat4(&_x, 3, 0, 1, 2);
	}

	inline ConstSwizzledFloat4 Float4::wxyw()
	{
		return ConstSwizzledFloat4(&_x, 3, 0, 1, 3);
	}

	inline ConstSwizzledFloat4 Float4::wxzx()
	{
		return ConstSwizzledFloat4(&_x, 3, 0, 2, 0);
	}

	inline SwizzledFloat4 Float4::wxzy()
	{
		return SwizzledFloat4(&_x, 3, 0, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::wxzz()
	{
		return ConstSwizzledFloat4(&_x, 3, 0, 2, 2);
	}

	inline ConstSwizzledFloat4 Float4::wxzw()
	{
		return ConstSwizzledFloat4(&_x, 3, 0, 2, 3);
	}

	inline ConstSwizzledFloat4 Float4::wxwx()
	{
		return ConstSwizzledFloat4(&_x, 3, 0, 3, 0);
	}

	inline ConstSwizzledFloat4 Float4::wxwy()
	{
		return ConstSwizzledFloat4(&_x, 3, 0, 3, 1);
	}

	inline ConstSwizzledFloat4 Float4::wxwz()
	{
		return ConstSwizzledFloat4(&_x, 3, 0, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::wxww()
	{
		return ConstSwizzledFloat4(&_x, 3, 0, 3, 3);
	}

	inline ConstSwizzledFloat4 Float4::wyxx()
	{
		return ConstSwizzledFloat4(&_x, 3, 1, 0, 0);
	}

	inline ConstSwizzledFloat4 Float4::wyxy()
	{
		return ConstSwizzledFloat4(&_x, 3, 1, 0, 1);
	}

	inline SwizzledFloat4 Float4::wyxz()
	{
		return SwizzledFloat4(&_x, 3, 1, 0, 2);
	}

	inline ConstSwizzledFloat4 Float4::wyxw()
	{
		return ConstSwizzledFloat4(&_x, 3, 1, 0, 3);
	}

	inline ConstSwizzledFloat4 Float4::wyyx()
	{
		return ConstSwizzledFloat4(&_x, 3, 1, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::wyyy()
	{
		return ConstSwizzledFloat4(&_x, 3, 1, 1, 1);
	}

	inline ConstSwizzledFloat4 Float4::wyyz()
	{
		return ConstSwizzledFloat4(&_x, 3, 1, 1, 2);
	}

	inline ConstSwizzledFloat4 Float4::wyyw()
	{
		return ConstSwizzledFloat4(&_x, 3, 1, 1, 3);
	}

	inline SwizzledFloat4 Float4::wyzx()
	{
		return SwizzledFloat4(&_x, 3, 1, 2, 0);
	}

	inline ConstSwizzledFloat4 Float4::wyzy()
	{
		return ConstSwizzledFloat4(&_x, 3, 1, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::wyzz()
	{
		return ConstSwizzledFloat4(&_x, 3, 1, 2, 2);
	}

	inline ConstSwizzledFloat4 Float4::wyzw()
	{
		return ConstSwizzledFloat4(&_x, 3, 1, 2, 3);
	}

	inline ConstSwizzledFloat4 Float4::wywx()
	{
		return ConstSwizzledFloat4(&_x, 3, 1, 3, 0);
	}

	inline ConstSwizzledFloat4 Float4::wywy()
	{
		return ConstSwizzledFloat4(&_x, 3, 1, 3, 1);
	}

	inline ConstSwizzledFloat4 Float4::wywz()
	{
		return ConstSwizzledFloat4(&_x, 3, 1, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::wyww()
	{
		return ConstSwizzledFloat4(&_x, 3, 1, 3, 3);
	}

	inline ConstSwizzledFloat4 Float4::wzxx()
	{
		return ConstSwizzledFloat4(&_x, 3, 2, 0, 0);
	}

	inline SwizzledFloat4 Float4::wzxy()
	{
		return SwizzledFloat4(&_x, 3, 2, 0, 1);
	}

	inline ConstSwizzledFloat4 Float4::wzxz()
	{
		return ConstSwizzledFloat4(&_x, 3, 2, 0, 2);
	}

	inline ConstSwizzledFloat4 Float4::wzxw()
	{
		return ConstSwizzledFloat4(&_x, 3, 2, 0, 3);
	}

	inline SwizzledFloat4 Float4::wzyx()
	{
		return SwizzledFloat4(&_x, 3, 2, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::wzyy()
	{
		return ConstSwizzledFloat4(&_x, 3, 2, 1, 1);
	}

	inline ConstSwizzledFloat4 Float4::wzyz()
	{
		return ConstSwizzledFloat4(&_x, 3, 2, 1, 2);
	}

	inline ConstSwizzledFloat4 Float4::wzyw()
	{
		return ConstSwizzledFloat4(&_x, 3, 2, 1, 3);
	}

	inline ConstSwizzledFloat4 Float4::wzzx()
	{
		return ConstSwizzledFloat4(&_x, 3, 2, 2, 0);
	}

	inline ConstSwizzledFloat4 Float4::wzzy()
	{
		return ConstSwizzledFloat4(&_x, 3, 2, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::wzzz()
	{
		return ConstSwizzledFloat4(&_x, 3, 2, 2, 2);
	}

	inline ConstSwizzledFloat4 Float4::wzzw()
	{
		return ConstSwizzledFloat4(&_x, 3, 2, 2, 3);
	}

	inline ConstSwizzledFloat4 Float4::wzwx()
	{
		return ConstSwizzledFloat4(&_x, 3, 2, 3, 0);
	}

	inline ConstSwizzledFloat4 Float4::wzwy()
	{
		return ConstSwizzledFloat4(&_x, 3, 2, 3, 1);
	}

	inline ConstSwizzledFloat4 Float4::wzwz()
	{
		return ConstSwizzledFloat4(&_x, 3, 2, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::wzww()
	{
		return ConstSwizzledFloat4(&_x, 3, 2, 3, 3);
	}

	inline ConstSwizzledFloat4 Float4::wwxx()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 0, 0);
	}

	inline ConstSwizzledFloat4 Float4::wwxy()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 0, 1);
	}

	inline ConstSwizzledFloat4 Float4::wwxz()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 0, 2);
	}

	inline ConstSwizzledFloat4 Float4::wwxw()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 0, 3);
	}

	inline ConstSwizzledFloat4 Float4::wwyx()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 1, 0);
	}

	inline ConstSwizzledFloat4 Float4::wwyy()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 1, 1);
	}

	inline ConstSwizzledFloat4 Float4::wwyz()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 1, 2);
	}

	inline ConstSwizzledFloat4 Float4::wwyw()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 1, 3);
	}

	inline ConstSwizzledFloat4 Float4::wwzx()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 2, 0);
	}

	inline ConstSwizzledFloat4 Float4::wwzy()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 2, 1);
	}

	inline ConstSwizzledFloat4 Float4::wwzz()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 2, 2);
	}

	inline ConstSwizzledFloat4 Float4::wwzw()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 2, 3);
	}

	inline ConstSwizzledFloat4 Float4::wwwx()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 3, 0);
	}

	inline ConstSwizzledFloat4 Float4::wwwy()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 3, 1);
	}

	inline ConstSwizzledFloat4 Float4::wwwz()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 3, 2);
	}

	inline ConstSwizzledFloat4 Float4::wwww()
	{
		return ConstSwizzledFloat4(&_x, 3, 3, 3, 3);
	}

	//**********************************************************
	// SampleTexture2D methods

	inline SampleTexture2D& SampleTexture2D::operator()(TextureUnit& texture, Float2& uv)
	{
		unit = &texture;
		this->uv = &uv;

		//texture.view->Sample2D(uv, texture.sampler, (Ceng::FLOAT32*)dataAddress);

		return *this;
	}

	inline void SampleTexture2D::SampleToFloat4(void* destBuffer) const
	{
		unit->view->Sample2D(*uv, unit->sampler, (Ceng::FLOAT32*)destBuffer);
	}

}; // namespace Ceng::Shader

#endif // Include guard