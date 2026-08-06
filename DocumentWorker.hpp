#ifndef DOCUMENTWORKER_HPP
#define DOCUMENTWORKER_HPP
 
#include <QObject>
#include <QString>
#include "Document.hpp"
#include "Lexicon.hpp"
 
class DocumentWorker : public QObject {
    Q_OBJECT
public:
    using QObject::sender;
    DocumentWorker(QObject* parent = nullptr);
    ~DocumentWorker();
 
    Document* getDocument() { return &doc; }
 
signals:
    void contentsReady(QString contents);
    void statsReady();
    void htmlReady();
    void documentLoadDone();
 
public slots:
    void startLoad(QString filename);
 
private:
    Document doc;
    QString htmlString;
};
 
#endif