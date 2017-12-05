#pragma once

#define Stringify(x) #x
#define No2String(x) Stringify(x)
#define SourceLocation __FILE__ ":" No2String(__LINE__)
