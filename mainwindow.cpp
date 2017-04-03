#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "PlatformImageUtils.h"
#include "ImageFramework.h"
#include "DebugHelper.h"
#include "Image.h"
#include "POIDetector.h"
#include "DescriptorService.h"

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
    //auto image1 = PlatformImageUtils::ConvertQImageToInternalImage(qImage)->GetNormalizedMatrix();
 //   auto image2 = PlatformImageUtils::ConvertQImageToInternalImage(qImage2)->GetNormalizedMatrix();
 //   DescriptorService service;
	//auto detector = ImageFramework::CreatePOIDetector(POISearchMethod::Harris);	
	//auto points = detector.FindPoints(*image1,true,100);
	//auto points2 = detector.FindPoints(*image2,true,100);
	//auto descriptors1 = service.BuildAverageValueDescriptors(*image1, points);
	//auto descriptors2 = service.BuildAverageValueDescriptors(*image2, points2);
	//vector<pair<Point,Point>> matches = service.FindMatches(descriptors1, descriptors2);
	//auto matchedImage = PlatformImageUtils::DrawImage(*image1,*image2, matches, image1->Width());
	//ShowImage(matchedImage);
	//auto& gaussPyramid = ImageFramework::BuildGaussPyramid(*image1, 6, 4, 0.5, 1.6);
	//auto& blobs = gaussPyramid->FindBlobs();
	//ShowImage(PlatformImageUtils::DrawImage(*image1,blobs));
	auto image = PlatformImageUtils::ConvertQImageToInternalImage(qImage)->GetDoubleMatrix();
	int octaveCount = log2(min(image->Height(), image->Width()));

	auto pyramid = ImageFramework::BuildGaussPyramid(*image, octaveCount, 3, 1.6, 0.5);
	auto& blobs = pyramid->FindBlobs();
	ShowImage(PlatformImageUtils::DrawImage(*image, blobs));
	//for (int i = 0; i < pyramid->OctavesCount(); i++)
	//{
	//	auto octave = pyramid->OctaveAt(i);
	//	for (int j = 0; j < octave.LayersCount(); j++)
	//	{
	//		auto layer = octave.LayerAt(j);
	//		PlatformImageUtils::SaveImage(Image(layer.ImageD()),
	//			"C:\\Pyramid\\Octave_" + QString::number(i) + "_Layer_" + QString::number(j) + "_Sigma_" + QString::number(layer.Sigma()) + "_ESigma_" + QString::number(layer.EffectiveSigma()) + ".png");
	//	}
	//}
}

void MainWindow::on_pushButton_5_clicked()
{//gradient match
	auto image1 = PlatformImageUtils::ConvertQImageToInternalImage(qImage)->GetNormalizedMatrix();
	auto image2 = PlatformImageUtils::ConvertQImageToInternalImage(qImage2)->GetNormalizedMatrix();
	DescriptorService service;
	auto detector = ImageFramework::CreatePOIDetector(POISearchMethod::Harris);
	auto points = detector.FindPoints(*image1,true,300);
	auto points2 = detector.FindPoints(*image2,true,300);
	auto descriptors1 = service.BuildGradientDirectionDescriptors(*image1, points);
	auto descriptors2 = service.BuildGradientDirectionDescriptors(*image2, points2);
	vector<pair<Point, Point>> matches = service.FindMatches(descriptors1, descriptors2);	
	auto matchedImage = PlatformImageUtils::DrawImage(*image1,*image2, matches, image1->Width());
	ShowImage(matchedImage);
}
