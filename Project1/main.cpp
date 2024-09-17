#include "stdafx.h"
#include <iostream>
#include "MFormats.h"
#include <atlstr.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <windows.h>
#include "MyVideoLibrary.h"
using namespace std;

volatile bool webcamFlag = false;
volatile bool stopFlag = true;
std::vector<cv::Mat> savedFrames;
CComPtr<IMFDevice> g_cpLive;

/**
* @brief Getting IMFrame and converting it to Opencv frame (Took example from documentations).
* 
* This function creates an OpenCV frame from IMFrame .
* @param _pFrame its the IMFrame.
* @return OpenCV mat (Frame)
*/
cv::Mat GetCVImageFromMFFrame(CComPtr<IMFFrame> _pFrame) {
    M_AV_PROPS avProps;
    LONG lAudioSamples;
    HRESULT hr = _pFrame->MFAVPropsGet(&avProps, &lAudioSamples);
    if (FAILED(hr)) {
        std::cerr << "Failed to get MFFrame properties." << std::endl;
        return cv::Mat();
    }
    // Getting all the Information about the current frame for next steps.
    int nFrameWidth = avProps.vidProps.nWidth;
    int nFrameHeight = abs(avProps.vidProps.nHeight);
    int nFrameRowBytes = avProps.vidProps.nRowBytes;
    LONG pcbSize;
    BYTE* pbVideo;
    hr = _pFrame->MFVideoGetBytes(&pcbSize, reinterpret_cast<LONGLONG*>(&pbVideo));
    if (FAILED(hr)) {
        std::cerr << "Failed to get video bytes from MFFrame." << std::endl;
        return cv::Mat();
    }
    if (!pbVideo) {
        return cv::Mat();
    }
    try {
        cv::Mat yuyv_frame(nFrameHeight, nFrameWidth, CV_8UC2, reinterpret_cast<void*>(pbVideo), nFrameRowBytes);
        if (yuyv_frame.empty()) {
            return cv::Mat();
        } else {
            //std::cout << "YUYV frame created successfully: " << yuyv_frame.cols << "x" << yuyv_frame.rows << std::endl;
        }
        cv::Mat bgr_frame;
        std::cout << avProps.vidProps.eVideoFormat << std::endl;
        //If the device is a Webcam - Use this code 
        if (webcamFlag == true) {
            cv::cvtColor(yuyv_frame, bgr_frame, cv::COLOR_YUV2BGR_YUYV);
        }
        else { // Else if the device is a video - Use this code
            cv::cvtColor(yuyv_frame, bgr_frame, cv::COLOR_YUV2BGR_UYVY);
        }
        // Resize the frame to 640x480 in order to fit the C# window GUI
        cv::Mat resized_frame;
        cv::resize(bgr_frame, resized_frame, cv::Size(640, 480));
        // Returning a clone of the frame and not the original because returning the original could couse unexpected behaviour. 
        return resized_frame.clone();
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV exception: " << e.what() << std::endl;
        return cv::Mat();
    }
}
/**
* @brief This method is getting the window of the c# gui and draws in it the frames.
* 
* This function is geting a framedata and a window and draws the frame on that window
* 
* param hwnd handle to the window ehere the webcam frames will be rendered, frameData width and height are parameters of the frame itself
* @return this function is not returning anything, but just modifying.
*/
extern "C" __declspec(dllexport) void DrawFrameOnCSharpWindow(HWND hwnd, uchar* frameData, int width, int height) {
    if (!frameData) {
        std::cerr << "Frame data is null." << std::endl;
        return;
    }
    // The frame to be drawn on the hwnd - (Window where its going to be drawn).
    cv::Mat frame(height, width, CV_8UC3, frameData);
    if (frame.empty()) {
        std::cerr << "Frame is empty." << std::endl;
        return;
    }
    // Creating bmi structure of type bitmap - it describes the format of bitmap that will be displayed on the GUI window.
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;  // Negative height for top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;
    // Structure defines set of graphical objects and their associated attributes. (Interface)
    HDC hdc = GetDC(hwnd);
    if (!hdc) {
        std::cerr << "Failed to get device context." << std::endl;
        return;
    }
    //  Creates a block of memory for pixel data - for drawing operation
    void* bits;
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    if (!hBitmap) {
        std::cerr << "Failed to create bitmap." << std::endl;
        ReleaseDC(hwnd, hdc);
        return;
    }
    // Copy pixel data from the frame into memory allocated for the bitmap using memcpy
    memcpy(bits, frame.data, width * height * 3);
    // compatible with the window's DC and is used for drawing operations
    HDC memDC = CreateCompatibleDC(hdc);
    if (!memDC) {
        std::cerr << "Failed to create compatible device context." << std::endl;
        ReleaseDC(hwnd, hdc);
        DeleteObject(hBitmap);
        return;
    }
    // selects the bitmap i nto the compatible DC (memDC)
    SelectObject(memDC, hBitmap);
    // Using BitBlt for performing blick block transfer from DC to the diwnow's DC - this drawn frame into the window.
    if (!BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY)) {
        //std::cerr << "BitBlt failed." << std::endl;
    }
    else {
        //std::cout << "BitBlt succeeded." << std::endl;
    }
    // Releasing resources.
    DeleteDC(memDC);
    ReleaseDC(hwnd, hdc);
    DeleteObject(hBitmap);
}

