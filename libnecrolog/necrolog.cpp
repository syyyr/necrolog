#include "necrolog.h"

NecroLog::Options &NecroLog::globalOptions()
{
	static Options global_options;
	return global_options;
}

NecroLog NecroLog::create(std::ostream &os, Level level, LogContext &&log_context)
{
	return NecroLog(os, level, std::move(log_context));
}

bool NecroLog::shouldLog(Level level, const LogContext &context)
{
	const std::map<std::string, Level> &tresholds = NecroLog::globalOptions().tresholds;
	if(tresholds.empty())
		return (level <= Level::Info); // default log level

	const char *ctx_topic = (context.topic && context.topic[0])? context.topic: context.file;
	for (size_t j = 0; ctx_topic[j]; ++j) {
		for(const auto &pair : tresholds) {
			const std::string &g_topic = pair.first;
			size_t i;
			//printf("%s vs %s\n", ctx_topic, g_topic.data());
			for (i = 0; i < g_topic.size() && ctx_topic[i+j]; ++i) {
				if(tolower(ctx_topic[i+j]) != g_topic[i])
					break;
			}
			if(i == g_topic.size())
				return (level <= pair.second);
		}
	}
	return false;
}

std::vector<std::string> NecroLog::setCLIOptions(int argc, char *argv[])
{
	using namespace std;
	std::vector<string> ret;
	Options& options = NecroLog::globalOptions();
	for(int i=1; i<argc; i++) {
		string s = argv[i];
		if(s == "-lfn" || s == "--log-long-file-names") {
			i++;
			options.logLongFileNames = true;
		}
		else if(s == "-d" || s == "-v" || s == "--verbose") {
			i++;
			string tresholds = (i < argc)? argv[i]: string();
			if(!tresholds.empty() && tresholds[0] == '-') {
				i--;
				tresholds = ":D";
			}
			{
				// split on ','
				size_t pos = 0;
				while(true) {
					size_t pos2 = tresholds.find_first_of(',', pos);
					string topic_level = (pos2 == string::npos)? tresholds.substr(pos): tresholds.substr(pos, pos2 - pos);
					if(!s.empty()) {
						auto ix = topic_level.find(':');
						Level level = Level::Debug;
						std::string topic = topic_level;
						if(ix != std::string::npos) {
							std::string s = topic_level.substr(ix + 1, 1);
							char l = s.empty()? 'D': toupper(s[0]);
							topic = topic_level.substr(0, ix);
							switch(l) {
							case 'D': level = Level::Debug; break;
							case 'W': level = Level::Warning; break;
							case 'E': level = Level::Error; break;
							case 'I':
							default: level = Level::Info; break;
							}
						}
						std::transform(topic.begin(), topic.end(), topic.begin(), ::tolower);
						options.tresholds[topic] = level;
					}
					if(pos2 == string::npos)
						break;
					pos = pos2 + 1;
				}
			}
		}
		else {
			ret.push_back(s);
		}
	}
	ret.insert(ret.begin(), argv[0]);
	return ret;
}

std::string NecroLog::tresholdsLogInfo()
{
	std::string ret;
	for (auto& kv : NecroLog::globalOptions().tresholds) {
		if(!ret.empty())
			ret += ',';
		ret += kv.first + ':';
		switch(kv.second) {
		case Level::Debug: ret += 'D'; break;
		case Level::Info: ret += 'I'; break;
		case Level::Warning: ret += 'W'; break;
		case Level::Error: ret += 'E'; break;
		case Level::Fatal: ret += 'F'; break;
		case Level::Invalid: ret += 'N'; break;
		default: ret += '?'; break;
		}
	}
	return ret;
}

std::string NecroLog::instantiationInfo()
{
	std::ostringstream ss;
	ss << "Instantiation info: ";
	ss << "globalOptions address: " << (void*)&(NecroLog::globalOptions().logLongFileNames);
	ss << ", cliHelp literal address: " << (void*)NecroLog::cliHelp();
	ss << ", cnt: " << ++NecroLog::globalOptions().cnt;
	return ss.str();
}


const char * NecroLog::cliHelp()
{
	static const char * ret =
		"-lfn, --log-long-file-names\n"
		"\tLog long file names\n"
		"-d, -v, --verbose [<pattern>]:[D|I|W|E]\n"
		"\tSet files or topics log treshold\n"
		"\tset treshold for all files or topics containing pattern to treshold D|I|W|E\n"
		"\twhen pattern is not set, set treshold for any filename or topic\n"
		"\twhen treshold is not set, set treshold D (Debug) for all files or topics containing pattern\n"
		"\twhen nothing is not set, set treshold D (Debug) for all files or topics\n"
		"\tExamples:\n"
		"\t\t-d\t\tset treshold D (Debug) for all files or topics\n"
		"\t\t-d :W\t\tset treshold W (Warning) for all files or topics\n"
		"\t\t-d foo,bar\t\tset treshold D for all files or topics containing 'foo' or 'bar'\n"
		"\t\t-d bar:W\tset treshold W (Warning) for all files or topics containing 'bar'\n"
		;
	return ret;
}
