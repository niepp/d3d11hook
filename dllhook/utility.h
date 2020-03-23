#pragma once

const float cEpsilon = 1e-4f;
const float cPi = 3.141592653589793f;
const float RAD_TO_DEG = (180.f) / cPi;

struct Vector3
{
	union
	{
		float vec[3];
		struct
		{
			float x, y, z;
		};
	};

	Vector3() : x(0), y(0), z(0)
	{}

	Vector3(const float &_x, const float &_y, const float &_z) :
		x(_x), y(_y), z(_z)
	{}

};

struct Vector4
{
	union
	{
		float vec[4];
		struct
		{
			float x, y, z, w;
		};
	};

	Vector4() : x(0), y(0), z(0), w(0)
	{}

	Vector4(const float &_x, const float &_y, const float &_z, const float &_w) :
		x(_x), y(_y), z(_z), w(_w)
	{}
};

struct Matrix
{
	static const size_t N = 4;
	union
	{
		float m[N][N];
		float v[N * N];
		struct
		{
			float m00, m01, m02, m03,
				m10, m11, m12, m13,
				m20, m21, m22, m23,
				m30, m31, m32, m33;
		};
	};

	Matrix() :
		m00(0), m01(0), m02(0), m03(0),
		m10(0), m11(0), m12(0), m13(0),
		m20(0), m21(0), m22(0), m23(0),
		m30(0), m31(0), m32(0), m33(0)
	{
	}

	Matrix(const float &_m00, const float &_m01, const float &_m02, const float &_m03,
		const float &_m10, const float &_m11, const float &_m12, const float &_m13,
		const float &_m20, const float &_m21, const float &_m22, const float &_m23,
		const float &_m30, const float &_m31, const float &_m32, const float &_m33) :
		m00(_m00), m01(_m01), m02(_m02), m03(_m03),
		m10(_m10), m11(_m11), m12(_m12), m13(_m13),
		m20(_m20), m21(_m21), m22(_m22), m23(_m23),
		m30(_m30), m31(_m31), m32(_m32), m33(_m33)
	{
	}

	Matrix(const Vector3 &_v0, const Vector3 &_v1, const Vector3 &_v2, const Vector3 &_v3) :
		m00(_v0.x), m01(_v0.y), m02(_v0.z), m03(0),
		m10(_v1.x), m11(_v1.y), m12(_v1.z), m13(0),
		m20(_v2.x), m21(_v2.y), m22(_v2.z), m23(0),
		m30(_v3.x), m31(_v3.y), m32(_v3.z), m33(1)
	{
	}

	Matrix(const Vector4 &_v0, const Vector4 &_v1, const Vector4 &_v2, const Vector4 &_v3) :
		m00(_v0.x), m01(_v0.y), m02(_v0.z), m03(_v0.w),
		m10(_v1.x), m11(_v1.y), m12(_v1.z), m13(_v1.w),
		m20(_v2.x), m21(_v2.y), m22(_v2.z), m23(_v2.w),
		m30(_v3.x), m31(_v3.y), m32(_v3.z), m33(_v3.w)
	{
	}

	Vector3 GetRow(int row) const
	{
		return Vector3(m[row][0], m[row][1], m[row][2]);
	}

	void SetRow(int row, const Vector3 &vec)
	{
		m[row][0] = vec.x;
		m[row][1] = vec.y;
		m[row][2] = vec.z;
		m[row][3] = 0;
	}

	void Transpose()
	{
		std::swap(m01, m10);
		std::swap(m02, m20);
		std::swap(m03, m30);
		std::swap(m12, m21);
		std::swap(m13, m31);
		std::swap(m23, m32);
	}

