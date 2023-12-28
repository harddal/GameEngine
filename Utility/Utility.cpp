#include "Utility.h"

#include <Windows.h>
#include <spdlog/spdlog.h>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;
using std::chrono::microseconds;

std::chrono::steady_clock::time_point g_PerfClockStart, g_PerfClockEnd;

void Utility::StartPerfClock()
{
	g_PerfClockStart = high_resolution_clock::now();
}
void Utility::StopPerfClock()
{
	g_PerfClockEnd = high_resolution_clock::now();
}
unsigned int Utility::GetPerfClockResultMilliseconds()
{
	return static_cast<unsigned int>(duration_cast<milliseconds>(g_PerfClockEnd - g_PerfClockStart).count());
}
unsigned int Utility::GetPerfClockResultMicroseconds()
{
	return static_cast<unsigned int>(duration_cast<microseconds>(g_PerfClockEnd - g_PerfClockStart).count());
}

void Utility::Warning(std::string error)
{
    spdlog::warn(error);
	MessageBox(nullptr, error.c_str(), "Warning!", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
}

void Utility::Error(std::string error)
{
    spdlog::error(error);
	MessageBox(nullptr, error.c_str(), "Error!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	std::exit(EXIT_FAILURE);
}

std::string Utility::FilenameFromPath(std::string path)
{
    // Full esample of splitpath
	//char path_buffer[_MAX_PATH];
	//char drive[_MAX_DRIVE];
	//char dir[_MAX_DIR];
	//char fname[_MAX_FNAME];
	//char ext[_MAX_EXT];
	//size_t sizeInCharacters = 100;
	/* _makepath(path_buffer, "g", "\\Testdir\\myexample\\", "testfile", "txt"); */
	//_makepath_s(path_buffer, sizeInCharacters, "g", "\\Testdir\\myexample\\", "testfile", "txt");
	//printf("\nPath created with _makepath_s(): %s\n", path_buffer);
	/* _splitpath(path_buffer, drive, dir, fname, ext); */
	//_splitpath_s(path_buffer, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
    
	char fname[_MAX_FNAME];
	_splitpath(path.c_str(), nullptr, nullptr, fname, nullptr);
	return std::string(fname);
}
// Includes the '.' example: image.png -> .png
std::string Utility::FileExtensionFromPath(std::string path)
{
	char ext[_MAX_EXT];
	_splitpath(path.c_str(), nullptr, nullptr, nullptr, ext);
	return std::string(ext);
}

std::string Utility::OpenFileDialog(const char* filter)
{
	char filename[MAX_PATH];

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Open";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	GetOpenFileNameA(&ofn);
    
#ifdef DEBUG_LOG_STRICT
	log::write("OpenFileDialog() - " + std::string(filename), LOG_INFO);
#endif
    
	return std::string(filename);
}

std::string Utility::SaveFileDialog(const char* filter)
{
	char filename[MAX_PATH];

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Save";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	GetSaveFileNameA(&ofn);
    
#ifdef DEBUG_LOG_STRICT
	log::write("SaveFileDialog() - " + std::string(filename), LOG_INFO);
#endif

	return std::string(filename);
}