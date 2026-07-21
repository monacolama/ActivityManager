#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QPalette>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    QColor darkWindow(30, 30, 30);
    QColor darkBase(45, 45, 45);
    QColor niceBlue(42, 130, 218);

    darkPalette.setColor(QPalette::Window, darkWindow);
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, darkBase);
    darkPalette.setColor(QPalette::AlternateBase, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, darkWindow);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, darkWindow);
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, niceBlue);
    darkPalette.setColor(QPalette::Highlight, niceBlue);
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);

    darkPalette.setColor(QPalette::Midlight, QColor(60, 60, 60));
    darkPalette.setColor(QPalette::Light, QColor(90, 90, 90));

    a.setPalette(darkPalette);

    MainWindow w;
    w.show();
    return a.exec();
}
