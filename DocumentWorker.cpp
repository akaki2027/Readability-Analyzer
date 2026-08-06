#include "DocumentWorker.hpp"
DocumentWorker::DocumentWorker(QObject* parent) : QObject(parent) {}
DocumentWorker::~DocumentWorker() {}

void DocumentWorker::startLoad(QString filename) {
    // loading the file
    doc.loadContentsOnly(filename.toStdString());
    emit contentsReady(QString::fromStdString(doc.getrawcontent()));

    // tokenize and stats
    doc.processTokens();
    emit statsReady();

    // lexicon already loaded — just annotate tokens and make HTML
    doc.setlexinfo(&Lexicon::getInstance());
    htmlString = QString::fromStdString(doc.getHTML());
    emit htmlReady();
    emit documentLoadDone();
}