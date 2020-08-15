#include <Input/cursor.h>

using namespace mle;

Cursor::Cursor() : Cursor(nullptr) {

}
Cursor::Cursor(GLFWcursor* in_cursor) : cursor(in_cursor){
    
}