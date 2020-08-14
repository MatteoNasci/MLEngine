#ifndef CONSOLE_H
#define CONSOLE_H

#include <mlengine_global.h>

#include <timermanager.h>

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <future>
#include <mutex>
#include <deque>

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
    Console(const std::string& loggingFileName, TimerManager& timerManager, const size_t commands_queue_max_size = 500); //TODO: test added commands
    Console(const Console& rhs) = delete;
    Console(Console&& rhs) = delete;
    ~Console();

    Console& operator=(const Console& rhs) = delete;
    Console& operator=(Console&& rhs) = delete;   

    void setMinimumLogClassificationToProcess(LogClassification minimum);
    LogClassification getMinimumLogClassificationToProcess();

    bool addCommand(const std::string& command_key, std::function<void(const std::string& input)> command);
    bool containsCommand(const std::string& command_key);
    bool removeCommand(const std::string& command_key);
    void clearCommands();
    void getCommandList(std::vector<std::string>& out_list);

    bool log(const std::string& msg, const LogClassification classification);

    bool logToFile(const std::string& msg, const LogClassification classification);

    bool command(const std::string& msg, const std::string& input);

    bool isClassificationProcessable(const LogClassification toTest);

    
    std::string logFilename() const;
    size_t commandsSize();
    bool isLoggingtoFile();
    void setCommandsHistoryMaxSize(const size_t size); //TODO: test
    size_t commandsHistoryMaxSize(); //TODO: test
    size_t commandsHistoryCurrentSize(); //TODO: test
    bool getCommandsHistory(std::deque<std::string>& out_commands); //TODO: test

    
    static bool lookForInputName(const std::string& input, size_t& out_nameStartIndex, size_t& out_nameLength); 
    static bool lookForInputArgument(const std::string& input, size_t& out_argStartIndex, size_t& out_argLength); 
    static bool separateInput(const std::string& input, std::function<void(std::string&&, std::string&&)> onNameInputPairAction);
    static bool separateCommand(const std::string& fullCommand, std::string& out_command, std::string& out_input);
    static bool isMsgCommand(const std::string& msg);
    static mle::LogClassification classificationFromString(const std::string& s, const LogClassification defaultValue = LogClassification::Normal);
    static std::string stringFromClassification(const mle::LogClassification classification, const std::string& defaultValue = "");
    static constexpr mle::LogClassification getHighestPriorityClassification() {
        return mle::LogClassification::Info;
    };
    static constexpr char getCommandInputSeparator(){
        return '-';
    };
    static constexpr char getCommandPrefix(){
        return '/';
    };
    static constexpr char getInputSeparator(){
        return ' ';
    };
    static constexpr char getInputNameSeparator(){
        return '=';
    };
    static const std::unordered_map<std::string, mle::LogClassification>& getMapStringToClass();
    static const std::unordered_map<mle::LogClassification, std::string>& getMapClassToString();
private:
    void createAsyncTask();
    void checkAsyncTask();
    void createTaskTimer();

    void addCommandToHistory(const std::string& command);

    static bool asyncWriteToFile(const std::string& filename, std::vector<std::string>&& logs);
private:
    std::mutex m_commandsMutex;
    std::unordered_map<std::string, std::function<void(const std::string& input)>> m_commands;
    std::mutex m_commandsHistoryMutex;
    std::deque<std::string> m_commandsHistory;
    size_t m_maxCommandHistorySize;
    std::string m_loggingFileName;
    std::mutex m_classificationMutex;
    LogClassification m_minimumLogClassificationAccepted;
    TimerManager& m_timeManager;

    std::mutex m_logsToFileMutex;
    std::vector<std::string> m_logsToFile;
    std::mutex m_logsToFileTaskMutex;
    std::future<bool> m_logToFileTask;
};
}

#endif //CONSOLE_H