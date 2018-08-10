#include "backgroundinterface.h"
#include "backgroundinterfaceplugin.h"

#include <QtPlugin>

BackgroundInterfacePlugin::BackgroundInterfacePlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void BackgroundInterfacePlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool BackgroundInterfacePlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *BackgroundInterfacePlugin::createWidget(QWidget *parent)
{
    return new BackgroundInterface(parent);
}

QString BackgroundInterfacePlugin::name() const
{
    return QLatin1String("BackgroundInterface");
}

QString BackgroundInterfacePlugin::group() const
{
    return QLatin1String("");
}

QIcon BackgroundInterfacePlugin::icon() const
{
    return QIcon(QLatin1String(":/128.ico"));
}

QString BackgroundInterfacePlugin::toolTip() const
{
    return QLatin1String("");
}

QString BackgroundInterfacePlugin::whatsThis() const
{
    return QLatin1String("");
}

bool BackgroundInterfacePlugin::isContainer() const
{
    return false;
}

QString BackgroundInterfacePlugin::domXml() const
{
    return QLatin1String("<widget class=\"BackgroundInterface\" name=\"backgroundInterface\">\n</widget>\n");
}

QString BackgroundInterfacePlugin::includeFile() const
{
    return QLatin1String("backgroundinterface.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(backgroundinterfaceplugin, BackgroundInterfacePlugin)
#endif // QT_VERSION < 0x050000
