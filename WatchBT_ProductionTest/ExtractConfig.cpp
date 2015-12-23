using System;
using System.Threading;
using System.Windows.Forms;

#include "stdafx.h"
#include "ExportFunction.h"
#include "ExtractConfig.h"

void smooth_sleep(int sec){
	for (int i = 0; i < 1000 *sec; i++){
		//this.Refresh();
		//System.Threading.Thread.Sleep(100);
		Sleep(1);
	}
}
void DebugPrint(const char *str, ...){
  char buf[2048];

  va_list ptr;
  va_start(ptr,str);
  vsprintf(buf,str,ptr);
  OutputDebugString(buf);

}
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

