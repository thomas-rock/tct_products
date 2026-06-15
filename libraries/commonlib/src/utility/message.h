#pragma once

// message types
typedef int             MessageType;

#define SysStatus       0x00000002
#define SysInfo         0x00000004
#define SysWarning      0x00000008
#define SysError        0x00000010
#define SysFatal        0x00000020

#define UserDebug       0x00000100
#define UserStatus      0x00000200
#define UserInfo        0x00000400
#define UserWarning     0x00000800
#define UserError       0x00001000
#define UserFatal       0x00002000

#define MESSAGE         emit message
