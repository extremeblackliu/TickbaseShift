#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>
#include <intrin.h>
#include <math.h>
#include <cmath>
#include <DirectXMath.h>

#define RAD2DEG(x) DirectX::XMConvertToDegrees(x)
#define DEG2RAD(x) DirectX::XMConvertToRadians(x)
#define M_PI 3.14159265358979323846
#define PI_F	((float)(M_PI)) 

class Vector
{
public:
    Vector(void)
    {
        Invalidate();
    }
    Vector(float X, float Y, float Z)
    {
        x = X;
        y = Y;
        z = Z;
    }
    Vector(const float* clr)
    {
        x = clr[0];
        y = clr[1];
        z = clr[2];
    }

    void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
    {
        x = ix; y = iy; z = iz;
    }
    bool IsValid() const
    {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }
    void Invalidate()
    {
        x = y = z = std::numeric_limits<float>::infinity();
    }

    float& operator[](int i)
    {
        return ((float*)this)[i];
    }
    float operator[](int i) const
    {
        return ((float*)this)[i];
    }

    void Zero()
    {
        x = y = z = 0.0f;
    }

    bool operator==(const Vector& src) const
    {
        return (src.x == x) && (src.y == y) && (src.z == z);
    }
    bool operator!=(const Vector& src) const
    {
        return (src.x != x) || (src.y != y) || (src.z != z);
    }

