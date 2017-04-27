#pragma once;
#include <QtGui>
#include <QMainWindow>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();


    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow *ui;
	QGraphicsScene *scene;
	QImage qImage;	
    QImage qImage2;
	void ShowImage(QImage image);
	void FindEllipses();
	void FindCircles();
};

