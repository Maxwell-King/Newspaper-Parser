#include <cstring>
#include <iostream>
#include <fstream>  

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/clipbrd.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "nlohmann/json.hpp"

#define SEPERATOR ","



std::string OCRregion(std::string , int x, int y, int w, int h) {
	
	
}

class wxImagePanel : public wxPanel {
public:
  wxImagePanel(wxFrame *parent, wxString file);
	wxImage image;
  wxBitmap resized;
	int w_bmp, h_bmp, w_img, h_img;
	nlohmann::json *jsData;


	bool b_dragging = false;
	bool b_mouseleftdown = false;

  void paintEvent(wxPaintEvent &evt);
	void mouseDownEvent(wxMouseEvent &evt);
	void mouseDragEvent(wxMouseEvent &evt);

	void mouseUpEvent(wxMouseEvent &evt);

  void render(wxDC& dc);
  wxSize ScaleToScreen(wxSize sz);  // longest side scaled to fit screen (1080P) maintaing aspect ratio
	wxPoint wxhghPoint;
	wxPoint wxlowPoint;
  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxImagePanel, wxPanel)
EVT_PAINT(wxImagePanel::paintEvent)
EVT_LEFT_DOWN(wxImagePanel::mouseDownEvent)
EVT_MOTION(wxImagePanel::mouseDownEvent)
EVT_LEFT_UP(wxImagePanel::mouseUpEvent)
END_EVENT_TABLE()

wxImagePanel::wxImagePanel(wxFrame *parent, wxString file) : wxPanel(parent) {
	image.LoadFile(file, wxBITMAP_TYPE_JPEG); 
	w_img = image.GetSize().x;
	h_img = image.GetSize().y;

  parent->SetSize(ScaleToScreen(image.GetSize()));
	w_bmp = parent->GetClientSize().x;
	h_bmp = parent->GetClientSize().y;
	resized = wxBitmap(image.Scale(w_bmp, h_bmp));
}

void wxImagePanel::paintEvent(wxPaintEvent &evt) {
  wxPaintDC dc(this);
  render(dc);
}

void wxImagePanel::mouseDownEvent(wxMouseEvent &evt) {
	wxhghPoint = evt.GetPosition(); // assuming topleft -> bottomright
	b_mouseleftdown = true;
} 

void wxImagePanel::mouseDragEvent(wxMouseEvent &evt) {
	if (b_mouseleftdown) {
		b_dragging = true;
	}
} 

void wxImagePanel::mouseUpEvent(wxMouseEvent &evt) {
  if (dragging) {
		wxlowPoint = evt.GetPosition();
		if (wxlowPoint.x < wxhghPoint.x) {
			int lowX = wxhghPoint.x;
			wxhghPoint.x = wxlowPoint.x;
			wxlowPoint.x = lowX;
		}	
		if (wxlowPoint.y < wxhghPoint.y) {
			int lowY = wxhghPoint.y;
			wxhghPoint.y = wxlowPoint.y;
			wxlowPoint.y = lowY;
		}	
		int xbb_img = wxhghPoint.x/w_bmp * w_img;
		int ybb_img = wxhghPoint.y/h_bmp * h_img;
		int wbb_img = (wxlowPoint.x-wxhghPoint.x)/w_bmp * w_img;
		int hbb_img = (wxlowPoint.y-wxhghPoint.y)/h_bmp * h_img;

		// std::string text = OCRregion(file, xbb_img, ybb_img, wbb_img, hbb_img);
		std::cerr << "highpoint: " << wxhghPoint.x << SEPERATOR << wxhghPoint.y << std::endl;
		std::cerr << "lowpoint: " << wxlowPoint.x << SEPERATOR << wxlowPoint.y << std::endl;
	}
	b_mouseleftdown = false;
	dragging = false;
}


void wxImagePanel::render(wxDC &dc) {
  dc.DrawBitmap(resized, 0, 0, false);
}

wxSize wxImagePanel::ScaleToScreen(wxSize sz) { // longest side scaled to fit screen (1080P) maintaing aspect ratio
  wxSize szNewSz = sz;
  double dNewW = sz.x/1920.f;
  double dNewH = sz.y/1080.f;
  
  if (dNewW > 1.0f || dNewH > 1.0f) {
    double dMaxF = dNewW >= dNewH ? dNewW : dNewH;
    szNewSz.x = round((double) szNewSz.x/dMaxF);
    szNewSz.y = round((double) szNewSz.y/dMaxF);
  }
  return szNewSz;
}

