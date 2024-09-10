#include "sudokumodel.h"
#include <iostream>
#include <QSet>

using namespace std;

sudokuModel::sudokuModel() : _lines(9, QVector<int>(9, 0)),
    _columns(9, QVector<int>(9, 0)),
    _miniGrids(9, QVector<int>(9, 0)),
    _initialState(9, QVector<int>(9, 0)) {}


QVector<QString> sudokuModel::readFile(const QString& filePath) {
    QFile file(filePath);
    QVector<QString> stringLines;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString stringLine = in.readLine();
            stringLines.push_back(stringLine);
        }
    } else {
        std::cerr << "Error opening the file: " << filePath.toStdString() << std::endl;
    }

    return stringLines;
}

void sudokuModel::extractData(const QVector<QString>& stringLines) {
    // Utiliser QRandomGenerator pour la génération de nombres aléatoires
    auto gen = QRandomGenerator::global();
    int randomIndex = gen->bounded(1, stringLines.size());
    QString randomLine = stringLines[randomIndex];


    // Extracting lines, columns, and mini-grids
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            int value = randomLine[i * 9 + j].digitValue();
            _lines[i][j] = value;
            _columns[j][i] = value;

            // Determine the index for mini-grids
            int index = (i / 3) * 3 + j / 3;
            _miniGrids[index][(i % 3) * 3 + j % 3] = value;
        }
    }
    _initialState = _lines;
}

bool sudokuModel::checkMove(int row, int column, int number) {
    // Vérification dans la ligne et la colonne
    for (int i = 0; i < 9; ++i) {
        if (_lines[row][i] == number || _columns[column][i] == number) {
            return false;
        }
    }

    // Calcul de l'indice de la mini-grille et de la cellule dans la mini-grille
    int gridIndex = (row / 3) * 3 + column / 3;
    int cellIndex = (row % 3) * 3 + (column % 3);

    // Vérification dans la mini-grille correspondante
    for (int i = 0; i < 9; ++i) {
        if (i != cellIndex && _miniGrids[gridIndex][i] == number) {
            return false;
        }
    }

    return true;
}


QVector<QVector<int>> sudokuModel::getLines(){
    return _lines;
}

QVector<QVector<int>> sudokuModel::getInitialState(){
    return _initialState;
}

bool sudokuModel::isUserInput(int row, int column) const {
    return _lines[row][column] != _initialState[row][column];
}


void sudokuModel::updateModel(int row, int column, int number) {
    _lines[row][column] = number;
    _columns[column][row] = number;
    int miniGridIndex = (row / 3) * 3 + column / 3;
    int miniGridCellIndex = (row % 3) * 3 + column % 3;
    _miniGrids[miniGridIndex][miniGridCellIndex] = number;
}


QVector<int> sudokuModel::getPossibleValues(int row, int column) const {
    QVector<int> possibleValues;
    if (_lines[row][column] != 0) {
        return possibleValues;
    }

    // Initialiser un ensemble de toutes les valeurs possibles
    QSet<int> values{1, 2, 3, 4, 5, 6, 7, 8, 9};

    // Retirer les valeurs déjà présentes dans la même ligne et colonne
    for (int i = 0; i < 9; ++i) {
        values.remove(_lines[row][i]);
        values.remove(_columns[column][i]);
    }

    int miniGridIndex = (row / 3) * 3 + column / 3;

    for (int i = 0; i < 9; i++) {
        values.remove(_miniGrids[miniGridIndex][i]);
    }

    for (int val : values) {
        possibleValues.push_back(val);
    }

    return possibleValues;
}

bool sudokuModel::checkVictory() const {
    for (int i = 0; i < 9; ++i) {
        QSet<int> rowSet, colSet;
        for (int j = 0; j < 9; ++j) {
            int rowValue = _lines[i][j];
            int colValue = _columns[j][i];

            if (rowValue == 0 || colValue == 0 || rowSet.contains(rowValue) || colSet.contains(colValue)) {
                return false;
            }

            rowSet.insert(rowValue);
            colSet.insert(colValue);
        }
    }

    for (int i = 0; i < 9; i ++) {
        for (int j = 0; j < 9; j++) {
            QSet<int> gridSet;
            int gridValue = _miniGrids[i][j];

            if (gridValue == 0 || gridSet.contains(gridValue)){
                return false;
            }

            gridSet.insert(gridValue);
        }
    }

    return true;
}

bool sudokuModel::findNextSinglePossibilityCell(int& row, int& col, int& value) const {
    for (row = 0; row < 9; ++row) {
        for (col = 0; col < 9; ++col) {
            if (_lines[row][col] == 0) {
                QVector<int> possibleValues = getPossibleValues(row, col);
                if (possibleValues.size() == 1) { // Une seule possibilité
                    value = possibleValues.at(0);
                    return true;
                }
            }
        }
    }
    return false;
}

