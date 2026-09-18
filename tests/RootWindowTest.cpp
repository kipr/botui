#include "Calibrate.h"
#include "InputProviderWidget.h"
#include "RootController.h"
#include "RootWindow.h"

#include <QApplication>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QHideEvent>
#include <QImage>
#include <QPointer>
#include <QPushButton>
#include <QStackedWidget>
#include <QTemporaryDir>
#include <QTimer>
#include <QVBoxLayout>
#include <QtTest>

class ObservedRootWindow : public RootWindow
{
public:
	int hideCount = 0;
	int closeCount = 0;

protected:
	void hideEvent(QHideEvent *event) override
	{
		++hideCount;
		RootWindow::hideEvent(event);
	}

	void closeEvent(QCloseEvent *event) override
	{
		++closeCount;
		RootWindow::closeEvent(event);
	}
};

class TestInputProvider : public InputProviderWidget
{
public:
	explicit TestInputProvider(QWidget *parent = nullptr)
		: InputProviderWidget(parent)
	{
	}

	void setInput(const QString &input) override
	{
		m_input = input;
	}

	QString input() const override
	{
		return m_input;
	}

private:
	QString m_input;
};

class RootWindowTest : public QObject
{
Q_OBJECT
private slots:
	void navigationUsesOnePersistentWindow();
	void pageSizeHintsCannotResizeShell();
	void inputProviderUsesChildOverlay();
	void activeInputRejectsNavigationAndNesting();
	void calibrationUsesPersistentWindowAndDefersWrites();
	void calibrationArtworkLoads();
};

void RootWindowTest::navigationUsesOnePersistentWindow()
{
	RootController controller;
	ObservedRootWindow window;
	window.resize(800, 480);
	QWidget *home = new QWidget;
	const QPalette applicationPalette = QApplication::palette();
	controller.initialize(&window, home);
	window.show();
	QCoreApplication::processEvents();

	const WId shellId = window.winId();
	const int topLevelCount = QApplication::topLevelWidgets().size();
	QVERIFY(!window.overlayLayer()->isVisible());
	QCOMPARE(controller.depth(), 1U);
	QCOMPARE(controller.currentWidget(), home);
	QVERIFY(!home->isWindow());
	QCOMPARE(home->palette().color(QPalette::Window),
	         applicationPalette.color(QPalette::Window));
	QCOMPARE(home->palette().color(QPalette::WindowText),
	         applicationPalette.color(QPalette::WindowText));

	QWidget *child = new QWidget;
	controller.presentWidget(child);
	QWidget *grandchild = new QWidget;
	controller.presentWidget(grandchild);
	QCOMPARE(controller.depth(), 3U);
	QCOMPARE(controller.currentWidget(), grandchild);
	QVERIFY(!child->isWindow());
	QVERIFY(!grandchild->isWindow());
	QCOMPARE(window.winId(), shellId);
	QCOMPARE(QApplication::topLevelWidgets().size(), topLevelCount);
	QCOMPARE(window.hideCount, 0);
	QCOMPARE(window.closeCount, 0);

	QPointer<QWidget> replaced = grandchild;
	QWidget *replacement = new QWidget;
	controller.replaceWidget(replacement);
	QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
	QVERIFY(replaced.isNull());
	QCOMPARE(controller.depth(), 3U);
	QCOMPARE(controller.currentWidget(), replacement);

	QPointer<QWidget> removed = replacement;
	controller.dismissWidget();
	QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
	QVERIFY(removed.isNull());
	QCOMPARE(controller.currentWidget(), child);
	QCOMPARE(controller.depth(), 2U);

	controller.dismissAllWidgets();
	QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
	QCOMPARE(controller.currentWidget(), home);
	QCOMPARE(controller.depth(), 1U);
	QCOMPARE(window.pageStack()->count(), 1);
	QCOMPARE(window.winId(), shellId);
	QCOMPARE(QApplication::topLevelWidgets().size(), topLevelCount);
	QCOMPARE(window.hideCount, 0);
	QCOMPARE(window.closeCount, 0);
}

