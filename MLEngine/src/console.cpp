#include <console.h>

#include <enginetime.h>

#include <iostream>
#include <fstream>
#include <ios>
#include <utility>

using namespace mle;

Console::Console(const std::string& loggingFileName) : m_commands(), m_loggingFileName(loggingFileName), /*m_logToFileTask(),*/ m_minimumLogClassificationAccepted(LogClassification::Normal){
    std::ofstream log_file;
    log_file.open(m_loggingFileName, std::ios::out | std::ios::trunc);
    if(log_file.is_open()){
        const size_t time_size = 100;
        char time[time_size];
        EngineTime::now(time, time_size);
        log_file << "Logging started at " << time << std::endl;
        log_file.close();
    }
}
Console::~Console(){
    
}
void Console::setMinimumLogClassificationToProcess(const LogClassification minimum){
    m_minimumLogClassificationAccepted = minimum;
}
LogClassification Console::getMinimumLogClassificationToProcess() const{
    return m_minimumLogClassificationAccepted;
}
bool Console::addCommand(const std::string& command_key, std::function<void(const std::string& input)> command){
    if(m_commands.count(command_key)){
        return false;
    }
    m_commands[command_key] = command;
    return true;
}
bool Console::removeCommand(const std::string& command_key){
    if(m_commands.count(command_key)){
        m_commands.erase(command_key);
        return true;
    }
    return false;
}
void Console::clearCommands(){
    m_commands.clear();
}
void Console::getCommandList(std::vector<std::string> out_list) const{
    out_list.resize(m_commands.size());
    size_t i = 0;
    for(const auto& pair : m_commands){
        out_list[i] = pair.first;
        ++i;
    }
}
bool Console::log(const std::string& msg, const LogClassification classification){
    if(isLogCommand(msg)){
        std::string command_key, input;
        if(!separateCommand(msg, command_key, input)){
            return false;
        }

        return command(command_key, input);
    }

    return logToFile(msg, classification);
}
bool Console::logToFile(const std::string& msg, const LogClassification classification){
    if(!isClassificationProcessable(classification)){
        return false;
    }

    std::ofstream log_file;
    log_file.open(m_loggingFileName, std::ios::out | std::ios::app);
    if(log_file.is_open()){
        log_file << msg << std::endl;
        log_file.close();
        return true;
    }
    return false;
}
bool Console::command(const std::string& msg, const std::string& input){
    const bool command_found = m_commands.count(msg);

    logToFile(msg + input + (command_found ? "" : " COMMAND_NOT_FOUND"), LogClassification::Command);

    if(command_found){
        m_commands[msg](input);
    }

    return command_found;
}

bool Console::isClassificationProcessable(const LogClassification toTest) const{
    return toTest >= getMinimumLogClassificationToProcess();
}
bool Console::separateCommand(const std::string& fullCommand, std::string& out_command, std::string& out_input) const{
    if(!fullCommand.size()){
        return false;
    }

    const size_t input_start_pos = fullCommand.find_first_of('-');
    const bool separator_found = input_start_pos != std::string::npos && input_start_pos != (fullCommand.size() - 1);
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
bool Console::isLogCommand(const std::string& log) const{
    if(!log.size()){
        return false;
    }
    return (log[0] == '/');
}
