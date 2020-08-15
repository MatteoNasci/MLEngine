#include <imagedata.h>

using namespace mle;
ImageData::ImageData() : ImageData::ImageData(nullptr, 0){

}
ImageData::ImageData(GLFWimage* in_images, const size_t in_count) : images(in_images), count(in_count){
    
}