#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "PlatformImageUtils.h"
#include "ImageFramework.h"
#include "DebugHelper.h"
#include "Image.h"
#include "POIDetector.h"
#include "DescriptorService.h"
#include "TransformationHelper.h"
#include <memory>
#include "HoughFeatureExtractor.h"
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
	auto kernel = make_unique<Kernel>(testKernel, 3,3,Point(1,1));
	uchar* input = new uchar[9]{ 1,2,3,4,5,6,7,8,9 };
	auto image = make_unique<Image>(input, 3, 3, 1);
	auto resultImg = ImageFramework::Convolve(*image->GetDoubleMatrix(), *kernel, BorderMode::zero);
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
		ShowImage(qImage);
    }
}

void MainWindow::FindCircles() 
{
	auto image = PlatformImageUtils::ConvertQImageToInternalImage(qImage)->GetNormalizedMatrix();
	//int octaveCount = log2(min(image->Height(), image->Width()));
	//
	//auto pyramid = make_unique<GaussPyramid>(*image, octaveCount);
	//auto& blobs = pyramid->FindBlobs();
	//ShowImage(PlatformImageUtils::DrawImage(*image, blobs));
	auto smoothedImage = ImageFramework::ApplyGaussSmooth(*image, 1.4);
	auto sobelX = ImageFramework::ApplySobelX(*smoothedImage);
	auto sobelY = ImageFramework::ApplySobelY(*smoothedImage);
	auto magnitude = Matrix2D(image->Width(), image->Height());
	auto direction = Matrix2D(image->Width(), image->Height());
	for (int y = 0; y < image->Height(); y++)
	{
		for (int x = 0; x < image->Width(); x++)
		{
			double dx = sobelX->At(x, y);
			double dy = sobelY->At(x, y);
			magnitude.SetElementAt(x, y, hypot(dx, dy));
			direction.SetElementAt(x, y, atan2(dy, dx));//-PI to PI
		}
	}
	int width = image->Width();
	int height = image->Height();
	auto edges = ImageFramework::ApplyCannyOperator(direction, magnitude);
	//auto& points = HoughFeatureExtractor::FindLines(*edges, magnitude, direction);
	CircleSpaceSettings settings(min(width,height)/2,3, 3, 3,-width*2,width*2,-height*2,height*2);
	auto& circles = HoughFeatureExtractor::FindCircles(*edges, magnitude, direction,settings);
	//auto& points = HoughFeatureExtractor::FindEllipsesFast(*edges, magnitude, direction);
	auto imageWithLines = PlatformImageUtils::QImageFromInternalImage(Image(*edges));
	//PlatformImageUtils::DrawEllipses(imageWithLines, points);
	PlatformImageUtils::DrawCircles(imageWithLines, circles);
	ShowImage(imageWithLines);
}

void MainWindow::on_pushButton_3_clicked()
{
	FindCircles();
    //QFileDialog imagePicker(this);
    //imagePicker.setFileMode(QFileDialog::ExistingFile);
    //imagePicker.setNameFilter(tr("Images (*.png *.jpg)"));
    //if (imagePicker.exec())
    //{
    //    auto fileName = imagePicker.selectedFiles()[0];
    //    qImage2 = PlatformImageUtils::LoadQImageFromFile(fileName);
	//	ShowImage(qImage2);
    //}
}

void MainWindow::ShowImage(QImage image)
{
	if (scene->items().count()>0)
	{
		auto item = scene->items().at(0); 
		scene->removeItem(item); 
		delete item;
	}
	scene->addPixmap(QPixmap::fromImage(image));
	ui->graphicsView->setScene(scene);
	ui->graphicsView->show();
}

