// ProjectTest.cpp     Aditya Kaki   3/30/2026
//  QtTest suite for the readability analyzer

#include "ProjectTest.h"
#include <string>
#include <iostream>
using namespace std;
#include <QtTest/QtTest>
#include <QDirIterator>
#include <QModelIndex>
#include <QRect>
#include <QPoint>

//helper to make syllable break vectr to a dashed string
static std::string syllabizedstring(std::string input, std::vector<int> breaks)
{
	std::string result = input;
	for(auto it = breaks.rbegin(); it != breaks.rend(); ++it) {
        if (*it > 0)
            result.insert(*it, "-");
    }
    return result;
}

void ProjectTest::initTestCase()
{
    //making windows
    pmw = new MainWindow();
    pmw->show();

    //doc window for the Gettysburg Address
    pdw = new DocumentWindow(pmw, "./text/gettysburg.txt");
    pdw->show();

    //widgets in opened window
    ptree = pmw->findChild<QTreeView*>();
    QVERIFY2(ptree, "no QTreeView in mainwindow");

    pfsmodel = pmw->fsmodel;
    QVERIFY2(pfsmodel, "no fsmodel in MainWindow");

    //finding labels in window
    pcharcount = pdw->findChild<QLabel*>("CharCount");
    QVERIFY2(pcharcount, "no Charcount in DocumentWindow");

    pwordcount = pdw->findChild<QLabel*>("WordCount");
    QVERIFY2(pwordcount, "no WordCount in DocumentWindow");

    psyllablecount = pdw->findChild<QLabel*>("SyllableCount");
    QVERIFY2(psyllablecount, "no SyllableCount in DocumentWindow");

    preadingease = pdw->findChild<QLabel*>("ReadingEase");
    QVERIFY2(preadingease, "no ReadingEase in DocumentWindow");

    pfklevel = pdw->findChild<QLabel*>("FKLevel");
    QVERIFY2(pfklevel, "no FKLevel in DocumentWindow");

    pgflevel = pdw->findChild<QLabel*>("GFLevel");
    QVERIFY2(pgflevel, "no GFLevel in DocumentWindow");
}

void ProjectTest::testsyllabizer()
{
    QCOMPARE(syllabizedstring("review", Syllabizer::syllabize("review")), std::string("re-view"));
    QCOMPARE(syllabizedstring("disturb", Syllabizer::syllabize("disturb")), std::string("dis-turb"));
    QCOMPARE(syllabizedstring("recalling", Syllabizer::syllabize("recalling")), std::string("re-call-ing"));
    QCOMPARE(syllabizedstring("working", Syllabizer::syllabize("working")), std::string("work-ing"));
    QCOMPARE(syllabizedstring("nation", Syllabizer::syllabize("nation")), std::string("na-tion"));
    QCOMPARE(syllabizedstring("open", Syllabizer::syllabize("open")), std::string("o-pen"));
    QCOMPARE(syllabizedstring("emit", Syllabizer::syllabize("emit")), std::string("e-mit"));
    QCOMPARE(syllabizedstring("rabbit", Syllabizer::syllabize("rabbit")), std::string("rab-bit"));
    QCOMPARE(syllabizedstring("parrot", Syllabizer::syllabize("parrot")), std::string("par-rot"));
    QCOMPARE(syllabizedstring("cabin", Syllabizer::syllabize("cabin")), std::string("cab-in"));
}


void ProjectTest::testdocument()
{
    //making from test string

    std::string teststr = "The quick brown fox jumps over the lazy dog.";
    Document d1(teststr);
    QVERIFY(d1.numtokens() > 0);
    QVERIFY(d1.numsyllables() > 0);
    QVERIFY(d1.numsentences() > 0);

    //load 
    Document d2;
    int chars = d2.load("./text/quick-brown-fox.txt");
    QVERIFY(chars > 0);
    QCOMPARE((int)d2.numtokens(), 9);

    //copy
    Document d3(d1);
    QCOMPARE((int)d3.numtokens(), (int)d1.numtokens());
    QCOMPARE(d3.getrawcontent(), d1.getrawcontent());

    //assignment
    Document d4;
    d4 = d1;
    QCOMPARE((int)d4.numtokens(), (int)d1.numtokens());
    QCOMPARE(d4.getrawcontent(), d1.getrawcontent());

    //equal check
    QVERIFY(d3 == d1); 
    QVERIFY(!(d2 == d1));
}

