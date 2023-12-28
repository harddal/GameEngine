#pragma once

#include <chrono>
#include <string>
#include <vector>

#include <vector3d.h>
#include <matrix4.h>
#include <ISceneNode.h>

#include <PxPhysicsAPI.h>

#define __pi 3.141592741f
#define deg2rad(x) (x * 0.017453293f)
#define rad2deg(x) (x * 57.29577791f)

#define clamp_to_delta(x) ((x) * dt)

#define dialog_filter_scene  "Scene Files\0*.scn\0Any File\0*.*\0"
#define dialog_filter_entity "Entity Files\0*.ent\0Any File\0*.*\0"
#define dialog_filter_script "Script Files\0*.asc\0Any File\0*.*\0"
#define dialog_filter_mesh   "Object\0*.obj\0DirectX Mesh\0*.x\0B3D Mesh\0*.b3d\0Any File\0*.*\0"
#define dialog_filter_image  "PNG\0*.png\0JPEG\0*.jpg\0Any File\0*.*\0"
#define dialog_filter_sound  "OGG\0*.ogg\0WAV\0*.wav\0Any File\0*.*\0"

namespace Utility
{
	void StartPerfClock();
	void StopPerfClock();
	unsigned int GetPerfClockResultMilliseconds();
	unsigned int GetPerfClockResultMicroseconds();
	
	void Warning(std::string error);
	void Error(std::string error);

	std::string FilenameFromPath(std::string path);
	std::string FileExtensionFromPath(std::string path);

	std::string OpenFileDialog(const char* filter = "Any File\0*.*\0");
	std::string SaveFileDialog(const char* filter = "Any File\0*.*\0");

	inline std::string GetRightSideOfDelim(const std::string &str, const std::string &delim) {
		return str.substr(str.find(delim) + delim.size());
	}
	inline std::string RemoveAbsDir(const std::string &path) { return "content" + GetRightSideOfDelim(path, "content"); }

	inline irr::core::vector3df PxVec3_To_IrrVec3(physx::PxVec3 x) { return irr::core::vector3df(x.x, x.y, x.z); }
	inline physx::PxVec3 IrrVec3_To_PxVec3(irr::core::vector3df x) { return physx::PxVec3(x.X, x.Y, x.Z); }

    inline bool GetCmdlOptionExists(const std::string& cmdl, const std::string& option)
    {
        return cmdl.find(option) == std::string::npos ? false : true;
    }

    inline bool EvalTrueFalse(const char *statement)
    {
        const auto string = std::string(statement);

        if (string == "true" || string == "1") 
        {
            return true;
        }

        return false;
    }

    inline bool ProcessBoolStatement(const std::string &in)
    {
        if (in == std::string("\0") || in == std::string("0") || in == std::string("false")) {
            return false;
        }
        if (in == std::string("1") || in == std::string("true")) {
            return true;
        }

        return false;
    }
}

namespace Math
{
    static double ConstrainAngle(double x)
    {
        x = fmod(x, 360);
        if (x < 0)
            x += 360;
        return x;
    }

    static irr::core::vector3df ConstrainAngleVector3(irr::core::vector3df v)
    {
        v.X = fmod<irr::f32, irr::f32>(v.X, 360);
        if (v.X < 0)
            v.X += 360;

        v.Y = fmod<irr::f32, irr::f32>(v.Y, 360);
        if (v.Y < 0)
            v.Y += 360;

        v.Z = fmod<irr::f32, irr::f32>(v.Z, 360);
        if (v.Z < 0)
            v.Z += 360;

        return v;
    }

	inline irr::core::vector3df GetDirectionVector(const irr::core::vector3df& rotation, const bool normalize = false)
	{
		const auto 
	        x = static_cast<float>( sin(deg2rad(rotation.Y)) * cos(deg2rad(rotation.X))),
			y = static_cast<float>( -sin(deg2rad(rotation.X))),
			z = static_cast<float>( cos(deg2rad(rotation.Y)) * cos(deg2rad(rotation.X)));

		if (normalize) 
        {
            return irr::core::vector3df(x, y, z).normalize();
		}

		return irr::core::vector3df(x, y, z);
	}

    // Returns a floating point stable 3d coord distance
    inline float Stable_3D_Distance(const irr::core::vector3df& start, const irr::core::vector3df& end)
    {
        return hypot(hypot(start.X - end.X, start.Y - end.Y), start.Z - end.Z);
    }
    
	// TODO: Refactor and improve performance
	inline irr::core::vector3df QuaternionToEuler(const physx::PxQuat& quaternion)
	{
		float w, x, y, z;

		w = quaternion.w;
		x = quaternion.x;
		y = quaternion.y;
		z = quaternion.z;

		double sqw = w * w;
		double sqx = x * x;
		double sqy = y * y;
		double sqz = z * z;

		irr::core::vector3df euler;

		euler.Z = static_cast<irr::f32>(atan2(2.0 * (x * y + z * w), ( sqx - sqy - sqz + sqw)) * (180.0f / __pi));
		euler.X = static_cast<irr::f32>(atan2(2.0 * (y * z + x * w), (-sqx - sqy + sqz + sqw)) * (180.0f / __pi));
		euler.Y = static_cast<irr::f32>(asin(-2.0 * (x * z - y * w)) * (180.0f / __pi));

		return euler;
	}

    // TODO: Refactor and improve performance
	inline physx::PxQuat EulerToQuaternion(irr::core::vector3df rot)
	{
		/*float 
	        cos_z = cosf(0.5f * rot.Z * __pi / 180.0f),
		    cos_y = cosf(0.5f * rot.Y * __pi / 180.0f),
		    cos_x = cosf(0.5f * rot.X * __pi / 180.0f),
		    sin_z = sinf(0.5f * rot.Z * __pi / 180.0f),
		    sin_y = sinf(0.5f * rot.Y * __pi / 180.0f),
		    sin_x = sinf(0.5f * rot.X * __pi / 180.0f);

	    physx::PxQuat q;
		q.w = cos_z * cos_y * cos_x + sin_z * sin_y * sin_x;
		q.x = cos_z * cos_y * sin_x - sin_z * sin_y * cos_x;
		q.y = cos_z * sin_y * cos_x + sin_z * cos_y * sin_x;
		q.z = sin_z * cos_y * cos_x - cos_z * sin_y * sin_x;

		return q;*/

        physx::PxQuat qx(deg2rad(rot.X), physx::PxVec3(1.0f, 0.0f, 0.0f));
        physx::PxQuat qy(deg2rad(rot.Y), physx::PxVec3(0.0f, 1.0f, 0.0f));
        physx::PxQuat qz(deg2rad(rot.Z), physx::PxVec3(0.0f, 0.0f, 1.0f));

        return qz * qy * qx;
	}
}