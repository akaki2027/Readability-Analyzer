#ifndef DOCUMENTWINDOW_HPP
#define DOCUMENTWINDOW_HPP

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QCheckBox>
#include <QString>
#include <QThread>
#include "Document.hpp"
#include "Lexicon.hpp"
#include "DocumentWorker.hpp"

class DocumentWindow : public QWidget {
	Q_OBJECT
public:
	using QObject::sender;
	explicit DocumentWindow(QWidget* parent=nullptr);
	explicit DocumentWindow(QWidget* parent, QString filePath);
	explicit DocumentWindow(QString filePath, QWidget* parent);
	~DocumentWindow();

	bool load(QString filename);
	DocumentWorker* pDW;

signals:
	void startLoad(QString filename);
	

public slots:

	void onContentsReady(QString contents);
	void onStatsReady();
    void onHtmlReady();
    void onDocumentLoadDone();
    void onHtmlToggle(int state);
private: 
	QThread* workerThread;
    QTextEdit* textArea;
    QCheckBox* htmlCheck;
 
    QLabel* pcharcount;
    QLabel* pwordcount;
    QLabel* puniquewords;
    QLabel* psyllablecount;
    QLabel* psentences;
    QLabel* phardwords;
    QLabel* preadingease;
    QLabel* pfklevel;
    QLabel* pgflevel;

	void buildUI();
};

#endif