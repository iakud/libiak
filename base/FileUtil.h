#ifndef IAK_BASE_FILEUTIL_H
#define IAK_BASE_FILEUTIL_H

#include <string>

namespace iak {

class FileReader {
public:
	static int readFile(const char* filename,
			std::string& content,
			size_t size) {
		FileReader file(filename);
		return file.readToString(content, size);
	}

public:
	FileReader(const char* filename);
	~FileReader();
	// noncopyable
	FileReader(const FileReader&) = delete;
	FileReader& operator=(const FileReader&) = delete;

	int readToString(std::string& content, size_t size);

private:
	int fd_;
	int err_;
}; // end class FileReader

} // end namespace iak

#endif // IAK_BASE_FILEUTIL_H
