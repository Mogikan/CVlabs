#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "PlatformImageUtils.h"
#include "ImageFramework.h"
#include "DebugHelper.h"
#include "Image.h"
#include "POIDetector.h"
#include "DescriptorService.h"
#include "HomographyHelper.h"
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


void MainWindow::on_pushButton_3_clicked()
{
    QFileDialog imagePicker(this);
    imagePicker.setFileMode(QFileDialog::ExistingFile);
    imagePicker.setNameFilter(tr("Images (*.png *.jpg)"));
    if (imagePicker.exec())
    {
        auto fileName = imagePicker.selectedFiles()[0];
        qImage2 = PlatformImageUtils::LoadQImageFromFile(fileName);
		ShowImage(qImage2);
    }
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

void MainWindow::on_pushButton_4_clicked()
{//value match
	//auto image = PlatformImageUtils::ConvertQImageToInternalImage(qImage)->GetNormalizedMatrix();
	//int octaveCount = log2(min(image->Height(), image->Width()));
	//
	//auto pyramid = make_unique<GaussPyramid>(*image, octaveCount);
	//auto& blobs = pyramid->FindBlobs();
	//ShowImage(PlatformImageUtils::DrawImage(*image, blobs));
	//ShowImage(PlatformImageUtils::QImageFromInternalImage(*ImageFramework::ApplyCannyOperator(*image)));

	auto image1 = PlatformImageUtils::ConvertQImageToInternalImage(qImage)->GetNormalizedMatrix();
	auto image2 = PlatformImageUtils::ConvertQImageToInternalImage(qImage2)->GetNormalizedMatrix();
	DescriptorService service;
	auto detector = ImageFramework::CreatePOIDetector(POISearchMethod::Harris);
	
	int octaveCount1 = log2(min(image1->Height(), image1->Width())) - 1;
	GaussPyramid pyramid1(*image1, octaveCount1);
	auto& blobs1 = pyramid1.FindBlobs();
	auto& descriptors1 = service.BuildGradientDirectionDescriptors(*image1, pyramid1, blobs1);
	
	int octaveCount2 = log2(min(image2->Height(), image2->Width())) - 1;
	GaussPyramid pyramid2(*image2, octaveCount2);
	auto& blobs2 = pyramid2.FindBlobs();
	auto& descriptors2 = service.BuildGradientDirectionDescriptors(*image2, pyramid2, blobs2);
	const vector<pair<Descriptor, Descriptor>>& matches = service.FindMatches(descriptors1, descriptors2);

	
	ShowImage(PlatformImageUtils::DrawImage(*image1, *image2, matches));
}

void MainWindow::on_pushButton_5_clicked()
{//gradient match
	auto image1 = PlatformImageUtils::ConvertQImageToInternalImage(qImage)->GetNormalizedMatrix();
	auto image2 = PlatformImageUtils::ConvertQImageToInternalImage(qImage2)->GetNormalizedMatrix();
	DescriptorService service;
	auto detector = ImageFramework::CreatePOIDetector(POISearchMethod::Harris);
	
	int octaveCount1 = log2(min(image1->Height(), image1->Width())) - 1;
	GaussPyramid pyramid1(*image1, octaveCount1);
	auto& blobs1 = pyramid1.FindBlobs();	
	auto& descriptors1 = service.BuildGradientDirectionDescriptors(*image1, pyramid1,blobs1);

	int octaveCount2 = log2(min(image2->Height(), image2->Width())) - 1;
	GaussPyramid pyramid2(*image2, octaveCount2);
	auto& blobs2 = pyramid2.FindBlobs();
	auto& descriptors2 = service.BuildGradientDirectionDescriptors(*image2, pyramid2,blobs2);
	const vector<pair<Descriptor, Descriptor>>& matches = service.FindMatches(descriptors1, descriptors2);
	
	auto& homography = HomographyHelper::FindBestHomography(matches);
	ShowImage(PlatformImageUtils::CombineImages(qImage,qImage2,homography));
	//ShowImage(PlatformImageUtils::DrawImage(*image1, *image2, matches, image1->Width()));
}
