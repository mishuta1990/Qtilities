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

#include "AbstractLoggerEngine.h"

Qtilities::Logging::AbstractLoggerEngine::AbstractLoggerEngine() : QObject()
{
    abstractLoggerEngineData = new AbstractLoggerEngineData();
    abstractLoggerEngineData->formatting_engine = 0;
    abstractLoggerEngineData->is_enabled = true;

    connect(this,SIGNAL(destroyed()),SLOT(finalize()));

    installEventFilter(this);
    enableAllMessageTypes();
}

Qtilities::Logging::AbstractLoggerEngine::~AbstractLoggerEngine()
{
    delete abstractLoggerEngineData;
}

bool Qtilities::Logging::AbstractLoggerEngine::isInitialized() const {
    return abstractLoggerEngineData->is_initialized;
}

bool Qtilities::Logging::AbstractLoggerEngine::isActive() const {
    return abstractLoggerEngineData->is_enabled;
}

void Qtilities::Logging::AbstractLoggerEngine::setActive(bool is_active) {
    if (is_active == abstractLoggerEngineData->is_enabled)
        return;

    QList<QVariant> message;
    if (is_active) {
        message << QString(tr("Engine is now active."));
    } else {
        message << QString(tr("Engine is now inactive."));
    }
    QString formatted_message = abstractLoggerEngineData->formatting_engine->formatMessage(Logger::Info,message);
    logMessage(formatted_message);
    abstractLoggerEngineData->is_enabled = is_active;
}

void Qtilities::Logging::AbstractLoggerEngine::setEnabledMessageTypes(Logger::MessageTypeFlags message_types) {
    abstractLoggerEngineData->enabled_message_types = message_types;
}

Qtilities::Logging::Logger::MessageTypeFlags Qtilities::Logging::AbstractLoggerEngine::getEnabledMessageTypes() const {
    return abstractLoggerEngineData->enabled_message_types;
}

void Qtilities::Logging::AbstractLoggerEngine::enableAllMessageTypes() {
    abstractLoggerEngineData->enabled_message_types = 0;
    abstractLoggerEngineData->enabled_message_types |= Logger::Info;
    abstractLoggerEngineData->enabled_message_types |= Logger::Warning;
    abstractLoggerEngineData->enabled_message_types |= Logger::Error;
    abstractLoggerEngineData->enabled_message_types |= Logger::Fatal;
    abstractLoggerEngineData->enabled_message_types |= Logger::Debug;
    abstractLoggerEngineData->enabled_message_types |= Logger::Trace;
}

void Qtilities::Logging::AbstractLoggerEngine::installFormattingEngine(AbstractFormattingEngine* engine) {
    if (engine == abstractLoggerEngineData->formatting_engine)
        return;

    if (!isFormattingEngineConstant() && abstractLoggerEngineData->formatting_engine && engine) {
        abstractLoggerEngineData->formatting_engine = engine;
        logMessage(" ");
        logMessage(QString(tr("Formatting engine change detected.")));
        logMessage(QString(tr("This engine now logs messages using the following formatting engine: %1")).arg(abstractLoggerEngineData->formatting_engine->name()));
        logMessage(" ");
    } else if (!abstractLoggerEngineData->formatting_engine && engine)
        abstractLoggerEngineData->formatting_engine = engine;
}

Qtilities::Logging::AbstractFormattingEngine* Qtilities::Logging::AbstractLoggerEngine::getInstalledFormattingEngine() {
    return abstractLoggerEngineData->formatting_engine;
}

QString Qtilities::Logging::AbstractLoggerEngine::formattingEngineName() {
    if (abstractLoggerEngineData->formatting_engine)
        return abstractLoggerEngineData->formatting_engine->objectName();
    else
        return tr("None");
}

void Qtilities::Logging::AbstractLoggerEngine::newMessages(const QString& engine_name, Logger::MessageType message_type, const QList<QVariant>& messages) {
    if ((engine_name != QString("All")) && (engine_name != objectName()))
        return;

    // Check if active
    if (abstractLoggerEngineData->is_enabled) {
        // Check if there is a formatting engine present
        if (abstractLoggerEngineData->formatting_engine) {
            //Check if this message type is allowed
            if (abstractLoggerEngineData->enabled_message_types & message_type)
                logMessage(abstractLoggerEngineData->formatting_engine->formatMessage(message_type,messages));
        }
    }
}