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

void MainWindow::on_pushButton_2_clicked()
{
	auto image = PlatformImageUtils::ConvertQImageToInternalImage(qImage);
	auto pyramid = ImageFramework::BuildGaussPyramid(*image->GetDoubleMatrix(), 5, 3, 1.6, 0.5);
	for (int i = 0; i < pyramid->OctavesCount(); i++)
	{
		auto octave = pyramid->OctaveAt(i);
		for (int j = 0; j < octave.LayersCount(); j++)
		{
			auto layer = octave.LayerAt(j);
			PlatformImageUtils::SaveImage(Image(layer.Image()),"C:\\piramid\\"+QString::number(i)+"_"+ QString::number(j)+".png");
		}
	}
	//auto sobelProcessedImageMatrix = ImageFramework::ApplySobelOperator(*image->GetDoubleMatrix(), ConvolutionBorderHandlingMode::zero);
	//auto sobelImage = make_unique<Image>(sobelProcessedImageMatrix->ExtractData(), sobelProcessedImageMatrix->GetWidth(), sobelProcessedImageMatrix->GetHeight());
	//
	//ShowImage(PlatformImageUtils::QImageFromInternalImage(sobelImage));
	
}

void MainWindow::on_pushButton_3_clicked()
{
	auto image = PlatformImageUtils::ConvertQImageToInternalImage(qImage);
	//double gaussValues[] = { 1,2,1 };
	//auto gausKernelY = make_unique<Kernel>(gaussValues, 1, 3,Point(0,1));
	//auto gausKernelX = make_unique<Kernel>(gaussValues, 3, 1,Point(1,0));
	auto gaussImageMatrix = ImageFramework::ApplyGaussSmooth(*image->GetDoubleMatrix(), 1);
	auto gaussImage = make_unique<Image>(gaussImageMatrix->ExtractData(), gaussImageMatrix->Width(), gaussImageMatrix->Height());
	qImage = PlatformImageUtils::QImageFromInternalImage(gaussImage);
	ShowImage(qImage);	
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
