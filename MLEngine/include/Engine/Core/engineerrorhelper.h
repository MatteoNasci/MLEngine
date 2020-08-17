#ifndef ENGINEERRORHELPER_H
#define ENGINEERRORHELPER_H

#include <mlengine_global.h>
#include <Engine/Core/engineerror.h>

#include <unordered_map>
#include <string>
namespace mle{
class MLENGINE_SHARED_EXPORT EngineErrorHelper{
public:
    inline static EngineError convertFromInternalError(const int internal_error){
        return static_cast<EngineError>(internal_error);
    }
    inline static int convertToInternalError(const EngineError error){
        return static_cast<int>(error);
    }
    static const std::unordered_map<EngineError, std::string>& getDescriptionFromErrorMap();
    inline static std::string getDescriptionFromError(const EngineError error, bool& out_found){
        const auto& map = getDescriptionFromErrorMap();
        out_found = map.count(error);
        return out_found ? map.at(error) : "";
    }
    inline static bool isErrorGlfw(const EngineError error);
    inline static bool isErrorVk(const EngineError error);
    inline static bool isErrorOther(const EngineError error);
private: EngineErrorHelper() = delete;
};
};

#endif