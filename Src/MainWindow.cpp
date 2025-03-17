#include "MainWindow.hpp"

#include <QFileDialog>
#include <QMessageBox>

#include "ui_MainWindow.h"
#include "TinDataLoader.hpp"
#include "TinDataSaver.hpp"





MainWindow::MainWindow(QWidget * aParent):
	QMainWindow(aParent),
	mUI(new Ui::MainWindow)
{
	mUI->setupUi(this);
	mUI->gvMain->setScene(&mGraphicsScene);
	
	// Connect the signals and slots:
	connect(mUI->actFileNew,    &QAction::triggered, this, &MainWindow::newFile);
	connect(mUI->actFileOpen,   &QAction::triggered, this, &MainWindow::openFile);
	connect(mUI->actFileSave,   &QAction::triggered, this, &MainWindow::saveFile);
	connect(mUI->actFileSaveAs, &QAction::triggered, this, &MainWindow::saveFileAs);
	connect(mUI->actFileImport, &QAction::triggered, this, &MainWindow::importFile);
	connect(mUI->actExit,       &QAction::triggered, this, &MainWindow::close);
}




MainWindow::~MainWindow()
{
	// Nothing explicit needed here, only the destructor needs to be defined in the cpp file due to unique_ptr
}





bool MainWindow::askSaveCurrentData()
{
	if (!mIsDataDirty)
	{
		return true;
	}
	auto resp = QMessageBox::question(
		this,
		tr("Save the data?"),
		tr("The data has changed, would you like to save it to a file?"),
		QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No | QMessageBox::StandardButton::Cancel
	);
	if (resp == QMessageBox::StandardButton::Cancel)
	{
		// The user doesn't want to save and doesn't want to continue:
		return false;
	}
	if (resp == QMessageBox::StandardButton::Yes)
	{
		saveFile();
		if (mIsDataDirty)
		{
			// The user cancelled saving the file, cancel the causing operation as well:
			return false;
		}
	}
	return true;
}





TinData MainWindow::loadTinDataFromFile(const QString & aFileName)
{
	// Read first 4 KiB of the file to detect the format:
	QFile f(aFileName);
	if (!f.open(QFile::ReadOnly))
	{
		QMessageBox::warning(
			this,
			tr("Cannot load TIN data"),
			tr("Cannot open file %1 for reading").arg(aFileName)
		);
		return {};
	}
	auto probe = f.read(4096);
	f.seek(0);
	
	// Try to detect the format:
	TinData data;
	if (TinDataLoader::doesProbeMatch(probe))
	{
		data = TinDataLoader::loadFrom(f);
	}
	else
	{
		QMessageBox::warning(
			this,
			tr("Cannot load TIN data"),
			tr("Cannot detect data format in file %1.").arg(aFileName)
		);
		return {};
	}
	return data;
}





void MainWindow::newFile()
{
	if (!askSaveCurrentData())
	{
		return;
	}
	
	mTinData.clear();
	mFileName.clear();
}





void MainWindow::openFile()
{
	auto fileName = QFileDialog::getOpenFileName(
		this,                                      // Parent widget
		tr("Open a TIN data file"),                // Title
		QString(),                                 // Initial folder
		tr("All supported formats (*.tin *.shp)")  // Filter
	);
	if (fileName.isEmpty())
	{
		return;
	}
	openSpecifiedFile(fileName);
}





void MainWindow::openSpecifiedFile(const QString & aFileName)
{
	try
	{
		auto data = loadTinDataFromFile(aFileName);
		std::swap(mTinData, data);
	}
	catch (const std::exception & exc)
	{
		QMessageBox::warning(
			this,
			tr("Cannot load TIN data"),
			tr("Cannot load TIN data from file %1:\n%2")
				.arg(aFileName, QString::fromUtf8(exc.what())
			)
		);
		return;
	}
	mFileName = aFileName;
	updateTitle();
}





void MainWindow::saveFile()
{
	// If the file is already assigned, save there:
	if (!mFileName.isEmpty())
	{
		saveFileTo(mFileName);
		return;
	}

	// If no file is assigned, ask the user where to save:
	saveFileAs();
}





void MainWindow::saveFileAs()
{	
	auto fileName = QFileDialog::getSaveFileName(
		this,                   // Parent widget
		tr("Save TIN data"),    // Title
		QString(),              // Initial folder
		tr("TIN file (*.tin)")  // Filter
	);
	if (fileName.isEmpty())
	{
		return;
	}
	saveFileTo(fileName);
	mFileName = fileName;
	updateTitle();
}





void MainWindow::saveFileTo(const QString & aFileName)
{
	QFile f(aFileName);
	if (!f.open(QFile::WriteOnly | QFile::Truncate))
	{
		QMessageBox::warning(
			this,
			tr("Cannot save TIN data"),
			tr("File %1 cannot be written to.").arg(aFileName)
		);
		return;
	}

	TinDataSaver::saveTo(mTinData, f);
}





void MainWindow::importFile()
{
	auto fileName = QFileDialog::getOpenFileName(
		this,                                      // Parent widget
		tr("Open a TIN data file"),                // Title
		QString(),                                 // Initial folder
		tr("All supported formats (*.tin *.shp)")  // Filter
	);
	if (fileName.isEmpty())
	{
		return;
	}
	try
	{
		auto data = loadTinDataFromFile(fileName);
		mTinData.import(std::move(data));
	}
	catch (const std::exception & exc)
	{
		QMessageBox::warning(
			this,
			tr("Cannot import TIN data"),
			tr("Cannot import TIN data from file %1:\n%2")
				.arg(fileName, QString::fromUtf8(exc.what())
			)
		);
	}
}





void MainWindow::updateTitle()
{
	if (mFileName.isEmpty())
	{
		this->setWindowTitle(tr("TinVisualiser"));
	}
	else
	{
		this->setWindowTitle(tr("TinVisualiser: %1").arg(mFileName));
	}
}