void RootWindowTest::pageSizeHintsCannotResizeShell()
{
	RootController controller;
	ObservedRootWindow window;
	window.resize(800, 480);
	QWidget *home = new QWidget;
	controller.initialize(&window, home);
	window.show();
	QCoreApplication::processEvents();

	const QRect shellGeometry = window.geometry();
	const QSize shellMinimumSize = window.minimumSize();
	const QSize shellMaximumSize = window.maximumSize();

	QWidget *oversizedPage = new QWidget;
	QVBoxLayout *pageLayout = new QVBoxLayout(oversizedPage);
	QWidget *oversizedContent = new QWidget;
	oversizedContent->setMinimumSize(1000, 600);
	pageLayout->addWidget(oversizedContent);

	controller.presentWidget(oversizedPage);
	QCoreApplication::processEvents();
	QCOMPARE(window.geometry(), shellGeometry);
	QCOMPARE(window.minimumSize(), shellMinimumSize);
	QCOMPARE(window.maximumSize(), shellMaximumSize);
	QCOMPARE(window.pageStack()->geometry(), window.rect());
	QCOMPARE(oversizedPage->geometry(), window.pageStack()->rect());

	controller.dismissWidget();
	QCoreApplication::processEvents();
	QCOMPARE(window.geometry(), shellGeometry);
	QCOMPARE(window.pageStack()->geometry(), window.rect());
	QCOMPARE(home->geometry(), window.pageStack()->rect());
}

void RootWindowTest::inputProviderUsesChildOverlay()
{
	RootController controller;
	ObservedRootWindow window;
	window.resize(800, 480);
	QWidget *home = new QWidget;
	TestInputProvider input(home);
	input.setInput(QStringLiteral("value"));
	controller.initialize(&window, home);
	window.show();
	QCoreApplication::processEvents();

	QVERIFY(!input.isVisible());
	const QSize shellSize = window.size();
	const QSize shellMinimumSize = window.minimumSize();
	const QRect stackGeometry = window.pageStack()->geometry();
	const QRect pageGeometry = home->geometry();
	const int topLevelCount = QApplication::topLevelWidgets().size();
	int topLevelCountDuringInput = -1;
	QTimer::singleShot(0, &input, [&]() {
		topLevelCountDuringInput = QApplication::topLevelWidgets().size();
		QVERIFY(window.overlayLayer()->isVisible());
		QCOMPARE(window.overlayLayer()->geometry(), window.rect());
		QCOMPARE(input.geometry(), window.overlayLayer()->rect());
		QVERIFY(!input.isWindow());
		input.accept();
	});

	QCOMPARE(controller.presentInput(&input), InputProviderWidget::Accepted);
	QCOMPARE(input.input(), QStringLiteral("value"));
	QCOMPARE(input.parentWidget(), home);
	QCOMPARE(topLevelCountDuringInput, topLevelCount);
	QVERIFY(!window.overlayLayer()->isVisible());
	QVERIFY(!input.isVisible());

	for (int i = 0; i < 4; ++i) {
		QTimer::singleShot(0, &input, &InputProviderWidget::reject);
		QCOMPARE(controller.presentInput(&input), InputProviderWidget::Rejected);
		QCoreApplication::processEvents();
		QCOMPARE(input.parentWidget(), home);
		QVERIFY(!input.isVisible());
		QCOMPARE(window.size(), shellSize);
		QCOMPARE(window.minimumSize(), shellMinimumSize);
		QCOMPARE(window.pageStack()->geometry(), stackGeometry);
		QCOMPARE(home->geometry(), pageGeometry);
		QCOMPARE(QApplication::topLevelWidgets().size(), topLevelCount);
	}
}

void RootWindowTest::activeInputRejectsNavigationAndNesting()
{
	RootController controller;
	ObservedRootWindow window;
	window.resize(800, 480);
	QWidget *home = new QWidget;
	controller.initialize(&window, home);
	window.show();

	TestInputProvider outer(home);
	TestInputProvider nested(home);
	QPointer<QWidget> refusedPage;
	InputProviderWidget::Result nestedResult = InputProviderWidget::Accepted;
	QTimer::singleShot(0, &outer, [&]() {
		QWidget *page = new QWidget;
		refusedPage = page;
		controller.presentWidget(page);
		nestedResult = controller.presentInput(&nested);
		outer.reject();
	});

	QCOMPARE(controller.presentInput(&outer), InputProviderWidget::Rejected);
	QCOMPARE(nestedResult, InputProviderWidget::Rejected);
	QCOMPARE(controller.depth(), 1U);
	QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
	QVERIFY(refusedPage.isNull());
}