/**
* @brief Function that autmeticly detecs a webcam and starts capturing its frames.
* 
* This function detects a webcam from the devices on the computer and then converting the frames of IMFrame
* and sends the frame for next proccedure in the "IMFrame converter to OpenCV frame" function and then after returning the mat - passing
* the OpenCV frame to the "drawing on the window" function.
* 
* @param hwnd Handle to the window where the webcam frames will be rendered.
* @return HRESULT s_OK if successful, or an error code if the function fails.
*/
extern "C" __declspec(dllexport) HRESULT PlayLiveSourceWebcam(HWND hwnd) {
    HRESULT hr = g_cpLive.CoCreateInstance(__uuidof(MFLive));
    if (FAILED(hr)) {
        _tprintf(_T("ERROR: Can't create MFLive instance \n"));
        return E_FAIL;
    }
    // Mechanism for auto-select the WebCamera 
    int selectedDevice = 0;
    int nCount = 0;
    hr = g_cpLive->DeviceGetCount(eMFDT_Video, &nCount);
    if (FAILED(hr) || nCount == 0) {
        std::cerr << "No available live video devices found" << std::endl;
        return 0;
    }
    // The Mechanisem that iterates through each device and selects the first WebCamera he catches by the name 
    for (int i = 0; i < nCount; ++i) {
        CComBSTR cbsDevName;
        BOOL bBusy = false;
        hr = g_cpLive->DeviceGetByIndex(eMFDT_Video, i, &cbsDevName, &bBusy);
        if (SUCCEEDED(hr)) {
            CString deviceName(cbsDevName);
            std::wstring lowercaseName = (LPCTSTR)deviceName;
            for (auto& ch : lowercaseName) {
                ch = std::tolower(ch);
            }
            if (lowercaseName.find(L"webcam") != std::wstring::npos || lowercaseName.find(L"camera") != std::wstring::npos) {
                hr = g_cpLive->DeviceSet(eMFDT_Video, i, CComBSTR(L""));
                if (SUCCEEDED(hr)) {
                    selectedDevice = i + 1;
                    //std::wcout << L"Auto-selected webcam: " << cbsDevName << std::endl;
                    _tprintf(_T("%i: %s\n"), i + 1, (LPCTSTR)cbsDevName);
                    break;
                }
                else {
                    std::cerr << "ERROR: Failed to set video device" << std::endl;
                    return 0;
                }
            }
        }
        else {
            std::cerr << "ERROR: Failed to get video device by index" << std::endl;
        }
    }
    // Creates frame obj
    CComPtr<IMFFrame> cpFrame;
    // Creates Source obj and initializing it with the webcam source
    CComQIPtr<IMFSource> cpSource(g_cpLive);
    M_AV_PROPS avProps = {};
    while (stopFlag) {
        cpFrame = NULL;
        hr = cpSource->SourceFrameConvertedGet(&avProps, -1, &cpFrame, CComBSTR(L""));
        if (cpFrame) {
            cv::Mat frame = GetCVImageFromMFFrame(cpFrame);
            DrawFrameOnCSharpWindow(hwnd, frame.data, frame.cols, frame.rows);
        }
    }
    stopFlag = true;
    cpFrame = NULL;
    g_cpLive->DeviceClose();
    g_cpLive = NULL;
    return S_OK;
}