void ProjectTest::testtoken()
{
    Token t1("hello");
    QCOMPARE(t1.get(), std::string("hello"));

    //copy
    Token t2(t1);
    QCOMPARE(t2.get(), std::string("hello"));

    //assignment
    Token t3;
    t3 = t1;
    QCOMPARE(t3.get(), std::string("hello"));

    //equality
    QVERIFY(t1 == t2);
    Token t4("world");
    QVERIFY(!(t1 == t4));
}

void ProjectTest::testreadinglevel()
{
    std::string s1 = "Would you needlessly ignore me... without proof of a disreputable "
        "action? For what reason am I thus disregarded? It will not stand! I will plead my "
        "case before a jury of my peers, and will see justice done.";

    Document d1(s1);
    QCOMPARE((int)d1.numtokens(), 38);
    QCOMPARE(d1.numsyllables(), 54);
    QCOMPARE(d1.numsentences(), 4);
    QCOMPARE(d1.numhardwords(), 4);

    double ease = d1.getreadingease();
    double fklvl = d1.getfkgradelevel();
    double gflvl = d1.getgfgradelevel();
    QVERIFY(ease > 76 && ease < 78);
    QVERIFY(fklvl > 4 && fklvl < 5);
    QVERIFY(gflvl > 4 && gflvl < 5);

    // Gettysburg Address: known reference counts
    Document d2;
    d2.load("./text/gettysburg.txt");
    QCOMPARE((int)d2.numtokens(),272);
    QCOMPARE(d2.numsyllables(),371);
    QCOMPARE(d2.numsentences(),10);
    QCOMPARE(d2.numhardwords(),24);

    double ease2 = d2.getreadingease();
    double fklvl2 = d2.getfkgradelevel();
    double gflvl2 = d2.getgfgradelevel();
    QVERIFY(ease2 > 63 && ease2 < 65);
    QVERIFY(fklvl2 > 11 && fklvl2 < 12);
    QVERIFY(gflvl2 > 11 && gflvl2 < 12);
}

void ProjectTest::testguimainwin()
{
    //win is shown and tree starts
    QVERIFY(pmw->isVisible());
    QVERIFY2(ptree, "Tree view not found");

    //click england to test
    QString filetoload = "england.txt";
    QString fullfilepath;
    QDirIterator it(QDir::currentPath() + "/text", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        if (it.fileName() == filetoload) {
            fullfilepath = it.filePath();
        }
    }

    QVERIFY2(!fullfilepath.isEmpty(), "england.txt not found in text folder");
    QModelIndex fidx = pfsmodel->index(fullfilepath);
    QVERIFY2(fidx.isValid(), "File index not valid in model");

    QRect  rect = ptree->visualRect(fidx);
    QPoint nameloc = rect.center();
    QTest::mouseClick(ptree->viewport(), Qt::LeftButton, Qt::NoModifier, nameloc);
    QTest::qWait(500);

    //if a window is open
    QList<DocumentWindow*> docwins = pmw->findChildren<DocumentWindow*>();
    QVERIFY2(!docwins.isEmpty(), "No DocumentWindow opened after clicking a file");
}


void ProjectTest::testguidocwin()
{
    //gettysburg.txt should be open now
    QVERIFY(pdw->isVisible());

    //char count
    bool ok;
    QString chartext = pcharcount->text();
    chartext.remove(',');
    long charval = chartext.toLong(&ok);
    QVERIFY2(ok, "CharCount label is not a number");
    QVERIFY2(charval > 1400 && charval < 1600, "CharCount out of range");
}

void ProjectTest::cleanupTestCase()
{
    delete pdw;
    delete pmw;
}

QTEST_MAIN(ProjectTest)

