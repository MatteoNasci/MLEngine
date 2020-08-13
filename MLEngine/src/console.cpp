#include <console.h>

#include <enginetime.h>

#include <iostream>
#include <fstream>
#include <ios>
#include <utility>
#include <chrono>

using namespace mle;

Console::Console(const std::string& loggingFileName, TimerManager& timerManager) :
    m_commandsMutex(),
    m_commands(), 
    m_loggingFileName(loggingFileName),
    m_minimumLogClassificationAccepted(LogClassification::Normal),
    m_timeManager(timerManager),
    m_logsToFile(),
    m_logToFileTask()
{
    std::ofstream log_file;
    log_file.open(m_loggingFileName, std::ios::out | std::ios::trunc);
    if(log_file.is_open()){
        const size_t time_size = 200;
        char time[time_size];
        EngineTime::nowString(time, time_size);
        log_file << "Logging started at " << time;
        log_file.close();
    }
    addCommand(Console::getCommandPrefix() + "log", [this](const std::string& input){
        log(input, Console::classificationFromString(input));
    });

    addCommand(Console::getCommandPrefix() + "log " + stringFromClassification(LogClassification::Normal), [this](const std::string& input){
        log(input, LogClassification::Normal);
    });
    addCommand(Console::getCommandPrefix() + "log " + stringFromClassification(LogClassification::Warning), [this](const std::string& input){
        log(input, LogClassification::Warning);
    });
    addCommand(Console::getCommandPrefix() + "log " + stringFromClassification(LogClassification::Error), [this](const std::string& input){
        log(input, LogClassification::Error);
    });
    addCommand(Console::getCommandPrefix() + "log " + stringFromClassification(LogClassification::Critical), [this](const std::string& input){
        log(input, LogClassification::Critical);
    });
    addCommand(Console::getCommandPrefix() + "log " + stringFromClassification(LogClassification::Fatal), [this](const std::string& input){
        log(input, LogClassification::Fatal);
    });
    addCommand(Console::getCommandPrefix() + "log " + stringFromClassification(LogClassification::Command), [this](const std::string& input){
        log(input, LogClassification::Command);
    });
    addCommand(Console::getCommandPrefix() + "log " + stringFromClassification(LogClassification::Info), [this](const std::string& input){
        log(input, LogClassification::Info);
    });
}
Console::~Console(){
    
}
size_t Console::commandsSize(){
    m_commandsMutex.lock();
    const size_t res = m_commands.size();
    m_commandsMutex.unlock();

    return res;
}
const std::unordered_map<std::string, mle::LogClassification>& Console::getMapStringToClass(){
    static std::unordered_map<std::string, mle::LogClassification> map = {
        {"normal", LogClassification::Normal},
        {"warning", LogClassification::Warning},
        {"error", LogClassification::Error},
        {"critical", LogClassification::Critical},
        {"fatal", LogClassification::Fatal},
        {"command", LogClassification::Command},
        {"info", LogClassification::Info},
    };
    return map;
}
const std::unordered_map<mle::LogClassification, std::string>& Console::getMapClassToString(){
    static std::unordered_map<mle::LogClassification, std::string> map = {
        {LogClassification::Normal, "normal"},
        {LogClassification::Warning, "warning"},
        {LogClassification::Error, "error"},
        {LogClassification::Critical, "critical"},
        {LogClassification::Fatal, "fatal"},
        {LogClassification::Command, "command"},
        {LogClassification::Info, "info"},
    };
    return map;
}
mle::LogClassification Console::classificationFromString(const std::string& s, const LogClassification defaultValue){
    const std::unordered_map<std::string, mle::LogClassification>& converter = Console::getMapStringToClass();
    if(converter.count(s)){
        return converter.at(s);
    }
    return defaultValue;
}
std::string Console::stringFromClassification(const mle::LogClassification classification, const std::string& defaultValue){
    const std::unordered_map<mle::LogClassification, std::string>& converter = Console::getMapClassToString();
    if(converter.count(classification)){
        return converter.at(classification);
    }
    return defaultValue;
}
void Console::setMinimumLogClassificationToProcess(LogClassification minimum){
    if(minimum > Console::getHighestPriorityClassification()){
        minimum = Console::getHighestPriorityClassification();
    }

    m_classificationMutex.lock();
    m_minimumLogClassificationAccepted = minimum;
    m_classificationMutex.unlock();
}
LogClassification Console::getMinimumLogClassificationToProcess(){
    m_classificationMutex.lock();
    const auto result = m_minimumLogClassificationAccepted;
    m_classificationMutex.unlock();

    return result;
}
std::string Console::logFilename() const{
    return m_loggingFileName;
}
bool Console::addCommand(const std::string& command_key, std::function<void(const std::string& input)> command){
    if(!command){
        return false;
    }
    if(!isMsgCommand(command_key)){
        return false;
    }
    m_commandsMutex.lock();
    if(m_commands.count(command_key)){
        m_commandsMutex.unlock();
        return false;
    }
    m_commands[command_key] = command;
    m_commandsMutex.unlock();

    return true;
}
bool Console::containsCommand(const std::string& command_key){
    m_commandsMutex.lock();
    const size_t count = m_commands.count(command_key);
    m_commandsMutex.unlock();

    return count;
}
bool Console::removeCommand(const std::string& command_key){
    m_commandsMutex.lock();
    const size_t removed = m_commands.erase(command_key);
    m_commandsMutex.unlock();

    return removed;
}
void Console::clearCommands(){
    m_commandsMutex.lock();
    m_commands.clear();
    m_commandsMutex.unlock();
}
void Console::getCommandList(std::vector<std::string>& out_list){  
    size_t i = 0;

    m_commandsMutex.lock();
    out_list.resize(m_commands.size());
    for(const auto& pair : m_commands){
        out_list[i] = pair.first;
        ++i;
    }
    m_commandsMutex.unlock();
}
bool Console::log(const std::string& msg, const LogClassification classification){  
    std::string command_key, input;
    if(separateCommand(msg, command_key, input)){
        return command(command_key, input);
    }  

    return logToFile(msg, classification);
}
bool Console::logToFile(const std::string& msg, const LogClassification classification){
    if(!isClassificationProcessable(classification)){
        return false;
    }

    m_logsToFileMutex.lock();
    m_logsToFile.push_back(msg);
    if(!m_logToFileTask.valid()){
        createAsyncTask();
    }
    m_logsToFileMutex.unlock();

    return true;
}
void Console::createAsyncTask(){
    m_logToFileTask = std::async(std::launch::async, &Console::asyncWriteToFile, logFilename(), std::move(m_logsToFile));
    m_logsToFile.clear();
    createTaskTimer();
}
void Console::createTaskTimer(){
    m_timeManager.addTimer(0.3, [this](){if(this != nullptr){this->checkAsyncTask();}});
}
bool Console::asyncWriteToFile(const std::string& filename, std::vector<std::string>&& logs){
    std::ofstream log_file;
    log_file.open(filename, std::ios::out | std::ios::app);
    if(log_file.is_open()){
        const size_t size = logs.size();

        for(size_t i = 0; i < size; ++i){
            log_file << logs[i] << std::endl;
        }
        
        log_file.close();
        return true;
    }
    return false;
}
void Console::checkAsyncTask(){
    m_logsToFileMutex.lock();
    const bool isTaskOver = (m_logToFileTask.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready);
    m_logsToFileMutex.unlock();

    if(!isTaskOver){
        createTaskTimer();
        return;
    }

    m_logsToFileMutex.lock();
    const bool result = m_logToFileTask.get();
    if(m_logsToFile.size()){
        createAsyncTask();
    }
    m_logsToFileMutex.unlock();
}
bool Console::command(const std::string& msg, const std::string& input){
    bool command_found = containsCommand(msg);   

    if(command_found){
        logToFile(msg + Console::getCommandInputSeparator() + input, LogClassification::Command);

        m_commandsMutex.lock();
        const auto command = m_commands[msg];
        m_commandsMutex.unlock();

        if(command){
            command(input);
        }else{
            logToFile("Failed to find a valid function associated with command '" + msg + "'! The command will be removed.", mle::LogClassification::Error);
            command_found = false;
            removeCommand(msg);
        }
    }else{
        logToFile("[COMMAND_NOT_FOUND]" + msg + input, LogClassification::Command);
    }

    return command_found;
}
bool Console::isLoggingtoFile(){
    m_logsToFileMutex.lock();
    const bool res = m_logToFileTask.valid();
    m_logsToFileMutex.unlock();

    return res;
}
bool Console::isClassificationProcessable(const LogClassification toTest){
    return toTest >= getMinimumLogClassificationToProcess();
}
bool Console::separateCommand(const std::string& fullCommand, std::string& out_command, std::string& out_input){
    if(!Console::isMsgCommand(fullCommand)){
        return false;
    }

    const size_t input_start_pos = fullCommand.find_first_of(Console::getCommandInputSeparator());
    const bool separator_found = input_start_pos != std::string::npos;
    if(separator_found){
        out_command = fullCommand.substr(0, input_start_pos);
        const size_t actual_input_start = input_start_pos + 1;
        out_input = fullCommand.substr(actual_input_start, fullCommand.size() - actual_input_start);
    }else{
        out_command = fullCommand;
        out_input = "";
    }
    
    return out_command.size();
}
bool Console::isMsgCommand(const std::string& msg){
    if(!msg.size()){
        return false;
    }
    return (msg[0] == Console::getCommandPrefix());
}
bool Console::lookForInputName(const std::string& input, size_t& out_nameStartIndex, size_t& out_nameLength){
    bool found = false;
    const size_t size = input.size();
    for(size_t i = out_nameStartIndex; i < size; ++i){
        if(input[i] == Console::getInputNameSeparator() && i != out_nameStartIndex){
            found = true;
            out_nameLength = i - out_nameStartIndex;
            break;
        }
    }
    return found;
}
bool Console::lookForInputArgument(const std::string& input, size_t& out_argStartIndex, size_t& out_argLength){
    const size_t size = input.size();
    size_t i = out_argStartIndex;
    out_argLength = 0;
    for(; i < size; ++i){
        if(input[i] == Console::getInputSeparator()){
            out_argLength = (i - out_argStartIndex);
            if(out_argLength >= 1){ //in case we have only a single char in the substr equal to the end char we ignore it and restart from next char, otherwise we found argument and exit
                break;
            }
            out_argLength = 0;
            out_argStartIndex = i + 1;
        }
    }
    if(!out_argLength && out_argStartIndex < size){
        out_argLength = (i - out_argStartIndex);
    }

    return out_argLength > 0;
}
bool Console::separateInput(const std::string& input, std::function<void(std::string&&, std::string&&)> onNameInputPairAction){
    if(onNameInputPairAction){
        const size_t input_size = input.size();
        size_t name_start_index = 0, name_length = 0, argument_start_index = 0, argument_length = 0;
        bool keep_looping = true;
        while(keep_looping){          
            keep_looping = Console::lookForInputName(input, name_start_index, name_length);
            argument_start_index = name_start_index + name_length + 1; //ignore separator if present
            if(keep_looping){
                argument_length = 0;
                keep_looping = Console::lookForInputArgument(input, argument_start_index, argument_length);
                if(keep_looping){
                    onNameInputPairAction(input.substr(name_start_index, name_length), input.substr(argument_start_index, argument_length));
                }
            }
            name_start_index = argument_start_index + argument_length + 1; //ignore separator if present
        }
        return true;
    }
    return false;
}
