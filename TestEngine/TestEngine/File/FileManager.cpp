#include "FileManager.h"

#include <algorithm>
#include <iostream>
#include <string>

#include <Windows.h>

#include <fstream>
#include <vector>
#include <list>



namespace Core
{
	// Temp functions


	std::string TempGetSystemPath()
	{
		TCHAR theExePath[2048];
		DWORD fileLength = GetModuleFileName(nullptr, theExePath, 2048);
		std::string exeString = std::string();
		for (size_t i = 0; i < fileLength; ++i)
		{
			exeString += theExePath[i];
		}

		unsigned lastSlash = exeString.find_last_of("/\\");

		return exeString.substr(0, lastSlash);
	}

	std::vector<std::string> TempGetAllFoldersInDirectory(const std::string& theDirectory)
	{
		std::vector<std::string> result;

		// Grab the first file
		WIN32_FIND_DATA fileFound;
		std::string origDirName = theDirectory;
		origDirName += "\\*";
		std::basic_string<TCHAR> theDir = std::basic_string<TCHAR>(origDirName.begin(), origDirName.end());
		HANDLE theHandle = FindFirstFile(theDir.c_str(), &fileFound);

		if (theHandle != INVALID_HANDLE_VALUE)
		{

			// Run through all the files in the directory
			while (FindNextFile(theHandle, &fileFound) != 0)
			{
				// If its NOT a directory, add it in (and if its not ..)
				if (fileFound.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					std::string theString = theDirectory + "\\";
					TCHAR* tCharPtr = fileFound.cFileName;
					while (*tCharPtr)theString += *tCharPtr++;
					if (theString != theDirectory + "\\..")
						result.push_back(theString);
				}
			}

		}

		return result;
	}

