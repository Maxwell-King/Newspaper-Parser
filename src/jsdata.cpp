#include "../include/jsdata.hpp"

std::ifstream js_in;
struct json js_data;
int img_id;

void match(char t) {
  if (t == js_in.peek()) {
    js_in.get();
  }
  else {
    std::cerr << "Expected " << t << " found " << std::to_string(js_in.peek()-'0') <<  " at " << js_in.tellg() << std::endl;
    exit(-1); // fail
  }
}

void match_string(std::string target) {
  match('\"');
  for (int pos = 0; pos < target.length(); pos++) {
    match(target[pos]);
  }
  match('\"');
}

std::string get_string() {
  std::string name;
  match('\"');
  while (js_in.peek() != '\"') {
    name += js_in.get();
  }
  match('\"');
  return name;
}

std::string match_number() {
	std::string num;
  while (std::isdigit(js_in.peek())) { // no negative numbers
		num += std::to_string(js_in.get()-'0');
  }
	return num;
}

void match_annotation(struct json *js_rd) {
  while (true) {
    if (js_in.peek() == ']') {
      match(']');
      break;
    }
    match('{');

    match_string("area");
    match(':');
    js_rd->annotations.append("{\"area\":" + match_number());
    match(',');
    match_string("bbox");
    match(':');
    match('[');
    js_rd->annotations.append(",\"bbox\":[" + match_number());
    match(',');
    js_rd->annotations.append("," + match_number());
    match(',');
    js_rd->annotations.append("," + match_number());
    match(',');
    js_rd->annotations.append("," + match_number());
    match(']');
    match(',');
    match_string("category_id");
    match(':');
    js_rd->annotations.append("],\"category_id\":" + match_number());
    match(',');
    match_string("id");
    match(':');
    js_rd->annotations.append(",\"id\":" + match_number());
    match(',');
    match_string("image_id");
    match(':');
    js_rd->annotations.append(",\"image_id\":" + match_number());
    match(',');
    match_string("iscrowd");
    match(':');
    js_rd->annotations.append(",\"iscrowd\":" + match_number());
    match('}');
		js_rd->annotations += '}';
	
    js_rd->annotations_size++;
    if (js_in.peek() == ',') {
      match(',');
			js_rd->annotations += ',';

    }
  }
}

std::string match_date() {
	std::string str_date;
  match('\"');
  str_date.append(match_number());
  match('-');
  str_date.append('-' + match_number());
  match('-');
  str_date.append('-' + match_number());
  match(' ');

  str_date.append(' ' + match_number());
  match(':');
  str_date.append(':' + match_number());
  match(':');
  str_date.append(':' + match_number());
  match('\"');
	return str_date;
}

void match_images(struct json *js_rd) {
  while (true) {
    if (js_in.peek() == ']') {
      match(']');
      break;
    }
    match('{');

    match_string("date_captured");
    match(':');
    js_rd->images.append("{\"date_captured\":\"" + match_date() + "\"");
    match(',');
    match_string("file_name");
    match(':');
		js_rd->images.append(",\"file_name\":\"" + get_string() + "\"");

    match(',');
    match_string("height");
    match(':');
    js_rd->images.append(",\"height\":" + match_number());
    match(',');
    match_string("id");
    match(':');
    js_rd->images.append(",\"id\":" + match_number());
    match(',');
    match_string("width");
    match(':');
    js_rd->images.append(",\"width\":" + match_number());
    
    match('}');
		js_rd->images += '}';

    js_rd->images_size++;
    if (js_in.peek() == ',') {
      match(',');
			js_rd->images += ',';
    }
  }
}

void match_categories(struct json *js_rd) { // must have atleast one category (provide user option?)
  while (true) {
    match('{');
    match_string("id");
    match(':');
    js_rd->categories.append("{\"id\":" + match_number());
    match(',');
    match_string("name");
    match(':');
		std::string cat_name = get_string();
		js_rd->categories.append(",\"name\":\"" + cat_name + '\"');
    js_rd->categories_list.push_back(cat_name);
    match('}');
		js_rd->categories += '}';
    js_rd->categories_size++;
    switch (js_in.peek()) {
    case ',':
      match(',');
			js_rd->categories += ',';
      continue;
    case ']':
      match(']');
      goto CATEGORIES_OVER;
    }
  }
	CATEGORIES_OVER:;
}

void parse(struct json *js_rd, std::string file) {
  js_in.open(file, std::ifstream::in);
  match('{');
  match_string("annotations");
  match(':');
  match('[');
  match_annotation(js_rd);
  match(',');
  match_string("categories"); 
  match(':');
  match('[');
  match_categories(js_rd);
  match(',');
  match_string("images");
  match(':');
  match('[');
  match_images(js_rd);
  match('}');
	js_in.close();
}

void add_image(std::string date_captured, std::string file_name, int h_img, int id, int w_img) {
  std::string image_obj = "{\"date_captured\":\"" + date_captured +
    "\",\"file_name\":\"" + file_name +
    "\",\"height\":" + std::to_string(h_img) +
    ",\"id\":" + std::to_string(id) +
    ",\"width\":" + std::to_string(w_img) +
    "}";
  if (js_data.images_size != 0) {
		js_data.images += ',';
	}
  js_data.images.append(image_obj);
	js_data.images_size++;

}

void add_annotation(int area, int bbox_x, int bbox_y, int bbox_w, int bbox_h, int cat_id, int id, int img_id) { // {"area":396459,"bbox":[15,50,1953,203],"category_id":0,"id":3000,"image_id":0,"iscrowd":0}
  std::string annotation_obj = "{\"area\":" + std::to_string(area) +
    ",\"bbox\":[" + std::to_string(bbox_x) + "," + std::to_string(bbox_y) + "," + std::to_string(bbox_w) + "," + std::to_string(bbox_h) + "]"
    ",\"category_id\":" + std::to_string(cat_id) +
    ",\"id\":" + std::to_string(id) +
    ",\"image_id\":" + std::to_string(img_id) +
    ",\"iscrowd\":0}";
	if (js_data.annotations_size != 0) {
		js_data.annotations += ',';
	}
  js_data.annotations.append(annotation_obj);
						js_data.annotations_size++;

}


void json_copy(struct json *dest, const struct json src) {
	dest->images_size = src.images_size; // commas in array + 1
	dest->categories_size = src.categories_size;
	dest->annotations_size = src.annotations_size;

	dest->images = src.images;
	dest->categories = src.categories;
	dest->annotations = src.annotations;
	dest->js_filename = src.js_filename;

	dest->categories_list = src.categories_list;	
}

void closeJSON() {
	std::ofstream js_out(js_data.js_filename, std::fstream::trunc);
	js_out << "{\"annotations\":[" << js_data.annotations << "],\"categories\":[" << js_data.categories << "],\"images\":[" << js_data.images << "]}";
  js_out.close();
}

void chooseJSON() {
	struct json js_val;
	struct json js_train;
	parse(&js_train, "train_80_percent.json");
	parse(&js_val, "val_20_percent.json");
	int total_img = js_val.images_size+js_train.images_size;
	img_id = total_img+1;
	double trn_pct = 0;
	double val_pct = 0;
	if (total_img > 0) {
		trn_pct = (double) js_train.images_size/total_img;
		val_pct = (double) js_val.images_size/total_img;
	}
	if (val_pct < 0.2) { //add to val set. keep 80/20 ratio
		json_copy(&js_data, js_val);
		js_data.js_filename = "val_20_percent.json";
	}
	else { //add to train set
		json_copy(&js_data, js_train);
		js_data.js_filename = "train_80_percent.json";
	}
}
