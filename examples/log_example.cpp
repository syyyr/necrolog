#include <necrolog.h>

#include <numeric>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iostream>

#define nFooInfo() nCInfo("foo")
#define nBarDebug() nCDebug("bar")

using namespace std;

inline NecroLog &operator<<(NecroLog &log, const std::vector<std::string> &sl)
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
	nInfo() << "args not used by necro log:" << args;
	nInfo() << "defined log thresholds:" << NecroLog::thresholdsLogInfo();

	nDebug() << "Debug mesage";
	nMessage() << "Message mesage";
	nInfo() << "Info mesage";
	nWarning() << "Warning mesage";
	nError() << "Error mesage";

	nInfo().color(NecroLog::Color::Green) << "Info in custom green color";

	nFooInfo() << "Topic 'foo' info";
	nBarDebug() << "Topic 'bar' debug";

	const std::vector<std::string> sl{"foo", "bar", "baz"};
	nInfo() << "Logging custom type (string list):" << sl;

	return 0;
}
