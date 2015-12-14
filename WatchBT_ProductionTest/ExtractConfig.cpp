using System;
#include "stdafx.h"
#include "ExportFunction.h"
#include "ExtractConfig.h"

int MatchNameWithConfigFile(char* compare, string file_name){

	string config_name, config_value;

	ifstream ConfigFile (file_name.c_str());
	if (!ConfigFile.fail()){
		WriteLogFile("%s Loading...", file_name.c_str());
		//OutputDebugString("Loding\n");
		if (ConfigFile.is_open())
		{	
			while ( !ConfigFile.eof())
			{
				ConfigFile >> config_name;
				OutputDebugString(config_name.c_str());
				if (compare == config_name){
					WriteLogFile("%s Matched\n", config_name);
					return 1;
				}
			}
		}	
		WriteLogFile("%s is not matched\n", config_name);
		//OutputDebugString("Not Matched\n");
		return 0;
	} else {
		WriteLogFile("%s is not found!", file_name.c_str()); 
		//OutputDebugString("not found!\n");
		return 0;
	}
	return 0;
}

