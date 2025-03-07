#include "filesys_interaction.h"

void watch_dir(void); //char *f_name, int date_cap[6]
static void call_cp(char chFullPath[MAX_PATH + FILENAME_MAX], char chDate[20]);
static void wait(WCHAR *wchBuf);
static void get_date(char chFullPath[MAX_PATH + FILENAME_MAX], char chDate[20]);
void set_dir(const char* dir_name);
static void ErrorExit(void);


char chDirName[MAX_PATH] = {};
HANDLE hDir = INVALID_HANDLE_VALUE;
HANDLE syncThread = INVALID_HANDLE_VALUE;
DWORD dwCompletionStatus;

void watch_dir(void) { //char *f_name, int date_cap[6]
	char chFileName[FILENAME_MAX];
	char chDate[20];
	//int iDate[6];
	WCHAR wchBuf[100] = {};
	if ((syncThread = CreateThread( NULL, 0,
			(LPTHREAD_START_ROUTINE)wait,
			&wchBuf, 0, NULL)) == NULL) {
		ErrorExit();
	}

	WaitForSingleObject(syncThread, INFINITE); //why can be 0?

	if (dwCompletionStatus == ERROR_SUCCESS) {
		char chFullPath[MAX_PATH + FILENAME_MAX] = {};
		wcstombs(chFileName, ((PFILE_NOTIFY_INFORMATION)wchBuf)->FileName, FILENAME_MAX - 1);
		snprintf(chFullPath, MAX_PATH + FILENAME_MAX, "%s/%s", chDirName, chFileName);
		fprintf(stderr, "chFullPath: %s\n", chFullPath);

		Sleep(500); // wait for things to close
		//get_date(chFullPath, iDate);
		get_date(chFullPath, chDate);
		//snprintf (chDate, 20, "%04d-%02d-%02d %02d:%02d:%02d", iDate[0], iDate[1], iDate[2], iDate[3], iDate[4], iDate[5] ); //YYYY-MM-DD 
		call_cp(chFullPath, chDate);
	}
}

static void wait(WCHAR *wchBuf) {
	dwCompletionStatus = ERROR_SUCCESS;
	WCHAR wchFileExt[4] = {};
	DWORD dwFileLen = 0, dwBytesRet = 0, dwAct = 0x0;

	fprintf(stderr, "Watching directory: %s\n", chDirName);

	do {
		if (ReadDirectoryChangesW( hDir, wchBuf, 100, FALSE, 
				FILE_NOTIFY_CHANGE_FILE_NAME,
				&dwBytesRet, NULL, NULL) == 0) {
			dwCompletionStatus = GetLastError();
			if (dwCompletionStatus != ERROR_OPERATION_ABORTED &&
					dwCompletionStatus != ERROR_IO_PENDING) {
				fprintf(stderr, "dwCompletionStatus != ERROR_OPERATION_ABORTED && dwCompletionStatus != ERROR_IO_PENDING)");
				ErrorExit();
			}
			break;
		}
		else {
			dwAct = ((PFILE_NOTIFY_INFORMATION)wchBuf)->Action;
			dwFileLen = ((PFILE_NOTIFY_INFORMATION)wchBuf)->FileNameLength / sizeof(WCHAR);
			wcsncpy(wchFileExt, &((PFILE_NOTIFY_INFORMATION)wchBuf)->FileName[dwFileLen - 3], 4); // get 3 char extension
		}
	} while ( dwAct != FILE_ACTION_ADDED || (wcscmp(wchFileExt, L"jpg") != 0 && wcscmp(wchFileExt, L"png") != 0) );
} 

static void call_cp(char chFullPath[MAX_PATH + FILENAME_MAX], char *date_cap) {
	STARTUPINFO start_info;
	PROCESS_INFORMATION proc_info;

	char cmd_line[MAX_PATH + FILENAME_MAX + 37]; // 13 + MAX_PATH + FILENAME_MAX + 23 + 1
	snprintf ( cmd_line, MAX_PATH + FILENAME_MAX + 37, "./annotator.exe \"%s\" \"%s\"", chFullPath, date_cap); //YYYY-MM-DD 

	ZeroMemory( &start_info, sizeof(start_info) );
	start_info.cb = sizeof(start_info);
	ZeroMemory( &proc_info, sizeof(proc_info) );
	
	fprintf(stderr, "cmd_line: %s\n", cmd_line);
	
	if( !CreateProcess( NULL, //"./annotator.exe"
			cmd_line, 
			NULL, NULL,				
			FALSE, 0,
			NULL, NULL,			
			&start_info,					 
			&proc_info ) ) {
		ErrorExit();
	}

	WaitForSingleObject( proc_info.hProcess, INFINITE );

	// Close process and thread handles. 
	CloseHandle( proc_info.hProcess );
	CloseHandle( proc_info.hThread );
}



static void get_date( char chFullPath[MAX_PATH], char chDate[20] ) {
	HANDLE hFile = CreateFileA( chFullPath, GENERIC_READ,
															FILE_SHARE_READ, NULL, OPEN_EXISTING,
															FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		ErrorExit(); //Cannot open files downloaded off internet (security). Unblock first.
	};

	FILETIME ftFileDate = {};
	SYSTEMTIME stFileDate = {};
	GetFileTime(hFile, &ftFileDate, NULL, NULL);
	CloseHandle(hFile); //close asap

	FileTimeToSystemTime(&ftFileDate, &stFileDate);

	snprintf (chDate, 20, "%04d-%02d-%02d %02d:%02d:%02d", stFileDate.wYear, stFileDate.wMonth, stFileDate.wDay, stFileDate.wHour, stFileDate.wMinute, stFileDate.wSecond ); //YYYY-MM-DD 

	// YYYY-MM-DD HH:MM:SS
}

void set_dir(const char* dir_name) {
	strncpy(chDirName, dir_name, MAX_PATH);
	hDir = CreateFileA( chDirName,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ, NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL);
	if (hDir == INVALID_HANDLE_VALUE) {
		ErrorExit();
	}
}

static void ErrorExit(void) { //https://learn.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
	// Retrieve the system error message for the last-error code
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();

	if (FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL) == 0) {
		MessageBox(NULL, TEXT("FormatMessage failed"), TEXT("Error"), MB_OK);
		ExitProcess(dw);
	}

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	ExitProcess(dw);
}




