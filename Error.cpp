#include "Error.h"

namespace TLUtil
{

FileOpen_Ex::FileOpen_Ex(std::string& message) : std::runtime_error("File can not be opened: "+message)
{
}

CameraOpen_Ex::CameraOpen_Ex(std::string& id) : std::runtime_error("Camera can not be opened: "+id)
{
}


OptionNotFound_Ex::OptionNotFound_Ex(std::string& message) : std::runtime_error("Option not found: "+message)
{
}

}
