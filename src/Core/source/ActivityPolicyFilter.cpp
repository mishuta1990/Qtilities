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

#include "ActivityPolicyFilter.h"
#include "QtilitiesCoreConstants.h"
#include "Observer.h"
#include "QtilitiesPropertyChangeEvent.h"

#include <Logger.h>

#include <QtDebug>
#include <QMutex>
#include <QVariant>
#include <QCoreApplication>

using namespace Qtilities::Core::Properties;

namespace Qtilities {
    namespace Core {
        FactoryItem<AbstractSubjectFilter, ActivityPolicyFilter> ActivityPolicyFilter::factory;
    }
}

Qtilities::Core::ActivityPolicyFilter::ActivityPolicyFilter(QObject* parent) : AbstractSubjectFilter(parent) {
    d_activity_policy = ActivityPolicyFilter::UniqueActivity;
    d_minimum_activity_policy = ActivityPolicyFilter::ProhibitNoneActive;
    d_new_subject_activity_policy = ActivityPolicyFilter::SetNewActive;
}

void Qtilities::Core::ActivityPolicyFilter::setActivityPolicy(ActivityPolicyFilter::ActivityPolicy activity_policy) {
    // Only change the policy if the observer context is not defined for the subject filter.
    if (!observer)
        d_activity_policy = activity_policy;
}

void Qtilities::Core::ActivityPolicyFilter::setMinimumActivityPolicy(ActivityPolicyFilter::MinimumActivityPolicy minimum_activity_policy) {
    // Only change the policy if the observer context is not defined for the subject filter.
    if (!observer)
        d_minimum_activity_policy = minimum_activity_policy;
}

void Qtilities::Core::ActivityPolicyFilter::setNewSubjectActivityPolicy(ActivityPolicyFilter::NewSubjectActivityPolicy new_subject_activity_policy) {
    // Only change the policy if the observer context is not defined for the subject filter.
    if (!observer)
        d_new_subject_activity_policy = new_subject_activity_policy;
}

int Qtilities::Core::ActivityPolicyFilter::numActiveSubjects() const {
    int count = 0;
    bool is_active = false;
    for (int i = 0; i < observer->subjectCount(); i++) {
        is_active = observer->getObserverPropertyValue(observer->subjectAt(i),OBJECT_ACTIVITY).toBool();
        if (is_active)
            ++count;
    }
    return count;
}

QList<QObject*> Qtilities::Core::ActivityPolicyFilter::activeSubjects() const {
    QList<QObject*> list;
    bool is_active = false;
    for (int i = 0; i < observer->subjectCount(); i++) {
        is_active = observer->getObserverPropertyValue(observer->subjectAt(i),OBJECT_ACTIVITY).toBool();
        if (is_active)
            list.push_back(observer->subjectAt(i));
    }
    return list;
}

QList<QObject*> Qtilities::Core::ActivityPolicyFilter::inactiveSubjects() const {
    QList<QObject*> list;
    bool is_active = false;
    for (int i = 0; i < observer->subjectCount(); i++) {
        is_active = observer->getObserverPropertyValue(observer->subjectAt(i),OBJECT_ACTIVITY).toBool();
        if (!is_active)
            list.push_back(observer->subjectAt(i));
    }
    return list;
}

bool Qtilities::Core::ActivityPolicyFilter::exportFilterSpecificBinary(QDataStream& stream) const {
    stream << (quint32) d_activity_policy;
    stream << (quint32) d_minimum_activity_policy;
    stream << (quint32) d_new_subject_activity_policy;

    return true;
}

bool Qtilities::Core::ActivityPolicyFilter::importFilterSpecificBinary(QDataStream& stream) {
    quint32 ui32;
    stream >> ui32;
    d_activity_policy = (ActivityPolicy) ui32;
    stream >> ui32;
    d_minimum_activity_policy = (MinimumActivityPolicy) ui32;
    stream >> ui32;
    d_new_subject_activity_policy = (NewSubjectActivityPolicy) ui32;

    return true;
}

