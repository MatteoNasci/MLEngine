#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <mlengine_global.h>

#include <Engine/Core/engineerror.h>

#include <vector>
#include <string>

namespace mle{
class MLENGINE_SHARED_EXPORT FileUtils{
public:
    static EngineError readAllFile(const std::string& filename, std::vector<char>& out_rawData);

private: FileUtils() = delete;
};
};

#endif