# commonLibs
Personal utility project.

## How to use
- The path of commonLibs must be registered as the name of **commonLibs** in the environment variable.
- The module to be used must be designated by declaring **COMMON_LIBS** in the **makelist** or **.pro** file.

- Include the **commonLibs.mk** file if it's a cmake project
> exp) Android.mk
<pre><code>...
COMMON_LIBS:= CBaseClass CCallbackMacro CLogger CStrManager CVideoCapture CVideoStreamer
include $(commonLibs)\commonLibs.mk
...</code></pre>

- Include the **commonLibs.pri** file if it's a Qt project
> exp) temp.pro
<pre><code>...
COMMON_LIBS = CLogger CStrManager
win32:include($(COMMONLIBS)/commonLibs.pri)
else:unix:include($(commonLibs)/commonLibs.pri)
...</code></pre>

- It is necessary to create a **cmnDef.h** file in the project and define the contents required for commonLibs.
> exp) cmnDef.h
<pre><code>#ifndef CMNDEF_H
#define CMNDEF_H

//Definition file for common library
#define __QT_DEBUG // Using QLOG: Qt project
#define LOG_LV       0//4//_INFO

#endif // CMNDEF_H</code></pre>
