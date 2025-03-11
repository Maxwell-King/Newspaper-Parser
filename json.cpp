#include "json.hpp"

nlohmann::json jsData;
std::string jsFilename;
std::vector<wxString> wxCatNames;
int iImgID;
int iNumCats;

void openJSON(nlohmann::json &js, const std::string strDataSet) {
  std::filebuf fb;
  if (fb.open (strDataSet, std::ios::in)) {
    std::istream is(&fb);
    js = nlohmann::json::parse(is);
  }
}

void closeJSON() {
  std::ofstream oDataSet(jsFilename, std::ofstream::trunc);
  oDataSet << jsData.dump(-1) << std::endl;
  std::cerr << "Contents saved to " << jsFilename << std::endl;
}

void chooseJSON() {
  iNumCats = 0;
  nlohmann::json jsTrnData, jsValData;
  openJSON(jsValData,"val_20_percent.json");
  openJSON(jsTrnData,"train_80_percent.json");
  int iTrnSz = jsTrnData["images"].size();
  int iValSz = jsValData["images"].size();
  int iImgSz = iTrnSz + iValSz;
  iImgID = iImgSz+1;
  double dTrnPrct = (double) iTrnSz/iImgSz;
  double dValPrct = (double) iValSz/iImgSz;
  for (nlohmann::json::const_iterator jsIt = jsValData["categories"].cbegin(); 
        jsIt != jsValData["categories"].end();
        ++jsIt) {
          std::cerr << (*jsIt)["name"] << std::endl;
          wxCatNames.push_back(nlohmann::to_string((*jsIt)["name"]));
          iNumCats++;
  }

  if (dValPrct < 0.2) { //add to val set. keep 80/20 ratio
    jsData = jsValData;
    jsFilename = "val_20_percent.json";
  }
  else { //add to train set
    jsData = jsTrnData;
    jsFilename = "train_80_percent.json";
  }
}
