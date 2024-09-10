#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sudokumodel.h"
#include <QPushButton>
#include <QTableWidgetItem>
#include <QTranslator>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void fillGrid();
    void setCellStyle(QTableWidgetItem* cell, QString color);
    void checkForWin();
    void restoreInitialCellColors();
    void cellsToBeConsidered(int row, int column);

private slots :
    void onDifficultyChanged(const QString& difficulty);
    void onNumberButtonClicked();
    void onEraseButtonClicked();
    void onResetButtonClicked();
    void onNewGameButtonClicked();
    void onHelpButtonClicked();
    void onCellSelected(int row, int column);
    void resetNumberButtonStyles();
    void onQuitPressed();
    void onAboutPressed();
    void switchToFrench();
    void switchToEnglish();

protected :
    void changeEvent(QEvent *event);

private:
    Ui::MainWindow *ui;
    sudokuModel *model;
    QString _filePath;
    QList<QPushButton*> _numberButtons;
    QTranslator translator;
    QVector<QVector<QColor>> _initialCellColors; // Pour stocker les couleurs initiales des cases
    void connectButtons();
    void connectMenuActions();
};
#endif // MAINWINDOW_H
