#include "nlohmann/json.hpp"

#include <cstring>
#include <iostream>
#include <fstream>  

#include <wx/wx.h>


extern nlohmann::json jsData;
extern std::string jsFilename;
extern std::vector<wxString> wxCatNames;
extern int iImgID;
extern int iNumCats;

void openJSON(const std::string strDataSet);
void chooseJSON();
void closeJSON();