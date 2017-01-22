#include "sge/core/file_utils.h"

std::string read_file(const char* file_path)
{
	/*
	std::ifstream file_stream(file_path, std::ios::in);
	std::string file_string;

	if(ifstream.is_open())
	{
	  std::string line = "";
	  while(getline(ifstream, line))
	  {
	  	file_string += line + "\n";
	  }
	  file_stream.close();
	}
	else
	{
	  printf("ERROR : %s could not be opened.\n", file_path);
	}
*/

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
