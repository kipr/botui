#include "FileActions.h"

#include <QFileInfo>

FileAction::FileAction(const QString &name)
	: m_name(name)
{
}

FileAction::~FileAction()
{
}

const QString &FileAction::name() const
{
	return m_name;
}

FileActionExtension::FileActionExtension(const QString &name, const QStringList &extensions)
	: FileAction(name),
	m_extensions(extensions)
{
}

bool FileActionExtension::canHandle(const QString &path) const
{
	return m_extensions.contains(QFileInfo(path).suffix());
}

FileActions::FileActions()
{
}

FileActions::~FileActions()
{
	foreach(FileAction *act, m_actions) delete act;
}

void FileActions::addAction(FileAction *action)
{
	m_actions.append(action);
}

FileAction *FileActions::action(const QString &path) const
{
	// Yes, we're doing a linear search.
	// In the future, it would be nice
	// to have a QMap or something.
	foreach(FileAction *action, m_actions) {
		if(action->canHandle(path)) return action;
	}
	return 0;
}

const FileActionList &FileActions::actions() const
{
	return m_actions;
}
