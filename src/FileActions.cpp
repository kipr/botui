#include "FileActions.h"

FileAction::FileAction(const QString &name, const QStringList &extensions)
	: m_name(name),
	m_extensions(extensions)
{
}

FileAction::~FileAction()
{
}

const QString &FileAction::name() const
{
	return m_name;
}

const QStringList &FileAction::extensions() const
{
	return m_extensions;
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

FileAction *FileActions::action(const QString &extension) const
{
	// Yes, we're doing a linear search.
	// In the future, it would be nice
	// to have a QMap or something.
	foreach(FileAction *action, m_actions) {
		if(action->extensions().contains(extension)) {
			return action;
		}
	}
	return 0;
}

const FileActionList &FileActions::actions() const
{
	return m_actions;
}
