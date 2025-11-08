#include <gtest/gtest.h>
#include <encoder.h>
#include <pipelines.h>
#include <qapplication.h>
#include <qguiapplication.h>
#include <audiooutput.h>

int _argc;
char** _argv;
//TEST(AudioTetst, DirectAudioPipeline) {
//	QApplication app(_argc,_argv);
//	Media::Audio::MicrophonePipeline mic;
//	Media::Audio::Output out;
//	auto config = mic.open();
//	EXPECT_NO_THROW(config.waitForFinished());
//	EXPECT_EQ(config.resultCount(), 1);
//	out.start(out.availableDevices().first(), mic.frameOutput());
//    app.exec();
//
//}
TEST(AudioTetst, OpuAudioPipeline) {
	QApplication app(_argc, _argv);
	Media::Audio::MicrophonePipeline mic;
	//mic.setFilterFactory(std::make_shared<Media::Audio::NoiseReductionFilterFactory>());

	Media::Audio::Output out;
	auto config = mic.open();
	EXPECT_NO_THROW(config.waitForFinished());
	EXPECT_EQ(config.resultCount(), 1);
	Media::Audio::OpusEncoder enc(config.result());
	Media::Audio::OpusDecoder dec;
	enc.start(mic.frameOutput());
	dec.open(enc.output());
	out.start(out.availableDevices().first(), dec.output());

	app.exec();

}
int main(int argc, char** argv) {
    qDebug() << "Running tests with args: ";
    for (size_t i = 1; i < argc; i++)
    {
        qDebug() << QString::fromLatin1(argv[i]);
    }
    testing::InitGoogleTest(&argc, argv);
    _argc = argc; _argv = argv;
    return RUN_ALL_TESTS();
}