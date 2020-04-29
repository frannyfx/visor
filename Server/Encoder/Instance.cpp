#include "Instance.h"

void Encoder::Instance::Start() {
	// Set start point
	encodeStart = system_clock::now();

	// Setup converter
	swsContext = sws_getContext(client.GetWidth(), client.GetHeight(), AV_PIX_FMT_RGB24, 1280, 720, AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);

	// Get name
	string filename = GetFileName();

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
		return;
	}

	// Setting up codec
	av_dict_set(&opt, "preset", "fast", 0);
	av_dict_set(&opt, "crf", "20", 0);

	// Create context
	context = avcodec_alloc_context3(codec);
	context->width = 1280;
	context->height = 720;
	context->framerate = AVRational({ 1, 60 });
	context->time_base = AVRational({ 1, 60 });
	context->pix_fmt = AV_PIX_FMT_YUV420P;

	if (outputContext->oformat->flags & AVFMT_GLOBALHEADER)
		context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	avcodec_open2(context, codec, &opt);
	av_dict_free(&opt);

	av_dump_format(outputContext, 0, filename.c_str(), 1);
	avio_open(&outputContext->pb, filename.c_str(), AVIO_FLAG_WRITE);
	avformat_write_header(outputContext, &opt);
	av_dict_free(&opt);

	cout << "Encoder initialised successfully." << endl;
}

string Encoder::Instance::GetFileName() {
	return client.windowTitle + ".mp4";
}

DWORD WINAPI Encoder::Instance::Run(LPVOID) {
	// Setup encoding
	AVFrame* frame;
	AVPacket packet;
	int ret;

	// Create lock
	unique_lock<mutex> lock(frameAvailableMutex);

	while (running) {
		// Wait for frames
		frameAvailableCV.wait(lock, [this] { return !frames.empty(); });
		for (auto& toEncode : frames) {
			// Get data into frame
			frame = av_frame_alloc();
			
			// ...

			// Encode and free the frame
			ret = avcodec_send_frame(context, frame);
			av_frame_free(&frame);

			if (ret < 0) {
				cout << "Error while sending a frame for encoding." << endl;
				continue;
			}

			while (ret >= 0) {
				ret = avcodec_receive_packet(context, &packet);
				if (ret < 0) {
					cout << "Error while receiving video packet." << endl;
					break;
				}
					

				ret = av_write_frame(outputContext, &packet);
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