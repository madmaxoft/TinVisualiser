#pragma once

#include <QMainWindow>
#include <QGraphicsScene>

#include "TinData.hpp"





// fwd:
namespace Ui
{
	class MainWindow;
}





class MainWindow:
	public QMainWindow
{
	Q_OBJECT

	
public:

	explicit MainWindow(QWidget * aParent = nullptr);
	
	~MainWindow();

	
private:

	/** The Qt-managed UI. */
	std::unique_ptr<Ui::MainWindow> mUI;
	
	/** The underlying data. */
	TinData mTinData;
	
	/** True if the data has been modified from its original (newFile / openFile) state.
	Used to detect whether the file needs saving. */
	bool mIsDataDirty;
	
	/** Name of the file fro which the data has been loaded. Empty if new file. */
	QString mFileName;
	
	/** The scene representing all the graphics shown. */
	QGraphicsScene mGraphicsScene;
	
	
	/** Asks the user to save the current data, if relevant.
	Returns true if the user wishes to continue (either saving or not), false if they cancel. */
	bool askSaveCurrentData();
	
	/** Loads the TIN data from the specified file, returns the loaded data. */
	TinData loadTinDataFromFile(const QString & aFileName);
	
	
public slots:

	/** Drops the current data (asking the user to save, if relevant) and clears everything. */
	void newFile();	
	
	/** Asks the user for a filename, then opens the specified file.
	Asks to save the current data, if relevant. */
	void openFile();
	
	/** Opens the specified file.
	Asks to save the current data, if relevant. */
	void openSpecifiedFile(const QString & aFileName);
	
	/** Saves the data to mFileName.
	If the filename is not provided (new file), asks the user where to save. */
	void saveFile();
	
	/** Asks the user for filename, then saves to that file. */
	void saveFileAs();
	
	/** Saves the data to the specified file and sets the current filename to that file. */
	void saveFileTo(const QString & aFileName);
	
	/** Asks the user for a filename, then imports the data from that file into the current data. */
	void importFile();
	
	/** Updates the window title to reflect the filename. */
	void updateTitle();
};
