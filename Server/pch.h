#pragma once

/*
게임서버에서 자주 화용하는 헤더를 여기에 넣어두면
빌드가 한번에 되서 미리 빌드가 되가지고
나머지에서 재차빌드 없이 빠르게 ㅅ용함.

단점은 여기가 수정되면 계속 많은 빌드를 하게된다.

미리컴파일된 헤더를 직접 만든 것이므로 설정을 해줌
프로젝트 속성 미리컴파일된 헤더에서... 사용으로 변경하고 헤더파일에 pch header 입력해줌.

pch cpp는 속성에서 미리컴파일된 헤더 만들기 (왜?) 해줘야한다.

*/

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifdef _DEBUG

#pragma comment(lib, "Debug\\ServerCore.lib")
#else
#pragma comment(lib, "Release\\ServerCore.lib")
#endif


// 여기에 미리 컴파일하려는 헤더 추가
#include "CorePch.h"
