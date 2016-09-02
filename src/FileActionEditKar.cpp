#include "FileActionEditKar.h"
#include "RootController.h"
#include "EditorWidget.h"

#include <kar/kar.hpp>
#include <QDebug>

FileActionEditKar::FileActionEditKar()
	: FileAction(QObject::tr("Edit"))
{
	
}

bool FileActionEditKar::canHandle(const QString &path) const
{
	return true;//FIXME return kiss::Kar::isValid(path);
}

bool FileActionEditKar::act(const QString &path, Device *device) const
{
	EditorWidget *editor = new EditorWidget(device);
	//FIXME: editor->setSavePath(path);
	//FIXME: editor->setArchive(kiss::Kar::load(path));
	RootController::ref().presentWidget(editor);
	return true;
}

REGISTER_FILE_ACTION(FileActionEditKar)
