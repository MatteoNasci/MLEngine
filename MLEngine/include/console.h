#ifndef CONSOLE_H
#define CONSOLE_H

#include <mlengine_global.h>

#include <timemanager.h>

#include <string>
#include <functional>
#include <vector>
#include <map>
#include <future>
#include <mutex>

namespace mle
{
enum class LogClassification{
    Normal = 0,
    Warning,
    Error,
    Critical,
    Fatal,
    Command,
    Info,
};

class MLENGINE_SHARED_EXPORT Console{
public:
    Console(const std::string& loggingFileName, TimeManager& timeManager);
    Console(const Console& rhs) = delete;
    Console(Console&& rhs) = delete;
    ~Console();

    Console& operator=(const Console& rhs) = delete;
    Console& operator=(Console&& rhs) = delete;   

    void setMinimumLogClassificationToProcess(const LogClassification minimum);
    LogClassification getMinimumLogClassificationToProcess();

    bool addCommand(const std::string& command_key, std::function<void(const std::string& input)> command);

    bool removeCommand(const std::string& command_key);
    void clearCommands();
    void getCommandList(std::vector<std::string> out_list) const;

    bool log(const std::string& msg, const LogClassification classification);

    bool logToFile(const std::string& msg, const LogClassification classification);

    bool command(const std::string& msg, const std::string& input);

    bool isClassificationProcessable(const LogClassification toTest);

    bool separateCommand(const std::string& fullCommand, std::string& out_command, std::string& out_input) const;
    bool isLogCommand(const std::string& log) const;
    std::string logFilename() const;
private:
    void createAsyncTask();
    void checkAsyncTask();
    void createTaskTimer();

    static bool asyncWriteToFile(const std::string& filename, std::vector<std::string>&& logs);
private:
    std::map<std::string, std::function<void(const std::string& input)>> m_commands;
    std::string m_loggingFileName;
    std::mutex m_classificationMutex;
    LogClassification m_minimumLogClassificationAccepted;
    TimeManager& m_timeManager;

    std::mutex m_logsToFileMutex;
    std::vector<std::string> m_logsToFile;
    std::future<bool> m_logToFileTask;
};
}

#endif //CONSOLE_H