#include "bar.h"

#include <necrolog.h>

#define nBarDebug() nCDebug("bar")

Bar::Bar()
{
	nBarDebug() << __FUNCTION__;
	nInfo() << NecroLog::tresholdsLogInfo();
	nInfo() << NecroLog::instantiationInfo();
}
