#pragma once

#using <mscorlib.dll>
#using <System.dll>

namespace My3D {

class AutoStr
{
  public:

    AutoStr (System::String^ from)
    {
      str = (char*)(void*) System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi (from);
    }

    ~AutoStr ()
    {
      System::Runtime::InteropServices::Marshal::FreeHGlobal ((System::IntPtr)str);
    }

    const char* c_str ()
    {
      return str;
    }

  private:

    char* str;
};

template < class T, class U > 
bool isinst (U u)
{
   return dynamic_cast <T> (u) != nullptr;
}

}
