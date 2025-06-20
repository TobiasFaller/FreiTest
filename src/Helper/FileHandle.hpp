#pragma once

#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include <iostream>
#include <string>

class FileHandle
{
public:
	FileHandle(std::string filename, bool forReading);
	virtual ~FileHandle();
	std::istream& GetStream();
	std::ofstream& GetOutStream();
	std::string GetFilename() {return _filename;};
	std::string GetCanonicalFilename();
	static std::string GetCanonicalFilename(std::string filename);
	static void CreateDirectoryIfNotExisting(std::string path);

private:
	std::string _filename;
	std::ifstream* _ifstream;
	std::ofstream* _ofstream;
	boost::iostreams::filtering_istream _inFilter;
	boost::iostreams::gzip_decompressor _boostZipDecompressor;
	bool _zipMode;
	bool _forReading;

};
