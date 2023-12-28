#include "Bindings.h"

#include "irrlicht.h"

using namespace irr::core;

static void ConstructVector3(void* ptr)
{
	new(ptr) vector3df();
}

static void DestructVector3(void* ptr)
{
	reinterpret_cast<vector3df*>(ptr)->~vector3df();
}

static void CopyConstructVector3(const vector3df& other, void* ptr)
{
	new(ptr) vector3df(other);
}

static void ConstructVector3FromFloats(float x, float y, float z, void* ptr)
{
	new(ptr) vector3df(x, y, z);
}

static void ConstructVector3FromVector2(const vector2df& other, float z, void* ptr)
{
	new(ptr) vector3df(other.X, other.Y, z);
}

static vector3df& AddAssignTwoVectors3(const vector3df& vec, vector3df& dest)
{
    dest += vec;

    return dest;
}

static vector3df& MinusAssignTwoVectors3(const vector3df& vec, vector3df& dest)
{
    dest -= vec;

    return dest;
}

static vector3df& MulAssignWithFloat3(float f, vector3df& dest)
{
    dest *= f;

    return dest;
}

static vector3df& DivAssignWithFloat3(float f, vector3df& dest)
{
    dest /= f;

    return dest;
}

static vector3df AddTwoVectors3(const vector3df& v, const vector3df& v2)
{
	return v + v2;
}

static vector3df MinusTwoVectors3(const vector3df& v, const vector3df& v2)
{
	return v - v2;
}

static vector3df MulVectorScalar3(const vector3df& v, float f)
{
	return v * f;
}

static vector3df DivVectorScalar3(const vector3df& v, float f)
{
	return v / f;
}

static bool VectorsEqual3(const vector3df& lval, const vector3df& rval)
{
	return lval == rval;
}

void ScriptBindings::RegisterVector3d(asIScriptEngine* engine)
{
    engine->RegisterObjectType("vector3d", sizeof(vector3df),
                               asOBJ_VALUE | asOBJ_APP_CLASS | asOBJ_APP_CLASS_CONSTRUCTOR | asOBJ_APP_CLASS_COPY_CONSTRUCTOR | asOBJ_APP_CLASS_DESTRUCTOR);

    engine->RegisterObjectBehaviour("vector3d", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructVector3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectBehaviour("vector3d", asBEHAVE_CONSTRUCT, "void f(const vector3d &in)", asFUNCTION(CopyConstructVector3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectBehaviour("vector3d", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(ConstructVector3FromFloats), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectBehaviour("vector3d", asBEHAVE_CONSTRUCT, "void f(vector2d, float)", asFUNCTION(ConstructVector3FromVector2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectBehaviour("vector3d", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructVector3), asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod("vector3d", "vector3d &opAssign(const vector3d &in)", asMETHODPR(vector3df, operator =, (const vector3df&), vector3df&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vector3d", "vector3d &opAddAssign(const vector3d &in)", asFUNCTION(AddAssignTwoVectors3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vector3d", "vector3d &opSubAssign(const vector3d &in)", asFUNCTION(MinusAssignTwoVectors3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vector3d", "vector3d &opMulAssign(float)", asFUNCTION(MulAssignWithFloat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vector3d", "vector3d &opDivAssign(float)", asFUNCTION(DivAssignWithFloat3), asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod("vector3d", "vector3d opAdd(const vector3d &in)", asFUNCTION(AddTwoVectors3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vector3d", "vector3d opSub(const vector3d &in)", asFUNCTION(MinusTwoVectors3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vector3d", "vector3d opMul(float)", asFUNCTION(MulVectorScalar3), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("vector3d", "vector3d opMul_r(float)", asFUNCTION(MulVectorScalar3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vector3d", "vector3d opDiv(float)", asFUNCTION(DivVectorScalar3), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("vector3d", "bool opEquals(const vector3d &in) const", asFUNCTION(VectorsEqual3), asCALL_CDECL_OBJFIRST);

    engine->RegisterObjectProperty("vector3d", "float x", asOFFSET(vector3df, X));
    engine->RegisterObjectProperty("vector3d", "float y", asOFFSET(vector3df, Y));
    engine->RegisterObjectProperty("vector3d", "float z", asOFFSET(vector3df, Z));
}
