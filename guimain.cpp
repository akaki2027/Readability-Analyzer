#include <QApplication>
#include "MainWindow.hpp"
#include "Lexicon.hpp"

static const char* kStyleSheet = R"(
QWidget {
    background-color: #1c1d21;
    color: #eceef0;
    font-family: "Segoe UI", sans-serif;
    font-size: 13px;
}

/* --- File browser --- */
#BrowserHeader {
    background-color: #202127;
    border-bottom: 1px solid #34363c;
}
#AppTitle {
    font-size: 17px;
    font-weight: 600;
    color: #eceef0;
}
#AppSubtitle {
    font-size: 12px;
    color: #8b8d93;
}
#FileTree {
    background-color: #1c1d21;
    border: none;
    outline: none;
    padding: 8px;
}
#FileTree::item {
    padding: 7px 6px;
    border-radius: 6px;
}
#FileTree::item:hover {
    background-color: #26272c;
}
#FileTree::item:selected {
    background-color: #2c4a47;
    color: #4fd1c5;
}

/* --- Document window --- */
#FileNameLabel {
    font-size: 13px;
    color: #8b8d93;
}
#StatCard {
    background-color: #26272c;
    border: 1px solid #34363c;
    border-radius: 8px;
}
#StatCaption {
    font-size: 11px;
    color: #8b8d93;
    text-transform: uppercase;
}
QLabel[statValue="true"] {
    font-size: 19px;
    font-weight: 600;
    color: #4fd1c5;
}
#TextArea {
    background-color: #202127;
    border: 1px solid #34363c;
    border-radius: 8px;
    padding: 14px;
    selection-background-color: #2c4a47;
}

QCheckBox {
    spacing: 8px;
}
QCheckBox::indicator {
    width: 15px;
    height: 15px;
    border-radius: 4px;
    border: 1px solid #4a4c54;
    background-color: #26272c;
}
QCheckBox::indicator:checked {
    background-color: #4fd1c5;
    border: 1px solid #4fd1c5;
}

QScrollBar:vertical {
    background: #1c1d21;
    width: 12px;
}
QScrollBar::handle:vertical {
    background: #34363c;
    border-radius: 5px;
    min-height: 24px;
}
QScrollBar::handle:vertical:hover {
    background: #4a4c54;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    height: 0px;
}
)";

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	a.setStyleSheet(kStyleSheet);
	Lexicon::getInstance().loadcsv("./text/US English word info.csv");
	MainWindow w;
	w.show();
	return a.exec();
}
