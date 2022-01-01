#pragma once

#include <string>

struct Action
{
	enum class ActionType
	{
		START,
		END
	};

	Action(std::string name, ActionType type) : name(name), type(type) { }

	std::string name;
	ActionType type;
};