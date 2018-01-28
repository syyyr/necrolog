#pragma once

#include "necrologglobal.h"

#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <sstream>

#ifdef __unix
#include <unistd.h>
#endif

class NECROLOG_DECL_EXPORT NecroLog
{
	friend class Necro;
public:
	enum class Level {Invalid = 0, Fatal, Error, Warning, Info, Debug};
	class NECROLOG_DECL_EXPORT LogContext
	{
	public:
		LogContext() : file(nullptr), line(0) {}
		LogContext(const char *file_name, int line_number, const char *topic)
			: file(file_name), line(line_number), topic(topic) {}

		const char *file;
		int line;
		const char *topic;
	};
public:
	NecroLog(std::ostream &os, Level level, LogContext &&log_context)
	{
		m_necro = std::make_shared<Necro>(os, level, std::move(log_context));
	}

	template<typename T>
	NecroLog& operator<<(const T &v) {*m_necro << v; return *this;}
	NecroLog& nospace() {m_necro->setSpace(false); return *this;}

	static NecroLog create(std::ostream &os, Level level, LogContext &&log_context);
	static bool shouldLog(Level level, const LogContext &context);
	static std::vector<std::string> setCLIOptions(int argc, char *argv[]);
	static std::string tresholdsLogInfo();
	static std::string instantiationInfo();
	static const char * cliHelp();
private:
	struct Options
	{
		std::map<std::string, Level> tresholds;
		bool logLongFileNames = false;
		int cnt = 0;
	};

	static Options& globalOptions();
private:
	class Necro {
		friend class NecroLog;
	public:
		Necro(std::ostream &os, NecroLog::Level level, LogContext &&log_context)
			: m_os(os)
			, m_level(level)
			, m_logContext(std::move(log_context))
		{
#ifdef __unix
			m_isTTI = true;//(&m_os == &std::clog) && ::isatty(STDERR_FILENO);
#endif
		}
		~Necro()
		{
			epilog();
			m_os << std::endl;
			m_os.flush();
		}

		void setSpace(bool b) {m_isSpace = b;}

		template<typename T>
		void operator<<(const T &v) {if(m_level != NecroLog::Level::Invalid) {maybeSpace(); m_os << v;}}

	private:
		void maybeSpace()
		{
			if(m_firstRun) {
				m_firstRun = false;
				prolog();
			}
			else {
				if(m_isSpace) {
					m_os << ' ';
				}
			}
		}

		std::string moduleFromFileName(const char *file_name)
		{
			if(NecroLog::globalOptions().logLongFileNames)
				return std::string(file_name);

			std::string ret(file_name);
			auto ix = ret.find_last_of('/');
#ifndef __unix
			if(ix == std::string::npos)
				ix = ret.find_last_of('\\');
#endif
			if(ix != std::string::npos)
				ret = ret.substr(ix + 1);
			return ret;
		}

		enum TTYColor {Black=0, Red, Green, Yellow, Blue, Magenta, Cyan, White};

		std::ostream& setTtyColor(TTYColor color, bool bright = false, bool bg_color = false)
		{
			if(m_isTTI)
				m_os << "\033[" << (bright? '1': '0') << ';' << (bg_color? '4': '3') << char('0' + color) << 'm';
			return m_os;
		}
		void prolog()
		{
			std::time_t t = std::time(nullptr);
			std::tm *tm = std::gmtime(&t); /// gmtime is not thread safe!!!
			char buffer[80] = {0};
			std::strftime(buffer, sizeof(buffer),"%Y-%m-%dT%H:%M:%S", tm);
			setTtyColor(TTYColor::Green, false) << std::string(buffer);
			setTtyColor(TTYColor::Yellow, false) << '[' << moduleFromFileName(m_logContext.file) << ':' << m_logContext.line << "]";
			if(m_logContext.topic && m_logContext.topic[0]) {
				setTtyColor(TTYColor::White, true) << '(' << m_logContext.topic << ")";
			}
			switch(m_level) {
			case NecroLog::Level::Fatal:
				setTtyColor(TTYColor::Red, true) << "|F|";
				break;
			case NecroLog::Level::Error:
				setTtyColor(TTYColor::Red, true) << "|E|";
				break;
			case NecroLog::Level::Warning:
				setTtyColor(TTYColor::Magenta, true) << "|W|";
				break;
			case NecroLog::Level::Info:
				setTtyColor(TTYColor::Cyan, true) << "|I|";
				break;
			case NecroLog::Level::Debug:
				setTtyColor(TTYColor::White, false) << "|D|";
				break;
			default:
				setTtyColor(TTYColor::Red, true) << "|?|";
				break;
			};
			m_os << " ";
		}
		void epilog()
		{
			if(m_isTTI)
				m_os << "\33[0m";
		}
	private:
		std::ostream &m_os;
		NecroLog::Level m_level;
		LogContext m_logContext;
		bool m_isSpace = true;
		bool m_firstRun = true;
		bool m_isTTI = false;
	};
private:
	std::shared_ptr<Necro> m_necro;
};

#ifdef NECROLOG_NO_DEBUG_LOG
#define nCDebug(topic) while(false) NecroLog::create(std::clog, NecroLog::Level::Debug, NecroLog::LogContext(__FILE__, __LINE__, topic))
#else
#define nCDebug(topic) for(bool en = NecroLog::shouldLog(NecroLog::Level::Debug, NecroLog::LogContext(__FILE__, __LINE__, topic)); en; en = false) NecroLog::create(std::clog, NecroLog::Level::Debug, NecroLog::LogContext(__FILE__, __LINE__, topic))
#endif
#define nCInfo(topic) for(bool en = NecroLog::shouldLog(NecroLog::Level::Info, NecroLog::LogContext(__FILE__, __LINE__, topic)); en; en = false) NecroLog::create(std::clog, NecroLog::Level::Info, NecroLog::LogContext(__FILE__, __LINE__, topic))
#define nCWarning(topic) for(bool en = NecroLog::shouldLog(NecroLog::Level::Warning, NecroLog::LogContext(__FILE__, __LINE__, topic)); en; en = false) NecroLog::create(std::clog, NecroLog::Level::Warning, NecroLog::LogContext(__FILE__, __LINE__, topic))
#define nCError(topic) for(bool en = NecroLog::shouldLog(NecroLog::Level::Error, NecroLog::LogContext(__FILE__, __LINE__, topic)); en; en = false) NecroLog::create(std::clog, NecroLog::Level::Error, NecroLog::LogContext(__FILE__, __LINE__, topic))

#define nDebug() nCDebug("")
#define nInfo() nCInfo("")
#define nWarning() nCWarning("")
#define nError() nCError("")

#ifdef NECROLOG_NO_DEBUG_LOG
#define nLogFuncFrame() while(0) nDebug()
#else
#define nLogFuncFrame() NecroLog __necrolog_func_frame_exit_logger__ = NecroLog::shouldLog(NecroLog::Level::Debug, NecroLog::LogContext(__FILE__, __LINE__, ""))? \
NecroLog::create(std::clog, NecroLog::Level::Debug, NecroLog::LogContext(__FILE__, __LINE__, "")) << "     EXIT FN" << __FUNCTION__: \
NecroLog::create(std::clog, NecroLog::Level::Invalid, NecroLog::LogContext(__FILE__, __LINE__, "")); \
shvDebug() << ">>>> ENTER FN" << __FUNCTION__
#endif
