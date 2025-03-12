#include "../include/jsdata.hpp"

nlohmann::json jsData;
std::string jsFilename;
std::vector<wxString> wxCatNames;
int iImgID;
int iNumCats;

void openJSON(nlohmann::json &js, const std::string strDataSet) {
	std::filebuf fb;
	if (fb.open(strDataSet, std::ios::in)) {
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
	nlohmann::json jsTrnData, jsValData;
	openJSON(jsValData,"val_20_percent.json");
	openJSON(jsTrnData,"train_80_percent.json");
	int iTrnSz = jsTrnData["images"].size();
	int iValSz = jsValData["images"].size();
	int iImgSz = iTrnSz + iValSz;
	std::cerr << iImgSz << std::endl;
	iImgID = iImgSz+1;
	double dTrnPrct = 0;
	double dValPrct = 0;
	if (iImgSz > 0) {
		dTrnPrct = (double) iTrnSz/iImgSz;
		dValPrct = (double) iValSz/iImgSz;
	}
	iNumCats = jsValData["categories"].size();
	for (int n=0; n < iNumCats; n++) {
		wxCatNames.push_back(nlohmann::to_string(jsValData["categories"][n]["name"]));
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
