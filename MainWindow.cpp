#include "MainWindow.hpp"
#include "DocumentWindow.hpp"
#include <QFileSystemModel>
#include <QTreeView>
#include <QDir>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	setWindowTitle("Readability Analyzer");
	resize(400, 600);
	fsmodel = new QFileSystemModel(this);
	QString path = QDir::currentPath() + "/text";
	fsmodel->setRootPath(path);

	treeView = new QTreeView(this);
	treeView->setModel(fsmodel);
	treeView->setRootIndex(fsmodel->index(path));

	for (int i = 1; i < 4; i++)
	{
		treeView->hideColumn(i);
	}

	setCentralWidget(treeView);
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