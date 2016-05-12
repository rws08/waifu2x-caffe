#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <utility>
#include <functional>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <opencv2/core.hpp>

#define CUDNN_DLL_NAME "cudnn64_5.dll"
#define CUDNN_REQUIRE_VERION_TEXT "v5 RC"


namespace caffe
{
	template <typename Dtype>
	class Net;
	class NetParameter;
};

class Waifu2x
{
public:
	enum eWaifu2xError
	{
		eWaifu2xError_OK = 0,
		eWaifu2xError_Cancel,
		eWaifu2xError_NotInitialized,
		eWaifu2xError_InvalidParameter,
		eWaifu2xError_FailedOpenInputFile,
		eWaifu2xError_FailedOpenOutputFile,
		eWaifu2xError_FailedOpenModelFile,
		eWaifu2xError_FailedParseModelFile,
		eWaifu2xError_FailedWriteModelFile,
		eWaifu2xError_FailedConstructModel,
		eWaifu2xError_FailedProcessCaffe,
		eWaifu2xError_FailedCudaCheck,
		eWaifu2xError_FailedUnknownType,
	};

	enum eWaifu2xCudaError
	{
		eWaifu2xCudaError_OK = 0,
		eWaifu2xCudaError_NotFind,
		eWaifu2xCudaError_OldVersion,
		eWaifu2xCudaError_OldDevice,
	};

	enum eWaifu2xcuDNNError
	{
		eWaifu2xcuDNNError_OK = 0,
		eWaifu2xcuDNNError_NotFind,
		eWaifu2xcuDNNError_OldVersion,
		eWaifu2xcuDNNError_CannotCreate,
	};

	typedef std::function<bool()> waifu2xCancelFunc;

	struct stOutputExtentionElement
	{
		std::wstring ext;
		std::vector<int> depthList;
		boost::optional<int> imageQualityStart;
		boost::optional<int> imageQualityEnd;
		boost::optional<int> imageQualityDefault;
		boost::optional<int> imageQualitySettingVolume;
	};

	const static std::vector<stOutputExtentionElement> OutputExtentionList;

private:
	bool is_inited;

	// ��x�ɏ�������摜�̕�
	int crop_size;
	// ��x�ɉ��u���b�N���������邩
	int batch_size;

	// �l�b�g�ɓ��͂���摜�̃T�C�Y
	int input_block_size;
	// �u���b�N�ϊ���̏o�̓T�C�Y
	int output_size;
	// �l�b�g���[�N�ɓ��͂���摜�̃T�C�Y(�o�͉摜�̕���layer_num * 2�����������Ȃ�)
	int block_width_height;
	// srcnn.prototxt�Œ�`���ꂽ���͂���摜�̃T�C�Y
	int original_width_height;

	std::string mode;
	int noise_level;
	boost::optional<double> scale_ratio;
	boost::optional<int> scale_width;
	boost::optional<int> scale_height;
	boost::filesystem::path model_dir;
	std::string process;

	int inner_padding;
	int outer_padding;

	int output_block_size;

	int input_plane;

	bool isCuda;

	boost::shared_ptr<caffe::Net<float>> net_noise;
	boost::shared_ptr<caffe::Net<float>> net_scale;

	float *input_block;
	float *dummy_data;
	float *output_block;

	bool use_tta;