	std::vector<std::string> TempGetAllFilesInDirectory(const std::string& theDirectory)
	{
		std::vector<std::string> result;

		// Grab the first file
		WIN32_FIND_DATA fileFound;
		std::string origDirName = theDirectory;
		origDirName += "\\*";
		std::basic_string<TCHAR> theDir = std::basic_string<TCHAR>(origDirName.begin(), origDirName.end());
		HANDLE theHandle = FindFirstFile(theDir.c_str(), &fileFound);

		if (theHandle != INVALID_HANDLE_VALUE)
		{

			// Run through all the files in the directory
			while (FindNextFile(theHandle, &fileFound) != 0)
			{
				// If its NOT a directory, add it in
				if (!(fileFound.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					std::string theString = theDirectory + "\\";
					TCHAR* tCharPtr = fileFound.cFileName;
					while (*tCharPtr)theString += *tCharPtr++;
					result.push_back(theString);
				}
			}

		}

		return result;
	}

	Folder::Folder(unsigned theID,
		const std::string& theFolderName,
		const std::string& theRelPath,
		const std::string& theFullName) :
		_folderID(theID),
		_folderName(theFolderName),
		_folderPath(theRelPath),
		_fullName(theFullName),
		_resources(),
		_subFolders() {}
	Folder::Folder(const Folder& rhs) :
		_folderID(rhs._folderID),
		_folderName(rhs._folderName),
		_folderPath(rhs._folderPath),
		_fullName(rhs._fullName),
		_resources(rhs._resources),
		_subFolders(rhs._subFolders) {}
	Folder::Folder(Folder&& rhs) :
		_folderID(rhs._folderID),
		_folderName(),
		_fullName(),
		_folderPath(),
		_resources(),
		_subFolders() {
		std::swap(_folderName, rhs._folderName);
		std::swap(_fullName, rhs._fullName);
		std::swap(_folderPath, rhs._folderPath);
		std::swap(_resources, rhs._resources);
		std::swap(_subFolders, rhs._subFolders);
	}
	// Grabs a resource from the folder (will return an invalid resource if it failed)
	Resource Folder::GetResource(std::string fileName, std::string fileType, bool searchSubfolders)
	{
		// Run through each of your files
		for (auto resourceIte = _resources.begin(); resourceIte != _resources.end(); ++resourceIte)
		{
			if ((*resourceIte).fileName == fileName && (fileType.empty() || (*resourceIte).fileType == fileType))
				return (*resourceIte);
		}
		// failed to find it in your files, search through your sub folders
		if (searchSubfolders)
		{
			for (auto folderIte = _subFolders.begin(); folderIte != _subFolders.end(); ++folderIte)
			{
				Resource foundResource = (*folderIte).GetResource(fileName, fileType, searchSubfolders);
				if (foundResource.IsValid()) return foundResource;
			}
		}
		// Failed, return blank
		return gEmptyResource;
	}

	Folder& Folder::GetFolder(std::string folderName)
	{
		if (folderName.empty()) return gEmptyFolder;
		// Run through each of your files
		for (auto folderIte = _subFolders.begin(); folderIte != _subFolders.end(); ++folderIte)
		{
			if ((*folderIte).GetName() == folderName) return (*folderIte);
		}
		// failed to find it in your files, search through your sub folders
		for (auto folderIte = _subFolders.begin(); folderIte != _subFolders.end(); ++folderIte)
		{
			Folder& foundFolder = (*folderIte).GetFolder(folderName);
			if (foundFolder.IsValid()) return foundFolder;
		}
		// Failed, return blank
		return gEmptyFolder;
	}

	void Folder::Print(unsigned indentLevel) const
	{
		// Print the name of the folder first
		std::cout << std::string(indentLevel, '-') << _folderPath << _folderName << std::endl;
		// Run through all the files first
		for (int i = 0; i < _resources.size(); ++i)
		{
			std::cout << std::string(indentLevel+1, '-') << _resources[i].fileName << "." << _resources[i].fileType << std::endl;
		}
		// Then run through the folders
		for (int i = 0; i < _subFolders.size(); ++i)
		{
			_subFolders[i].Print(indentLevel + 1);
		}
	}

	bool Resource::operator<(const Resource& rhs)
	{
		return fileID < rhs.fileID;
	}

	bool Resource::SameAs(const Resource& rhs)
	{
		return (fileName == rhs.fileName && fileType == rhs.fileType);
	}

	std::string Resource::GetFullLocation() const
	{
		return fullName;
	}

	// Call to set the relative resource path (by default its gDEFAULT_RELATIVE_RESOURCE_PATH)
	void FileManager::SetRelativeResourcePath(std::string resPath)
	{
		_mainPath = TempGetSystemPath() + resPath;
	}

	// Starts up the resource manager, returns MgrStatus. (CALL ONCE IN BEGINNING OF PROGRAM ONLY)
	bool FileManager::_Init()
	{
		////std::cout << "Starting up: Resource Manager" << std::endl;

		//TempReadConfigFile()


		// Get the main path first
		if (_mainPath.empty())
			_mainPath = TempGetSystemPath() + gDEFAULT_RELATIVE_RESOURCE_PATH;
		// For grabbing out the relative path later
		//unsigned mainPathLength = _mainPath.length();
		// Begin grabbing all paths
		std::vector<std::string> directoriesLeft = TempGetAllFoldersInDirectory(_mainPath);
		std::string currentPath = _mainPath;
		directoriesLeft.push_back(currentPath);

		//unsigned resourceNum = 0;
		//_resourceList = std::vector<Resource>();
		_folderList = std::vector<Folder>();
		//std::list<Resource> resourceList;
		// Keep grabbing until all the child directories have been checked

		// Create the base folder
		Folder baseFolder(++_resourceNum, "", "\\", _mainPath);
		// Then run through
		_GetAllFilesInFolder(baseFolder);
		// Then stuff it in
		_folderList.push_back(baseFolder);

		//_resourceList = resourceList.begin();
		//_resourceNum = resourceNum;

		/*
		// Load up the old file vec
		std::vector<std::pair<std::string, L_Time::Time>> oldFileTimeVec;
		L_Serialize::Serializer_ResourceInfoFile().Load(GetResource("Files", "xml").GetFullLocation(), oldFileTimeVec);

		// Run through all the files, and get the time
		std::vector<std::pair<std::string, L_Time::Time>> fileTimeVec;
		std::vector<Folder> foldersLeft;
		foldersLeft.push_back(_folderList.front());
		while (!foldersLeft.empty())
		{
			std::string folderName = foldersLeft.back().GetName();
			foldersLeft.pop_back();
			Folder& theFolder = GetFolder(folderName);

			std::vector<Resource>& resourcesLeft = theFolder._resources;
			size_t resSize = resourcesLeft.size();
			for (size_t i = 0; i < resSize; ++i)
			{
				// Grab the resource
				Resource& theResource = resourcesLeft[i];
				// Grab the time
				L_Time::Time theTime;
				GetTimeModified(theResource, theTime);
				// Grab the name
				std::string theName = theResource.relPath + theResource.fileName + "." + theResource.fileType;
				// Then stuff it into the vec
				fileTimeVec.push_back(std::make_pair(theName, theTime));

				// And set them to modified if their time was changed
				auto oldResIte = std::find_if(oldFileTimeVec.begin(), oldFileTimeVec.end(), [this, theName](std::pair<std::string, L_Time::Time> thePair) {
					return thePair.first == theName;
				});
				if (oldResIte != oldFileTimeVec.end() && (*oldResIte).second == theTime)
				{
					theResource._wasModified = false;
				}
			}
			std::vector<Folder>& theFolders = theFolder._subFolders;
			for (auto folderIte = theFolders.begin(); folderIte != theFolders.end(); ++folderIte)
			{
				foldersLeft.push_back((*folderIte));
			}

		}
		// Then save it
#ifndef FINAL
		L_Serialize::Serializer_ResourceInfoFile().Save(GetPath() + std::string("Files.xml"), fileTimeVec);
#endif
*/

		return true;
	}

	// Closes the resource manager, clears all resources, returns MgrStatus (CALL ONCE AT END OF PROGRAM ONLY)
	void FileManager::_ShutDown()
	{
		//std::cout << "Shutting down: Resource Manager" << std::endl;

		//_resourceList.clear();
		_folderList.clear();
		_resourceNum = 0;
		_mainPath = std::string();
	}

	// Retrieve a resource by ID
	Resource FileManager::GetResource(unsigned ID)
	{
		/*
		// Search through list, returning first resource which fits criteria
		auto ite = std::find_if(_resourceList.begin(), _resourceList.end(),
		[ID](Resource r){
		if (r.fileID == ID) return true;
		else return false;
		});
		if (ite != _resourceList.end()) return *ite;
		// If it couldn't find it, return an empty resource
		else
		{
		return gEmptyResource;
		}
		*/
		return _folderList.front().GetResource([&ID](Resource& theResource)
		{
			return theResource.fileID == ID;
		});
	}

	// Finds a folder with the name
	Folder& FileManager::GetFolder(std::string folderName)
	{
		if (folderName.empty()) return _folderList.front();
		else return _folderList.front().GetFolder(folderName);
	}

	// Retrieve a resource by name (will return the first one found)
	Resource FileManager::GetResource(std::string fileName, std::string fileType, std::string filePath)
	{
		/*
		// Search through list, returning first resource which fits criteria
		auto ite = std::find_if(_resourceList.begin(), _resourceList.end(),
		[fileName, fileType, filePath](Resource r){
		if (r.fileName == fileName && (fileType.empty() || (!fileType.empty() && r.fileType == fileType)) && (filePath.empty() || (!filePath.empty() && r.relPath == ("\\" + filePath + "\\")))) return true;
		else return false;
		});
		if (ite != _resourceList.end()) return *ite;
		// If it couldn't find it, return an empty resource
		else
		{
		return gEmptyResource;
		}
		*/

		if (filePath.empty())
		{
			return _folderList.front().GetResource(fileName, fileType, true);
		}
		else
		{
			Folder& theFolder = _folderList.front().GetFolder([&filePath](Folder& theFolder)
			{
				return theFolder.GetName() == filePath;
			});
			if (theFolder == gEmptyFolder) return gEmptyResource;
			else return theFolder.GetResource(fileName, fileType);
		}


	}

	// Retrieve all resources of type theType
	std::vector<Resource> FileManager::GetResourcesOfType(std::string theType, std::string filePath)
	{
		/*
		std::vector<Resource> ret = std::vector<Resource>();
		// Search through list, stuffing all resources which fit the type into ret
		std::for_each(_resourceList.begin(), _resourceList.end(),
		[&ret, &theType, &filePath](Resource r){
		if (r.fileType == theType && (filePath.empty() || r.relPath == ("\\" + filePath + "\\"))) ret.push_back(r);
		});
		return ret;
		*/

		if (filePath.empty())
		{
			return _folderList.front().GetResources([&theType](Resource& theResource)
			{
				return theResource.fileType == theType;
			});
		}
		else
		{
			// Find the folder with the file path
			Folder& theFolder = _folderList.front().GetFolder(filePath);
			if (theFolder.IsValid())
			{
				return theFolder.GetResources([&theType](Resource& theResource) {
					return theResource.fileType == theType;
				});
			}
			else return std::vector<Resource>();
		}

	}

	// Retrieve all resources in path thePath, if getInChildren is true, will get all resources in child paths too (Note: thePath is relative to main)
	std::vector<Resource> FileManager::GetResourcesInPath(std::string thePath, bool getInChildren)
	{
		/*
		std::vector<Resource> ret = std::vector<Resource>();
		// Search through list, stuffing all resources which have the path name
		std::for_each(_resourceList.begin(), _resourceList.end(),
		[&ret, thePath, getInChildren](Resource r){
		if (r.relPath == thePath) ret.push_back(r);
		// If looking in children, check if that path exists
		else if (getInChildren)
		{
		// If the first part is the same, its a child path
		if (std::equal(thePath.begin(), thePath.end(), r.relPath.begin())) ret.push_back(r);
		}
		});
		return ret;
		*/

		// Grab the folder
		Folder& theFolder = _folderList.front().GetFolder(thePath);
		if (theFolder.IsValid())
			return theFolder.GetResources();
		else
			return std::vector<Resource>();
	}

	// Returns the total number of resources of type theType
	unsigned FileManager::GetResourceNumberOfType(std::string theType)
	{
		return GetResourcesOfType(theType).size();
	}

	// Returns the total number of resource in path thePath, if getInChildren is true, will get all resources in child paths too
	unsigned FileManager::GetResourceNumberInPath(std::string thePath, bool getInChildren)
	{
		return GetResourcesInPath(thePath, getInChildren).size();
	}

	// Returns the RELATIVE directory with the given name (if there are multiple, it will give the first one)
	Folder& FileManager::GetDirectory(std::string directoryName)
	{
		/*
		// Search through all the paths, looking for the directory name
		auto ite = std::find_if(_thePaths.begin(), _thePaths.end(), [&directoryName, this](std::string& theDirectory)
		{
		return ExtractFileName(theDirectory) == ("\\" + directoryName + "\\");
		});
		// If it found it, return the name
		if (ite != _thePaths.end()) return *ite;
		// If not, return empty string
		else return std::string();
		*/

		return _folderList.front().GetFolder(directoryName);

	}

	// Prints out all the resources, formatted
	void FileManager::Print()
	{
		/*
		std::string thePath = _mainPath;
		std::cout << _mainPath << std::endl;
		//Run through entire resource list and print out info
		*/
		/*
		std::for_each(_resourceList.begin(), _resourceList.end(),
		[thePath](Resource r){
		std::cout << r.relPath << r.fileName << "." << r.fileType << std::endl;
		});
		*/

		// Run through all the folders
		for (int i = 0; i < _folderList.size(); ++i)
		{
			Folder& folder = _folderList[i];
			folder.Print();
		}
		
	}

	// Load a text file (Doesn't have to be a txt file, it will just read the file as a text file)
	std::string FileManager::LoadTextFile(const Resource& theResource)
	{
		// The resultant string
		std::string theTextString = std::string();
		// Open up the file
		std::ifstream ifs(theResource.fullName);
		// Copy over the entire file
		if (ifs.is_open()) {
			while (!ifs.eof()) theTextString += static_cast<char>(ifs.get());
		}
		theTextString.pop_back();
		// Close the file
		ifs.close();
		// Return the string
		return theTextString;


	}

	std::string FileManager::LoadTextFile(std::string fileName, std::string fileType)
	{
		Resource theResource = GetResource(fileName, fileType);
		if (theResource.SameAs(gEmptyResource)) return std::string();
		else return LoadTextFile(theResource);
	}

	std::string FileManager::LoadTextFile(unsigned ID)
	{
		Resource theResource = GetResource(ID);
		if (theResource.SameAs(gEmptyResource)) return std::string();
		else return LoadTextFile(theResource);
	}

	// Extracts a file type from a full file name string (without the .)
	std::string FileManager::ExtractFileType(std::string fileName)
	{
		if (fileName.find_last_of('.') == std::string::npos) return std::string();
		return fileName.substr(fileName.find_last_of('.') + 1, fileName.size() - fileName.find_last_of('.'));
	}

	// Extracts a file name from a full file name string (without the .)
	std::string FileManager::ExtractFileName(std::string fileName, bool withPath)
	{
		// Grab with path first
		std::string fileWithPath = fileName.substr(0, fileName.find_last_of('.'));
		if (withPath) return fileWithPath;
		// If no need path, substring again
		std::string fileNoPath = fileWithPath.substr(fileName.find_last_of("/\\") + 1, fileWithPath.size() - (fileName.find_last_of("/\\") + 1));
		return fileNoPath;
	}

	// Adds in a new file information into the resource manager (if it doesn't exist)
	Resource FileManager::AddResourceIntoManager(std::string newFileNameWithFileType, std::string directory)
	{
		// Check whteher can find it first
		Resource theResource = GetResource(ExtractFileName(newFileNameWithFileType), ExtractFileType(newFileNameWithFileType), directory);
		// Just pushes it into the resources if cannot
		if (!theResource.IsValid())
		{
			/*
			_resourceList.push_back(Resource(_resourceNum++, ExtractFileName(newFileNameWithFileType), ExtractFileType(newFileNameWithFileType), directory, _mainPath + directory + newFileNameWithFileType));
			return _resourceList.back();
			*/
			Folder& theFolder = GetFolder(directory);
			if (theFolder.IsValid())
			{
				theFolder._resources.push_back(Resource(_resourceNum++, ExtractFileName(newFileNameWithFileType), ExtractFileType(newFileNameWithFileType), directory, _mainPath + directory + "\\" + newFileNameWithFileType));
				return theFolder._resources.back();
			}
			else
				return gEmptyResource;
		}
		else return theResource;
	}

	
	// Recursive function to grab all the files in the entire folder
	void FileManager::_GetAllFilesInFolder(Folder& parentFolder)
	{
		// Grab all files the parent directory
		std::vector<std::string> allFiles = TempGetAllFilesInDirectory(parentFolder._fullName);
		// Run through each of them and stuff it in
		for (auto fileIte = allFiles.begin(); fileIte != allFiles.end(); ++fileIte)
		{
			std::string theFile = *fileIte;
			std::string filePath = theFile.substr(_mainPath.length(), theFile.find_last_of("/\\") - _mainPath.length() + 1);
			std::string fileNameAndType = theFile.substr(theFile.find_last_of("/\\") + 1, theFile.size());
			std::string fileType = fileNameAndType.substr(fileNameAndType.find_last_of('.') + 1, fileNameAndType.size());
			std::string fileName = fileNameAndType.substr(0, fileNameAndType.find_last_of("."));
			Resource newResource = Resource(++_resourceNum, fileName, fileType, filePath, theFile);
			parentFolder._resources.push_back(newResource);
		}
		// Then run through all the child directories
		std::vector<std::string> childDirectories = TempGetAllFoldersInDirectory(parentFolder._fullName);
		for (auto folderIte = childDirectories.begin(); folderIte != childDirectories.end(); ++folderIte)
		{
			// Create the folder
			std::string folderPath = (*folderIte).substr(_mainPath.length(), (*folderIte).find_last_of("/\\") - _mainPath.length() + 1);
			std::string folderName = (*folderIte).substr((*folderIte).find_last_of("/\\") + 1, (*folderIte).size());
			Folder newFolder(++_resourceNum, folderName, folderPath + folderName + "\\", (*folderIte));
			_GetAllFilesInFolder(newFolder);
			parentFolder._subFolders.push_back(newFolder);
		}
		// Done

	}

}

