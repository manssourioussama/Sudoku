#ifndef SUDOKUMODEL_H
#define SUDOKUMODEL_H

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QVector>
#include <QRandomGenerator>

class sudokuModel
{
public:
    sudokuModel();
    QVector<QString> readFile(const QString& filePath);
    void extractData(const QVector<QString>& stringLines);
    bool checkMove(int row, int column, int number);
    QVector<QVector<int>> getLines();
    QVector<QVector<int>> getInitialState();
    void updateModel(int row, int column, int number);
    bool isUserInput(int row, int column) const;
    QVector<int> getPossibleValues(int row, int column) const;
    bool checkVictory() const;
    bool findNextSinglePossibilityCell(int& row, int& col, int& value) const;

private:
    QVector<QVector<int>> _lines;
    QVector<QVector<int>> _columns;
    QVector<QVector<int>> _miniGrids;
    QVector<QVector<int>> _initialState;
};

#endif // SUDOKUMODEL_H
