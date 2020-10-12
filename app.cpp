#include <atomic>
#include <iostream>
#include <map>

#include "lib/NDI SDK for Linux/include/Processing.NDI.Lib.h"
#include "lib/screen_capture_lite/include/ScreenCapture.h"

std::map<int, void *> frame_buffers;
std::map<int, NDIlib_video_frame_v2_t> frames;
std::map<int, NDIlib_send_instance_t> senders;

/////////////////////////////////////////////////////////////////////////

void ExtractAndConvertToRGBA(const SL::Screen_Capture::Image &img, unsigned char *dst, size_t dst_size) {
    assert(dst_size >= static_cast<size_t>(SL::Screen_Capture::Width(img) * SL::Screen_Capture::Height(img) * sizeof(SL::Screen_Capture::ImageBGRA)));
    auto imgsrc = StartSrc(img);
    auto imgdist = dst;
    for (auto h = 0; h < Height(img); h++) {
        auto startimgsrc = imgsrc;
        for (auto w = 0; w < Width(img); w++, imgsrc++) {
            *imgdist++ = imgsrc->B;
            *imgdist++ = imgsrc->G;
            *imgdist++ = imgsrc->R;
            *imgdist++ = 0;
        }
        imgsrc = SL::Screen_Capture::GotoNextRow(img, startimgsrc);
    }
}

using namespace std::chrono_literals;
std::shared_ptr<SL::Screen_Capture::IScreenCaptureManager> dispCapture;
std::atomic<int> onNewFramecounter;

auto onNewFramestart = std::chrono::high_resolution_clock::now();
void createframegrabber() {
    onNewFramecounter = 0;
    dispCapture = nullptr;
    dispCapture =
        SL::Screen_Capture::CreateCaptureConfiguration([]() {
            auto mons = SL::Screen_Capture::GetMonitors();
            std::cout << "Library is requesting the list of monitors to capture!" << std::endl;
            for (auto &m : mons) {
                NDIlib_send_create_t NDI_send_create_desc;
                NDI_send_create_desc.p_ndi_name = m.Name;

                NDIlib_send_instance_t pNDI_send = NDIlib_send_create(&NDI_send_create_desc);
                if (!pNDI_send) {
                    std::cout << "Error?" << std::endl;
                    continue;
                }

                // We are going to create a 1920x1080 interlaced frame at 29.97Hz.
                NDIlib_video_frame_v2_t NDI_video_frame;
                NDI_video_frame.xres = m.Width;
                NDI_video_frame.yres = m.Height;
                NDI_video_frame.FourCC = NDIlib_FourCC_type_BGRX;
                NDI_video_frame.line_stride_in_bytes = m.Width * 4;

                senders[m.Id] = pNDI_send;
                frames[m.Id] = NDI_video_frame;
                frame_buffers[m.Id] = malloc(m.Width * m.Height * 4);

                std::cout << m.Name << std::endl;

                // free(p_frame_buffers[]);

                // NDIlib_send_destroy(pNDI_send);
                // NDIlib_destroy();

                // std::cout << m.Name << std::endl;
            }
            return mons;
        })
            ->onNewFrame([&](const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Monitor &monitor) {
                if (monitor.Id != 0) return;

                ExtractAndConvertToRGBA(img, (unsigned char *)frame_buffers[monitor.Id], monitor.Width * monitor.Height * 4);
                frames[monitor.Id].p_data = (uint8_t *)frame_buffers[monitor.Id];
                NDIlib_send_send_video_v2(senders[monitor.Id], &frames[monitor.Id]);
                // NDIlib_send_send_video_async_v2(senders[monitor.Id], &frames[monitor.Id]);

                if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - onNewFramestart).count() >=
                    1000) {
                    std::cout << "onNewFrame fps" << onNewFramecounter << std::endl;
                    onNewFramecounter = 0;
                    onNewFramestart = std::chrono::high_resolution_clock::now();
                }
                onNewFramecounter += 1;
            })
            ->start_capturing();

    dispCapture->setFrameChangeInterval(std::chrono::milliseconds(1000 / 60));
}

#include <atomic>
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32

#ifdef _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x64.lib")
#else  // _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x86.lib")
#endif  // _WIN64

#include <windows.h>

#endif

static std::atomic<bool> exit_loop(false);
static void sigint_handler(int) {
    exit_loop = true;
}

int main(int argc, char *argv[]) {
    if (!NDIlib_initialize()) {  // Cannot run NDI. Most likely because the CPU is not sufficient (see SDK documentation).
        printf("Cannot run NDI.");
        return 0;
    }

    signal(SIGINT, sigint_handler);

    std::srand(std::time(nullptr));
    std::cout << "Running display capture" << std::endl;
    createframegrabber();

    std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::hours(std::numeric_limits<int>::max()));

    return 0;
}
