#include "EnvironmentFactory.h"

#include <iostream>
#include <memory>
#include "MoveEnv.h"
#include "RoughMoveEnv.h"
#include "EnvPhototaxis.h"

using namespace std;

EnvironmentFactory::EnvironmentFactory()
{
}

EnvironmentFactory::~EnvironmentFactory()
{
}

shared_ptr<Environment> EnvironmentFactory::createNewEnvironment(const shared_ptr<Settings>& st)
{

	if (st->environmentType == st->DEFAULT_ENV) {
		shared_ptr<Environment> moveEnv(new MoveEnv);
		moveEnv->settings = st;
		return moveEnv;
	}
	else if (st->environmentType == st->ROUGH) {
		shared_ptr<Environment> roughMoveEnv(new RoughMoveEnv);
		roughMoveEnv->settings = st;
		return roughMoveEnv;
	}
	else if (st->environmentType == st->ENV_PHOTOTAXIS) {
		shared_ptr<Environment> env(new EnvPhototaxis);
		env->settings = st;
		return env;
	}

	return nullptr;
}
