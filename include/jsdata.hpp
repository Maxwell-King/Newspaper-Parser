#ifndef JSDATA_H
#define JSDATA_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>


#include <wx/wx.h>

struct json {
	int images_size = 0; 
	int annotations_size = 0;
	int categories_size = 0;

	std::string images;
	std::string annotations;
	std::string categories;
	std::string js_filename;
	std::vector<wxString> categories_list;
	std::unordered_map<std::string, int> cat_map;
};

extern struct json js_data;

extern wxString file;
extern wxString date;

extern int img_id;
extern int w_img;
extern int h_img;
void add_annotation(int area, int bbox_x, int bbox_y, int bbox_w, int bbox_h, int cat_id, int id, int img_id);
void add_image(std::string date_captured, std::string file_name, int h_img, int id, int w_img);
void js_choose();
void js_close();

#endif
