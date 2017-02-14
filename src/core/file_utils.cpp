#include "elk/core/file_utils.h"

namespace elk { namespace core {

std::string read_file(const char* file_path)
{
	std::string result;
	if (FILE *fp = fopen(file_path, "rb"))
	{
		char buf[1024];
		while (size_t len = fread(buf, 1, sizeof(buf), fp))
			result.insert(result.end(), buf, buf + len);
		fclose(fp);
	}
	else
	{
		printf("ERROR : %s could not be opened.\n", file_path);
	}
  
	return result;
}

} }