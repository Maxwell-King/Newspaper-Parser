#include "dialog.hpp"

#include <cstring>
#include <iostream>
#include <fstream>

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/clipbrd.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

class wxImagePanel : public wxPanel {
public:
	wxImagePanel(wxFrame *parent);
	wxImage image;
	wxPoint wxhghPoint;
	wxPoint wxlowPoint;
	wxPoint wxcurPoint;
	Pix *pixImg;
	wxBitmap resized;
	int w_bmp, h_bmp;

	bool b_dragging = false;
	bool b_mouseleftdown = false;

	void paintEvent(wxPaintEvent &evt);

	void mouseDownEvent(wxMouseEvent &evt);
	void mouseDragEvent(wxMouseEvent &evt);
	void mouseUpEvent(wxMouseEvent &evt);
	void render(wxDC &dc);
	wxSize ScaleToScreen(wxSize sz);	// longest side scaled to fit screen (1080P) maintaing aspect ratio
};

