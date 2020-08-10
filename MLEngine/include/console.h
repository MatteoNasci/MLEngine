#ifndef CONSOLE_H
#define CONSOLE_H

#include <mlengine_global.h>
#include <string>
#include <functional>
#include <vector>
#include <map>

namespace mle
{
enum class LogClassification{
    Normal = 0,
    Warning,
    Error,
    Fatal,
    Critical,
    Command,
    Info,
};

class MLENGINE_SHARED_EXPORT Console{
public:
    Console(const std::string& loggingFileName);
    Console(const Console& rhs) = delete;
    Console(Console&& rhs) = delete;
    ~Console();

    Console& operator=(const Console& rhs) = delete;
    Console& operator=(Console&& rhs) = delete;   

    void setMinimumLogClassificationToProcess(const LogClassification minimum);
    LogClassification getMinimumLogClassificationToProcess() const;

    bool addCommand(const std::string& command_key, std::function<void(const std::string& input)> command);

    bool removeCommand(const std::string& command_key);
    void clearCommands();
    void getCommandList(std::vector<std::string> out_list) const;

    bool log(const std::string& msg, const LogClassification classification);

    bool logToFile(const std::string& msg, const LogClassification classification);

    bool command(const std::string& msg, const std::string& input);

    bool isClassificationProcessable(const LogClassification toTest) const;

    bool separateCommand(const std::string& fullCommand, std::string& out_command, std::string& out_input) const;
    bool isLogCommand(const std::string& log) const;
private:
    std::map<std::string, std::function<void(const std::string& input)>> m_commands;
    std::string m_loggingFileName;
    LogClassification m_minimumLogClassificationAccepted;
};
}

#endif //CONSOLE_H