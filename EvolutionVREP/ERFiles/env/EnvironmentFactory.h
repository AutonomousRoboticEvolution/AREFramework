#pragma once

#include <memory>
#include "Environment.h"


class EnvironmentFactory
{
public:
	EnvironmentFactory();
	~EnvironmentFactory();
    std::shared_ptr<Environment> createNewEnvironment(const std::shared_ptr<Settings>& st);
};

