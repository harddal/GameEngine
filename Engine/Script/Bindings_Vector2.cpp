#include "Bindings.h"

#include "irrlicht.h"

using namespace irr::core;

static void ConstructVector2(void* ptr)
{
	new(ptr) vector2df();
}

static void DestructVector2(void* ptr)
{
	reinterpret_cast<vector2df*>(ptr)->~vector2df();
}

static void CopyConstructVector2(const vector2df& other, void* ptr)
{
	new(ptr) vector2df(other);
}

static void ConstructVector2FromFloats(float x, float y, void* ptr)
{
	new(ptr) vector2df(x, y);
}

static vector2df& AddAssignTwoVectors2(const vector2df& vec, vector2df& dest)
{
    dest += vec;

    return dest;
}

static vector2df& MinusAssignTwoVectors2(const vector2df& vec, vector2df& dest)
{
    dest -= vec;

    return dest;
}

static vector2df& MulAssignWithFloat2(float f, vector2df& dest)
{
    dest *= f;

    return dest;
}

static vector2df& DivAssignWithFloat2(float f, vector2df& dest)
{
    dest /= f;

    return dest;
}

static vector2df AddTwoVectors2(const vector2df& v, const vector2df& v2)
{
	return v + v2;
}

static vector2df MinusTwoVectors2(const vector2df& v, const vector2df& v2)
{
	return v - v2;
}

static vector2df MulVectorScalar2(const vector2df& v, float f)
{
	return v * f;
}

static vector2df DivVectorScalar2(const vector2df& v, float f)
{
	return v / f;
}

static bool VectorsEqual2(const vector2df& lval, const vector2df& rval)
{
	return lval == rval;
}

void ScriptBindings::RegisterVector2d(asIScriptEngine* engine)
{
    engine->RegisterObjectType("vector2d", sizeof(vector2df),
                               asOBJ_VALUE | asOBJ_APP_CLASS | asOBJ_APP_CLASS_CONSTRUCTOR | asOBJ_APP_CLASS_COPY_CONSTRUCTOR | asOBJ_APP_CLASS_DESTRUCTOR);

    engine->RegisterObjectBehaviour("vector2d", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructVector2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectBehaviour("vector2d", asBEHAVE_CONSTRUCT, "void f(const vector2d &in)", asFUNCTION(CopyConstructVector2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectBehaviour("vector2d", asBEHAVE_CONSTRUCT, "void f(float, float)", asFUNCTION(ConstructVector2FromFloats), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectBehaviour("vector2d", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructVector2), asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod("vector2d", "vector2d &opAssign(const vector2d &in)", asMETHODPR(vector2df, operator =, (const vector2df&), vector2df&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vector2d", "vector2d &opAddAssign(const vector2d &in)", asFUNCTION(AddAssignTwoVectors2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vector2d", "vector2d &opSubAssign(const vector2d &in)", asFUNCTION(MinusAssignTwoVectors2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vector2d", "vector2d &opMulAssign(float)", asFUNCTION(MulAssignWithFloat2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vector2d", "vector2d &opDivAssign(float)", asFUNCTION(DivAssignWithFloat2), asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod("vector2d", "vector2d opAdd(const vector2d &in)", asFUNCTION(AddTwoVectors2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vector2d", "vector2d opSub(const vector2d &in)", asFUNCTION(MinusTwoVectors2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vector2d", "vector2d opMul(float)", asFUNCTION(MulVectorScalar2), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("vector2d", "vector2d opMul_r(float)", asFUNCTION(MulVectorScalar2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vector2d", "vector2d opDiv(float)", asFUNCTION(DivVectorScalar2), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("vector2d", "bool opEquals(const vector2d &in) const", asFUNCTION(VectorsEqual2), asCALL_CDECL_OBJFIRST);

    engine->RegisterObjectProperty("vector2d", "float x", asOFFSET(vector2df, X));
    engine->RegisterObjectProperty("vector2d", "float y", asOFFSET(vector2df, Y));
}
