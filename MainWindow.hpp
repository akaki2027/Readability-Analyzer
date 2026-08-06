#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QTreeView>
#include <QFileSystemModel>

/*! \class MainWindow
	\brief The main application window for the Syllabizer project.

	This class sets up the GUI, including the file system tree view
	and the logic for opening document windows.
*/
class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	/*! \brief Constructs the main window. \param parent The parent widget (usually nullptr). */
	MainWindow(QWidget* parent = nullptr);
	/*! \brief The data model for the local file system. */
	QFileSystemModel* fsmodel;
private slots:
	/*! \brief Slot triggered when a file is double-clicked in the tree. \param index The index of the item clicked. */
	void onFileClicked(const QModelIndex& index);
private:
	/*! \brief The tree view widget used to display files. */
	QTreeView* treeView;

};
#endif