    Vector& operator+=(const Vector& v)
    {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    Vector& operator-=(const Vector& v)
    {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    Vector& operator*=(float fl)
    {
        x *= fl;
        y *= fl;
        z *= fl;
        return *this;
    }
    Vector& operator*=(const Vector& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
    Vector& operator/=(const Vector& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }
    Vector& operator+=(float fl)
    {
        x += fl;
        y += fl;
        z += fl;
        return *this;
    }
    Vector& operator/=(float fl)
    {
        x /= fl;
        y /= fl;
        z /= fl;
        return *this;
    }
    Vector& operator-=(float fl)
    {
        x -= fl;
        y -= fl;
        z -= fl;
        return *this;
    }
    void NormalizeInPlace()
    {
        *this = Normalized();
    }
    Vector Normalized() const
    {
        Vector res = *this;
        float l = res.Length();
        if (l != 0.0f) {
            res /= l;
        }
        else {
            res.x = res.y = res.z = 0.0f;
        }
        return res;
    }
    float DistTo(const Vector& vOther) const
    {
        Vector delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.Length();
    }
    float DistToSqr(const Vector& vOther) const
    {
        Vector delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.LengthSqr();
    }
    float Dot(const Vector& vOther) const
    {
        return (x * vOther.x + y * vOther.y + z * vOther.z);
    }
    float Length() const
    {
        return sqrt(x * x + y * y + z * z);
    }
    float LengthSqr(void) const
    {
        return (x * x + y * y + z * z);
    }
    float Length2D() const
    {
        return sqrt(x * x + y * y);
    }
    Vector& operator=(const Vector& vOther)
    {
        x = vOther.x; y = vOther.y; z = vOther.z;
        return *this;
    }
    float x, y, z, w;
};
class CUserCmd
{
public:
    CUserCmd()
    {
        memset(this, 0, sizeof(*this));
    };
    virtual ~CUserCmd() {};

    int     command_number;     // 0x04 For matching server and client commands for debugging
    int     tick_count;         // 0x08 the tick the client created this command
    Vector  viewangles;         // 0x0C Player instantaneous view angles.
    Vector  aimdirection;       // 0x18
    float   forwardmove;        // 0x24
    float   sidemove;           // 0x28
    float   upmove;             // 0x2C
    int     buttons;            // 0x30 Attack button states
    char    impulse;            // 0x34
    int     weaponselect;       // 0x38 Current weapon id
    int     weaponsubtype;      // 0x3C
    int     random_seed;        // 0x40 For shared random functions
    short   mousedx;            // 0x44 mouse accum in x from create move
    short   mousedy;            // 0x46 mouse accum in y from create move
    bool    hasbeenpredicted;   // 0x48 Client only, tracks whether we've predicted this command at least once
    char    pad_0x4C[0x18];     // 0x4C Current sizeof( usercmd ) =  100  = 0x64
};

static float clamp(float angle,float min,float max)
{
    return (angle < min ? min : (angle > max ? max : angle));
}

static void SinCos(float m_fRad, float* m_pSin, float* m_pCos)
{
    *m_pSin = sinf(m_fRad);
    *m_pCos = cosf(m_fRad);
}
static void Angle2Vectors(Vector& m_vSrc, Vector* m_pForward, Vector* m_pRight, Vector* m_pUp)
{
    static float sp, sy, sr, cp, cy, cr;

    SinCos(DEG2RAD(m_vSrc.x), &sp, &cp);
    SinCos(DEG2RAD(m_vSrc.y), &sy, &cy);
    SinCos(DEG2RAD(m_vSrc.z), &sr, &cr);

    m_pForward->x = cp * cy;
    m_pForward->y = cp * sy;
    m_pForward->z = -sp;

    if (m_pRight)
    {
        m_pRight->x = -sr * sp * cy + cr * sy;
        m_pRight->y = -sr * sp * sy - cr * cy;
        m_pRight->z = -sr * cp;
    }

    if (m_pUp)
    {
        m_pUp->x = cr * sp * cy + sr * sy;
        m_pUp->y = cr * sp * sy - sr * cy;
        m_pUp->z = cr * cp;
    }
}

inline IVEngineClient* engineclient = nullptr;
static void MovementFix(CUserCmd* m_pCMD)
{
    Vector m_vEngineViewAngles;
    engineclient->GetViewAngles(&m_vEngineViewAngles);

    Vector m_vVectors[2][2];
    Angle2Vectors(m_vEngineViewAngles, &m_vVectors[0][0], &m_vVectors[0][1], nullptr);
    Angle2Vectors(m_pCMD->viewangles, &m_vVectors[1][0], &m_vVectors[1][1], nullptr);

    m_vVectors[0][0].z = 0.f;
    m_vVectors[0][1].z = 0.f;
    m_vVectors[1][0].z = 0.f;
    m_vVectors[1][1].z = 0.f;

    m_vVectors[0][0].NormalizeInPlace();
    m_vVectors[0][1].NormalizeInPlace();
    m_vVectors[1][0].NormalizeInPlace();
    m_vVectors[1][1].NormalizeInPlace();

    float m_fSpeedX = m_vVectors[0][0].x * m_pCMD->forwardmove + m_vVectors[0][1].x * m_pCMD->sidemove;
    float m_fSpeedY = m_vVectors[0][0].y * m_pCMD->forwardmove + m_vVectors[0][1].y * m_pCMD->sidemove;

    float m_fDivide = (m_vVectors[1][0].x * m_vVectors[1][1].y - m_vVectors[1][1].x * m_vVectors[1][0].y);
    m_pCMD->forwardmove = fmaxf(-450.f, fminf((m_vVectors[1][1].y * m_fSpeedX - m_vVectors[1][1].x * m_fSpeedY) / m_fDivide, 450.f));
    m_pCMD->sidemove = fmaxf(-450.f, fminf((m_vVectors[1][0].x * m_fSpeedY - m_vVectors[1][0].y * m_fSpeedX) / m_fDivide, 450.f));
}
namespace Hooks
{
	void Init(HMODULE mod);

    bool __fastcall hk_VerifyReturnAddress(void* ecx, void* rdx, const char* modulename);
    bool __fastcall hk_SendNetMsg(void* netchan, void* edx, void* msg, bool a1, bool a2);
    bool __stdcall hk_CreateMove(float a1, CUserCmd* cmd);
}

namespace Utils
{
    static std::uint8_t* PatternScan(void* module, const char* signature)
    {
        static auto pattern_to_byte = [](const char* pattern) {
            auto bytes = std::vector<int>{};
            auto start = const_cast<char*>(pattern);
            auto end = const_cast<char*>(pattern) + strlen(pattern);

            for (auto current = start; current < end; ++current) {
                if (*current == '?') {
                    ++current;
                    if (*current == '?')
                        ++current;
                    bytes.push_back(-1);
                }
                else {
                    bytes.push_back(strtoul(current, &current, 16));
                }
            }
            return bytes;
        };

        auto dosHeader = (PIMAGE_DOS_HEADER)module;
        auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto patternBytes = pattern_to_byte(signature);
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

        auto size = patternBytes.size();
        auto data = patternBytes.data();

        for (auto i = 0ul; i < sizeOfImage - size; ++i) {
            bool found = true;
            for (auto counter = 0ul; counter < size; ++counter) {
                if (scanBytes[i + counter] != data[counter] && data[counter] != -1) {
                    found = false;
                    break;
                }
            }
            if (found) {
                return &scanBytes[i];
            }
        }
        return nullptr;
    }
}
