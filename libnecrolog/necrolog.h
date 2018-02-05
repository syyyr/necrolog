#pragma once

#include "necrologglobal.h"

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <iostream>

class NECROLOG_DECL_EXPORT NecroLog
{
	friend class Necro;
public:
	enum class Level : uint8_t {Invalid = 0, Fatal, Error, Warning, Info, Debug};
	class NECROLOG_DECL_EXPORT LogContext
	{
	public:
		LogContext() : file(nullptr), line(0) {}
		LogContext(const char *file_name, int line_number, const char *topic)
			: topic(topic), file(file_name), line(line_number) {}

		const char *topic;
		const char *file;
		int line;
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
	static const char * cliHelp();
private:
	struct Options
	{
		std::map<std::string, Level> fileTresholds;
		std::map<std::string, Level> topicTresholds;
		bool logLongFileNames = false;
	};

	static Options& globalOptions();
	static int moduleNameStart(const char *file_name);
	static std::string moduleFromFileName(const char *file_name);
private:
	class NECROLOG_DECL_EXPORT Necro {
		friend class NecroLog;
	public:
		Necro(std::ostream &os, NecroLog::Level level, LogContext &&log_context);
		~Necro();

		void setSpace(bool b) {m_isSpace = b;}

		template<typename T>
		void operator<<(const T &v) {if(m_level != NecroLog::Level::Invalid) {maybeSpace(); m_os << v;}}

	private:
		void maybeSpace();

		enum TTYColor {Black=0, Red, Green, Yellow, Blue, Magenta, Cyan, White};

		std::ostream& setTtyColor(TTYColor color, bool bright = false, bool bg_color = false);
		void prolog();
		void epilog();
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
