#include "DocumentWindow.hpp"
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QCheckBox>

DocumentWindow::DocumentWindow(QWidget* parent) : QWidget(parent), pDW(nullptr), workerThread(nullptr) {
    buildUI();

     // Creating a worker and thread
    workerThread = new QThread(this);
    pDW = new DocumentWorker();
    pDW->moveToThread(workerThread);

    // signals connected
    connect(this, &DocumentWindow::startLoad, pDW, &DocumentWorker::startLoad);
    connect(pDW, &DocumentWorker::contentsReady, this, &DocumentWindow::onContentsReady);
    connect(pDW, &DocumentWorker::statsReady, this, &DocumentWindow::onStatsReady);
    connect(pDW, &DocumentWorker::htmlReady, this, &DocumentWindow::onHtmlReady);
    connect(pDW, &DocumentWorker::documentLoadDone, this, &DocumentWindow::onDocumentLoadDone);
    connect(workerThread, &QThread::finished, pDW, &QObject::deleteLater);

    workerThread->start();
}

DocumentWindow::DocumentWindow(QWidget* parent, QString filePath) : QWidget(parent), pDW(nullptr), workerThread(nullptr) {
    buildUI();

     // Creating a worker and thread
    workerThread = new QThread(this);
    pDW = new DocumentWorker();
    pDW->moveToThread(workerThread);

    // signals connected
    connect(this, &DocumentWindow::startLoad, pDW, &DocumentWorker::startLoad);
    connect(pDW, &DocumentWorker::contentsReady, this, &DocumentWindow::onContentsReady);
    connect(pDW, &DocumentWorker::statsReady, this, &DocumentWindow::onStatsReady);
    connect(pDW, &DocumentWorker::htmlReady, this, &DocumentWindow::onHtmlReady);
    connect(pDW, &DocumentWorker::documentLoadDone, this, &DocumentWindow::onDocumentLoadDone);
    connect(workerThread, &QThread::finished, pDW, &QObject::deleteLater);

    workerThread->start();

    load(filePath);
}

DocumentWindow::DocumentWindow(QString filePath, QWidget* parent) : QWidget(parent), pDW(nullptr), workerThread(nullptr) {
    buildUI();

     // Creating a worker and thread
    workerThread = new QThread(this);
    pDW = new DocumentWorker();
    pDW->moveToThread(workerThread);

    // signals connected
    connect(this, &DocumentWindow::startLoad, pDW, &DocumentWorker::startLoad);
    connect(pDW, &DocumentWorker::contentsReady, this, &DocumentWindow::onContentsReady);
    connect(pDW, &DocumentWorker::statsReady, this, &DocumentWindow::onStatsReady);
    connect(pDW, &DocumentWorker::htmlReady, this, &DocumentWindow::onHtmlReady);
    connect(pDW, &DocumentWorker::documentLoadDone, this, &DocumentWindow::onDocumentLoadDone);
    connect(workerThread, &QThread::finished, pDW, &QObject::deleteLater);

    workerThread->start();

    load(filePath);
}

DocumentWindow::~DocumentWindow() {
    if (workerThread) {
        workerThread->quit();
        workerThread->wait();
    }
}

void DocumentWindow::buildUI() {
    this->setObjectName("DocumentWindow");
    setWindowFlags(Qt::Window);
    setAttribute(Qt::WA_DeleteOnClose);
    resize(800, 600);

    QVBoxLayout* layout = new QVBoxLayout(this);

    //r1 with char count, word count, and syllable count
    QHBoxLayout* row1 = new QHBoxLayout();
    pcharcount = new QLabel("---"); pcharcount->setObjectName("CharCount");
    pwordcount = new QLabel("---"); pwordcount->setObjectName("WordCount");
        puniquewords = new QLabel("---"); puniquewords->setObjectName("UniqueWords");
        psyllablecount = new QLabel("---"); psyllablecount->setObjectName("SyllableCount");
        psentences = new QLabel("---"); psentences->setObjectName("Sentences");

    row1->addWidget(new QLabel("chars:")); row1->addWidget(pcharcount); row1->addStretch();
    row1->addWidget(new QLabel("words:")); row1->addWidget(pwordcount); row1->addStretch();
    row1->addWidget(new QLabel("unique Words:")); row1->addWidget(puniquewords); row1->addStretch();
    row1->addWidget(new QLabel("syllables:")); row1->addWidget(psyllablecount); row1->addStretch();
    row1->addWidget(new QLabel("sentences:")); row1->addWidget(psentences);

    //r2 reading ease, FK grade, GF grade
        QHBoxLayout* row2 = new QHBoxLayout();
         phardwords = new QLabel("---"); phardwords->setObjectName("HardWords");
        preadingease = new QLabel("---"); preadingease->setObjectName("ReadingEase");
        pfklevel = new QLabel("---"); pfklevel->setObjectName("FKLevel");
        pgflevel = new QLabel("---"); pgflevel->setObjectName("GFLevel");

    htmlCheck = new QCheckBox("HTML output");
    htmlCheck->setObjectName("HTMLCheck");
    htmlCheck->setChecked(false);
    htmlCheck->setVisible(false);

    row2->addWidget(new QLabel("hard words:")); row2->addWidget(phardwords); row2->addStretch();
    row2->addWidget(new QLabel("reading ease:")); row2->addWidget(preadingease); row2->addStretch();
    row2->addWidget(new QLabel("F-K grade level:")); row2->addWidget(pfklevel); row2->addStretch();
    row2->addWidget(new QLabel("G-F grade level:")); row2->addWidget(pgflevel); row2->addStretch();
    row2->addWidget(htmlCheck);

    layout->addLayout(row1);
    layout->addLayout(row2);

    //Text area
    textArea = new QTextEdit(this);
    textArea->setReadOnly(true);
    textArea->setFont(QFont("Times New Roman", 11));
    layout->addWidget(textArea);

    connect(htmlCheck, &QCheckBox::stateChanged, this, &DocumentWindow::onHtmlToggle);
}

bool DocumentWindow::load(QString filename) {
    this->setWindowTitle(filename);

   
    emit startLoad(filename);
    return true;
}

void DocumentWindow::onContentsReady(QString contents) {
    textArea->setPlainText(contents);
        Document* doc = pDW->getDocument();
    pcharcount->setText(QString::number(doc->getrawcontent().size()));
}

    void DocumentWindow::onStatsReady() {
    Document* doc = pDW->getDocument();
    pwordcount->setText(QString::number((int)doc->numtokens()));
    puniquewords->setText(QString::number(doc->numuniquewords()));
    psyllablecount->setText(QString::number(doc->numsyllables()));
    psentences->setText(QString::number(doc->numsentences()));
    phardwords->setText(QString::number(doc->numhardwords()));
    preadingease->setText(QString::number(doc->getreadingease(), 'f', 2));
    pfklevel->setText(QString::number(doc->getfkgradelevel(), 'f', 2));
    pgflevel->setText(QString::number(doc->getgfgradelevel(), 'f', 2));
}

void DocumentWindow::onHtmlReady() {
        htmlCheck->setVisible(true);
}

void DocumentWindow::onDocumentLoadDone() {
}


void DocumentWindow::onHtmlToggle(int state) {
    if (state == Qt::Checked) {
         Document* doc = pDW->getDocument();
        textArea->setHtml(QString::fromStdString(doc->getHTML()));
    } else {
        Document* doc = pDW->getDocument();
         textArea->setPlainText(QString::fromStdString(doc->getrawcontent()));
    }
}
