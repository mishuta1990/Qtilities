/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
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

#ifndef IMODIFICATIONNOTIFIER_H
#define IMODIFICATIONNOTIFIER_H

#include "QtilitiesCore_global.h"
#include "IObjectBase.h"

#include <QList>
#include <QPointer>
#include <QString>
#include <QObject>

namespace Qtilities {
    namespace Core {
        namespace Interfaces {
            /*!
            \class IModificationNotifier
            \brief Objects can implement this interface if they want to broadcast information when they are modified/change.

            Objects can implement this interface if they want to broadcast information when they are modified. The use
            of this interface will be described using an example of a program supports project management.

            The project manager will manage some objects all implementing this interface. When the object changes it
            will emit the modificationStateChanged() signal. The user will then be notified that the project changed.
            When the user saves the project, the project manager will call the setModificationState() slot on
            all objects.

            When you want to connect to the modificationStateChange signal on this interface, connect to it on the object
            returned by objectBase().
              */
            class QTILIITES_CORE_SHARED_EXPORT IModificationNotifier : virtual public IObjectBase {
            public:
                IModificationNotifier() : d_isModified(false) {}
                virtual ~IModificationNotifier() {}

                //! Indicates the modification state of the object.
                virtual bool isModified() const = 0;
                //! Sets the modification state of the object. Returns true if it was successfull.
                /*!
                  \param notify_listeners Indicates if listeners to this interface (objects connected to the modificationStateChanged() signal)
                  must be notified, thus it controls if the signal is emitted or not.
                  \param notify_subjects Indicates if objects which this object is listening to must be updated.

                    When implementing this interface, the setModificationState() function must be declared as a slot.
                  */
                virtual void setModificationState(bool new_state, bool notify_listeners = true, bool notify_subjects = false) = 0;
                //! Implement this function as a signal when implementing the object.
                /*!
                  Because this interface does not use the Q_OBJECT macro, you cannot connect to this signal directly.
                  Instead you need to connect to the signal on the base object implementing this interface as follows:

                  \code
                  connect(iface->modifierBase(),SIGNAL(modificationStateChanged(bool)),SLOT(yourSlot(bool)));
                  \endcode

                  \sa modifierBase()
                  */
                virtual void modificationStateChanged(bool is_modified) const = 0;
                //! Signal which can be used to indicate that a specific part of the object implementing the interface changed.
                /*!
                  In some cases you don't want to respond to certain modification. For example: When an ObserverTreeModel
                  is used to view and observer in an item view, we don't want to rebuild the tree structure on every emission
                  of modificationStateChanged() since setting for example and hint on the observer will emit
                  modificationStateChanged() since the observer changed. However we only want to rebuild the tree structure
                  when the actual structure change. In this the ObserverTreeModel will monitor the partialStateChanged() signal
                  and check for the correct part_name before rebuilding the tree structure.

                  \param part_name A string representing the part of the object that changed.
                  */
                virtual void partialStateChanged(const QString& part_name) const = 0;

            protected:
                bool d_isModified;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::IModificationNotifier,"com.Qtilities.Core.IModificationNotifier/1.0")

#endif // IMODIFICATIONNOTIFIER_H