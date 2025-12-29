// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

// pch.h: 预编译头文件
#ifndef PCH_H
#define PCH_H

// 1. 添加这一行，解决 'localtime' unsafe 和其他安全函数报错
#define _CRT_SECURE_NO_WARNINGS

#include "framework.h"

// 2. 确保包含 MFC 核心头文件
#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展
#include <afxdisp.h>        // MFC 自动化类
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif

// 3. 包含资源头文件 (包含控件ID定义)
#include "resource.h"

#endif //PCH_H