void MainWindow::FindEllipses() 
{
	auto image = PlatformImageUtils::ConvertQImageToInternalImage(qImage)->GetNormalizedMatrix();
	image = ImageFramework::DownscaleImageTwice(*image);
	image = ImageFramework::DownscaleImageTwice(*image);
	//int octaveCount = log2(min(image->Height(), image->Width()));
	//
	//auto pyramid = make_unique<GaussPyramid>(*image, octaveCount);
	//auto& blobs = pyramid->FindBlobs();
	//ShowImage(PlatformImageUtils::DrawImage(*image, blobs));
	auto smoothedImage = ImageFramework::ApplyGaussSmooth(*image, 1.4);
	auto sobelX = ImageFramework::ApplySobelX(*smoothedImage);
	auto sobelY = ImageFramework::ApplySobelY(*smoothedImage);
	auto magnitude = Matrix2D(image->Width(), image->Height());
	auto direction = Matrix2D(image->Width(), image->Height());
	int height = image->Height();
	int width = image->Width();
	for (int y = 0; y < image->Height(); y++)
	{
		for (int x = 0; x < image->Width(); x++)
		{
			double dx = sobelX->At(x, y);
			double dy = sobelY->At(x, y);
			magnitude.SetElementAt(x, y, hypot(dx, dy));
			direction.SetElementAt(x, y, atan2(dy, dx));//-PI to PI
		}
	}
	auto edges = ImageFramework::ApplyCannyOperator(direction, magnitude);
	//auto& points = HoughFeatureExtractor::FindLines(*edges, magnitude, direction);
	double threshold = 60;
	auto settings = EllipseSpaceSettings(threshold,2*M_PI,0,2*M_PI/72,100,2,4,width,0,height,0,5,4);
	auto& points = HoughFeatureExtractor::FindEllipses(*edges, magnitude, direction, settings);
	//auto& points = HoughFeatureExtractor::FindEllipsesFast(*edges, magnitude, direction);
	auto imageWithLines = PlatformImageUtils::QImageFromInternalImage(Image(*edges));
	//PlatformImageUtils::DrawEllipses(imageWithLines, points);
	PlatformImageUtils::DrawEllipses(imageWithLines, points);
	ShowImage(imageWithLines);
}

void MainWindow::on_pushButton_4_clicked()
{//value match
	FindEllipses();
	//start lab 9
	//auto image1 = PlatformImageUtils::ConvertQImageToInternalImage(qImage)->GetNormalizedMatrix();
	//auto image2 = PlatformImageUtils::ConvertQImageToInternalImage(qImage2)->GetNormalizedMatrix();
	//DescriptorService service;
	//auto detector = ImageFramework::CreatePOIDetector(POISearchMethod::Harris);
	//
	//int octaveCount1 = log2(min(image1->Height(), image1->Width())) - 1;
	//GaussPyramid pyramid1(*image1, octaveCount1);
	//auto& blobs1 = pyramid1.FindBlobs();	
	//auto& descriptors1 = service.BuildGradientDirectionDescriptors(*image1, pyramid1,blobs1);
	//
	//int octaveCount2 = log2(min(image2->Height(), image2->Width())) - 1;
	//GaussPyramid pyramid2(*image2, octaveCount2);
	//auto& blobs2 = pyramid2.FindBlobs();
	//auto& descriptors2 = service.BuildGradientDirectionDescriptors(*image2, pyramid2,blobs2);
	//const vector<pair<Descriptor, Descriptor>>& matches = service.FindMatches(descriptors1, descriptors2);
	//Size image1Size(image1->Width(), image1->Height());
	//Size image2Size(image2->Width(), image2->Height());
	//DescriptorDimentionSettings houghDimensionSettings(
	//	image2->Width()*2, 
	//	-image2->Width()*2, 
	//	20, 
	//	image2->Height()*2, 
	//	-image2->Height()*2, 
	//	20, 
	//	1 / 8.,
	//	octaveCount2*2);
	//
	//TransformationMetaInfo metaInfo;
	//vector<int> inliers;
	//tie(metaInfo,inliers)=HoughFeatureExtractor::FindObjectPose(image1Size, matches, houghDimensionSettings);
	//auto affineTransform = TransformationHelper::CalculateAffineTransform(matches, inliers);
	//auto& objectPicture = qImage2.copy();
	//PlatformImageUtils::DrawObjectBounds(objectPicture, image1Size, affineTransform);	
	//PlatformImageUtils::DrawObjectBounds(objectPicture,image1Size, metaInfo);
	//ShowImage(objectPicture);
	//end
	//auto& homography = HomographyHelper::FindBestHomography();
//	ShowImage(PlatformImageUtils::CombineImages(qImage,qImage2,homography));

	//auto image1 = PlatformImageUtils::ConvertQImageToInternalImage(qImage)->GetNormalizedMatrix();
	//auto image2 = PlatformImageUtils::ConvertQImageToInternalImage(qImage2)->GetNormalizedMatrix();
	//DescriptorService service;
	//auto detector = ImageFramework::CreatePOIDetector(POISearchMethod::Harris);
	//
	//int octaveCount1 = log2(min(image1->Height(), image1->Width())) - 1;
	//GaussPyramid pyramid1(*image1, octaveCount1);
	//auto& blobs1 = pyramid1.FindBlobs();
	//auto& descriptors1 = service.BuildGradientDirectionDescriptors(*image1, pyramid1, blobs1);
	//
	//int octaveCount2 = log2(min(image2->Height(), image2->Width())) - 1;
	//GaussPyramid pyramid2(*image2, octaveCount2);
	//auto& blobs2 = pyramid2.FindBlobs();
	//auto& descriptors2 = service.BuildGradientDirectionDescriptors(*image2, pyramid2, blobs2);
	//const vector<pair<Descriptor, Descriptor>>& matches = service.FindMatches(descriptors1, descriptors2);
	//
	//
	//ShowImage(PlatformImageUtils::DrawImage(*image1, *image2, matches));
}

