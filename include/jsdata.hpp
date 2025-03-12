#ifndef JSDATA_H
#define JSDATA_H
#include "nlohmann/json.hpp"

#include <cstring>
#include <iostream>
#include <fstream>

#include <wx/wx.h>

extern nlohmann::json jsData;
extern std::string jsFilename;
extern std::vector<wxString> wxCatNames;
extern wxString file;
extern wxString date;

extern int iImgID;
extern int iNumCats;
extern int w_img;
extern int h_img;

void openJSON(const std::string strDataSet);
void chooseJSON();
void closeJSON();

#endif
