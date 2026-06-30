#pragma once

#include "string"
#include <fstream>
#include <iostream>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#define NOMINMAX
#if defined(_WIN32)
#include <windows.h>
#undef near
#undef far
#elif defined(__linux__)
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif


namespace FileSystem
{
	std::string getExecutablePath();

	// Remove last N path components to get project directory
	std::string getProjectDir(int levelsUp=3);

	std::string getFileContents(const char* filename);

	bool fileExists(const char* path);

	void openFile(const char* filename);
	



}