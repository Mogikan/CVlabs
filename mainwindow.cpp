#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "PlatformImageUtils.h"
#include "ImageFramework.h"
#include "DebugHelper.h"
#include "Image.h"
#include <memory>
using namespace std;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	scene = new QGraphicsScene(this);
}

MainWindow::~MainWindow()
{
    delete ui;
	delete scene;
}

void TestConvolution() 
{
	double testKernel[] = { -1,-2,-1,0,0,0,1,2,1 };
	auto kernel = make_unique<Kernel>(testKernel, 3,3);
	uchar* input = new uchar[9]{ 1,2,3,4,5,6,7,8,9 };
	auto image = make_unique<Image>(input, 3, 3, 1);
	auto resultImg = ImageFramework::Convolve(image->GetDoubleMatrix(), kernel, ConvolutionBorderHandlingMode::zero);
	auto result = resultImg->ExtractData();
}

void MainWindow::on_pushButton_clicked()
{
    QFileDialog imagePicker(this);
    imagePicker.setFileMode(QFileDialog::ExistingFile);
    imagePicker.setNameFilter(tr("Images (*.png *.jpg)"));
    if (imagePicker.exec())
    {
        auto fileName = imagePicker.selectedFiles()[0];		
		qImage = PlatformImageUtils::LoadQImageFromFile(fileName);
		scene->addPixmap(QPixmap::fromImage(qImage));
		ui->graphicsView->setScene(scene);
		ui->graphicsView->show();


		auto image = PlatformImageUtils::LoadInternalImage(fileName);
		auto sobelXImage = ImageFramework::Convolve(image->GetDoubleMatrix(), Kernel::GetSobelXKernel(),ConvolutionBorderHandlingMode::zero);		
		PlatformImageUtils::SaveImage(make_unique<Image>(sobelXImage->ReadData(), sobelXImage->GetWidth(),sobelXImage->GetHeight()), "C:\\sobelX.png");

		
		auto sobelYImage = ImageFramework::Convolve(image->GetDoubleMatrix(), Kernel::GetSobelYKernel(), ConvolutionBorderHandlingMode::zero);
		auto img = make_unique<Image>(sobelYImage->ReadData(), sobelYImage->GetWidth(), sobelYImage->GetHeight());
		//img->NormalizeImage();
		PlatformImageUtils::SaveImage(img, "C:\\sobelY.png");
		
		double gauss[] = {1,2,1};
		
		auto gausKernelX = make_unique<Kernel>(gauss, 1, 3);
		auto gausKernelY = make_unique<Kernel>(gauss, 3, 1);
		auto gaussImage = ImageFramework::Convolve(ImageFramework::Convolve(image->GetDoubleMatrix(), gausKernelX,ConvolutionBorderHandlingMode::extend),gausKernelY,ConvolutionBorderHandlingMode::extend);
		PlatformImageUtils::SaveImage(make_unique<Image>(gaussImage), "C:\\gauss.png");
		//"C:\Users\korney\AppData\Roaming\Skype\vitaliy.korney\media_messaging\media_cache_v3\^28F5D15ABD418D5C0F09FB51FCDFC8DC9BBEDD146ED56E0785^pimgpsh_fullsize_distr.jpg"

		//auto sobelXPixels = sobelXImage->GetDoubleData();
		//scene->addPixmap(QPixmap::fromImage(sobelXQImage));
		//ui->graphicsView->setScene(scene);
		//ui->graphicsView->show();
		//auto sobelYImage = ImageFramework::Convolve(image, Kernel::GetSobelYKernel(), ConvolutionBorderHandlingMode::extend);
		//auto sobelYQImage = PlatformImageUtils::QImageFromInternalImage(sobelYImage);
		//PlatformImageUtils::SaveImage(sobelYImage, "C:\\sobelY.png");
		//auto sobelYPixels = sobelYImage->GetDoubleData();
		//int width = image->GetWidth();
		//int height = image->GetHeight();
		//auto sobelOperatorResult = make_unique<uchar[]>(width*height);		
		//std::transform(sobelXPixels.get(), sobelXPixels.get() + width*height, sobelYPixels.get(), sobelOperatorResult.get(), 
		//	[](double sx, double sy)->uchar 
		//{
		//	return (uchar)(std::min(255.,sqrt(sx*sx + sy*sy))); 
		//});
		//TestConvolution();
		
		auto sobel2D = ImageFramework::ApplySobelOperator(image->GetDoubleMatrix(),ConvolutionBorderHandlingMode::zero);
		auto sobelImage = make_unique<Image>(sobel2D->ReadData(),sobel2D->GetWidth(),sobel2D->GetHeight());
		PlatformImageUtils::SaveImage(sobelImage, "C:\\sobel.png");
		
		

		
		//double test[] = { -2, -1, 0, -1, 1, 1,0, 1, 2 };
		//auto conv0Matrix = ImageFramework::Convolve(image, make_unique<Kernel>(test,3), ConvolutionBorderHandlingMode::zero);
		//PlatformImageUtils::SaveImage(make_unique<Image>(conv0Matrix->ExtractData(),image->GetWidth(),image->GetHeight()), "C:\\test.png");
		//
		//double conv1[] = { 0,1,0,0,0,0,0,0,0 };
		//auto conv1Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv1, 3), ConvolutionBorderHandlingMode::zero);
		//PlatformImageUtils::SaveImage(conv1Image, "C:\\conv1.png");
		//
		//double conv2[] = { 0,0,1,0,0,0,0,0,0 };
		//auto conv2Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv2, 3), ConvolutionBorderHandlingMode::zero);
		//PlatformImageUtils::SaveImage(conv2Image, "C:\\conv2.png");
		//
		//double conv3[] = { 0,0,0,1,0,0,0,0,0 };
		//auto conv3Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv3, 3), ConvolutionBorderHandlingMode::zero);
		//PlatformImageUtils::SaveImage(conv3Image, "C:\\conv3.png");
	
		//PlatformImageUtils::SaveImage(image, "c:\\original.png");
		//auto newImage = unique_ptr<Image>(new Image(image->GetNormilizedDoubleData(), image->GetWidth(), image->GetHeight()));
		//PlatformImageUtils::SaveImage(newImage, "C:\\greyImg.png");
		//double conv4[] = { 0,0,0,0,1,0,0,0,0 };
		//auto conv4Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv4, 3), ConvolutionBorderHandlingMode::zero);
		//PlatformImageUtils::SaveImage(conv4Image, "C:\\conv4.png");
		//
		//
		//double conv5[] = { 0,0,0,0,0,1,0,0,0 };		
		//auto conv5Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv5, 3), ConvolutionBorderHandlingMode::zero);
		//PlatformImageUtils::SaveImage(conv5Image, "C:\\conv5.png");
		//
		//double conv6[] = { 0,0,0,0,0,0,1,0,0 };
		//auto conv6Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv6, 3), ConvolutionBorderHandlingMode::zero);
		//PlatformImageUtils::SaveImage(conv6Image, "C:\\conv6.png");
		//
		//double conv7[] = { 0,0,0,0,0,0,0,1,0 };
		//auto conv7Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv7, 3), ConvolutionBorderHandlingMode::zero);
		//PlatformImageUtils::SaveImage(conv7Image, "C:\\conv7.png");
		//
		//double conv8[] = { 0,0,0,0,0,0,0,0,1 };
		//auto conv8Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv8, 3), ConvolutionBorderHandlingMode::zero);
		//PlatformImageUtils::SaveImage(conv8Image, "C:\\conv8.png");

		
    }
}

void MainWindow::on_pushButton_2_clicked()
{

}

void MainWindow::on_pushButton_3_clicked()
{

}
