#include "Bindings.h"

#include "Engine/Engine.h"

static void AS_PlayEntitySound(int e, std::string sound)
{
		auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

		if (!entity.isValid())
		{
			return;
		}

		if (entity.hasComponent<SoundComponent>()) 
		{
			entity.getComponent<SoundComponent>().play(sound);
		}
}

void ScriptBindings::RegisterSound(asIScriptEngine* engine)
{
	engine->SetDefaultNamespace("sound");
	{
		engine->RegisterGlobalFunction("void play(int, string)", asFUNCTION(AS_PlayEntitySound), asCALL_CDECL);
	}

	engine->SetDefaultNamespace("");
}