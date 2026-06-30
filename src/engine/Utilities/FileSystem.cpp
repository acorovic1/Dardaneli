#include "FileSystem.h"


std::string FileSystem::getExecutablePath() {
#if defined(_WIN32)
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	return std::string(buffer);
#elif defined(__linux__)
	char buffer[1024];
	ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	if (len == -1) throw std::runtime_error("Cannot get executable path");
	buffer[len] = '\0';
	return std::string(buffer);
#elif defined(__APPLE__)
	char buffer[1024];
	uint32_t size = sizeof(buffer);
	if (_NSGetExecutablePath(buffer, &size) != 0)
		throw std::runtime_error("Cannot get executable path");
	return std::string(buffer);
#else
	throw std::runtime_error("Unsupported platform");
#endif
}


std::string FileSystem::getProjectDir(int levelsUp)
{
	std::string path = FileSystem::getExecutablePath();
	for (int i = 0; i < levelsUp; ++i) {
		auto pos = path.find_last_of("/\\");
		if (pos == std::string::npos) break;
		path = path.substr(0, pos);
	}
	return path;
}

std::string FileSystem::getFileContents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);

	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return (contents);
	}
	else
	{
		std::cout << "Failed to open file: " << filename;
		throw std::runtime_error(std::string("Failed to open file: ") + filename);
	}
}



bool FileSystem::fileExists(const char* path) {
	std::ifstream f(path);
	return f.is_open();
}


void FileSystem::openFile(const char* filename) {
#if defined(_WIN32)
	ShellExecuteA(NULL, "open", filename, NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__)
	std::string cmd = "open ";
	cmd += filename;
	system(cmd.c_str());
#elif defined(__linux__)
	std::string cmd = "xdg-open ";
	cmd += filename;
	system(cmd.c_str());
#endif
}

