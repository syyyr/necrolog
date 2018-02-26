#include "necrolog.h"

#include <ctime>
#include <algorithm>
#include <iostream>

#ifdef __unix
#include <unistd.h>
#endif

NecroLog::Options &NecroLog::globalOptions()
{
	static Options global_options;
	return global_options;
}

NecroLog NecroLog::create(Level level, LogContext &&log_context)
{
	return NecroLog(level, std::move(log_context));
}

bool NecroLog::shouldLog(Level level, const LogContext &context)
{
	Options &opts = NecroLog::globalOptions();

	const bool topic_set = (context.isTopicSet());
	if(!topic_set && opts.fileTresholds.empty())
		return level <= Level::Info; // when tresholds are not set, log non-topic INFO messages

	const char *searched_str = "";
	if(topic_set) {
		searched_str = context.topic();
	}
	else {
		searched_str = context.file();
		if(!opts.logLongFileNames) {
			int ix = moduleNameStart(searched_str);
			if(ix >= 0)
				searched_str += ix + 1;
		}
	}

	const std::map<std::string, Level> &tresholds = topic_set? opts.topicTresholds: opts.fileTresholds;
	for(const auto &pair : tresholds) {
		const std::string &needle = pair.first;
		for (size_t j = 0; searched_str[j]; ++j) {
			size_t i;
			//printf("%s vs %s\n", ctx_topic, g_topic.data());
			for (i = 0; i < needle.size()  && searched_str[i+j]; ++i) {
				if(tolower(searched_str[i+j]) != needle[i])
					break;
			}
			if(i == needle.size())
				return (level <= pair.second);
		}
	}
	// not found in tresholds
	if(!topic_set)
		return level <= Level::Info; // log non-topic INFO messages
	return false;
}

NecroLog::MessageHandler NecroLog::setMessageHandler(NecroLog::MessageHandler h)
{
	MessageHandler ret = globalOptions().messageHandler;
	globalOptions().messageHandler = h;
	return ret;
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
			bool use_topics = (s != "-d");
			i++;
			string tresholds = (i < argc)? argv[i]: string();
			if(tresholds.empty() || (!tresholds.empty() && tresholds[0] == '-')) {
				i--;
				tresholds = ":D";
			}
			{
				// split on ','
				size_t pos = 0;
				while(true) {
					size_t pos2 = tresholds.find_first_of(',', pos);
					string topic_level = (pos2 == string::npos)? tresholds.substr(pos): tresholds.substr(pos, pos2 - pos);
					if(!topic_level.empty()) {
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
						if(use_topics)
							options.topicTresholds[topic] = level;
						else
							options.fileTresholds[topic] = level;
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

static std::string tersholds_to_string(const std::map<std::string, NecroLog::Level> &tresholds)
{
	std::string ret;
	for (auto& kv : tresholds) {
		if(!ret.empty())
			ret += ',';
		ret += kv.first + ':';
		switch(kv.second) {
		case NecroLog::Level::Debug: ret += 'D'; break;
		case NecroLog::Level::Info: ret += 'I'; break;
		case NecroLog::Level::Warning: ret += 'W'; break;
		case NecroLog::Level::Error: ret += 'E'; break;
		case NecroLog::Level::Fatal: ret += 'F'; break;
		case NecroLog::Level::Invalid: ret += 'N'; break;
		default: ret += '?'; break;
		}
	}
	return ret;
}

std::string NecroLog::tresholdsLogInfo()
{
	std::string ret;
	Options &opts = NecroLog::globalOptions();
	ret += "-d ";
	if(opts.fileTresholds.empty())
		ret += ":I";
	else
		ret += tersholds_to_string(opts.fileTresholds);
	if(!opts.topicTresholds.empty()) {
		ret += " -v ";
		ret += tersholds_to_string(opts.topicTresholds);
	}
	return ret;
}

const char * NecroLog::cliHelp()
{
	static const char * ret =
		"-lfn, --log-long-file-names\n"
		"\tLog long file names\n"
		"-d [<pattern>]:[D|I|W|E] set file name log treshold\n"
		"-v, --topic [<pattern>]:[D|I|W|E] set topic log treshold\n"
		"\tSet log treshold\n"
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

NecroLog::Necro::Necro(NecroLog::Level level, NecroLog::LogContext &&log_context)
	: m_level(level)
	, m_logContext(std::move(log_context))
{
}

NecroLog::Necro::~Necro()
{
	auto h = globalOptions().messageHandler;
	if(h)
		h(m_level, m_logContext, m_os.str());
}

void NecroLog::Necro::maybeSpace()
{
	if(m_isSpace && m_os.tellp() > 0) {
		m_os << ' ';
	}
}

int NecroLog::moduleNameStart(const char *file_name)
{
	char sep = '/';
#ifndef __unix
	//sep = '\\'; mingw use '/' even on windows
#endif
	int ret = -1;
	if(file_name) {
		for (int ix = 0; file_name[ix]; ++ix) {
			if(file_name[ix] == sep)
				ret = ix+1;
		}
	}
	return ret;
}

std::string NecroLog::moduleFromFileName(const char *file_name)
{
	if(NecroLog::globalOptions().logLongFileNames)
		return std::string(file_name);

	int ix = moduleNameStart(file_name);
	if(ix < 0)
		return std::string(file_name);
	return std::string(file_name + ix);
}

namespace {

enum TTYColor {Black=0, Red, Green, Yellow, Blue, Magenta, Cyan, White};

static int is_TTI = -1;

std::ostream & set_TTY_color(std::ostream &os, TTYColor color, bool bright, bool bg_color = false)
{
	if(is_TTI)
		os << "\033[" << (bright? '1': '0') << ';' << (bg_color? '4': '3') << char('0' + color) << 'm';
	return os;
}

}

void NecroLog::defaultMessageHandler(NecroLog::Level level, const NecroLog::LogContext &context, std::string &&msg)
{
	if(is_TTI < 0) {
#ifdef __unix
	is_TTI = ::isatty(STDERR_FILENO);
#else
	is_TTI = 0;
#endif
	}

	std::ostream &os = std::clog;

	std::time_t t = std::time(nullptr);
	std::tm *tm = std::gmtime(&t); /// gmtime is not thread safe!!!
	char buffer[80] = {0};
	std::strftime(buffer, sizeof(buffer),"%Y-%m-%dT%H:%M:%S", tm);
	set_TTY_color(os, TTYColor::Green, false) << std::string(buffer);
	set_TTY_color(os, TTYColor::Yellow, false) << '[' << moduleFromFileName(context.file()) << ':' << context.line() << "]";
	if(context.isTopicSet()) {
		set_TTY_color(os, TTYColor::White, true) << '(' << context.topic() << ")";
	}
	switch(level) {
	case NecroLog::Level::Fatal:
		set_TTY_color(os, TTYColor::Red, true) << "|F|";
		break;
	case NecroLog::Level::Error:
		set_TTY_color(os, TTYColor::Red, true) << "|E|";
		break;
	case NecroLog::Level::Warning:
		set_TTY_color(os, TTYColor::Magenta, true) << "|W|";
		break;
	case NecroLog::Level::Info:
		set_TTY_color(os, TTYColor::Cyan, true) << "|I|";
		break;
	case NecroLog::Level::Debug:
		set_TTY_color(os, TTYColor::White, false) << "|D|";
		break;
	default:
		set_TTY_color(os, TTYColor::Red, true) << "|?|";
		break;
	};
	os << " ";

	os << msg;

	if(is_TTI)
		os << "\33[0m";

	os << std::endl;
	os.flush();
}

