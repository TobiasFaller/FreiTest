#include "Helper/FileHandle.hpp"

#include <boost/iostreams/chain.hpp>

#include <fstream>
#include <filesystem>

#include "Helper/StringHelper.hpp"

#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"

using namespace std;

FileHandle::FileHandle(string rawFilename, bool forReading)
{
	ASSERT(rawFilename.size() > 0) << "Got empty file name";
	_ifstream = nullptr;
	_ofstream = nullptr;
	_zipMode = false;
	_forReading = forReading;

	string mappedFilename = Settings::GetInstance()->MapFileName(rawFilename, forReading);
	mappedFilename = StringHelper::ReplaceString("/./","/",mappedFilename);
	_filename = mappedFilename;


	if (forReading)
	{

		_ifstream = new ifstream(mappedFilename, std::ios_base::in | std::ios_base::binary );

		if (StringHelper::EndsWith(".gz", mappedFilename))
		{
			_inFilter.push(_boostZipDecompressor);
			_inFilter.push(*_ifstream);
			_zipMode = true;
		}
	}
	else
	{
		DASSERT(!StringHelper::EndsWith(".gz", mappedFilename)) << " Writing to gz files is not supported!";

		FileHandle::CreateDirectoryIfNotExisting(mappedFilename);
		_ofstream = new ofstream(mappedFilename, std::ios_base::out);
	}

}

void FileHandle::CreateDirectoryIfNotExisting(string path)
{
	filesystem::path file(path);
	filesystem::path dir = file.parent_path();
	if (dir.string() != "")
	{
		if(!filesystem::exists(dir))
		{
			if (filesystem::create_directories(dir))
			{
				DVLOG(1) << ".... path " << path << " did not exist and was created!";
			}
		}
	}
}

FileHandle::~FileHandle()
{
	if (_ifstream != nullptr)
	{
		_ifstream->close();
		delete _ifstream;
	}
	if (_ofstream != nullptr)
	{
		_ofstream->close();
		delete _ofstream;
	}
}


string FileHandle::GetCanonicalFilename()
{
	return filesystem::canonical(_filename).string();
}

string FileHandle::GetCanonicalFilename(string filename)
{
	return filesystem::canonical(filename).string();
}


std::istream& FileHandle::GetStream()
{
	DASSERT(_forReading) << " Stream has not been configured for reading) << use GetOutStream instead";
	if (_zipMode)
	{
		return _inFilter;
	}
	else
	{
		return *_ifstream;
	}
}


ofstream& FileHandle::GetOutStream()
{
	DASSERT(!_forReading) << " Stream has not been configured for writing) << use GetStream instead";
	return *_ofstream;
}
