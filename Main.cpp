#include <string>
#include <algorithm>
#include <cstring>
#include <string>
#include <iostream>
#include <ctime>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

#define MDPath "./MetaDataFile/All_snapshot_details.txt"
// #define MDPathLogFile "./LogFile/logfile.txt"
// #include "library.h"
#include "initialCopy.h"
#include "createSnapshot.h"



struct SnapShotMetaDataInformation
{
	string sourcePath;
	string destinationPath;
	string creationTimeStamp;
	string lastRunTime;
};

struct iequal
{
    bool operator()(int c1, int c2) const
    {
        return std::toupper(c1) == std::toupper(c2);
    }
};
 
bool iequals(const std::string& str1, const std::string& str2)
{
    return std::equal(str1.begin(), str1.end(), str2.begin(), iequal());
}

string getCurrentTimeZone()
{
    std::time_t rawtime;
    std::tm* timeinfo;
    char buffer[80];
    // char * buffer = (char *)malloc(sizeof(80*sizeof(char)));

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(buffer,80,"%Y-%m-%d-%H-%M-%S",timeinfo);
    std::puts(buffer);
	string finalresult(buffer);
	// delete buffer;
	return finalresult;
}

char * getCurrentTime()
{
    std::time_t result = std::time(nullptr);
    return std::asctime(std::localtime(&result));
}

SnapShotMetaDataInformation setDetailsOfMetaData(string sourcePath,string destinationPath)
{
	struct SnapShotMetaDataInformation information;
	information.sourcePath=sourcePath;
	information.destinationPath=destinationPath;
	information.creationTimeStamp=getCurrentTimeZone();
	information.lastRunTime=information.creationTimeStamp;
	return information;
}	

string PrepareData(struct SnapShotMetaDataInformation info)
{
	string strData;
	//if(info)
	{
		strData.append(info.sourcePath);
		strData.append("\t");
		strData.append(info.destinationPath);
		strData.append("\t");
		strData.append(info.creationTimeStamp);
		strData.append("\t");
		strData.append(info.lastRunTime);
		strData.erase( std::remove(strData.begin(), strData.end(), '\r'), strData.end() );
		strData.erase( std::remove(strData.begin(), strData.end(), '\n'), strData.end() );
		strData.append("\n");
	}
	return strData;
}

bool writeToEndOfFile(string Data)
{
	bool success = false;
	if(Data!="")
	{
		std::ofstream out;
 		out.open(MDPath, std::ios::app);
 		out << Data;
		out.close();
	}
	return success;
}

bool writeLog(string Data)
{
	bool success = false;
	if(Data!="")
	{
		std::ofstream out;
 		out.open(MDPathLogFile, std::ios::app);
 		out << getCurrentTime() << ":" << Data << endl;
		out.close();
	}
	return success;
}

bool endsWith(const std::string& s, const std::string& suffix)
{
    return s.size() >= suffix.size() &&
           s.substr(s.size() - suffix.size()) == suffix;
}

std::vector<std::string> split(const std::string& s, const std::string& delimiter, const bool& removeEmptyEntries = false)
{
    std::vector<std::string> tokens;

    for (size_t start = 0, end; start < s.length(); start = end + delimiter.length())
    {
         size_t position = s.find(delimiter, start);
         end = position != string::npos ? position : s.length();

         std::string token = s.substr(start, end - start);
         if (!removeEmptyEntries || !token.empty())
         {
             tokens.push_back(token);
         }
    }

    if (!removeEmptyEntries &&
        (s.empty() || endsWith(s, delimiter)))
    {
        tokens.push_back("");
    }

    return tokens;
}