void RootWindowTest::calibrationUsesPersistentWindowAndDefersWrites()
{
	QTemporaryDir directory;
	QVERIFY(directory.isValid());
	const QByteArray originalHome = qgetenv("HOME");
	QVERIFY(qputenv("HOME", directory.path().toLocal8Bit()));
	QDir configDirectory(directory.filePath(QStringLiteral(".config/labwc")));
	QVERIFY(configDirectory.mkpath(QStringLiteral(".")));
	const QString configPath = configDirectory.filePath(QStringLiteral("rc.xml"));
	const QByteArray original =
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<openbox_config xmlns=\"http://openbox.org/3.4/rc\">"
		"<libinput><device category=\"TSC2007 Touchscreen\">"
		"<calibrationMatrix>1 0 0 0 1 0</calibrationMatrix>"
		"</device></libinput></openbox_config>\n";
	QFile config(configPath);
	QVERIFY(config.open(QIODevice::WriteOnly));
	QCOMPARE(config.write(original), original.size());
	config.close();

	RootController &controller = RootController::ref();
	ObservedRootWindow window;
	window.resize(800, 480);
	QWidget *home = new QWidget;
	controller.initialize(&window, home);
	window.show();
	QCoreApplication::processEvents();
	const WId shellId = window.winId();
	const int topLevelCount = QApplication::topLevelWidgets().size();

	Calibrate::calibrate();
	QCoreApplication::processEvents();
	QWidget *calibrationPage = controller.currentWidget();
	QVERIFY(calibrationPage != home);
	QCOMPARE(calibrationPage->objectName(), QStringLiteral("calibrationPage"));
	QCOMPARE(window.winId(), shellId);
	QCOMPARE(QApplication::topLevelWidgets().size(), topLevelCount);
	QCOMPARE(controller.depth(), 2U);
	const QImage calibrationImage = calibrationPage->grab().toImage();
	int artworkPixelCount = 0;
	for (int y = 0; y < 120; ++y) {
		for (int x = 40; x < 155; ++x) {
			const QColor pixel = calibrationImage.pixelColor(x, y);
			if (pixel.red() < 80 && pixel.green() < 80 && pixel.blue() < 80)
				++artworkPixelCount;
		}
	}
	QVERIFY2(artworkPixelCount > 100, "The raccoon artwork was not painted below the first crosshair");
	QFile unchangedConfig(configPath);
	QVERIFY(unchangedConfig.open(QIODevice::ReadOnly));
	QCOMPARE(unchangedConfig.readAll(), original);
	unchangedConfig.close();
	QVERIFY(!QFile::exists(configPath + QStringLiteral(".bak")));

	QPushButton *cancel = calibrationPage->findChild<QPushButton *>(
		QStringLiteral("calibrationActionButton"));
	QVERIFY(cancel);
	QTest::mouseClick(cancel, Qt::LeftButton);
	QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
	QCOMPARE(controller.currentWidget(), home);
	QCOMPARE(controller.depth(), 1U);
	QCOMPARE(QApplication::topLevelWidgets().size(), topLevelCount);
	QVERIFY(!QFile::exists(configPath + QStringLiteral(".bak")));
	QVERIFY(qputenv("HOME", originalHome));
}

void RootWindowTest::calibrationArtworkLoads()
{
	QImage raccoon(QStringLiteral(":/raccoon.png"));
	QVERIFY2(!raccoon.isNull(), "Qt could not decode the embedded raccoon artwork");
	QVERIFY(raccoon.width() > 0);
	QVERIFY(raccoon.height() > 0);
	QCOMPARE(raccoon.pixelColor(0, 0).alpha(), 0);
}

QTEST_MAIN(RootWindowTest)
#include "RootWindowTest.moc"
