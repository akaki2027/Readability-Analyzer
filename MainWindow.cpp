#include "MainWindow.hpp"
#include "DocumentWindow.hpp"
#include <QFileSystemModel>
#include <QTreeView>
#include <QDir>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	setWindowTitle("Readability Analyzer");
	resize(420, 640);

	QWidget* central = new QWidget(this);
	central->setObjectName("MainWindowCentral");
	QVBoxLayout* layout = new QVBoxLayout(central);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	QWidget* header = new QWidget(central);
	header->setObjectName("BrowserHeader");
	QVBoxLayout* headerLayout = new QVBoxLayout(header);
	headerLayout->setContentsMargins(20, 20, 20, 16);
	headerLayout->setSpacing(2);

	QLabel* title = new QLabel("Readability Analyzer");
	title->setObjectName("AppTitle");
	QLabel* subtitle = new QLabel("Select a document to analyze");
	subtitle->setObjectName("AppSubtitle");

	headerLayout->addWidget(title);
	headerLayout->addWidget(subtitle);

	fsmodel = new QFileSystemModel(this);
	QString path = QDir::currentPath() + "/text";
	fsmodel->setRootPath(path);

	treeView = new QTreeView(central);
	treeView->setObjectName("FileTree");
	treeView->setModel(fsmodel);
	treeView->setRootIndex(fsmodel->index(path));
	treeView->setHeaderHidden(true);
	treeView->setIndentation(14);
	treeView->setAnimated(true);

	for (int i = 1; i < 4; i++)
	{
		treeView->hideColumn(i);
	}

	layout->addWidget(header);
	layout->addWidget(treeView, 1);

	setCentralWidget(central);
	connect(treeView, &QTreeView::clicked, this, &MainWindow::onFileClicked);
}

void MainWindow::onFileClicked(const QModelIndex& index)
{
	if (!fsmodel->isDir(index))
	{
		QString filePath = fsmodel->filePath(index);
		DocumentWindow* dw = new DocumentWindow(this);
		dw->load(filePath);
        dw->show();
	}
}