void Qtilities::Core::ActivityPolicyFilter::setActiveSubjects(QList<QObject*> objects) {
    if (!observer) {
        LOG_TRACE("Cannot set active objects in an activity subject filter without an observer context.");
        return;
    }

    // Make sure all objects in the list is observed by this observer context.
    for (int i = 0; i < objects.count(); i++) {
        if (!objects.at(i)) {
            LOG_TRACE(QString("Invalid objects in list sent to setActiveSubjects(). Null pointer to object detected at list position %1.").arg(i));
            return;
        }

        if (!observer->contains(objects.at(i))) {
            LOG_TRACE(QString("Invalid objects in list sent to setActiveSubjects(). Object %1 is not observed in this context.").arg(objects.at(i)->objectName()));
            return;
        }
    }

    // Check the number of objects in the list against the policies of this filter.
    if (objects.count() == 0) {
        if (d_minimum_activity_policy == ActivityPolicyFilter::ProhibitNoneActive)
            return;
    }
    if (objects.count() > 1) {
        if (d_activity_policy == ActivityPolicyFilter::UniqueActivity)
            return;
    }

    // Now we know that the list is valid, lock the mutex
    filter_mutex.tryLock();
    // Set all objects as inactive
    for (int i = 0; i < observer->subjectCount(); i++) {
        observer->setObserverPropertyValue(observer->subjectAt(i),OBJECT_ACTIVITY,QVariant(false));
    }
    // Set objects in the list as active
    for (int i = 0; i < objects.count(); i++) {
        observer->setObserverPropertyValue(objects.at(i),OBJECT_ACTIVITY,QVariant(true));
    }
    filter_mutex.unlock();

    // Post the QtilitiesPropertyChangeEvents on all subjects
    for (int i = 0; i < observer->subjectCount(); i++) {
        QByteArray property_name_byte_array = QByteArray(OBJECT_ACTIVITY);
        QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observer->observerID());
        QCoreApplication::postEvent(observer->subjectAt(i),user_event);
        #ifndef QT_NO_DEBUG
            // Get activity of object for debugging purposes
            QVariant activity = observer->getObserverPropertyValue(observer->subjectAt(i),OBJECT_ACTIVITY);
            if (activity.isValid()) {
                if (activity.toBool())
                    LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2) with activity true").arg(OBJECT_ACTIVITY).arg(observer->subjectAt(i)->objectName()));
                else
                    LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2) with activity false").arg(OBJECT_ACTIVITY).arg(observer->subjectAt(i)->objectName()));
            }
        #endif
    }

    // Emit needed signals
    emit activeSubjectsChanged(objects,inactiveSubjects());
}

Qtilities::Core::AbstractSubjectFilter::EvaluationResult Qtilities::Core::ActivityPolicyFilter::evaluateAttachment(QObject* obj) const {
    return AbstractSubjectFilter::Allowed;
}

bool Qtilities::Core::ActivityPolicyFilter::initializeAttachment(QObject* obj) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return false;
    #endif

    if (!observer) {
        LOG_TRACE(QString("Cannot evaluate an attachment in a subject filter without an observer context."));
        return false;
    } else
        return true;
}

void Qtilities::Core::ActivityPolicyFilter::finalizeAttachment(QObject* obj, bool attachment_successful) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return;
    #endif

    if (!observer) {
        LOG_TRACE(QString("Cannot evaluate an attachment in a subject filter without an observer context."));
        return;
    }

    if (!attachment_successful)
        return;

    // Ensure that property changes are not handled by the QDynamicPropertyChangeEvent handler.
    filter_mutex.tryLock();

    bool new_activity;
    // First determine the activity of the new subject
    // At this stage the object is not yet attached to the observer, thus dynamic property changes are not handled, we need
    // to do everyhing manually here.
    if (observer->subjectCount() == 1) {
        if (d_minimum_activity_policy == ActivityPolicyFilter::ProhibitNoneActive || d_new_subject_activity_policy == ActivityPolicyFilter::SetNewActive) {
            new_activity = true;
        } else {
            new_activity = false;
        }
    } else {
        if (d_new_subject_activity_policy == ActivityPolicyFilter::SetNewActive) {
            if (d_activity_policy == ActivityPolicyFilter::UniqueActivity) {
                for (int i = 0; i < observer->subjectCount(); i++) {
                    if (observer->subjectAt(i) != obj)
                        observer->setObserverPropertyValue(observer->subjectAt(i),OBJECT_ACTIVITY,QVariant(false));
                }
            }
            new_activity = true;
        } else {
            new_activity = false;
        }
    }

    ObserverProperty subject_activity_property = observer->getObserverProperty(obj,OBJECT_ACTIVITY);
    if (subject_activity_property.isValid()) {
        // Thus, the property already exists
        subject_activity_property.addContext(QVariant(new_activity),observer->observerID());
        observer->setObserverProperty(obj,subject_activity_property);
    } else {
        // We need to create the property and add it to the object
        ObserverProperty new_subject_activity_property(OBJECT_ACTIVITY);
        new_subject_activity_property.setIsExportable(false);
        new_subject_activity_property.addContext(QVariant(new_activity),observer->observerID());
        observer->setObserverProperty(obj,new_subject_activity_property);
    }

    if (new_activity) {
        QList<QObject*> active_subjects = activeSubjects();
        active_subjects.push_back(obj);
        // Note that the object which is attached is not yet in the observer context, thus we must add it to the active subject list.
        emit activeSubjectsChanged(active_subjects,inactiveSubjects());
        // Emit dirty property signal
        // emit notifyDirtyProperty(OBJECT_ACTIVITY);
    }

    filter_mutex.unlock();
}

