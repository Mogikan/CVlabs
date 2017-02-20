#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "PlatformImageUtils.h"
#include "ImageFramework.h"
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

void MainWindow::on_pushButton_clicked()
{
    QFileDialog imagePicker(this);
    imagePicker.setFileMode(QFileDialog::ExistingFile);
    imagePicker.setNameFilter(tr("Images (*.png *.jpg)"));
    if (imagePicker.exec())
    {
        auto fileName = imagePicker.selectedFiles()[0];		
        
//		scene->addPixmap(QPixmap(fileName));

//        ui->graphicsView->setScene(scene);
//		ui->graphicsView->show();

		auto image = PlatformImageUtils::LoadInternalImage(fileName);
		//auto sobelXImage = ImageFramework::Convolve(image,Kernel::GetSobelXKernel(),ConvolutionBorderHandlingMode::extend);
		//PlatformImageUtils::SaveImage(sobelXImage, "C:\\sobel.png");
		//
		//double conv0[] = { 1,0,0,0,0,0,0,0,0 };
		//auto conv0Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv0,3), ConvolutionBorderHandlingMode::extend);
		//PlatformImageUtils::SaveImage(conv0Image, "C:\\conv0.png");
		//
		//double conv1[] = { 0,1,0,0,0,0,0,0,0 };
		//auto conv1Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv1, 3), ConvolutionBorderHandlingMode::extend);
		//PlatformImageUtils::SaveImage(conv1Image, "C:\\conv1.png");
		//
		//double conv2[] = { 0,0,1,0,0,0,0,0,0 };
		//auto conv2Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv2, 3), ConvolutionBorderHandlingMode::extend);
		//PlatformImageUtils::SaveImage(conv2Image, "C:\\conv2.png");
		//
		//double conv3[] = { 0,0,0,1,0,0,0,0,0 };
		//auto conv3Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv3, 3), ConvolutionBorderHandlingMode::extend);
		//PlatformImageUtils::SaveImage(conv3Image, "C:\\conv3.png");
		PlatformImageUtils::SaveImage(image, "c:\\original.png");
		auto newImage = unique_ptr<Image>(new Image(image->GetNormilizedDoubleData(), image->GetWidth(), image->GetHeight()));
		PlatformImageUtils::SaveImage(newImage, "C:\\greyImg.png");
		double conv4[] = { 0,0,0,0,1,0,0,0,0 };
		auto conv4Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv4, 3), ConvolutionBorderHandlingMode::extend);
		PlatformImageUtils::SaveImage(conv4Image, "C:\\conv4.png");


		//double conv5[] = { 0,0,0,0,0,1,0,0,0 };		
		//auto conv5Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv5, 3), ConvolutionBorderHandlingMode::extend);
		//PlatformImageUtils::SaveImage(conv5Image, "C:\\conv5.png");
		//
		//double conv6[] = { 0,0,0,0,0,0,1,0,0 };
		//auto conv6Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv6, 3), ConvolutionBorderHandlingMode::extend);
		//PlatformImageUtils::SaveImage(conv6Image, "C:\\conv6.png");
		//
		//double conv7[] = { 0,0,0,0,0,0,0,1,0 };
		//auto conv7Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv7, 3), ConvolutionBorderHandlingMode::extend);
		//PlatformImageUtils::SaveImage(conv7Image, "C:\\conv7.png");
		//
		//double conv8[] = { 0,0,0,0,0,0,0,0,1 };
		//auto conv8Image = ImageFramework::Convolve(image, make_unique<Kernel>(conv8, 3), ConvolutionBorderHandlingMode::extend);
		//PlatformImageUtils::SaveImage(conv8Image, "C:\\conv8.png");

		
    }
}

void MainWindow::on_pushButton_2_clicked()
{

}
