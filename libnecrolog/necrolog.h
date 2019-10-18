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
	friend class NecroLogSharedData;
public:
	enum class Level : uint8_t {Invalid = 0, Fatal, Error, Warning, Info, Message, Debug};
	enum class Color : int8_t {Default = -1, Black=0, Red, Green, Brown, Blue, Magenta, Cyan, LightGray, Gray, LightRed, LightGreen, Yellow, LightBlue, LightMagenta, LightCyan, White};
	class NECROLOG_DECL_EXPORT LogContext
	{
	public:
		LogContext() : m_file(nullptr), m_line(0) {}
		LogContext(const char *file_name, int line_number, const char *topic)
			: m_topic(topic), m_file(file_name), m_line(line_number) {}

		const char *topic() const {return m_topic? m_topic: "";}
		const char *file() const {return m_file? m_file: "";}
		int line() const {return m_line;}

		bool isColorSet() const {return m_color != NecroLog::Color::Default;}
		NecroLog::Color color() const {return m_color;}
		void setColor(NecroLog::Color c) {m_color = c;}

		bool isTopicSet() const {return topic()[0];}
		bool isFileSet() const {return file()[0];}
	private:
		const char *m_topic = nullptr;
		const char *m_file = nullptr;
		int m_line = 0;
		NecroLog::Color m_color = NecroLog::Color::Default;
	};

	static const char* levelToString(Level level);

	using MessageHandler = std::function<void (Level level, const LogContext &context, const std::string &msg)>;
public:
	NecroLog(Level level, LogContext &&log_context)
	{
		if(level > Level::Invalid)
			m_necro = std::make_shared<NecroLogSharedData>(level, std::move(log_context));
	}

	template<typename T>
	NecroLog& operator<<(const T &v) {if(m_necro) *m_necro << v; return *this;}
	NecroLog& nospace() {if(m_necro) m_necro->setSpace(false); return *this;}
	NecroLog& color(NecroLog::Color c) {if(m_necro) m_necro->setColor(c); return *this;}

	static NecroLog create(Level level, LogContext &&log_context);
	static bool shouldLog(Level level, const LogContext &context);

	static MessageHandler setMessageHandler(MessageHandler h);

	static void defaultMessageHandler(Level level, const LogContext &context, const std::string &msg);
	static void writeWithDefaultFormat(std::ostream &os, bool is_colorized, Level level, const LogContext &context, const std::string &msg);
	static std::string moduleFromFileName(const char *file_name);

	static std::vector<std::string> setCLIOptions(int argc, char *argv[]);
	static std::string tresholdsLogInfo();
	static std::string topicsLogTresholds();
	static void setTopicsLogTresholds(const std::string &tresholds);
	static std::string fileLogTresholds();
	static void registerTopic(const std::string &topic, const std::string &info);
	static std::string registeredTopicsInfo();
	static const char * cliHelp();

private:
	struct Options
	{
		std::map<std::string, Level> fileTresholds;
		std::map<std::string, Level> topicTresholds;
		std::map<std::string, std::string> registeredTopics;
		bool logLongFileNames = false;
		MessageHandler messageHandler = defaultMessageHandler;
	};

	static Options& globalOptions();
	static int moduleNameStart(const char *file_name);
private:
	class NECROLOG_DECL_EXPORT NecroLogSharedData {
		friend class NecroLog;
	public:
		NecroLogSharedData(NecroLog::Level level, LogContext &&log_context);
		~NecroLogSharedData();

		void setSpace(bool b) {m_isSpace = b;}
		void setColor(NecroLog::Color c) {m_logContext.setColor(c);}

		template<typename T>
		void operator<<(const T &v) {if(m_level != NecroLog::Level::Invalid) {maybeSpace(); m_os << v;}}
	private:
		void maybeSpace() { if(m_isSpace && m_os.tellp() > 0) m_os << ' '; }
	private:
		std::ostringstream m_os;
		NecroLog::Level m_level;
		LogContext m_logContext;
		bool m_isSpace = true;
	};
private:
	std::shared_ptr<NecroLogSharedData> m_necro;
};

#ifdef NECROLOG_NO_DEBUG_LOG
#define nCDebug(topic) while(0) NecroLog::create(NecroLog::Level::Debug, NecroLog::LogContext(__FILE__, __LINE__, topic))
#else
#define nCDebug(topic) for(bool en = NecroLog::shouldLog(NecroLog::Level::Debug, NecroLog::LogContext(__FILE__, __LINE__, topic)); en; en = false) NecroLog::create(NecroLog::Level::Debug, NecroLog::LogContext(__FILE__, __LINE__, topic))
#endif
#define nCInfo(topic) for(bool en = NecroLog::shouldLog(NecroLog::Level::Info, NecroLog::LogContext(__FILE__, __LINE__, topic)); en; en = false) NecroLog::create(NecroLog::Level::Info, NecroLog::LogContext(__FILE__, __LINE__, topic))
#define nCMessage(topic) for(bool en = NecroLog::shouldLog(NecroLog::Level::Message, NecroLog::LogContext(__FILE__, __LINE__, topic)); en; en = false) NecroLog::create(NecroLog::Level::Message, NecroLog::LogContext(__FILE__, __LINE__, topic))
#define nCWarning(topic) for(bool en = NecroLog::shouldLog(NecroLog::Level::Warning, NecroLog::LogContext(__FILE__, __LINE__, topic)); en; en = false) NecroLog::create(NecroLog::Level::Warning, NecroLog::LogContext(__FILE__, __LINE__, topic))
#define nCError(topic) for(bool en = NecroLog::shouldLog(NecroLog::Level::Error, NecroLog::LogContext(__FILE__, __LINE__, topic)); en; en = false) NecroLog::create(NecroLog::Level::Error, NecroLog::LogContext(__FILE__, __LINE__, topic))

#define nDebug() nCDebug("")
#define nInfo() nCInfo("")
#define nWarning() nCWarning("")
#define nError() nCError("")

#ifdef NECROLOG_NO_DEBUG_LOG
#define nLogFuncFrame() while(0) nDebug()
#else
#define nLogFuncFrame() NecroLog __necrolog_func_frame_exit_logger__ = NecroLog::shouldLog(NecroLog::Level::Debug, NecroLog::LogContext(__FILE__, __LINE__, ""))? \
NecroLog::create(NecroLog::Level::Debug, NecroLog::LogContext(__FILE__, __LINE__, "")) << "     EXIT FN" << __FUNCTION__: \
NecroLog::create(NecroLog::Level::Invalid, NecroLog::LogContext(__FILE__, __LINE__, "")); \
nDebug() << ">>>> ENTER FN" << __FUNCTION__
#endif
