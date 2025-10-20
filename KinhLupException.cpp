#include"KinhLupException.h"
#include"sstream"

KinhLupException::KinhLupException(int line, const char* file) noexcept 
	:line(line), file(file){ }
const char* KinhLupException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}
 const char* KinhLupException::GetType() const noexcept
{
	 return "Kinh Lup Exception !";
}
 int KinhLupException::GetLine() const noexcept
 {
	 return line;
}
 const std::string& KinhLupException::GetFile() const noexcept
 {
	 return file;
}
 std::string KinhLupException::GetOriginString() const noexcept
 {
	 std::ostringstream oss;
	 oss << "File : " << file << std::endl
		 << "Line : " << line;
	 return oss.str();
}