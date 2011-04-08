/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
**
** This file is part of Qtilities which is released under the following
** licensing options.
**
** Option 1: Open Source
** Under this license Qtilities is free software: you can
** redistribute it and/or modify it under the terms of the GNU General
** Public License as published by the Free Software Foundation, either
** version 3 of the License, or (at your option) any later version.
**
** Qtilities is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Qtilities. If not, see http://www.gnu.org/licenses/.
**
** Option 2: Commercial
** Alternatively, this library is also released under a commercial license
** that allows the development of closed source proprietary applications
** without restrictions on licensing. For more information on this option,
** please see the project website's licensing page:
** http://www.qtilities.org/licensing.html
**
** If you are unsure which license is appropriate for your use, please
** contact support@qtilities.org.
**
****************************************************************************/

#ifndef QTILITIES_APPLICATION_H
#define QTILITIES_APPLICATION_H

#include "QtilitiesCoreGui_global.h"
#include "ActionManager.h"
#include "ClipboardManager.h"

#include <LoggingConstants>

#include <QtilitiesCoreApplication>

#include <QApplication>
#include <QMainWindow>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;
        using namespace Qtilities::Core;

        /*!
          \class QtilitiesApplication
          \brief The QtilitiesApplication class is the same as QApplication with added functionality.

          The QtilitiesApplication class is an extension of QApplication and should be used when building
          %Qtilities applications. For example:

\code
QtilitiesApplication a(argc, argv);
QtilitiesApplication::setOrganizationName("Jaco Naude");
QtilitiesApplication::setOrganizationDomain("Qtilities");
QtilitiesApplication::setApplicationName("Example");
QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersionString());
\endcode

         If you for some reason don't want to use the QtilitiesApplication as shown above, you must call the initialize()
         function instead in your application's main function.

         This class provides extra functionality which is not found in QApplication:
         - Access to an object manager through objectManager().
         - Access to a context manager through contextManager().
         - Access to an action manager through actionManager().
         - Access to a clipboard manager through clipboardManager().
         - The ability to define and access your top level main window in your application using setMainWindow() and mainWindow().
         - Settings update requests using newSettingsUpdateRequest() and settingsUpdateRequest().
         - Information about %Qtilities through qtilitiesVersionString() and aboutQtilities().
         - Access to the application's configuration page through configWidget().
         - Export version informatin for both %Qtilities as well as application specific versioning information. For more information see Qtilities::Core::Interfaces::IExportable.
         - Allows setting of a session path for application information, see applicationSessionPath().

          \sa Qtilities::Core::QtilitiesCoreApplication
         */
        class QTILITIES_CORE_GUI_SHARED_EXPORT QtilitiesApplication : public QApplication
        {
            Q_OBJECT

        public:
            QtilitiesApplication(int &argc, char ** argv);
            ~QtilitiesApplication();

            //! Returns a reference to the object manager.
            static IObjectManager* objectManager();
            //! Returns a reference to the context manager.
            static IContextManager* contextManager();
            //! Returns a reference to the action manager.
            /*!
              If you are using the action manager, you must specify a main window using setMainWindow() for
              the action manager to work correctly.
              */
            static IActionManager* actionManager();
            //! Returns a reference to the clipboard manager.
            static IClipboard* clipboardManager();
            //! Sets the main window reference for the application.
            static void setMainWindow(QWidget* mainWindow);
            //! Gets the main window reference for the application.
            static QWidget* mainWindow();
            //! Sets the configuration widget reference for the application.
            static void setConfigWidget(QWidget* configWidget);
            //! Gets the configuration widget reference for the application.
            static QWidget* configWidget();
            //! Returns the version string of %Qtilities as a QString.
            /*!
              \return The version of %Qtilities, for example: 0.1 Beta 1. Note that the v is not part of the returned string.
              */
            static QString qtilitiesVersionString();
            //! Returns the version number of %Qtilities.
            VersionNumber qtilitiesVersion();

            //! Initializes the QtilitiesApplication object.
            /*!
              Normally the QtilitiesApplication object will be initialized automatically in the application's main function
              as follows:

\code
QtilitiesApplication a(argc, argv);
QtilitiesApplication::setOrganizationName("Jaco Naude");
QtilitiesApplication::setOrganizationDomain("Qtilities");
QtilitiesApplication::setApplicationName("Example");
QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersionString());
\endcode

            If it however not possible to use QtilitiesApplication as show above it should be initialized directly after the above
            set of statements, as follows:

\code
QApplication a(argc, argv);
QApplication::setOrganizationName("Jaco Naude");
QApplication::setOrganizationDomain("Qtilities");
QApplication::setApplicationName("Example");
QApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersionString());
QtilitiesApplication::initialize();
\endcode

            Example scenarios where this might happen is when you are using a different class which inherits QApplication, for
            example the QtSingleApplication solution.

            It is important to note that the initialize() function does not create an QtilitiesApplication instance, thus instance() will return 0.
            The goal of the function used in this way is to register all needed %Qtilities classes in the %Qtilities factory.
              */
            static void initialize();
            //! Returns a reference to the QtilitiesApplication instance.
            /*!
              \param silent When true, a warning message will not be printed when the QtilitiesCoreApplication instance was not found.
              */
            static QtilitiesApplication* instance(bool silent = false);
            //! Checks if a valid instance has been created.
            /*!
              \param silent When true, a warning message will not be printed when the QtilitiesCoreApplication instance was not found.
              */
            static bool hasInstance(const char *function, bool silent = false);

            //! QCoreApplication::notify() overload. This allows exception handling in GUI applications.
            bool notify(QObject * object, QEvent * event);

            //! Create a new settings update request.
            /*!
                This function allows settings update requests to be sent anywhere in an application. This
                allows objects which depend on the settings to update themselves when the settings change.

                This function will emit settingsUpdateRequest() with the given \p request_id.
              */
            static inline void newSettingsUpdateRequest(const QString& request_id) { if (m_Instance) { emit m_Instance->settingsUpdateRequest(request_id); } }

            //! Returns a session path for your application where you can store session related information (for example shortcut configurations etc.)
            /*!
              By default this is QCoreApplication::applicationDirPath() + qti_def_PATH_SESSION.

              \sa setApplicationSessionPath()
              */
            static QString applicationSessionPath();
            //! Sets the session path to be used in your application:
            static void setApplicationSessionPath(const QString& path);

            //! Sets the application export format for your application.
            /*!
              \sa Qtilities::Core::IExportable::applicationExportVersion(), applicationExportVersion()
              */
            static void setApplicationExportVersion(quint32 application_export_version);
            //! Gets the application export format for your application.
            /*!
              Default is 0.

              \sa Qtilities::Core::IExportable::applicationExportVersion(), setApplicationExportVersion()
              */
            static quint32 applicationExportVersion();

        signals:
            //! Signal which broadcasts that settings identified by the \p request_id changed and requires updating.
            void settingsUpdateRequest(const QString& request_id);

        public slots:
            //! Returns a Qtilities::CoreGui::AboutWindow with information about the Qtilities libraries.
            static QWidget* aboutQtilities(bool show = true);

        private:
            Q_DISABLE_COPY(QtilitiesApplication)

            static QtilitiesApplication* m_Instance;
        };
    }
}

#if defined(QtilitiesApp)
#undef QtilitiesApp
#endif
#define QtilitiesApp ((QtilitiesApplication *) QApplication::instance())
#define ACTION_MANAGER ((QtilitiesApplication *) QApplication::instance())->actionManager()
#define CLIPBOARD_MANAGER ((QtilitiesApplication *) QApplication::instance())->clipboardManager()

#endif // QTILITIES_APPLICATION_H