Qtilities::Core::AbstractSubjectFilter::EvaluationResult Qtilities::Core::ActivityPolicyFilter::evaluateDetachment(QObject* obj) const {
    return AbstractSubjectFilter::Allowed;
}

bool Qtilities::Core::ActivityPolicyFilter::initializeDetachment(QObject* obj, bool subject_deleted) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return false;
    #endif

    return true;
}

void Qtilities::Core::ActivityPolicyFilter::finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return;
    #endif

    if (!detachment_successful && !subject_deleted)
        return;

    // Ensure that property changes are not handled by the QDynamicPropertyChangeEvent handler.
    filter_mutex.tryLock();

    if (observer->subjectCount() >= 1) {
        if (d_minimum_activity_policy == ActivityPolicyFilter::ProhibitNoneActive) {
            // Check if this subject was active.
            bool is_active = observer->getObserverPropertyValue(obj,OBJECT_ACTIVITY).toBool();
            if (is_active && (numActiveSubjects() == 0)) {
                // We need to set a different subject to be active.
                observer->setObserverPropertyValue(observer->subjectAt(0),OBJECT_ACTIVITY, QVariant(true));
            }
        }

        emit notifyDirtyProperty(OBJECT_ACTIVITY);
        emit activeSubjectsChanged(activeSubjects(),inactiveSubjects());
    }

    // Unlock the filter mutex.
    filter_mutex.unlock();
}

QStringList Qtilities::Core::ActivityPolicyFilter::monitoredProperties() {
    QStringList reserved_properties;
    reserved_properties << QString(OBJECT_ACTIVITY);
    return reserved_properties;
}

bool Qtilities::Core::ActivityPolicyFilter::monitoredPropertyChanged(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent) {
    if (!filter_mutex.tryLock())
        return true;

    if (!observer)
        return false;

    bool new_activity = observer->getObserverPropertyValue(obj,OBJECT_ACTIVITY).toBool();
    if (new_activity) {
        if (d_activity_policy == ActivityPolicyFilter::UniqueActivity) {
            for (int i = 0; i < observer->subjectCount(); i++) {
                if (observer->subjectAt(i) != obj) {
                    observer->setObserverPropertyValue(observer->subjectAt(i),OBJECT_ACTIVITY, QVariant(false));
                }
            }
        }
    } else {
        if (d_minimum_activity_policy == ActivityPolicyFilter::ProhibitNoneActive && (numActiveSubjects() == 0)) {
            // In this case, we allow the change to go through but we change the value here.
            // To update GUIs monitoring OBJECT_ACTIVITY, we need to restore the property manually and emit that
            // the property is dirty. Perhaps a better way of doing this will be thought out and used in the future.
            observer->setObserverPropertyValue(obj,OBJECT_ACTIVITY, QVariant(true));
            emit activeSubjectsChanged(activeSubjects(),inactiveSubjects());
            filter_mutex.unlock();
            return false;
        }
    }

    // Emit active subjects changed signal
    emit activeSubjectsChanged(activeSubjects(),inactiveSubjects());

    filter_mutex.unlock();
    return false;
}