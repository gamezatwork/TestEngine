#pragma once

#include <string>
#include <vector>

#include <iostream>
#include <fstream>

#include "..\Core\IManager.h"

const std::string gDEFAULT_RELATIVE_RESOURCE_PATH = "\\..\\..\\Resources";

namespace Core
{
	std::string TempGetSystemPath();


	class FileManager;



	class Resource
	{
	public:
		Resource(unsigned theID,
			const std::string& theFileName,
			const std::string& theFileType,
			const std::string& theRelPath,
			const std::string& theFullName) :
			fileID(theID),
			fileName(theFileName),
			fileType(theFileType),
			relPath(theRelPath),
			fullName(theFullName){}

		Resource(const Resource& rhs) :
			fileID(rhs.fileID),
			fileName(rhs.fileName),
			fileType(rhs.fileType),
			relPath(rhs.relPath),
			fullName(rhs.fullName) {}

		Resource(Resource&& rhs) :
			fileID(rhs.fileID),
			fileName(std::move(rhs.fileName)),
			fileType(std::move(rhs.fileType)),
			relPath(std::move(rhs.relPath)),
			fullName(std::move(rhs.fullName)) {}

		Resource& operator=(const Resource& rhs) = delete;

		bool operator==(const Resource& rhs) const { return fileID == rhs.fileID; }

		bool operator<(const Resource&);
		bool SameAs(const Resource&);

		bool IsValid() const { return fileID != 0; }

		// Returns the full location of the resource (the resource path + the full name)
		std::string GetFullLocation() const;

		const unsigned fileID;          // The ID of the file in the resource manager
		const std::string fileName;     // The name of the file (without the type)
		const std::string fileType;     // The type of the file (without .)
		const std::string relPath;      // The relative path of the file (based off the manager's main path)
		const std::string fullName;     // The full path name of the file

	private:

		friend class FileManager;
	};

	// A folder
	class Folder
	{
	public:
		Folder(unsigned theID,
			const std::string& theFolderName,
			const std::string& theRelPath,
			const std::string& theFullName);
		Folder(const Folder& rhs);
		Folder(Folder&& rhs);

		Folder& operator=(const Folder& rhs) = delete;

		bool operator==(const Folder& rhs) const { return _folderID == rhs._folderID; }

		// Checks whether it is valid
		bool IsValid() const { return _folderID != 0; }
		// Grabs a resource from the folder (will return an invalid resource if it failed)
		Resource GetResource(std::string fileName, std::string fileType = "", bool searchSubfolders = true);
		// Grabs a resource from the folder which fulfils the predicate
		template <typename PredType>
		Resource GetResource(PredType thePred, bool searchSubFolders = true)
		{
			// Run through each of your files
			for (auto resourceIte = _resources.begin(); resourceIte != _resources.end(); ++resourceIte)
			{
				if (thePred(*resourceIte)) return (*resourceIte);
			}
			// failed to find it in your files, search through your sub folders
			if (searchSubFolders)
			{
				for (auto folderIte = _subFolders.begin(); folderIte != _subFolders.end(); ++folderIte)
				{
					Resource foundResource = (*folderIte).GetResource(thePred, searchSubFolders);
					if (foundResource.IsValid()) return foundResource;
				}
			}
			// Failed, return blank
			return gEmptyResource;
		}
		// Grabs resources from the folder which fulfils the predicate
		template <typename PredType>
		std::vector<Resource> GetResources(PredType thePred, bool searchSubFolders = true)
		{
			std::vector<Resource> theResources;
			// Run through each of your files
			for (auto resourceIte = _resources.begin(); resourceIte != _resources.end(); ++resourceIte)
			{
				if (thePred(*resourceIte))
				{
					theResources.push_back(*resourceIte);
				}
			}
			// failed to find it in your files, search through your sub folders
			if (searchSubFolders)
			{
				for (auto folderIte = _subFolders.begin(); folderIte != _subFolders.end(); ++folderIte)
				{
					std::vector<Resource> foundResources = (*folderIte).GetResources(thePred, searchSubFolders);
					if (!foundResources.empty()) std::copy(foundResources.begin(), foundResources.end(), std::back_inserter(theResources));
				}
			}
			// Failed, return blank
			return theResources;
		}
		std::vector<Resource> GetResources() { return _resources; }
		Folder& GetFolder(std::string folderName);
		// Grabs a folder from the folder which fulfils the predicate
		template <typename PredType>
		Folder& GetFolder(PredType thePred)
		{
			// Run through each of your files
			for (auto folderIte = _subFolders.begin(); folderIte != _subFolders.end(); ++folderIte)
			{
				if (thePred(*folderIte)) return (*folderIte);
			}
			// failed to find it in your files, search through your sub folders
			for (auto folderIte = _subFolders.begin(); folderIte != _subFolders.end(); ++folderIte)
			{
				Folder& foundFolder = (*folderIte).GetFolder(thePred);
				if (foundFolder.IsValid()) return foundFolder;
			}
			// Failed, return blank
			return *this;
		}

		std::vector<Folder>& GetFolders() { return _subFolders; }
		std::string GetName() const {
			return _folderName;
		}
		std::string GetPath() const { return _folderPath; }

		void Print(unsigned indentLevel = 0) const;

