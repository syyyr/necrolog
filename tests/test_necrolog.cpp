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

void log_case(const string &case_name, const vector<string> &params)
{
	cerr << "--------------------------------------------" << endl;
	cerr << case_name << endl;
	cerr << "params: ";
	for(const auto &s : params)
		cerr << s << ' ';
	cerr << endl;
	std::vector<std::string> args = NecroLog::setCLIOptions(params);
	cerr << "tresholds parsed: " << NecroLog::thresholdsLogInfo() << endl;
	if(!args.empty()) {
		cerr << "args not used by necro log: ";
		for(const auto &s : args)
			cerr << s << ' ';
		cerr << endl;
	}
	cerr << "--------------------------------------------" << endl;

	nDebug() << "Debug mesage";
	nMessage() << "Message mesage";
	nInfo() << "Info mesage";
	nWarning() << "Warning mesage";
	nError() << "Error mesage";

	nCDebug("Foo") << "Debug mesage - topic 'Foo'";
	nCMessage("Foo") << "Message mesage - topic 'Foo'";
	nCInfo("Foo") << "Info mesage - topic 'Foo'";
	nCWarning("Foo") << "Warning mesage - topic 'Foo'";
	nCError("Foo") << "Error mesage - topic 'Foo'";

	nCDebug("Bar") << "Debug mesage - topic 'Bar'";
	nCMessage("Bar") << "Message mesage - topic 'Bar'";
	nCInfo("Bar") << "Info mesage - topic 'Bar'";
	nCWarning("Bar") << "Warning mesage - topic 'Bar'";
	nCError("Bar") << "Error mesage - topic 'Bar'";
}

tuple<string, vector<string>> make_args(const string &params, const string &msg)
{
	istringstream iss(params);
	vector<string> tokens{istream_iterator<string>{iss}, istream_iterator<string>{}};
	return make_tuple(msg, tokens);
}

int main(int argc, char *argv[])
{
	std::vector<std::string> args = NecroLog::setCLIOptions(argc, argv);

	for(const auto &[msg, v] : {
		make_args("foo ", "You should see INFO+ for messages without topic set and WARN+ for topic ones"),
		make_args("foo -v", "You should see all messages"),
		make_args("foo -v :D", "You should see all messages"),
		make_args("foo -v :M", "You should see MSG+ for all messages"),
		make_args("foo -v :I", "You should see INFO+ for all messages"),
		make_args("foo -v :W", "You should see WARN+ for all messages"),
		make_args("foo -v :E", "You should see ERR+ for all messages"),
		make_args("foo -v foo:D", "You should see INFO+ for messages without topic, WARN+ for ones with topic, DEB+ for messages with topic or module name starting with 'foo'"),
		make_args("foo -v foo,bar:W", "You should see INFO+ for messages without topic, WARN+ for ones with topic, DEB+ for messages with topic or module name starting with 'foo', WARN+ for messages with topic or module name starting with 'bar'"),
		make_args("foo -v foo,bar:W", "You should see INFO+ for messages without topic set, DEB+ for ones with topic 'foo' or module name 'foo.cpp', WARN+ for ones with topic or module name starting with 'bar'"),
		make_args("foo -v foo:E", "You should see INFO+ for messages without topic set and DEB+ for ones with topic 'foo' or module name 'foo.cpp'"),
		make_args("foo -v test_necr:D", "You should see INFO+ for messages without topic, WARN+ for ones with topic, DEB+ for messages with topic or module name starting with 'test_necr'"),
		make_args("foo -v test_necr:E", "You should see INFO+ for messages without topic, WARN+ for ones with topic, ERR+ for messages with topic or module name starting with 'test_necr'"),
	}) {
		log_case(msg, v);
	}

	return 0;
}
