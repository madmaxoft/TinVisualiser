#include <QApplication>
#include <QTranslator>
#include <QFile>

#include "MainWindow.hpp"





/** Loads translation for the specified locale from all reasonable locations.
Returns true if successful, false on failure.
Tries: resources, <curdir>/translations, <exepath>/translations. */
static bool tryLoadTranslation(QTranslator & aTranslator, const QLocale & aLocale)
{
	static const QString exePath = QCoreApplication::applicationDirPath();
	auto lng = aLocale.name();

	if (aTranslator.load("TinVisualiser_" + lng, ":/Translations"))
	{
		qDebug() << "Loaded translation " << lng << " from resources";
		return true;
	}
	if (aTranslator.load("TinVisualiser_" + lng, "Translations"))
	{
		qDebug() << "Loaded translation " << lng << " from current folder";
		return true;
	}
	if (aTranslator.load("TinVisualiser_" + lng, exePath + "/Translations"))
	{
		qDebug() << "Loaded translation " << lng << " from exe folder";
		return true;
	}
	return false;
}





void initTranslations(QApplication & aApp)
{
	auto translator = std::make_unique<QTranslator>();
	auto locale = QLocale::system();
	if (!tryLoadTranslation(*translator, locale))
	{
		qWarning() << "Could not load translations for locale " << locale.name() << ", trying all UI languages " << locale.uiLanguages();
		if (!translator->load(locale, "TinVisualiser", "_", "Translations"))
		{
			qWarning() << "Could not load translations for " << locale;
			return;
		}
	}
	qDebug() << "Translator empty: " << translator->isEmpty();
	aApp.installTranslator(translator.release());
}





int main(int argc, char ** argv)
{
	QApplication app(argc, argv);
	initTranslations(app);
	MainWindow wnd;

	// If a filename was given on the command line, load it:
	const auto args = app.arguments();
	auto numArgs = args.size();
	for (auto i = 1; i < numArgs; ++i)  // Skip the 0-th arg, which is the program executable name
	{
		if (QFile::exists(args[i]))
		{
			wnd.openSpecifiedFile(args[i]);
		}
	}

	wnd.showMaximized();
	wnd.zoomAll();
	return app.exec();
}