void MainWindow::on_pushButton_5_clicked()
{
	auto image = PlatformImageUtils::ConvertQImageToInternalImage(qImage)->GetNormalizedMatrix();
	//int octaveCount = log2(min(image->Height(), image->Width()));
	//
	//auto pyramid = make_unique<GaussPyramid>(*image, octaveCount);
	//auto& blobs = pyramid->FindBlobs();
	//ShowImage(PlatformImageUtils::DrawImage(*image, blobs));
	auto smoothedImage = ImageFramework::ApplyGaussSmooth(*image, 1.4);
	auto sobelX = ImageFramework::ApplySobelX(*smoothedImage);
	auto sobelY = ImageFramework::ApplySobelY(*smoothedImage);
	auto magnitude = Matrix2D(image->Width(), image->Height());
	auto direction = Matrix2D(image->Width(), image->Height());
	for (int y = 0; y < image->Height(); y++)
	{
		for (int x = 0; x < image->Width(); x++)
		{
			double dx = sobelX->At(x, y);
			double dy = sobelY->At(x, y);
			magnitude.SetElementAt(x, y, hypot(dx, dy));
			direction.SetElementAt(x, y, atan2(dy, dx));//-PI to PI
		}
	}
	auto edges = ImageFramework::ApplyCannyOperator(direction, magnitude);
	int roMax = hypot(magnitude.Width(), magnitude.Height());
	int roMin = -roMax;
	double threshold = 10;
	LineSpaceSettings settings(roMax, roMin, 2, 2 * M_PI / 144,threshold);
	auto& lineDesriptors = HoughFeatureExtractor::FindLines(*edges, magnitude, direction,settings);
	double lineThreshold = 30;
	
	auto& lines = HoughFeatureExtractor::FindLineSegments(lineDesriptors,Size(image->Width(),image->Height()), lineThreshold);
	auto imageWithLines = PlatformImageUtils::QImageFromInternalImage(Image(*edges));
	PlatformImageUtils::DrawLines(imageWithLines, lines);
	ShowImage(imageWithLines);
	//gradient match
	//auto image1 = PlatformImageUtils::ConvertQImageToInternalImage(qImage)->GetNormalizedMatrix();
	//auto image2 = PlatformImageUtils::ConvertQImageToInternalImage(qImage2)->GetNormalizedMatrix();
	//DescriptorService service;
	//auto detector = ImageFramework::CreatePOIDetector(POISearchMethod::Harris);
	//
	//int octaveCount1 = log2(min(image1->Height(), image1->Width())) - 1;
	//GaussPyramid pyramid1(*image1, octaveCount1);
	//auto& blobs1 = pyramid1.FindBlobs();	
	//auto& descriptors1 = service.BuildGradientDirectionDescriptors(*image1, pyramid1,blobs1);
	//
	//int octaveCount2 = log2(min(image2->Height(), image2->Width())) - 1;
	//GaussPyramid pyramid2(*image2, octaveCount2);
	//auto& blobs2 = pyramid2.FindBlobs();
	//auto& descriptors2 = service.BuildGradientDirectionDescriptors(*image2, pyramid2,blobs2);
	//const vector<pair<Descriptor, Descriptor>>& matches = service.FindMatches(descriptors1, descriptors2);
	//
	//auto& homography = HomographyHelper::FindBestHomography(matches);
	//ShowImage(PlatformImageUtils::CombineImages(qImage,qImage2,homography));
	//ShowImage(PlatformImageUtils::DrawImage(*image1, *image2, matches, image1->Width()));


}
