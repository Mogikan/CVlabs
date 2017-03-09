#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "PlatformImageUtils.h"
#include "ImageFramework.h"
#include "DebugHelper.h"
#include "Image.h"
#include "MoravecPOIDetector.h"
#include "HarrisPOIDetector.h"
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
	auto image = PlatformImageUtils::ConvertQImageToInternalImage(qImage);

	MoravecPOIDetector detector;
	auto& matrix = image->GetNormalizedMatrix();
	auto points = detector.FindPoints(*matrix, true, 300);
	for (int i = 0; i < points.size(); i++)
	{
		matrix->SetElementAt(points[i].x, points[i].y, 1);
	}
	auto newImage = make_unique<Image>(matrix->ExtractData(), matrix->Width(), matrix->Height());
	ShowImage(PlatformImageUtils::QImageFromInternalImage(*newImage));
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
{
	auto image = PlatformImageUtils::ConvertQImageToInternalImage(qImage);
	
	HarrisPOIDetector detector;
	auto& matrix = image->GetNormalizedMatrix();
	auto points = detector.FindPoints(*matrix,true,300);
	for (int i = 0; i < points.size(); i++)
	{
		matrix->SetElementAt(points[i].x, points[i].y, 1);
	}
	//PlatformImageUtils::SaveImage(Image(*matrix), "C:\\harris.png");
	auto newImage = make_unique<Image>(matrix->ExtractData(), matrix->Width(), matrix->Height());
	ShowImage(PlatformImageUtils::QImageFromInternalImage(*newImage));
}
