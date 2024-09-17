#ifndef MYVIDEOLIBRARY_H
#define MYVIDEOLIBRARY_H

#include "MFormats.h"
#include <atlstr.h>
#include <string>

// Function declarations with __declspec(dllexport)
extern "C" __declspec(dllexport) void playWebcam(HWND hwnd); // Updated declaration
extern "C" __declspec(dllexport) void playFile(HWND hwnd);
extern "C" __declspec(dllexport) void stopCapture();
extern "C" __declspec(dllexport) void ReplaySavedFrames(HWND hwnd);

// Additional declarations, if needed

#endif // MYVIDEOLIBRARY_H
