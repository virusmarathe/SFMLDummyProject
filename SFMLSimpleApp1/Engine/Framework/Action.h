#pragma once

#include <string>
#include "Math/Vector2.h"

struct Action
{
	enum class ActionType
	{
		START,
		END
	};

	Action(std::string name, ActionType type, Vector2 pos) : name(name), type(type), pos(pos) { }

	std::string name;
	ActionType type;
	Vector2 pos;
};