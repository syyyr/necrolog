#include "necrolog.h"

#include <numeric>
#include <vector>

#define nFooInfo() nCInfo("foo")
#define nBarDebug() nCDebug("bar")

inline NecroLog &operator<<(NecroLog log, const std::vector<std::string> &sl)
{
	std::string s = std::accumulate(sl.begin(), sl.end(), std::string(),
										  [](const std::string& a, const std::string& b) -> std::string {
											  return a + (a.length() > 0 ? "," : "") + b;
										  } );
	return log << s;
}

int main(int argc, char *argv[])
{
	std::vector<std::string> args = NecroLog::setCLIOptions(argc, argv);

	if(std::find(args.begin(), args.end(), "--help") != args.end()) {
		std::cout << NecroLog::cliHelp();
		exit(0);
	}

	nDebug() << "Debug mesage";
	nInfo() << "Info mesage";
	nWarning() << "Warning mesage";
	nError() << "Error mesage";

	nInfo() << "args not used by necro log:" << args;

	nFooInfo() << "Info mesage" << "foo" << "topic";
	nBarDebug() << "Debug mesage" << "bar" << "topic";
	return 0;
}
