#include "physfs.h"

#include "pixelboost/debug/assert.h"
#include "pixelboost/debug/log.h"
#include "pixelboost/file/fileHelpers.h"
#include "pixelboost/file/fileSystem.h"

using namespace pb;

FileSystem* FileSystem::_Instance = 0;

File::File(PHYSFS_File* file)
{
    _File = file;
}

File::~File()
{
    PHYSFS_close(_File);
}

bool File::ReadAll(std::vector<unsigned char>& data)
{
    if (!_File)
        return false;
    
    long len;
    unsigned char* temp;
    len = PHYSFS_fileLength(_File);
    PHYSFS_seek(_File, 0);
    temp = new unsigned char[len+1];
    PHYSFS_readBytes(_File, temp, len);
    temp[len] = 0;
    
    data.assign(temp, temp+len);
    
    delete[] temp;
    
    return true;
}

bool File::ReadAll(std::string& data)
{
    if (!_File)
        return false;
    
    long len;
    char* temp;
    len = PHYSFS_fileLength(_File);
    PHYSFS_seek(_File, 0);
    temp = new char[len+1];
    PHYSFS_readBytes(_File, temp, len);
    temp[len] = 0;
    
    data.assign(temp, len);
    
    delete[] temp;
    
    return true;
}

bool File::Read(unsigned char* data, int length)
{
    PHYSFS_readBytes(_File, data, length);
    return true;
}

bool File::Read(char& data)
{
    PHYSFS_readBytes(_File, &data, sizeof(char));
    return true;
}

bool File::Read(float& data)
{
    PHYSFS_readBytes(_File, &data, sizeof(float));
    return true;
}

bool File::Read(short& data)
{
    PHYSFS_readBytes(_File, &data, sizeof(short));
    return true;
}

bool File::Read(int& data)
{
    PHYSFS_readBytes(_File, &data, sizeof(int));
    return true;
}

bool File::Read(bool& data)
{
    PHYSFS_readBytes(_File, &data, sizeof(bool));
    return true;
}

bool File::Write(const std::vector<unsigned char>& data)
{
    PHYSFS_writeBytes(_File, &data[0], data.size());
    return true;
}

bool File::Write(const unsigned char* data, int length)
{
    PHYSFS_writeBytes(_File, data, length);
    return true;
}

bool File::Write(const std::string& data)
{
    PHYSFS_writeBytes(_File, data.c_str(), data.length());
    return true;
}

bool File::Write(const char& data)
{
    PHYSFS_writeBytes(_File, &data, sizeof(char));
    return true;
}

bool File::Write(const float& data)
{
    PHYSFS_writeBytes(_File, &data, sizeof(float));
    return true;
}

bool File::Write(const short& data)
{
    PHYSFS_writeBytes(_File, &data, sizeof(short));
    return true;
}

bool File::Write(const int& data)
{
    PHYSFS_writeBytes(_File, &data, sizeof(int));
    return true;
}

bool File::Write(const bool& data)
{
    PHYSFS_writeBytes(_File, &data, sizeof(bool));
    return true;
}

bool File::Seek(SeekMode mode, int offset)
{
    int filePosition;
    
    switch (mode)
    {
        case kFileSeekStart:
            filePosition = offset;
            break;
        case kFileSeekCurrent:
            filePosition = PHYSFS_tell(_File) + offset;
            break;
        case kFileSeekEnd:
            filePosition = PHYSFS_fileLength(_File) + offset;
            break;
    }
    
    return PHYSFS_seek(_File, filePosition) == 0;
}

FileSystem::FileSystem(const char* appPath)
{
    PbAssert(!_Instance);
    
    _Instance = this;
    
    PHYSFS_init(appPath);
    
#ifdef PIXELBOOST_PLATFORM_ANDROID
    PbLogDebug("pb.file.system", "Mounting file system root (%s)", pb::FileHelpers::GetBundlePath().c_str());
    PHYSFS_mount(pb::FileHelpers::GetBundlePath().c_str(), "/", 0);
#else
    PbLogDebug("pb.file.system", "Mounting file system root (%s)", (pb::FileHelpers::GetBundlePath() + "/data/").c_str());
    PHYSFS_mount((pb::FileHelpers::GetBundlePath() + "/data/").c_str(), "/", 0);
#endif
    
    PbLogDebug("pb.file.system", "Setting write directory (%s)", pb::FileHelpers::GetSavePath().c_str());
    PHYSFS_setWriteDir(pb::FileHelpers::GetSavePath().c_str());
    
    MountReadLocation(pb::FileHelpers::GetSavePath(), "/", true);
    MountReadLocation(pb::FileHelpers::GetSavePath() + "asset_override/", "/", true);
}

FileSystem::~FileSystem()
{
    
}

FileSystem* FileSystem::Instance()
{
    return _Instance;
}

void FileSystem::OverrideWriteDirectory(const std::string& path)
{
    PbLogDebug("pb.file.system", "Overwriting write location (%s)", path.c_str());
    PHYSFS_setWriteDir(path.c_str());
}

void FileSystem::MountReadLocation(const std::string& path, const std::string& mountPoint, bool prepend)
{
    PbLogDebug("pb.file.system", "Mounting read location (%s) to (%s)", path.c_str(), mountPoint.c_str());
    PHYSFS_mount(path.c_str(), mountPoint.c_str(), prepend ? 0 : 1);
}

std::shared_ptr<File> FileSystem::OpenFile(const std::string& path, FileMode mode)
{
    PHYSFS_file* handle = 0;
    
    if (path == "")
        return 0;
    
    switch (mode)
    {
        case kFileModeRead:
        {
            handle = PHYSFS_openRead(path.c_str());
            // Special case for Android, since the assets are contained within a subfolder within the zip we have
            // to try opening with that prefix. We try without the prefix first to allow for user specified files
            // to override the bundle assets. This isn't perfect, as it effectively disallows assets to be in the
            // bundle, but it will do until PHYSFS is modified to allow a subdirectory within a zip to be mounted
            #ifdef PIXELBOOST_PLATFORM_ANDROID
            if (!handle)
                handle = PHYSFS_openRead(("assets/data/"+path).c_str());
            #endif
            break;
        }
        case kFileModeWrite:
            handle = PHYSFS_openWrite(path.c_str());
            break;
        case kFileModeAppend:
            handle = PHYSFS_openAppend(path.c_str());
            break;
    }
    
    if (!handle)
        return 0;
    
    return std::shared_ptr<File>(new File(handle));
}

bool FileSystem::QueryFile(FileInfo& info, const std::string& path)
{
    PbAssert(!"Not yet implemented");
    
    return false;
}

bool FileSystem::CreateDirectory(const std::string& path)
{
    return PHYSFS_mkdir(path.c_str());
}