	boost::optional<int> output_quality;
	int output_depth;

private:
	static eWaifu2xError LoadMat(cv::Mat &float_image, const boost::filesystem::path &input_file);
	static eWaifu2xError LoadMatBySTBI(cv::Mat &float_image, const std::vector<char> &img_data);
	static eWaifu2xError AlphaMakeBorder(std::vector<cv::Mat> &planes, const cv::Mat &alpha, const int offset);
	eWaifu2xError CreateBrightnessImage(const cv::Mat &float_image, cv::Mat &im);
	eWaifu2xError PaddingImage(const cv::Mat &input, cv::Mat &output);
	eWaifu2xError Zoom2xAndPaddingImage(const cv::Mat &input, cv::Mat &output, cv::Size_<int> &zoom_size);
	eWaifu2xError CreateZoomColorImage(const cv::Mat &float_image, const cv::Size_<int> &zoom_size, std::vector<cv::Mat> &cubic_planes);
	eWaifu2xError ConstractNet(boost::shared_ptr<caffe::Net<float>> &net, const boost::filesystem::path &model_path, const boost::filesystem::path &param_path, const std::string &process);
	eWaifu2xError LoadParameterFromJson(boost::shared_ptr<caffe::Net<float>> &net, const boost::filesystem::path &model_path, const boost::filesystem::path &param_path
		, const boost::filesystem::path &modelbin_path, const boost::filesystem::path &caffemodel_path, const std::string &process);
	eWaifu2xError SetParameter(caffe::NetParameter &param, const std::string &process) const;
	eWaifu2xError ReconstructImage(boost::shared_ptr<caffe::Net<float>> net, cv::Mat &im);
	static eWaifu2xError WriteMat(const cv::Mat &im, const boost::filesystem::path &output_file, const boost::optional<int> &output_quality);

	eWaifu2xError BeforeReconstructFloatMatProcess(const cv::Mat &in, cv::Mat &out, bool &convertBGRflag);
	eWaifu2xError ReconstructFloatMat(const bool isReconstructNoise, const bool isReconstructScale, const waifu2xCancelFunc cancel_func, const cv::Mat &in, cv::Mat &out);
	eWaifu2xError Reconstruct(const bool isReconstructNoise, const bool isReconstructScale, const waifu2xCancelFunc cancel_func, const cv::Mat &in, cv::Mat &out);
	eWaifu2xError AfterReconstructFloatMatProcess(const bool isReconstructScale, const waifu2xCancelFunc cancel_func, const cv::Mat &floatim, cv::Mat &in, cv::Mat &out);

	eWaifu2xError waifu2xConvetedMat(const bool isJpeg, const cv::Mat &inMat, cv::Mat &outMat,
		const waifu2xCancelFunc cancel_func = nullptr);

	double CalcScaleRatio(const cv::Size_<int> &size) const;

	static int DepthBitToCVDepth(const int depth_bit);
	static double GetValumeMaxFromCVDepth(const int cv_depth);
	static double GetEPS(const int cv_depth);

public:
	Waifu2x();
	~Waifu2x();

	static eWaifu2xcuDNNError can_use_cuDNN();
	static eWaifu2xCudaError can_use_CUDA();

	static void init_liblary();
	static void quit_liblary();

	// mode: noise or scale or noise_scale or auto_scale
	// process: cpu or gpu or cudnn
	eWaifu2xError init(int argc, char** argv, const std::string &mode, const int noise_level,
		const boost::optional<double> scale_ratio, const boost::optional<int> scale_width, const boost::optional<int> scale_height,
		const boost::filesystem::path &model_dir, const std::string &process,
		const boost::optional<int> output_quality = boost::optional<int>(), const int output_depth = 8, const bool use_tta = false,
		const int crop_size = 128, const int batch_size = 1);

	void destroy();

	eWaifu2xError waifu2x(const boost::filesystem::path &input_file, const boost::filesystem::path &output_file,
		const waifu2xCancelFunc cancel_func = nullptr);

	// factor: �{��
	// source: (4�`�����l���̏ꍇ��)RGBA�ȉ�f�z��
	// dest: (4�`�����l���̏ꍇ��)��������RGBA�ȉ�f�z��
	// in_stride: source�̃X�g���C�h(�o�C�g�P��)
	// out_stride: dest�̃X�g���C�h(�o�C�g�P��)
	eWaifu2xError waifu2x(double factor, const void* source, void* dest, int width, int height, int in_channel, int in_stride, int out_channel, int out_stride);

	const std::string& used_process() const;

	static cv::Mat LoadMat(const boost::filesystem::path &path);
};
