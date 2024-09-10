#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QTimer>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(new sudokuModel())
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Sudoku Assistant"));
    this->setStyleSheet("QPushButton { border: 2px solid black; border-radius: 5px; background-color: lightblue;}"
                        "QPushButton:hover{ background-color: white; }");

    _numberButtons = {ui->b1, ui->b2, ui->b3,
                      ui->b4, ui->b5, ui->b6,
                      ui->b7, ui->b8, ui->b9};

    connectButtons();
    connectMenuActions();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::fillGrid(){
    QVector<QVector<int>> lines = model->getLines();
    _initialCellColors = QVector<QVector<QColor>>(9, QVector<QColor>(9));

        for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            QTableWidgetItem *cell = ui->tableWidget->item(i, j);
            if (!cell) {
                cell = new QTableWidgetItem();
                ui->tableWidget->setItem(i, j, cell);
            }

            int x0 = (i / 3) * 3;
            int y0 = (j / 3) * 3;
            if (((x0 == 0 || x0 == 6) && (y0 == 0 || y0 == 6)) || (x0 == 3 && y0 == 3)) {
                cell->setBackground(QColor("lightblue"));
            } else {
                cell->setBackground(QColor("white"));
            }

            if (lines[i][j] != 0) {
                cell->setText(QString::number(lines[i][j]));
                setCellStyle(cell, "black");
            } else {
                cell->setText("");
            }

            _initialCellColors[i][j] = cell->background().color();
        }
    }
}


void MainWindow::onDifficultyChanged(const QString& difficulty) {
    if (difficulty == "Easy") {
        _filePath = ":/Easy.txt";
    } else if (difficulty == "Medium") {
        _filePath = ":/Medium.txt";
    } else if (difficulty == "Hard") {
        _filePath = ":/Hard.txt";
    } else if (difficulty == "Insane"){
        _filePath = ":/Insane.txt";
    }

    QVector<QString> stringLines = model->readFile(_filePath);
    model->extractData(stringLines);
    fillGrid();
}

void MainWindow::setCellStyle(QTableWidgetItem* cell, QString color){
    cell->setForeground(QColor(color));
    cell->setTextAlignment(Qt::AlignCenter);
    QFont font("Helvetica", 15);
    cell->setFont(font);
}

void MainWindow::onNumberButtonClicked() {
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    int number = clickedButton->text().toInt();

    int currentRow, currentColumn;

    QTableWidgetItem* currentItem = ui->tableWidget->currentItem();
    if (currentItem) {
        currentRow = ui->tableWidget->currentRow();
        currentColumn = ui->tableWidget->currentColumn();

        if (model->checkMove(currentRow, currentColumn, number)) {
            currentItem->setText(QString::number(number));
            setCellStyle(currentItem, "purple");
            model->updateModel(currentRow, currentColumn, number);
        }
    }
}

void MainWindow::onEraseButtonClicked() {
    QTableWidgetItem* currentItem = ui->tableWidget->currentItem();
    if (currentItem) {
        int currentRow = ui->tableWidget->currentRow();
        int currentColumn = ui->tableWidget->currentColumn();

        if (model->isUserInput(currentRow, currentColumn)){
            currentItem->setText("");
            model->updateModel(currentRow, currentColumn, 0);
        }
    }
}

void MainWindow::onResetButtonClicked() {
    const QVector<QVector<int>>& initialState = model->getInitialState();
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QTableWidgetItem* cell = ui->tableWidget->item(row, col);
            if (!cell) {
                cell = new QTableWidgetItem();
                ui->tableWidget->setItem(row, col, cell);
            }

            int value = initialState[row][col];
            cell->setText(value > 0 ? QString::number(value) : "");
        }
    }
    restoreInitialCellColors();
}

void MainWindow::onNewGameButtonClicked(){
    QVector<QString> stringLines = model->readFile(_filePath);
    model->extractData(stringLines);
    fillGrid();
}

void MainWindow::onHelpButtonClicked() {
    int row = 0, col = 0, value = 0;
    if (model->findNextSinglePossibilityCell(row, col, value)) {
        QTableWidgetItem* cell = ui->tableWidget->item(row, col);
        if (cell) {
            cell->setText(QString::number(value));
            setCellStyle(cell, "red");
            QTimer::singleShot(2300, [=]() {
                setCellStyle(cell, "purple");
            });
            model->updateModel(row, col, value);
        }
    } else {
        QMessageBox::information(this, tr("Help"), tr("No cell with a single possibility found !"));
    }
}

void MainWindow::restoreInitialCellColors() {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            QTableWidgetItem* cell = ui->tableWidget->item(i, j);
            if (cell) {
                cell->setBackground(_initialCellColors[i][j]);
            }
        }
    }
}

void MainWindow::cellsToBeConsidered(int row, int column) {
    restoreInitialCellColors();
    for (int i = 0; i < 9; i++){
        QTableWidgetItem* CellColumn = ui->tableWidget->item(i, column);
        CellColumn->setBackground(QColor("lightgray"));
        QTableWidgetItem* CellRow = ui->tableWidget->item(row, i);
        CellRow->setBackground(QColor("lightgray"));
    }
    int x0=(column/3)*3;
    int y0=(row/3)*3;
    for (int i=0; i<3; i++){
        for (int j=0; j<3; j++){
            QTableWidgetItem* Cell = ui->tableWidget->item(y0+i,x0+j);
            Cell ->setBackground(QColor("lightgray"));
        }

    }
}

