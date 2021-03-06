#include "calibration.h"

int calibrate(info *data, const char *whichOne, int flag) //return 1 to stop the same interface from repeating, whichOne will be displayed along with the Text, set flag to 6 if you don't care about better base values
{
	//RETURN VALUES:
	//return 0 for exit, 1 for next camera, 2 for previous camera, 3 for continue
	const char *FILENAME;
	if (data->color == 1)
	{
		FILENAME = FILENAME1;
	}
	else if (data->color == 2)
	{
		FILENAME = FILENAME2;
	}
	else
	{
		FILENAME = FILENAME3;
	}
	UMat mainImage, hsvImage, thresholdedImage, combinedView;
	int retValue;
	//UMat allows for OpenCV to use OpenGL acceleration
	int cont = 1; //check if continue
	int lowh = 90;
	int lows = 170;
	int lowv = 70;
	int highh = 179;
	int highs = 255;
	int highv = 255;
	if (flag != 6) //skipping this part if color flag is 6
	{
		provideBaseValues(lowh, lows, lowv, flag);
	}
	int key = 0;
	VideoCapture source(data->camera); //start camera from camera number defined in structs
	//Create trackbars, usually low changing values is sufficient
	source.read(mainImage); //quick read to figure out rectangle size for gaussian blur to display text
	Rect region(0, 0, mainImage.cols, 180);
	namedWindow(WINDOWNAME, CV_WINDOW_AUTOSIZE); //create calibration window
	cvCreateTrackbar("Low H", WINDOWNAME, &lowh, 179); //Hue (0 - 179)
	cvCreateTrackbar("Low S", WINDOWNAME, &lows, 255); //Saturation (0 - 255)
	cvCreateTrackbar("Low V", WINDOWNAME, &lowv, 255); //Value (0 - 255)
	while (cont)
	{
		source.read(mainImage);
		//creating HSV image
		cvtColor(mainImage, hsvImage, COLOR_BGR2HSV);
		inRange(hsvImage, Scalar(lowh, lows, lowv), Scalar(highh, highs, highv), thresholdedImage);
		//blur top part of original image to display instructions
		GaussianBlur(mainImage(region), mainImage(region), Size(0, 0), 6);
		//morphological opening
		erode(thresholdedImage, thresholdedImage, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(thresholdedImage, thresholdedImage, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		//morphological closing
		dilate(thresholdedImage, thresholdedImage, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(thresholdedImage, thresholdedImage, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		//convert back to same type
		cvtColor(thresholdedImage, thresholdedImage, CV_GRAY2BGR);
		hconcat(mainImage, thresholdedImage, combinedView);
		//add instructions for usage
		putText(combinedView, INSTRUCTION1, cvPoint(20, 20), FONT_HERSHEY_COMPLEX_SMALL, 0.75, cvScalar(0, 0, 255), 1, CV_AA);
		putText(combinedView, INSTRUCTION2, cvPoint(20, 40), FONT_HERSHEY_COMPLEX_SMALL, 0.75, cvScalar(0, 0, 255), 1, CV_AA);
		putText(combinedView, INSTRUCTION3, cvPoint(20, 60), FONT_HERSHEY_COMPLEX_SMALL, 0.7, cvScalar(0, 0, 255), 1, CV_AA);
		putText(combinedView, INSTRUCTION4, cvPoint(20, 80), FONT_HERSHEY_COMPLEX_SMALL, 0.7, cvScalar(0, 0, 255), 1, CV_AA);
		putText(combinedView, INSTRUCTION5, cvPoint(20, 100), FONT_HERSHEY_COMPLEX_SMALL, 0.7, cvScalar(0, 0, 255), 1, CV_AA);
		putText(combinedView, INSTRUCTION6, cvPoint(20, 120), FONT_HERSHEY_COMPLEX_SMALL, 0.7, cvScalar(0, 0, 255), 1, CV_AA);
		putText(combinedView, INSTRUCTION7, cvPoint(20, 140), FONT_HERSHEY_COMPLEX_SMALL, 0.7, cvScalar(0, 0, 255), 1, CV_AA); //display current one being calibrated
		putText(combinedView, whichOne, cvPoint(20, 160), FONT_HERSHEY_COMPLEX_SMALL, 0.7, cvScalar(0, 0, 255), 1, CV_AA); //display current one being calibrated
		//imshow("Calibration View (FreeHandVR)", combinedView);
		imshow(WINDOWNAME, combinedView);
		key = waitKey(20);
		if ((key == 27) || (key == 110) || (key == 98)|| (key == 13)) //ESC, n, b, return
		{
			cont = 0;
			switch (key)
			{
				case 27:
					retValue = 0;
					break;
				case 110:
					retValue = 1;
					break;
				case 98:
					retValue = 2;
					break;
				default: //13
					writeData(lowh, lows, lowv, data->camera, FILENAME);
					retValue = 3;
			}
		}
	}
	source.release();
	destroyAllWindows();
	return retValue;
}