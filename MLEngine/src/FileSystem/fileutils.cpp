#include <FileSystem/fileutils.h>


#include <fstream>
using namespace mle;

EngineError FileUtils::readAllFile(const std::string& filename, std::vector<char>& out_rawData){
    std::ifstream file(filename, std::ios_base::ate | std::ios_base::in | std::ios_base::binary);
    if(file.is_open()){
        auto fileSize = file.tellg();
        out_rawData.resize(static_cast<size_t>(fileSize));
        file.seekg(0);
        file.read(out_rawData.data(), fileSize);
        file.close();

        return EngineError::Ok;
    }
    return EngineError::FileCouldNotBeOpened;
}