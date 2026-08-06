// ProjectTest.hpp     Aditya Kaki   3/30/2026
// Header for QTest class

#ifndef PROJECTTEST_H
#define PROJECTTEST_H

#include <QObject>
#include <QtTest/QtTest>
#include <QTreeView>
#include <QLabel>
#include <QFileSystemModel>

#include "MainWindow.hpp"
#include "DocumentWindow.hpp"
#include "Document.hpp"
#include "Syllabizer.hpp"
#include "Token.hpp"

class ProjectTest : public QObject {
    Q_OBJECT

private:
    // Window pointers
    MainWindow* pmw = nullptr;
    DocumentWindow* pdw = nullptr;

    // Widgets in MainWindow
    QTreeView* ptree = nullptr;
    QFileSystemModel* pfsmodel = nullptr;

    // Labels in DocumentWindow
    QLabel* pcharcount = nullptr;
    QLabel* pwordcount = nullptr;
    QLabel* psyllablecount = nullptr;
    QLabel* preadingease = nullptr;
    QLabel* pfklevel = nullptr;
    QLabel* pgflevel = nullptr;

private slots:
    // Setup and teardown
    void initTestCase();
    void cleanupTestCase();

    // Tests
    void testsyllabizer();
    void testdocument();
    void testtoken();
    void testreadinglevel();
    void testguimainwin();
    void testguidocwin();
};

#endif
