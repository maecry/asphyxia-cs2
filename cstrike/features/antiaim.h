#pragma once
#include "../sdk/entity.h"
#include "../sdk/interfaces/cgameentitysystem.h"
#include "../sdk/interfaces/iengineclient.h"
#include "../sdk/interfaces/ccsgoinput.h"

#include "../core/sdk.h"
#include "../core/interfaces.h"

#include "../sdk/datatypes/usercmd.h"

namespace antiaim
{
	void RunAA(CUserCmd* pCmd);
}
