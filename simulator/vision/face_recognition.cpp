#include "pch.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <Windows.h>
#include <iostream>
#include "opencv2/objdetect.hpp"
#include <stdio.h>

using namespace std;
using namespace cv;

/** Global variables */
String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
String window_name = "Capture - Face detection";

/** @function detectAndDisplay */
void detectAndDisplay(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
		ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);

		Mat faceROI = frame_gray(faces[i]);
		std::vector<Rect> eyes;

		//-- In each face, detect eyes
		eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

		for (size_t j = 0; j < eyes.size(); j++)
		{
			Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
			int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
			circle(frame, eye_center, radius, Scalar(255, 0, 0), 4, 8, 0);
		}
	}
	//-- Show what you got
	imshow(window_name, frame);
}

Mat hwnd2mat(HWND hwnd)
{
	int x_size = 1368, y_size = 912;

	HBITMAP hBitmap;
	Mat matBitmap;

	HDC hdcSys = GetDC(hwnd); //gets device context (DC) of screen
	HDC hdcComp = CreateCompatibleDC(hdcSys); // creates compatable DC

	void *ptrBitmapPixels; //pointer for pixels

	BITMAPINFO bi;
	HDC hdc;
	ZeroMemory(&bi, sizeof(BITMAPINFO)); //fills in bitmap with zeros
	bi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = x_size;
	bi.bmiHeader.biHeight = -y_size;  //negative so (0,0) is at top left
	
	bi.bmiHeader.biBitCount = 32; //1-monochrome
	
	bi.bmiHeader.biPlanes = 1;
	hdc = GetDC(hwnd);
	// set output bitmap
	hBitmap = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &ptrBitmapPixels, NULL, 0);

	// Set hBitmap in the hdcMem 
	SelectObject(hdcComp, hBitmap);

	// Set matBitmap to point to the pixels of the hBitmap
	matBitmap = Mat(y_size, x_size, CV_8UC4, ptrBitmapPixels, 0);
	//                ^^ note: first it is y, then it is x. very confusing

	// Now update the pixels using BitBlt
	BitBlt(hdcComp, 0, 0, x_size, y_size, hdcSys, 0, 0, SRCCOPY);

	return matBitmap;
}

int main(int argc, char **argv)
{
	HWND hwndDesktop = GetDesktopWindow();
	namedWindow("output", WINDOW_NORMAL);
	int key = 0;

	//-- 1. Load the cascades
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading face cascade\n"); return -1; };
	if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading eyes cascade\n"); return -1; };


	while (key != 27)
	{
		Mat src = hwnd2mat(hwndDesktop);
		//-- 3. Apply the classifier to the frame
		
		Mat frame = src(Range(456, 912), Range(456, 912));

		detectAndDisplay(frame);
		key = waitKey(60); // you can change wait time
	}
}