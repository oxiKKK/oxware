#include <iostream>
#include <fstream>
#include <filesystem>

void write_buffer(uint8_t* buffer, uintmax_t size, const char* variable_name, const char* output_filename)
{
	std::ofstream ofs(output_filename);

	if (ofs.bad())
	{
		printf("error: failed to open output file.\n");
		return;
	}

	ofs << "unsigned char " << variable_name << "[" << size + 1 << "] = \n{\n\t\"";

	for (uintmax_t i = 0; i < size; i++)
	{
		uint8_t byte = buffer[i];

		if (i > 0 && i % 40 == 0)
		{
			ofs << "\"\n\t\"";
		}

		ofs << std::format("\\x{:02X}", byte);
	}

	ofs << "\"\n};";

	ofs.close();

	printf("wrote to %s\n", output_filename);
}

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		printf("error: invalid arguments.\n");
		printf("usage: png2cpp <png file> <variable name> <cpp filename>.\n");
		return 1;
	}

	std::filesystem::path png_path = argv[1];

	std::ifstream ifs(png_path, std::ios_base::binary | std::ios_base::in);

	if (ifs.bad())
	{
		printf("error: invalid input file.\n");
		return 1;
	}

	auto filesize = std::filesystem::file_size(png_path);
	uint8_t* file_buffer = new uint8_t[filesize];

	ifs.read((char*)file_buffer, filesize);

	write_buffer(file_buffer, filesize, argv[2], argv[3]);

	delete[] file_buffer;

	ifs.close();

	printf("success!\n");
}
