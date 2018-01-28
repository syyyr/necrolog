#include "foo.h"

#include <necrolog.h>

#define nFooInfo() nCInfo("foo")

Foo::Foo()
{
	nFooInfo() << __FUNCTION__;
	nInfo() << NecroLog::tresholdsLogInfo();
	nInfo() << (void*)NecroLog::cliHelp();
}