void MainWindow::onCellSelected(int row, int column) {
    QVector<int> possibleValues = model->getPossibleValues(row, column);
    cellsToBeConsidered(row, column);

    if (!possibleValues.isEmpty()) {
        for (int i = 0; i < 9; ++i) {
            QPushButton* button = _numberButtons.at(i);
            if (!button) continue;

            // Si la valeur du bouton est dans les valeurs possibles, activer le bouton, sinon le désactiver
            int buttonValue = i + 1;
            bool isValuePossible = possibleValues.contains(buttonValue);
            button->setEnabled(isValuePossible);

            if (isValuePossible) {
                button->setStyleSheet("QPushButton { border: 2px solid green; border-radius: 5px; color: black; background-color: lightgreen; }"
                                      "QPushButton:hover{ background-color: white; }");

            } else {
                button->setStyleSheet("QPushButton { border: 2px solid red; border-radius: 5px; color: black; background-color: lightcoral; }");
            }
        }
    }
}

void MainWindow::checkForWin() {
    if (model->checkVictory()) {
        QMessageBox::information(this, tr("Congratulations"), tr("Congratulations! You have won!"));
    } else {
        QMessageBox::information(this, tr("Unfortunately"), tr("Unfortunately, you did not win."));
    }
}

void MainWindow::resetNumberButtonStyles() {
    QString initialStyle = "QPushButton { border: 2px solid black; border-radius: 5px; background-color: lightblue;}"
                           "QPushButton:hover{ background-color: white; }";

    for (auto button : _numberButtons) {
        if (button) {
            button->setStyleSheet(initialStyle);
            button->setEnabled(true);
        }
    }
}

void MainWindow::onQuitPressed(){
    int button = QMessageBox::question(this, tr("You want to quit..."),
                                       tr("Are you sure that you want to quit ?"),
                                       QMessageBox::Yes, QMessageBox::No | QMessageBox::Default);
    if (button == QMessageBox::Yes) {
        close();
    }
}

void MainWindow::onAboutPressed(){
    QMessageBox::about(this, tr("About"), tr("This sudoku assistant is a tool designed to aid players in solving sudoku grids of varying difficulties. "
                                             "It offers a user-friendly interface for filling in the grid with numbers and checking solutions. "
                                             "Additionally, the sudoku assistant provides features such as generating new sudoku grids, "
                                             "adjusting difficulty levels, and automatically verifying errors. "
                                             "It aims to provide an enjoyable and challenging gaming experience for sudoku enthusiasts of all skill levels."));
}


void MainWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::switchToFrench() {
    ui->action_French->setChecked(true);
    ui->action_English->setChecked(false);
    if (translator.load(":/lang_fr.qm")) {
        QApplication::installTranslator(&translator);
    }
    ui->retranslateUi(this);
}

void MainWindow::switchToEnglish() {
    ui->action_French->setChecked(false);
    ui->action_English->setChecked(true);
    QApplication::removeTranslator(&translator);
    ui->retranslateUi(this);
}


void MainWindow::connectButtons(){
    connect(ui->comboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(onDifficultyChanged(QString)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(resetNumberButtonStyles()));

    // Remplir par défaut la grille avec la difficulté "Easy"
    onDifficultyChanged("Easy");

    for (auto *button : _numberButtons) {
        connect(button, &QPushButton::clicked, this, &MainWindow::onNumberButtonClicked);
    }

    connect(ui->tableWidget, &QTableWidget::cellClicked, this, &MainWindow::onCellSelected);

    connect(ui->erase, &QPushButton::clicked, this, &MainWindow::onEraseButtonClicked);
    connect(ui->reset, &QPushButton::clicked, this, &MainWindow::onResetButtonClicked);
    connect(ui->reset, &QPushButton::clicked, this, &MainWindow::resetNumberButtonStyles);

    connect(ui->new_game, &QPushButton::clicked, this, &MainWindow::onNewGameButtonClicked);
    connect(ui->new_game, &QPushButton::clicked, this, &MainWindow::resetNumberButtonStyles);
    connect(ui->confirm, &QPushButton::clicked, this, &MainWindow::checkForWin);
    connect(ui->help, &QPushButton::clicked, this, &MainWindow::onHelpButtonClicked);
}

void MainWindow::connectMenuActions(){
    connect(ui->actionNew_game, &QAction::triggered, this, &MainWindow::onNewGameButtonClicked);
    connect(ui->action_Quit, &QAction::triggered, this, &MainWindow::onQuitPressed);
    connect(ui->action_About, &QAction::triggered, this, &MainWindow::onAboutPressed);
    connect(ui->action_English, &QAction::triggered, this, &MainWindow::switchToEnglish);
    connect(ui->action_French, &QAction::triggered, this, &MainWindow::switchToFrench);
}

