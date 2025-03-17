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
	mUI->gvMain->setDragMode(QGraphicsView::ScrollHandDrag);

	// Connect the signals and slots:
	connect(mUI->actFileNew,    &QAction::triggered, this, &MainWindow::newFile);
	connect(mUI->actFileOpen,   &QAction::triggered, this, &MainWindow::openFile);
	connect(mUI->actFileSave,   &QAction::triggered, this, &MainWindow::saveFile);
	connect(mUI->actFileSaveAs, &QAction::triggered, this, &MainWindow::saveFileAs);
	connect(mUI->actFileImport, &QAction::triggered, this, &MainWindow::importFile);
	connect(mUI->actExit,       &QAction::triggered, this, &MainWindow::close);
	connect(mUI->actZoomIn,     &QAction::triggered, this, &MainWindow::zoomIn);
	connect(mUI->actZoomOut,    &QAction::triggered, this, &MainWindow::zoomOut);
	connect(mUI->actZoomAll,    &QAction::triggered, this, &MainWindow::zoomAll);
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





void MainWindow::rebuildScene()
{
	mGraphicsScene.clear();
	for (const auto & pt: mTinData.points())
	{
		mGraphicsScene.addRect(pt->mX, -pt->mY, 5, 5);
	}
	for (const auto & constraint: mTinData.constraints())
	{
		auto pt1 = constraint->point1();
		auto pt2 = constraint->point2();
		mGraphicsScene.addLine(
			pt1->mX, -pt1->mY,
			pt2->mX, -pt2->mY
		);
	}
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
	rebuildScene();
	updateTitle();
	zoomAll();
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
	rebuildScene();
	zoomAll();
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





void MainWindow::zoomIn()
{
	mUI->gvMain->scale(mUI->gvMain->mZoomFactor, mUI->gvMain->mZoomFactor);
}





void MainWindow::zoomOut()
{
	mUI->gvMain->scale(1 / mUI->gvMain->mZoomFactor, 1 / mUI->gvMain->mZoomFactor);
}





void MainWindow::zoomAll()
{
	// If there are no points, there's nothing to zoom to:
	if (mTinData.points().empty())
	{
		return;
	}

	// Get the data extents:
	double minX = mTinData.points()[0]->mX, maxX = minX;
	double minY = -mTinData.points()[0]->mY, maxY = minY;
	for (const auto & pt: mTinData.points())
	{
		auto x = pt->mX, y = -pt->mY;
		if (x < minX)
		{
			minX = x;
		}
		if (x > maxX)
		{
			maxX = x;
		}
		if (y < minY)
		{
			minY = y;
		}
		if (y > maxY)
		{
			maxY = y;
		}
	}

	// Zoom:
	mUI->gvMain->fitInView(minX, minY, maxX - minX, maxY - minY, Qt::KeepAspectRatio);
}
