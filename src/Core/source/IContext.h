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

#ifndef ICONTEXT_H
#define ICONTEXT_H

#include "QtilitiesCore_global.h"

#include <QObject>
#include <QString>

namespace Qtilities {
    namespace Core {
        namespace Interfaces {
            /*!
            \class IContext
            \brief Objects implementing this interface will have a context associated with them.
              */
            class QTILIITES_CORE_SHARED_EXPORT IContext {
            public:
                IContext() {}
                virtual ~IContext() {}

                //! Returns a context string for the context represented by this interface.
                virtual QString contextString() const = 0;
                //! Returns a help ID for this context.
                virtual QString contextHelpId() const { return QString(); }
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::IContext,"com.Qtilities.Core.IContext/1.0");

#endif // ICONTEXT_H