// remove the line from file with name as passed sourceName 
bool replaceCurrentLineInFile(string sourceNameToremove)
{

	writeLog("Entering into replaceCurrentLineInFile");

	string deleteline;
	std::string line;

	std::ifstream file(MDPath);
	
	//ofstream temp;
	//temp.open("temp.txt");
	
	vector<string> tokens;
	bool firstLine=false;

	vector<string> newfileLineToDumpForMetadata;

	while (getline(file,line))
	{
		// if(firstLine==false)
		// {
		// 	firstLine=true;
		// 	//temp << line <<endl;
		// 	line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
		// 	line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );
		// 	newfileLineToDumpForMetadata.push_back(line);
		// 	continue;
		// }

 		tokens = split(line,"\t");		
		if(tokens.size()>0 && (tokens[0] != sourceNameToremove))
		{
			struct SnapShotMetaDataInformation information;
			information.sourcePath=tokens[0];
			information.destinationPath=tokens[1];
			information.creationTimeStamp=tokens[2];
			information.lastRunTime=tokens[3];
			string strDataToWrite = PrepareData(information);

			strDataToWrite.erase( std::remove(strDataToWrite.begin(), strDataToWrite.end(), '\r'), strDataToWrite.end() );
			strDataToWrite.erase( std::remove(strDataToWrite.begin(), strDataToWrite.end(), '\n'), strDataToWrite.end() );
			//temp << strDataToWrite ;
			newfileLineToDumpForMetadata.push_back(strDataToWrite);
		}
	}

	file.close();

	std::ofstream out;
	out.open(MDPath);

	for(auto itemContent : newfileLineToDumpForMetadata)
	{
		out << itemContent << endl;
	}
	out.close();

	writeLog("Exiting into replaceCurrentLineInFile");

	return true;
}

// remove the line from file with name as passed sourceName 
bool checkExistenceOfSource(string sourceNameToCheck)
{

	writeLog("Entering into checkExistenceOfSource");

	std::string line;
	std::ifstream file(MDPath);
	vector<string> tokens;
	bool firstLine=false;

	while (getline(file,line))
	{
		// if(firstLine==false)
		// {
		// 	firstLine=true;
		// 	continue;
		// }

 		tokens = split(line,"\t");	
		//cout << tokens[0]  << ":" << sourceNameToremove << endl;	
		if(tokens.size()>0 && tokens[0] == sourceNameToCheck)
		{
			writeLog("Exiting into checkExistenceOfSource");
			return true;
		}
	}

	writeLog("Exiting into checkExistenceOfSource");

	return false;
}

void performCopyOperation(string sourceParam,string destinationParam){

	writeLog("Exiting into performCopyOperation");

	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL) 
	{
		printf("Current working dir: %s\n", cwd);
	}

    char source[sourceParam.length()+1];
    char destination[destinationParam.length()+1];
    strcpy(source,sourceParam.c_str());strcpy(destination,destinationParam.c_str());
    // copy(source,destination,1);
    CopyFunctionality copyFunctionalityObj;
    copyFunctionalityObj.copy(source,destination,1);
    chdir(cwd);

    writeLog("Exiting into performCopyOperation");

}

void performCreateSnapShotFileOperation(string sourceParam,string destinationParam){

	writeLog("Entering into performCreateSnapShotFileOperation");

	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL) 
	{
		printf("Current working dir: %s\n", cwd);
	}

    char source[sourceParam.length()+1];
    char destination[destinationParam.length()+1];
    strcpy(source,sourceParam.c_str());strcpy(destination,destinationParam.c_str());
    
    CreateSnapShotClass CreateSnapShotClassObject;
    CreateSnapShotClassObject.prepareMetadataForSnapshot(source,destination,1);
    CreateSnapShotClassObject.CreateSnapshotFile(source,destination);
    chdir(cwd);

    writeLog("Exiting into performCreateSnapShotFileOperation");
}