	private:
		// The ID of the folder
		unsigned _folderID;
		// The name of the folder
		std::string _folderName;
		// The path of the folder
		std::string _folderPath;
		// The full name of thef older
		std::string _fullName;
		// All the files in the folder
		std::vector<Resource> _resources;
		// All the subfolders in the folder
		std::vector<Folder> _subFolders;

		friend class FileManager;
	};



	// The main resource manager (SINGLETON)
	class FileManager : public Core::IManager<FileManager>
	{
	public:

		// Call to set the relative resource path (by default its gDEFAULT_RELATIVE_RESOURCE_PATH)
		void SetRelativeResourcePath(std::string resPath);

		// Retrieve a resource by ID
		Resource GetResource(unsigned ID);

		// Retrieve a resource by name (will return the first one found)
		Resource GetResource(std::string fileName, std::string fileType = std::string(), std::string filePath = std::string());

		// Retrieve all resource IDs of type theType
		std::vector<Resource> GetResourcesOfType(std::string theType, std::string filePath = std::string());

		// Retrieve all resource IDs in path thePath, if getInChildren is true, will get all resources in child paths too
		std::vector<Resource> GetResourcesInPath(std::string thePath, bool getInChildren = false);

		// Finds a folder with the name
		Folder& GetFolder(std::string folderName);

		// Returns the main path of the resource manager
		std::string GetPath() { return _mainPath; }

		// Returns the RELATIVE directory with the given name (if there are multiple, it will give the first one)
		Folder& GetDirectory(std::string directoryName);

		// Returns the full list of all the directories
		//std::vector<std::string> GetDirectories() const { return _thePaths; }
		std::vector<Folder>& GetDirectories() { return _folderList; }

		// Returns the total number of resources
		unsigned GetResourceNumber() { return _resourceNum; }

		// Returns the total number of resources of type theType
		unsigned GetResourceNumberOfType(std::string theType);

		// Returns the total number of resource in path thePath, if getInChildren is true, will get all resources in child paths too
		unsigned GetResourceNumberInPath(std::string thePath, bool getInChildren = false);

		// Prints out all the resources, formatted
		void Print();

		// Load a text file (Doesn't have to be a txt file, it will just read the file as a text file)
		std::string LoadTextFile(const Resource& theResource);
		std::string LoadTextFile(std::string fileName, std::string fileType = std::string());
		std::string LoadTextFile(unsigned ID);

		// Extracts a file type from a full file name string (without the .)
		std::string ExtractFileType(std::string fileName);

		// Extracts a file name from a full file name string (without the .)
		std::string ExtractFileName(std::string fileName, bool withPath = true);

		// Adds a new file into the desired directory, and returns the Resource (NOTE: CLASS MUST HAVE OVERLOADED <<) (Note: New file will have diff IDs upon next load)
		template <typename InfoType>
		Resource AddFile(std::string newFileNameWithFileType, std::string directory, InfoType& theInfo)
		{
			// Open up the file stream
			std::ofstream fileStream;
			// "Open" up the file
			fileStream.open(_mainPath + directory + newFileNameWithFileType, std::ios::trunc | std::ios::binary | std::ios::out);
			// Stuff in the stuff
			fileStream << theInfo;
			// Then close it
			fileStream.close();

			// Next, check whether have the file already
			// Extract the file name
			std::string newFileName = ExtractFileName(newFileNameWithFileType);
			// Go to the path
			std::vector<Resource> resourcesInPath = GetResourcesInPath(directory);
			// Check whether it has the file name
			auto ite = std::find_if(resourcesInPath.begin(), resourcesInPath.end(), [&newFileName, this](Resource theResource)
			{
				return theResource.fileName == newFileName;
			});
			// If it exists, then do nothing (don't add a new resource)
			// If it doesn't, add a new resource
			if (ite == resourcesInPath.end())
			{
				_resourceList.push_back(Resource(_resourceNum++, newFileName, ExtractFileType(newFileNameWithFileType), directory, _mainPath + directory + newFileNameWithFileType));
				// And return the new resource
				return _resourceList.back();
			}
			// Since it exists, just return it
			else return (*ite);
		}

		// Adds in a new file information into the resource manager (if it doesn't exist)
		Resource AddResourceIntoManager(std::string newFileNameWithFileType, std::string directory);

	private:

		// Default Constructor (does nothing)
		FileManager() :_mainPath(""), _resourceNum(0) {}

		// The init function which is called at the start (returns false if it failed)
		virtual bool _Init();

		// The update function which is called every loop (OVERRIDE THIS ONE)
		virtual void _Update() {}

		// The shut down function which is called at the end (OVERRIDE THIS ONE)
		virtual void _ShutDown();

		// Recursive function to grab all the files in the entire folder
		void _GetAllFilesInFolder(Folder& parentFolder);

		// Contains the main path
		std::string _mainPath;

		// Contains the number of resources
		unsigned _resourceNum;

		// Contains all the folders which contain the resources
		std::vector<Folder> _folderList;

		// Contains all the paths (which were registered at the beginning)
		//std::vector<std::string> _thePaths;

		friend class Folder;
		friend class Core::IManager<FileManager>;


		/*
		// TEMP (Will be in system manager later)
		unsigned _winSizeWidth;
		unsigned _winSizeHeight;
		unsigned _defaultFPS;
		void TempReadConfigFile(std::string filePath);
		*/



	};

	static Resource gEmptyResource = Resource(0, std::string(), std::string(), std::string(), std::string());
	static Folder gEmptyFolder(0, std::string(), std::string(), std::string());

}