/**
* @brief Function to handle playback video from choosing a video file 
*
* This function opens a Video File from the Files on your computer using "Open Files & Select" function, and then converting the frames of IMFrame
* and sends the frame for next proccedure in the "IMFrame converter to OpenCV frame" function and then after returning the mat - passing
* the OpenCV frame to the "drawing on the window" function.
*
* @param fileName of the choosen video file ,hwnd Handle to the window where the webcam frames will be rendered.
* @return HRESULT s_OK if successful, or an error code if the function fails.
*/
HRESULT FilePlay(CComBSTR cbsFileName, const CComBSTR& _cbsProps, HWND hwnd) {
    CComPtr<IMFReader> cpReader; // IMFReader pointer
    M_AV_PROPS avProps = {}; // Video props structure
    // Create MFReader instance
    HRESULT hr = cpReader.CoCreateInstance(__uuidof(MFReader));
    if (FAILED(hr)) {
        _tprintf(_T("ERROR: Can't create MFReader instance \n"));
        return E_FAIL;
    }
    // Opens a source File which is passed as argument from openFileDialog() function 
    hr = cpReader->ReaderOpen(cbsFileName, _cbsProps);
    if (FAILED(hr)) {
        _tprintf(_T("ERROR: Can't open this media file\n"));
        return E_FAIL;
    }
    CComPtr<IMFFrame> cpFrame;
    bool bRewind{ false };
    int couterOfFrames = 0;
    while (stopFlag) {
        cpFrame = NULL;
        // Get frame by number. -1 as frame number means next frame
        if (bRewind) {
            hr = cpReader->SourceFrameConvertedGetByNumber(&avProps, 0, -1, &cpFrame, CComBSTR(L""));
            bRewind = false;
        }
        else {
            hr = cpReader->SourceFrameConvertedGetByTime(&avProps, -1, -1, &cpFrame, CComBSTR(L""));
        }
        if (cpFrame) {
            // Check for the last frame to stop the playback
            M_TIME mTime;
            hr = cpFrame->MFTimeGet(&mTime);
            if ((mTime.eFFlags & eMFF_Last) != 0) {
                cv::Mat frame = GetCVImageFromMFFrame(cpFrame);
                DrawFrameOnCSharpWindow(hwnd, frame.data, frame.cols, frame.rows);
                // Save the frame
                savedFrames.push_back(frame.clone());
                break; // Break from the loop because its the last frame
            }
            else {
                cv::Mat frame = GetCVImageFromMFFrame(cpFrame);
                DrawFrameOnCSharpWindow(hwnd, frame.data, frame.cols, frame.rows);
                // Save the frame
                savedFrames.push_back(frame.clone());
            }
        }
        else {
            std::cout << "No Frame has been captured." << std::endl;
            
        }
    }
    if (stopFlag == false) savedFrames.clear(); 
    cpFrame = NULL;
    cpReader = NULL;
    return S_OK;
}
/**
* @brief Function for the replaying the video if not stoped
* 
* This function replaying the video that the user choose from his files, and if in the C# GUI he didnt press STOP, the video repeats
* itself untill the user will hit the STOP button.
* the frames are saved in a mat vector, and they were saved after the first repeat of the video.
* 
* @param hwnd Handle to the window ehere the video frames will be rendered.
* @return returns nothing, but just uses the draw function in order to draw the frames on the C# GUI.
*/
void ReplaySavedFrames(HWND hwnd) {
    size_t frameIndex = 0;
    while (stopFlag && !savedFrames.empty()) {
        if (GetAsyncKeyState(VK_ESCAPE) != 0) {
            break; // Exit the loop if ESC is pressed
        }
        cv::Mat frame = savedFrames[frameIndex];
        DrawFrameOnCSharpWindow(hwnd, frame.data, frame.cols, frame.rows);
        frameIndex = (frameIndex + 1) % savedFrames.size(); // Loop back to the first frame
        // Add a delay to control the replay speed, e.g., 30 FPS
        Sleep(33);
    }
    stopFlag = true;
}
/**
* @brief Function to open file dialog and return selected file path as std::wstring
* 
* This function opens the File system and letting you choose a file (Video file for example MP4) for next procedure by taking
* this file and using it on the playFile() function.
* 
* @param nothing.
* @return the choosen video wstring file addr.
*/
std::wstring openFileDialog() {
    OPENFILENAME ofn; // Common dialog box structure
    wchar_t szFile[260]; // Buffer for file name

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    // Display the Open dialog box
    if (GetOpenFileName(&ofn) == TRUE) {
        // Convert the wide-character string to std::wstring
        return std::wstring(ofn.lpstrFile);
    }
    else {
        // If the user cancels the dialog, return an empty string
        return L"";
    }
}
/**
* @brief Main function for starting Webcam Capturing
* 
* This function is the main function that once running it, it will do the job for capturing Webcam frames from your
* Web Camera - its done by using in integrative way of function from this module.
* 
* @param hwnd Handle to the window where the video frames will be rendered.
* @return nothing.
*/
void playWebcam(HWND hwnd) {
    webcamFlag = true;
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    hr = PlayLiveSourceWebcam(hwnd);
    if (FAILED(hr)) {
        std::cerr << "Failed to play live source webcam." << std::endl;
    }
    webcamFlag = false;
    CoUninitialize();
}
/**
* @brief Main function for starting File Capturing
*
* This function is the main function that once running it, it will do the job for capturing Video File (for example MP4) frames from your
* choosen video - its done by using in integrative way of function from this module.
*
* @param hwnd Handle to the window where the video frames will be rendered.
* @return nothing.
*/
void playFile(HWND hwnd) {
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    std::wstring filePath = openFileDialog();
    if (!filePath.empty()) {
        hr = FilePlay(CComBSTR(filePath.c_str()), CComBSTR(L""), hwnd);
        if (FAILED(hr)) {
            std::cerr << "Failed to play file." << std::endl;
        }
    }
    else {
        std::wcout << L"No file selected." << std::endl;
    }
    ReplaySavedFrames(hwnd);
    stopFlag = true;
    savedFrames.clear();
    CoUninitialize();
}
/**
* @brief A Signal function for signaling the other functions that the Stop button has been pressed.\
* 
* Using this function is letting us modify flag in real time for certain function in this module in order to indecate them when to stop
* executing the function and exiting them.
* 
* @param nothing
* @return nothing
*/

void stopCapture() {
    stopFlag = false;
}


