#pragma once

#include "necrologglobal.h"

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <sstream>
#include <functional>

class NECROLOG_DECL_EXPORT NecroLog
{
	friend class Necro;
public:
	enum class Level : uint8_t {Invalid = 0, Fatal, Error, Warning, Info, Debug};
	class NECROLOG_DECL_EXPORT LogContext
	{
	public:
		LogContext() : m_file(nullptr), m_line(0) {}
		LogContext(const char *file_name, int line_number, const char *topic)
			: m_topic(topic), m_file(file_name), m_line(line_number) {}

		const char *topic() const {return m_topic? m_topic: "";}
		const char *file() const {return m_file? m_file: "";}
		int line() const {return m_line;}

		bool isTopicSet() const {return topic()[0];}
		bool isFileSet() const {return file()[0];}
	private:
		const char *m_topic = nullptr;
		const char *m_file = nullptr;
		int m_line = 0;
	};

	static const char* levelToString(Level level);

	using MessageHandler = std::function<void (Level level, const LogContext &context, std::string &&msg)>;
public:
	NecroLog(Level level, LogContext &&log_context)
	{
		if(level > Level::Invalid)
			m_necro = std::make_shared<Necro>(level, std::move(log_context));
	}

	template<typename T>
	NecroLog& operator<<(const T &v) {if(m_necro) *m_necro << v; return *this;}
	NecroLog& nospace() {if(m_necro) m_necro->setSpace(false); return *this;}

	static NecroLog create(Level level, LogContext &&log_context);
	static bool shouldLog(Level level, const LogContext &context);

	static MessageHandler setMessageHandler(MessageHandler h);

	static std::vector<std::string> setCLIOptions(int argc, char *argv[]);
	static std::string tresholdsLogInfo();
	static const char * cliHelp();

private:
	static void defaultMessageHandler(Level level, const LogContext &context, std::string &&msg);
	struct Options
	{
		std::map<std::string, Level> fileTresholds;
		std::map<std::string, Level> topicTresholds;
		bool logLongFileNames = false;
		MessageHandler messageHandler = defaultMessageHandler;
	};

	static Options& globalOptions();
	static int moduleNameStart(const char *file_name);
	static std::string moduleFromFileName(const char *file_name);
private:
	class NECROLOG_DECL_EXPORT Necro {
		friend class NecroLog;
	public:
		Necro(NecroLog::Level level, LogContext &&log_context);
		~Necro();

		void setSpace(bool b) {m_isSpace = b;}

		template<typename T>
		void operator<<(const T &v) {if(m_level != NecroLog::Level::Invalid) {maybeSpace(); m_os << v;}}
	private:
		void maybeSpace();
	private:
		std::ostringstream m_os;
		NecroLog::Level m_level;
		LogContext m_logContext;
		bool m_isSpace = true;
	};
private:
	std::shared_ptr<Necro> m_necro;
};

#define nCDebug(topic) for(bool en = NecroLog::shouldLog(NecroLog::Level::Debug, NecroLog::LogContext(__FILE__, __LINE__, topic)); en; en = false) NecroLog::create(NecroLog::Level::Debug, NecroLog::LogContext(__FILE__, __LINE__, topic))
#define nCInfo(topic) for(bool en = NecroLog::shouldLog(NecroLog::Level::Info, NecroLog::LogContext(__FILE__, __LINE__, topic)); en; en = false) NecroLog::create(NecroLog::Level::Info, NecroLog::LogContext(__FILE__, __LINE__, topic))
#define nCWarning(topic) for(bool en = NecroLog::shouldLog(NecroLog::Level::Warning, NecroLog::LogContext(__FILE__, __LINE__, topic)); en; en = false) NecroLog::create(NecroLog::Level::Warning, NecroLog::LogContext(__FILE__, __LINE__, topic))
#define nCError(topic) for(bool en = NecroLog::shouldLog(NecroLog::Level::Error, NecroLog::LogContext(__FILE__, __LINE__, topic)); en; en = false) NecroLog::create(NecroLog::Level::Error, NecroLog::LogContext(__FILE__, __LINE__, topic))

#ifdef NECROLOG_NO_DEBUG_LOG
#define nDebug() while(0) nCDebug("")
#else
#define nDebug() nCDebug("")
#endif
#define nInfo() nCInfo("")
#define nWarning() nCWarning("")
#define nError() nCError("")

#ifdef NECROLOG_NO_DEBUG_LOG
#define nLogFuncFrame() while(0) nDebug()
#else
#define nLogFuncFrame() NecroLog __necrolog_func_frame_exit_logger__ = NecroLog::shouldLog(NecroLog::Level::Debug, NecroLog::LogContext(__FILE__, __LINE__, ""))? \
NecroLog::create(NecroLog::Level::Debug, NecroLog::LogContext(__FILE__, __LINE__, "")) << "     EXIT FN" << __FUNCTION__: \
NecroLog::create(NecroLog::Level::Invalid, NecroLog::LogContext(__FILE__, __LINE__, "")); \
shvDebug() << ">>>> ENTER FN" << __FUNCTION__
#endif
