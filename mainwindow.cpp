#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "PlatformImageUtils.h"
#include "ImageFramework.h"
#include "Image.h"
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
        
		scene->addPixmap(QPixmap(fileName));

        ui->graphicsView->setScene(scene);
		ui->graphicsView->show();

		auto image = PlatformImageUtils::LoadInternalImage(fileName);
		auto sobelXImage = ImageFramework::Convolve(image,Kernel::GetSobelXKernel(),ConvolutionBorderHandlingMode::extend);
		PlatformImageUtils::SaveImage(sobelXImage, "C:\\sobel.png");
    }
}

void MainWindow::on_pushButton_2_clicked()
{

}
