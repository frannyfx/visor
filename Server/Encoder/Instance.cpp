#include "Instance.h"

bool Encoder::Instance::Start() {
	// Set start point
	encodeStart = system_clock::now();

	// Setup converter
	swsContext = sws_getContext(client.GetWidth(), client.GetHeight(), AV_PIX_FMT_RGB24, 1280, 720, AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);

	// Get name & open output handle
	string filename = GetFileName();
	if (fopen_s(&file, filename.c_str(), "wb") != 0) {
		cout << "Could not open output file." << endl;
		return false;
	}

	// Setup output
	AVOutputFormat* fmt = av_guess_format(filename.c_str(), NULL, NULL);
	avformat_alloc_output_context2(&outputContext, NULL, NULL, filename.c_str());
	AVStream* stream = avformat_new_stream(outputContext, 0);
	AVCodec* codec = NULL;
	AVCodecContext* context = NULL;
	AVDictionary* opt = NULL;

	codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (codec == NULL) {
		cout << "Codec not found." << endl;
		return false;
	}

	// Setting up codec
	av_dict_set(&opt, "preset", "fast", 0);
	av_dict_set(&opt, "crf", "20", 0);

	// Create context
	context = avcodec_alloc_context3(codec);
	context->width = 1280;
	context->height = 720;
	context->framerate = AVRational({ 60, 1 });
	context->time_base = AVRational({ 1, 60 });
	context->pix_fmt = AV_PIX_FMT_YUV420P;

	if (outputContext->oformat->flags & AVFMT_GLOBALHEADER)
		context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	avcodec_open2(context, codec, &opt);
	
	// Start encoder thread
	running = true;
	HANDLE hThread = CreateThread(
		NULL,
		0,
		Instance::Run,
		(void*) this,
		0,
		NULL
	);

	if (hThread == NULL) {
		cout << "Failed to start encoder thread." << endl;
		return false;
	}

	return true;
}

string Encoder::Instance::GetFileName() {
	return client.windowTitle + ".mp4";
}

DWORD WINAPI Encoder::Instance::Run(void* instance) {
	// Setup encoding
	cout << "Allocating frames..." << endl;
	AVFrame *rgbFrame, *yuvFrame;
	rgbFrame = av_frame_alloc();
	rgbFrame->format = AV_PIX_FMT_RGB24;
	rgbFrame->width = ((Instance*)instance)->client.GetWidth();
	rgbFrame->height = ((Instance*)instance)->client.GetHeight();
	av_frame_get_buffer(rgbFrame, 1);

	yuvFrame = av_frame_alloc();
	yuvFrame->format = AV_PIX_FMT_YUV420P;
	yuvFrame->width = 1280;
	yuvFrame->height = 720;
	av_frame_get_buffer(yuvFrame, 1);

	cout << "Allocated frames successfully." << endl;

	AVPacket packet;
	int ret;

	return NULL;

	while (((Instance*)instance)->running) {
		cout << "Waiting for frame..." << endl;
		// Wait for frames
		while (((Instance*)instance)->frames.size() == 0) {}

		cout << "Got frames..." << endl;
		for (auto& toEncode : ((Instance*)instance)->frames) {
			// Get data into frame
			av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, toEncode, (AVPixelFormat)rgbFrame->format, rgbFrame->width, rgbFrame->height, 1);

			// Convert & scale
			sws_scale(((Instance*)instance)->swsContext, rgbFrame->data, rgbFrame->linesize, 0, rgbFrame->height, yuvFrame->data, yuvFrame->linesize);

			// Encode and free the frame
			ret = avcodec_send_frame(((Instance*)instance)->context, yuvFrame);

			if (ret < 0) {
				cout << "Error while sending a frame for encoding." << endl;
				continue;
			}

			while (ret >= 0) {
				ret = avcodec_receive_packet(((Instance*)instance)->context, &packet);
				if (ret < 0) {
					cout << "Error while receiving video packet." << endl;
					break;
				}
					

				ret = av_write_frame(((Instance*)instance)->outputContext, &packet);
				if (ret < 0) {
					cout << "Error while writing video frame." << endl;
					break;
				}

				av_packet_unref(&packet);
			}
		}
	}

	return NULL;
}