	void Inverse()
	{
		float d12 = m20 * m31 - m30 * m21;
		float d13 = m20 * m32 - m30 * m22;
		float d23 = m21 * m32 - m31 * m22;
		float d24 = m21 * m33 - m31 * m23;
		float d34 = m22 * m33 - m32 * m23;
		float d41 = m23 * m30 - m33 * m20;

		Matrix r;

		r.m00 = (m11 * d34 - m12 * d24 + m13 * d23);
		r.m10 = -(m10 * d34 + m12 * d41 + m13 * d13);
		r.m20 = (m10 * d24 + m11 * d41 + m13 * d12);
		r.m30 = -(m10 * d23 - m11 * d13 + m12 * d12);

		// Com.mpute determinant as early as possible using these cofactors.
		float det = m00 * r.m00 + m01 * r.m10 + m02 * r.m20 + m03 * r.m30;

		float invDet = float(1) / det;
		// Com.mpute rest of inverse.
		r.m00 *= invDet;
		r.m10 *= invDet;
		r.m20 *= invDet;
		r.m30 *= invDet;

		r.m01 = -(m01 * d34 - m02 * d24 + m03 * d23) * invDet;
		r.m11 = (m00 * d34 + m02 * d41 + m03 * d13) * invDet;
		r.m21 = -(m00 * d24 + m01 * d41 + m03 * d12) * invDet;
		r.m31 = (m00 * d23 - m01 * d13 + m02 * d12) * invDet;

		// Pre-compute 2x2 dets for first two rows when computing
		// cofactors of last two rows.
		d12 = m00 * m11 - m10 * m01;
		d13 = m00 * m12 - m10 * m02;
		d23 = m01 * m12 - m11 * m02;
		d24 = m01 * m13 - m11 * m03;
		d34 = m02 * m13 - m12 * m03;
		d41 = m03 * m10 - m13 * m00;

		r.m02 = (m31 * d34 - m32 * d24 + m33 * d23) * invDet;
		r.m12 = -(m30 * d34 + m32 * d41 + m33 * d13) * invDet;
		r.m22 = (m30 * d24 + m31 * d41 + m33 * d12) * invDet;
		r.m32 = -(m30 * d23 - m31 * d13 + m32 * d12) * invDet;
		r.m03 = -(m21 * d34 - m22 * d24 + m23 * d23) * invDet;
		r.m13 = (m20 * d34 + m22 * d41 + m23 * d13) * invDet;
		r.m23 = -(m20 * d24 + m21 * d41 + m23 * d12) * invDet;
		r.m33 = (m20 * d23 - m21 * d13 + m22 * d12) * invDet;

		*this = r;

	}

};

inline Vector3 Normalize(const Vector3 &vec)
{
	Vector3 newv(vec);
	float len = sqrtf(newv.x * newv.x + newv.y * newv.y + newv.z * newv.z);
	if (len < cEpsilon) {
		return Vector3(0, 0, 0);
	}
	newv.x /= len;
	newv.y /= len;
	newv.z /= len;
	return newv;
}


inline float Dot(const Vector3 &vec1, const Vector3 &vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

inline Vector3 Cross(const Vector3 &vec1, const Vector3 &vec2)
{
	 Vector3 prod(
		vec1.y * vec2.z - vec1.z * vec2.y,
		vec1.z * vec2.x - vec1.x * vec2.z,
		vec1.x * vec2.y - vec1.y * vec2.x);
	 return Normalize(prod);
}

inline Vector4 operator *(Vector4 &vec, const Matrix &mat)
{
	Vector4 prod;
	prod.x = vec.x * mat.m00 + vec.y * mat.m10 + vec.z * mat.m20 + vec.w * mat.m30;
	prod.y = vec.x * mat.m01 + vec.y * mat.m11 + vec.z * mat.m21 + vec.w * mat.m31;
	prod.z = vec.x * mat.m02 + vec.y * mat.m12 + vec.z * mat.m22 + vec.w * mat.m32;
	prod.w = vec.x * mat.m03 + vec.y * mat.m13 + vec.z * mat.m23 + vec.w * mat.m33;
	return prod;
}

struct CarParams
{
	Vector3 pos;
	Vector3 euler;
};

struct CameraParams
{
	float aspect;
	float fovX;
	float fovY;
	Vector3 pos;
	Vector3 euler;
};

struct FrameParams
{
	uint64_t timems;
	CameraParams cameraParams;
	CarParams carParams;
};

struct ConstantBufferParams
{
	const unsigned char* pdata;
	size_t datanum;
	ConstantBufferParams()		
	{
		Reset();
	}

	void Reset()
	{
		pdata = nullptr;
		datanum = 0;
	}
};

inline uint64_t get_now_ms()
{
	auto tp_now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(tp_now.time_since_epoch()).count();
}

