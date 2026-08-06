#include "DocumentWindow.hpp"
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QFont>
#include <QCheckBox>
#include <QFileInfo>

namespace {

// A small "stat card": a caption on top, a bold value underneath, inside a
// bordered panel. Returns the value label so callers can update it later.
QLabel* addStatCard(QGridLayout* grid, int row, int col, const QString& caption, const QString& objectName) {
    QFrame* card = new QFrame();
    card->setObjectName("StatCard");

    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(14, 10, 14, 10);
    cardLayout->setSpacing(2);

    QLabel* captionLabel = new QLabel(caption);
    captionLabel->setObjectName("StatCaption");

    QLabel* valueLabel = new QLabel("—");
    valueLabel->setObjectName(objectName);
    valueLabel->setProperty("statValue", true);

    cardLayout->addWidget(captionLabel);
    cardLayout->addWidget(valueLabel);

    grid->addWidget(card, row, col);
    return valueLabel;
}

} // namespace

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

    workerThread = new QThread(this);
    pDW = new DocumentWorker();
    pDW->moveToThread(workerThread);

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

    workerThread = new QThread(this);
    pDW = new DocumentWorker();
    pDW->moveToThread(workerThread);

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
    resize(760, 640);

    QVBoxLayout* outer = new QVBoxLayout(this);
    outer->setContentsMargins(20, 18, 20, 18);
    outer->setSpacing(14);

    // --- Header: filename + HTML toggle ---
    QHBoxLayout* header = new QHBoxLayout();
    fileNameLabel = new QLabel("—");
    fileNameLabel->setObjectName("FileNameLabel");
    fileNameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    htmlCheck = new QCheckBox("Colorize by part of speech");
    htmlCheck->setObjectName("HTMLCheck");
    htmlCheck->setChecked(false);
    htmlCheck->setVisible(false);

    header->addWidget(fileNameLabel, 1);
    header->addWidget(htmlCheck, 0, Qt::AlignRight);
    outer->addLayout(header);

    // --- Stats grid: a 3x3 wall of stat cards ---
    QGridLayout* statsGrid = new QGridLayout();
    statsGrid->setSpacing(10);
    for (int c = 0; c < 3; c++) statsGrid->setColumnStretch(c, 1);

    pcharcount     = addStatCard(statsGrid, 0, 0, "Characters", "CharCount");
    pwordcount     = addStatCard(statsGrid, 0, 1, "Words", "WordCount");
    puniquewords   = addStatCard(statsGrid, 0, 2, "Unique Words", "UniqueWords");

    psyllablecount = addStatCard(statsGrid, 1, 0, "Syllables", "SyllableCount");
    psentences     = addStatCard(statsGrid, 1, 1, "Sentences", "Sentences");
    phardwords     = addStatCard(statsGrid, 1, 2, "Hard Words", "HardWords");

    preadingease   = addStatCard(statsGrid, 2, 0, "Reading Ease", "ReadingEase");
    pfklevel       = addStatCard(statsGrid, 2, 1, "F-K Grade Level", "FKLevel");
    pgflevel       = addStatCard(statsGrid, 2, 2, "Gunning Fog", "GFLevel");

    outer->addLayout(statsGrid);

    // --- Document text ---
    textArea = new QTextEdit(this);
    textArea->setObjectName("TextArea");
    textArea->setReadOnly(true);
    textArea->setFont(QFont("Georgia", 12));
    outer->addWidget(textArea, 1);

    connect(htmlCheck, &QCheckBox::stateChanged, this, &DocumentWindow::onHtmlToggle);
}

bool DocumentWindow::load(QString filename) {
    this->setWindowTitle(QFileInfo(filename).fileName());
    fileNameLabel->setText(filename);

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