bool updateLastUpdatedTimeForSnapShot(map<string,string> DataWithValue)
{

	writeLog("Entering into updateLastUpdatedTimeForSnapShot");

	string deleteline;
	std::string line;

	std::ifstream file(MDPath);
	
	//ofstream temp;
	//temp.open("temp.txt");
	
	vector<string> tokens;
	bool firstLine=false;

	vector<string> newfileLineToDumpForMetadata;

	while (getline(file,line))
	{
		// if(firstLine==false)
		// {
		// 	firstLine=true;
		// 	//temp << line <<endl;
		// 	line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
		// 	line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );
		// 	newfileLineToDumpForMetadata.push_back(line);
		// 	continue;
		// }

 		tokens = split(line,"\t");		
		if(tokens.size()>0 && DataWithValue.find(tokens[0])!=DataWithValue.end())
		{
			struct SnapShotMetaDataInformation information;
			information.sourcePath=tokens[0];
			information.destinationPath=tokens[1];
			information.creationTimeStamp=tokens[2];
			information.lastRunTime=DataWithValue[tokens[0]];
			string strDataToWrite = PrepareData(information);
			//temp << strDataToWrite ;
			strDataToWrite.erase( std::remove(strDataToWrite.begin(), strDataToWrite.end(), '\r'), strDataToWrite.end() );
			strDataToWrite.erase( std::remove(strDataToWrite.begin(), strDataToWrite.end(), '\n'), strDataToWrite.end() );

			newfileLineToDumpForMetadata.push_back(strDataToWrite);
		}
		else
		{
			//temp << line <<endl;
			line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
			line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );
			
			newfileLineToDumpForMetadata.push_back(line);
		}
	}

	file.close();

	std::ofstream out;
	out.open(MDPath);

	for(auto itemContent : newfileLineToDumpForMetadata)
	{
		//if(itemContent!="")
			itemContent=itemContent+"\n";
			out << itemContent;
	}
	out.close();

	//temp.close();
	
	//remove(MDPath);
	//rename("temp.txt",MDPath);

	writeLog("Exiting into updateLastUpdatedTimeForSnapShot");

	return true;
}

//110 Success With Addition into File 
//120 Success With Removing of Snapshot 
//130 Scuccess ; existence of Source Directory already in snapshot file 
//140 User is trying to remove non-existed Snapshot
int main(int argc,char *argv[])
{	

	writeLog("Entering into main of main.cpp");

	std::string strCommandName = argv[1];
	//cout << strCommandName << endl;
	
	if(iequals(strCommandName,"CreateSnapShot") && argc >= 3)
	{
		// Make an Entry in .snapshot__metadata_file
		struct SnapShotMetaDataInformation information = setDetailsOfMetaData(argv[2],argv[3]);
		
		if(checkExistenceOfSource(argv[2])==true)
		{	
			printf("Already snapshot of this directory is getting Created\n");
			return 130;
		}

		//$$$$$$$

		// Step 1:  Perform Copy 
		// Step 2 : Perform Createtion Of SnapShot File 
		// Step 3 : Entry in snapshot_metadata file 

		
		writeLog("Main : Performing Copy Operation ");
		performCopyOperation(information.sourcePath,information.destinationPath);
		writeLog("Main : Performing Copy Operation : Complete ");

		writeLog("Main : Creation of SnapShot File ");
		performCreateSnapShotFileOperation(information.sourcePath,information.destinationPath);
		writeLog("Main : Creation of SnapShot File : Complete ");

		writeLog("Main : Performing Entry in SnapShot Metadata File");
		string data = PrepareData(information);
		if(writeToEndOfFile(data)==true)
		{
			printf(" FIle Successfully Wirtten");
			return 110;
		}
		writeLog("Main : Performing Entry in SnapShot Metadata File : Complete ");				
	}

	//only for testing 
	if(strCommandName=="update")
	{
		// Call to Another method of restoring snapshot 

		 map<string,string> AllDetailsForTimeUpdate;
		 AllDetailsForTimeUpdate[argv[2]]=getCurrentTimeZone();
		 updateLastUpdatedTimeForSnapShot(AllDetailsForTimeUpdate);
	

	}

	//if(strCommandName=="stopSnapshotScheduler")
	if(iequals(strCommandName,"stopSnapshotScheduler") && argc >= 2)
	{

		if(checkExistenceOfSource(argv[2])==false)
		{	
			printf("User is trying to remove non-existed Snapshot\n");
			return 140;
		}

		// Delete an Entry in .snapshot__metadata_file
		string sourcePath = argv[2];
		if(replaceCurrentLineInFile(sourcePath)==true)
		{
			cout << "Success" << endl;

			return 120;
		}
	}


	writeLog("Exiting main of main.cpp");

	return 0;
	
}



/*
stopSnapshotScheduler "sourcePath"
createSnapShot "sourcePath" "destinationPath"
*/
