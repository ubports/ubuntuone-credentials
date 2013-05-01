/*
 * Copyright 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <functional>
#include <QDBusPendingCallWatcher>
#include <QDebug>
#include <QHash>
#include <QHostInfo>
#include <QList>
#include <QObject>
#include <QUrl>

#include "dbus/signal_mapper.h"
#include "async_call_data.h"
#include "collection.h"
#include "interface_factory.h"
#include "item.h"
#include "prompt.h"
#include "session.h"
#include "keyring_signal_mapper.h"
#include "keyring.h"

#define TOKEN_SEP " @ "
#define TOKEN_SEP_REPLACEMENT " AT "

namespace keyring
{

// useful macros to remove repetitive code to make things cleaner
#define SETUP_DBUS_MAPPER(mapper, callback) \
    mapper = QSharedPointer<dbus::DBusSignalMapper>(new dbus::DBusSignalMapper()); \
    q->connect(mapper.data(), SIGNAL(mapped(QDBusPendingCallWatcher*, QObject*)), \
        q, SLOT(callback(QDBusPendingCallWatcher*, QObject* )));

#define SETUP_KEYRING_MAPPER(mapper, callback) \
    mapper = QSharedPointer<KeyringSignalMapper>(new KeyringSignalMapper()); \
    q->connect(mapper.data(), SIGNAL(mapped(bool, const QDBusVariant, QObject*)), \
        q, SLOT(callback(bool, const QDBusVariant, QObject* )));

#define ASSERT_SESSION_NOT_NULL(message, signal) \
    if (!_session){ \
        qCritical() << message; \
        signal; \
        return; \
    }

#define SET_WATCH_MAPPING(mapper, watcher, data) \
    mapper->setMapping(watcher, data); \
    q->connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), \
        mapper.data(), SLOT(map(QDBusPendingCallWatcher*)));

#define ASSERT_DBUS_REPLY_IS_ERROR(call, reply, signal) \
    if (reply.isError()) { \
        qCritical() << reply.error().message() << reply.error().type(); \
        signal; \
        call->deleteLater(); \
        return; \
    }

// define types to make code cleaner
typedef std::function<void(AsyncCallData*)> async_callback_cb;
typedef std::function<void(PromptInterface*, AsyncCallData*)> prompt_interface_cb;
typedef std::function<void(QString, AsyncCallData*)> locked_items_cb;

class KeyringPrivate
{

    Q_DECLARE_PUBLIC(Keyring)
public:
    KeyringPrivate(QDBusConnection connection, Keyring* parent, QString windowId);

    void openSession();
    void getCredentials(QString id);
    void setCredentials(QString id, QString token, QString tokenSecret, QString consumer, QString consumerSecret);
    void deleteCredentials(QString id);

    static class _init
    {
        public:
            _init()
            {
                qDBusRegisterMetaType<DBusSecretHash>();
                qDBusRegisterMetaType<Secret>();
            }
    } _initializer;

protected:
    QHash<QString, QString>getKeyringAttrs(QString id);
    void getCredentialsFromItem(QDBusObjectPath itemPath, AsyncCallData* data);
    void deleteItemInterface(AsyncCallData* data);
    void doPrompt(QString promptPath, AsyncCallData* data, prompt_interface_cb connectSignal);
    void doPromptForGet(QString promptPath, AsyncCallData* data);
    void doPromptForDelete(QString promptPath, AsyncCallData* data);
    void doPromptForCollectionUnlock(QString promptPath, AsyncCallData* data);

    void onSessionOpened(QDBusPendingCallWatcher* call);
    void onSearchItems(QDBusPendingCallWatcher* call, QObject* obj, async_callback_cb dbusErrorCb,
        async_callback_cb unlockDbusError, locked_items_cb lockedItemsCb,
        async_callback_cb unlockedItemsCb, async_callback_cb credentialsNotFoundCb);
    void onSearchItemsForGet(QDBusPendingCallWatcher* call, QObject* obj);
    void onSearchItemsForDelete(QDBusPendingCallWatcher* call, QObject* obj);
    void onPromptCompleted(bool dismissed, const QDBusVariant&, QObject* obj, async_callback_cb emptyCb,
        async_callback_cb lockedCb);
    void onPromptCompletedForGet(bool dismissed, const QDBusVariant &result, QObject* obj);
    void onPromptCompletedForDelete(bool dismissed, const QDBusVariant &result, QObject* obj);
    void onCollectionUnlocked(QDBusPendingCallWatcher* call, QObject* obj);
    void onReadAliasDone(QDBusPendingCallWatcher* call, QObject* obj);
    void onItemDeleted(QDBusPendingCallWatcher* call, QObject* obj);
    void onGetSecret(QDBusPendingCallWatcher* call, QObject* obj);
    void onCreateItem(QDBusPendingCallWatcher* call, QObject* obj);
    void onUnlock(QDBusPendingCallWatcher* call, QObject* obj);

protected:
    static QString SECRET_SERVICE;
    static QString TOKEN_NAME;
    static QString TOKEN_KEY;
    static QString TOKEN_SECRET_KEY;
    static QString CONSUMER_KEY;
    static QString CONSUMER_SECRET_KEY;
    static QString ATTR_KEY_TYPE_KEY;
    static QString ATTR_KEY_TYPE_VALUE;
    static QString ATTR_KEY_NAME_KEY;
    static QString DEFAULT_COLLECTION_PATH;
    static QString DEFAULT_LABEL;
    static QString ITEM_LABEL_PROPERTY;
    static QString ITEM_ATTRIBUTES_PROPERTY;
    static QString NO_PROMPT_REQUIRED;

private:
    QString _windowId;
    QDBusConnection _conn;
    QSharedPointer<dbus::DBusSignalMapper> _searchItemsForGetMapper;
    QSharedPointer<dbus::DBusSignalMapper> _searchItemsForDeleteMapper;
    QSharedPointer<dbus::DBusSignalMapper> _readAliasMapper;
    QSharedPointer<dbus::DBusSignalMapper> _unlockCollectionMapper;
    QSharedPointer<dbus::DBusSignalMapper> _onDeletedMapper;
    QSharedPointer<dbus::DBusSignalMapper> _onGetSecretMapper;
    QSharedPointer<dbus::DBusSignalMapper> _onCreateItemMapper;
    QSharedPointer<dbus::DBusSignalMapper> _onUnlockMapper;
    QSharedPointer<KeyringSignalMapper> _promptCompletedMapperOnGet;
    QSharedPointer<KeyringSignalMapper> _promptCompletedMapperOnDelete;
    QSharedPointer<KeyringSignalMapper> _promptCompletedMapperOnCollectionUnlocked;
    QSharedPointer<ServiceInterface> _service;
    QSharedPointer<SessionInterface> _session;
    QSharedPointer<InterfaceFactory> _interfaceFactory;

    Keyring *q_ptr;
};

KeyringPrivate::_init KeyringPrivate::_initializer;
QString KeyringPrivate::SECRET_SERVICE = "org.freedesktop.secrets";
QString KeyringPrivate::TOKEN_NAME = "name";
QString KeyringPrivate::TOKEN_KEY = "token";
QString KeyringPrivate::TOKEN_SECRET_KEY = "token_secret";
QString KeyringPrivate::CONSUMER_KEY = "consumer_key";
QString KeyringPrivate::CONSUMER_SECRET_KEY = "consumer_secret";
QString KeyringPrivate::ATTR_KEY_TYPE_KEY = "key-type";
QString KeyringPrivate::ATTR_KEY_TYPE_VALUE = "sso token";
QString KeyringPrivate::ATTR_KEY_NAME_KEY = "token-name";
QString KeyringPrivate::DEFAULT_COLLECTION_PATH = "/org/freedesktop/secrets/aliases/default";
QString KeyringPrivate::DEFAULT_LABEL = "default";
QString KeyringPrivate::ITEM_LABEL_PROPERTY = "org.freedesktop.Secret.Item.Label";
QString KeyringPrivate::ITEM_ATTRIBUTES_PROPERTY = "org.freedesktop.Secret.Item.Attributes";
QString KeyringPrivate::NO_PROMPT_REQUIRED = "/";

KeyringPrivate::KeyringPrivate(QDBusConnection connection, Keyring* parent, QString windowId) :
    _windowId(windowId),
    _conn(connection),
    q_ptr(parent)
{
    Q_Q(Keyring);

    // mappers used to track the operations and be abble to pass the data between the objects

    // mappers used to map the watcher calls
    SETUP_DBUS_MAPPER(_searchItemsForGetMapper, onSearchItemsForGet)
    SETUP_DBUS_MAPPER(_searchItemsForDeleteMapper, onSearchItemsForDelete)
    SETUP_DBUS_MAPPER(_readAliasMapper, onReadAliasDone)
    SETUP_DBUS_MAPPER(_unlockCollectionMapper, onCollectionUnlocked)
    SETUP_DBUS_MAPPER(_onDeletedMapper, onItemDeleted)
    SETUP_DBUS_MAPPER(_onGetSecretMapper, onGetSecret)
    SETUP_DBUS_MAPPER(_onCreateItemMapper, onCreateItem)
    SETUP_DBUS_MAPPER(_onUnlockMapper, onUnlock)

    // used for the async call to the prompt
    SETUP_KEYRING_MAPPER(_promptCompletedMapperOnGet, onPromptCompletedForGet)
    SETUP_KEYRING_MAPPER(_promptCompletedMapperOnDelete, onPromptCompletedForDelete)

    _promptCompletedMapperOnCollectionUnlocked = QSharedPointer<KeyringSignalMapper>(new KeyringSignalMapper());

    _interfaceFactory = QSharedPointer<InterfaceFactory>(new InterfaceFactory());
    _service = QSharedPointer<ServiceInterface>(
        _interfaceFactory->create<ServiceInterface>(SECRET_SERVICE, "/org/freedesktop/secrets", _conn));
}


/**
 * \fn void Keyring::openSession()
 *
 * Open a unique session for the caller application. This method must be called before
 * trying to use any of the methods of the class. If the methods is not called all
 * other methods will fail.
 */
void KeyringPrivate::openSession()
{
    Q_Q(Keyring);
    QDBusPendingReply<QDBusVariant, QDBusObjectPath> async =
        _service->OpenSession(QString("plain"), QDBusVariant(""));

    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(async);
    q->connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
        q, SLOT(onSessionOpened(QDBusPendingCallWatcher*)));

    qDebug() << "Keyring::Opening session for user using plain algorithm.";
}

/**
 * \fn void Keyring::getCredentials(int id)
 *
 * Tries to retrieve the token and token secret related to the account with the
 * given id. The result will be returned via the credentialsFound signal.
 */
void KeyringPrivate::getCredentials(QString id)
{
    Q_Q(Keyring);

    ASSERT_SESSION_NOT_NULL("Unable to get credentials: Session is NULL.",
        q->credentialsFound(id, QString(""), QString(""), QString(""), QString(""), false))

    // search the items with the given attrs
    QDBusPendingReply<QList<QDBusObjectPath>, QList<QDBusObjectPath>> async =
        _service->SearchItems(getKeyringAttrs(id));

    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(async);

    // store a ref to the watcher with the needed data to later compute the rest
    // of the operations
    AsyncCallData* data = new AsyncCallData();
    data->accId = id;

    SET_WATCH_MAPPING(_searchItemsForGetMapper, watcher, data)
}

/**
 * \fn void Keyring::setCredentials(QString id, QString token, QString tokenSecret)
 *
 * Tries to store the credentials in the keyring for the account with the given id. The result of
 * the operation will be returned via the credentialsSet signal.
 */
void KeyringPrivate::setCredentials(QString id, QString token, QString tokenSecret, QString consumer, QString consumerSecret)
{
    Q_Q(Keyring);
    AsyncCallData* data = new AsyncCallData();
    data->accId = id;
    data->token = token;
    data->tokenSecret = tokenSecret;
    data->consumer = consumer;
    data->consumerSecret = consumerSecret;

    ASSERT_SESSION_NOT_NULL( "Unable to set credentials: Session is NULL.",
        q->credentialsSet(id, false))

    // store the creds in the default collection
    QScopedPointer<CollectionInterface> defaultCollection(
        _interfaceFactory->create<CollectionInterface>(SECRET_SERVICE, DEFAULT_COLLECTION_PATH, _conn));

    if (defaultCollection->label().isEmpty())
    {
        qWarning() << "Keyring::Could not open default collection.";

        QDBusPendingReply<QDBusObjectPath> async =_service->ReadAlias(DEFAULT_LABEL);
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(async);

        SET_WATCH_MAPPING(_readAliasMapper, watcher, data)
    }
    else
    {
        qDebug() << "Keyring::Unlocking default collection.";

        QList<QDBusObjectPath> paths;
        paths.append(QDBusObjectPath(DEFAULT_COLLECTION_PATH));

        QDBusPendingReply<QList<QDBusObjectPath> , QDBusObjectPath> async = _service->Unlock(paths);
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(async);

        data->interfacePath = defaultCollection->path();

        SET_WATCH_MAPPING(_unlockCollectionMapper, watcher, data)
    }
}

/**
 * \fn  void Keyring::deleteCredentials(QString id)
 *
 * Allows to delete the stored credential for the account with the given id.
 */
void KeyringPrivate::deleteCredentials(QString id)
{
    qDebug() << "Keyring::Deleting credentials for account: " << id;
    Q_Q(Keyring);

    ASSERT_SESSION_NOT_NULL("Unable to get credentials: Session is NULL.",
        q->credentialsDeleted(id, false))

    // search the items with the given attrs
    QDBusPendingReply<QList<QDBusObjectPath>, QList<QDBusObjectPath>> async =
        _service->SearchItems(getKeyringAttrs(id));

    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(async);

    // store a ref to the watcher with the needed data to later compute the rest
    // of the operations
    AsyncCallData* data = new AsyncCallData();
    data->accId = id;

    SET_WATCH_MAPPING(_searchItemsForDeleteMapper, watcher, data)
}

QHash<QString, QString> KeyringPrivate::getKeyringAttrs(QString id)
{
    QHash<QString, QString> attrs;
    attrs[KeyringPrivate::ATTR_KEY_TYPE_KEY] = "Ubuntu SSO credentials";
    attrs[KeyringPrivate::ATTR_KEY_NAME_KEY] = Keyring::getTokenName(id);

    return attrs;
}

void KeyringPrivate::getCredentialsFromItem(QDBusObjectPath itemPath, AsyncCallData* data)
{

    Q_Q(Keyring);
    QScopedPointer<ItemInterface> item(
        _interfaceFactory->create<ItemInterface>(SECRET_SERVICE, itemPath.path(), _conn));

    QDBusPendingReply<Secret> async = item->GetSecret(QDBusObjectPath(_session->path()));
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(async);

    SET_WATCH_MAPPING(_onGetSecretMapper, watcher, data)
}

void KeyringPrivate::deleteItemInterface(AsyncCallData* data)
{
    Q_Q(Keyring);
    qDebug() << "Keyring::Deleting item for path: " << data->unlocked.at(0).path();
    QScopedPointer<ItemInterface> item(
    _interfaceFactory->create<ItemInterface>(SECRET_SERVICE, data->unlocked.at(0).path(), _conn));

    // perform the operation and block if necesary
    QDBusPendingReply<QDBusObjectPath> async = item->Delete();
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(async);

    SET_WATCH_MAPPING(_onDeletedMapper, watcher, data)
}

void KeyringPrivate::doPrompt(QString promptPath, AsyncCallData* data, prompt_interface_cb connectSignal)
{
    if (promptPath != KeyringPrivate::NO_PROMPT_REQUIRED)
    {
        qDebug() << "Keyring::Performing prompt for path: " << promptPath;

        PromptInterface* prompt = _interfaceFactory->create<PromptInterface>(SECRET_SERVICE, promptPath, _conn);

        connectSignal(prompt, data);

        prompt->Prompt(_windowId);
    }
}

void KeyringPrivate::doPromptForGet(QString promptPath, AsyncCallData* data)
{
    Q_Q(Keyring);
    qDebug() << "Keyring::doPromptForGet with path: " << promptPath;

    prompt_interface_cb connectSignal = [this, q](PromptInterface* interface, AsyncCallData* in_data)
    {
        _searchItemsForGetMapper->setMapping(interface, in_data);

        q->connect(interface, SIGNAL(Completed(bool, const QDBusVariant)),
            _promptCompletedMapperOnGet.data(), SLOT(map(bool, const QDBusVariant)));
    };

    doPrompt(promptPath, data, connectSignal);
}

void KeyringPrivate::doPromptForDelete(QString promptPath, AsyncCallData* data)
{
    Q_Q(Keyring);
    qDebug() << "Keyring::doPromptForDelete with path: " << promptPath;

    prompt_interface_cb connectSignal = [this, q](PromptInterface* interface, AsyncCallData* in_data)
    {
        _searchItemsForDeleteMapper->setMapping(interface, in_data);

        q->connect(interface, SIGNAL(Completed(bool, const QDBusVariant)),
            _promptCompletedMapperOnDelete.data(), SLOT(map(bool, const QDBusVariant)));
    };
    doPrompt(promptPath, data, connectSignal);
}

void KeyringPrivate::doPromptForCollectionUnlock(QString promptPath, AsyncCallData* data)
{
    Q_Q(Keyring);
    qDebug() << "Keyring::doPromptForCollectionUnlock with path: " << promptPath;

    prompt_interface_cb connectSignal = [this, q](PromptInterface* interface, AsyncCallData* in_data)
    {
        // TODO
    };
    doPrompt(promptPath, data, connectSignal);
}

void KeyringPrivate::onSessionOpened(QDBusPendingCallWatcher* call)
{
    Q_Q(Keyring);
    // retrieve the object path and creation a session object, once done emit the signal
    QDBusPendingReply<QDBusVariant, QDBusObjectPath> reply = *call;

    ASSERT_DBUS_REPLY_IS_ERROR(call, reply, q->sessionOpenError())

    QDBusObjectPath objPath = reply.argumentAt<1>();
    qDebug() << "Keyring::Opened session with path " << objPath.path();
    _session = QSharedPointer<SessionInterface>(
        _interfaceFactory->create<SessionInterface>(SECRET_SERVICE, objPath.path(), _conn));;
    emit q->sessionOpened();
    call->deleteLater();
}

void KeyringPrivate::onSearchItems(QDBusPendingCallWatcher* call, QObject* obj, async_callback_cb dbusErrorCb,
    async_callback_cb unlockDbusError, locked_items_cb lockedItemsCb, async_callback_cb unlockedItemsCb,
    async_callback_cb credentialsNotFoundCb)
{
    Q_Q(Keyring);

    QDBusPendingReply<QList<QDBusObjectPath>, QList<QDBusObjectPath>> reply = *call;
    AsyncCallData* data = (AsyncCallData*) obj;

    qDebug() << "Keyring::onSearchItemsFor account id:" << data->accId;

    ASSERT_DBUS_REPLY_IS_ERROR(call, reply, dbusErrorCb(data))

    // we may get lock and already unlock items, lets try to unlock them to emit the
    // signal once we have all the different items, we can block in this method because
    // we are a signal handler.
    QList<QDBusObjectPath> unlocked = reply.argumentAt<0>();
    data->unlocked = unlocked;
    qDebug() << "Keyring::Number of unlocked items is:" << data->unlocked.length();

    QList<QDBusObjectPath> locked = reply.argumentAt<0>();
    data->locked = locked;

    if (data->locked.length() > 0)
    {
        // set required cbs
        data->unlockDbusError = unlockDbusError;
        data->lockedItemsCb = lockedItemsCb;
        data->unlockedItemsCb = unlockedItemsCb;

        QDBusPendingReply<QList<QDBusObjectPath> , QDBusObjectPath> unlockAsync =_service->Unlock(data->locked);
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(unlockAsync );

        SET_WATCH_MAPPING(_onUnlockMapper, watcher, data)

        call->deleteLater();
        return;
    }

    if (data->unlocked.length() > 0)
    {
        unlockedItemsCb(data);
        call->deleteLater();
        return;
    }

    qDebug() << "Keyring::Items not found for account with id:" << data->accId;
    credentialsNotFoundCb(data);

    call->deleteLater();
}

void KeyringPrivate::onSearchItemsForGet(QDBusPendingCallWatcher* call, QObject* obj)
{
    Q_Q(Keyring);

    async_callback_cb dbusErrorCb = [q](AsyncCallData* data)
    {
        emit q->credentialsFound(data->accId, QString(""), QString(""), QString(""), QString(""), false);
    };

    async_callback_cb unlockDbusError = [q](AsyncCallData* data)
    {
        emit q->credentialsError(data->accId);
    };

    locked_items_cb lockedItemsCb = [this, q](QString promptPath, AsyncCallData* data)
    {
        this->doPromptForGet(promptPath, data);
    };

    async_callback_cb unlockedItemsCb = [this](AsyncCallData* data)
    {
        this->getCredentialsFromItem(data->unlocked.at(0), data);
    };

    async_callback_cb credentialsNotFoundCb = [q](AsyncCallData* data)
    {
        emit q->credentialsFound(data->accId, QString(""), QString(""), QString(""), QString(""), false);
    };

    onSearchItems(call, obj, dbusErrorCb, unlockDbusError, lockedItemsCb, unlockedItemsCb, credentialsNotFoundCb);
}

void KeyringPrivate::onSearchItemsForDelete(QDBusPendingCallWatcher* call, QObject* obj)
{
    Q_Q(Keyring);

    async_callback_cb dbusErrorCb = [](AsyncCallData*) {};

    async_callback_cb unlockDbusError = [q](AsyncCallData* data)
    {
        emit q->credentialsError(data->accId);
    };

    locked_items_cb lockedItemsCb = [this, q](QString promptPath, AsyncCallData* data)
    {
        this->doPromptForDelete(promptPath, data);
    };

    async_callback_cb unlockedItemsCb = [this](AsyncCallData* data){this->deleteItemInterface(data);};

    async_callback_cb credentialsNotFoundCb = [q](AsyncCallData* data)
    {
        emit q->credentialsDeleted(data->accId, true);
    };

    onSearchItems(call, obj, dbusErrorCb, unlockDbusError, lockedItemsCb, unlockedItemsCb, credentialsNotFoundCb);
}

void KeyringPrivate::onPromptCompleted(bool dismissed, const QDBusVariant&, QObject* obj, async_callback_cb emptyCb,
    async_callback_cb lockedCb)
{
    Q_Q(Keyring);
    AsyncCallData* data = (AsyncCallData*) obj;

    if (dismissed)
    {
        // Keyring unlocked dismissed, Access was denied.
        qWarning() << "Keyring::User dismissed prompt for account with id" << data->accId;
        emit q->userCancelation(data->accId);
        return;
    }

    // merged locked and unlocked object
    QList<QDBusObjectPath> newUnlocked;
    newUnlocked << data->unlocked << data->locked;

    data->unlocked = newUnlocked;
    data->locked = QList<QDBusObjectPath>();

    if (data->unlocked.length() == 0)
    {
        qDebug() << "Keyring::Unlocked items length is 0.";
        emptyCb(data);
        return;
    }

    lockedCb(data);
}

void KeyringPrivate::onPromptCompletedForGet(bool dismissed, const QDBusVariant &result, QObject* obj)
{

    Q_Q(Keyring);

    async_callback_cb emptyCb = [q](AsyncCallData* data)
    {
        emit q->credentialsFound(data->accId, QString(""), QString(""), QString(""), QString(""), false);
    };

    async_callback_cb lockedCb = [this](AsyncCallData* data)
    {
        this->getCredentialsFromItem(data->unlocked.at(0), data);
    };

    onPromptCompleted(dismissed, result, obj, emptyCb, lockedCb);
}

void KeyringPrivate::onPromptCompletedForDelete(bool dismissed, const QDBusVariant &result, QObject* obj)
{
    Q_Q(Keyring);

    async_callback_cb emptyCb = [q](AsyncCallData* data)
    {
        // if it is empty lets assume we deleted it
        emit q->credentialsDeleted(data->accId, true);
    };

    async_callback_cb lockedCb = [this](AsyncCallData* data){this->deleteItemInterface(data);};

    onPromptCompleted(dismissed, result, obj, emptyCb, lockedCb);
}

void KeyringPrivate::onCollectionUnlocked(QDBusPendingCallWatcher* call, QObject* obj)
{
    Q_Q(Keyring);
    QDBusPendingReply<QList<QDBusObjectPath> , QDBusObjectPath> reply = *call;
    AsyncCallData* data = (AsyncCallData*)obj;

    QString promptPath = reply.argumentAt<1>().path().trimmed();
    if (promptPath != KeyringPrivate::NO_PROMPT_REQUIRED)
    {
        qDebug() << "Keyring::Prompt required for" << promptPath;
        doPromptForCollectionUnlock(promptPath, data);
        return;
    }

    // we are free to open the default collection and insert the required item
    QScopedPointer<CollectionInterface> interface(
    _interfaceFactory->create<CollectionInterface>(SECRET_SERVICE, data->interfacePath, _conn));

    // serialize token and token secret in query string
    QStringList secret;
    secret.prepend(QString(TOKEN_NAME) + "=" + QUrl::toPercentEncoding(Keyring::getTokenName(data->accId).replace(" ", "+"), "+", "@"));
    secret.prepend(QString(TOKEN_KEY) + "=" + data->token);
    secret.prepend(QString(TOKEN_SECRET_KEY) + "=" + data->tokenSecret);
    secret.prepend(QString(CONSUMER_KEY) + "=" + data->consumer);
    secret.prepend(QString(CONSUMER_SECRET_KEY) + "=" + data->consumerSecret);

    secret.sort();

    QString secret_data = secret.join("&");
    Secret secretStruct(QDBusObjectPath(_session->path()),
                        QString("").toUtf8(),
                        secret_data.toUtf8(),
                        QString("application/octet-stream").toUtf8());

    QVariantMap properties;
    properties[ITEM_LABEL_PROPERTY] = QString("%1").arg(data->accId);
    properties[ITEM_ATTRIBUTES_PROPERTY] = dbus::DBusHelper::getVariant(getKeyringAttrs(data->accId));

    QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> async = interface->CreateItem(properties, secretStruct, true);

    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(async);

    SET_WATCH_MAPPING(_onCreateItemMapper, watcher, data)
}

void KeyringPrivate::onReadAliasDone(QDBusPendingCallWatcher* call, QObject* obj)
{
    QDBusPendingReply<QList<QDBusObjectPath>, QList<QDBusObjectPath>> reply = *call;

    // TODO: deal with storing it in a diff collection
}

void KeyringPrivate::onItemDeleted(QDBusPendingCallWatcher* call, QObject* obj)
{
    Q_Q(Keyring);
    AsyncCallData* data = (AsyncCallData*)obj;
    QDBusPendingReply<QDBusObjectPath> reply = *call;

    ASSERT_DBUS_REPLY_IS_ERROR(call, reply, q->credentialsDeleted(data->accId, false))

    emit q->credentialsDeleted(data->accId, true);
}

void KeyringPrivate::onGetSecret(QDBusPendingCallWatcher* call, QObject* obj)
{
    Q_Q(Keyring);
    AsyncCallData* data = (AsyncCallData*)obj;
    QDBusPendingReply<Secret> reply = *call;

    ASSERT_DBUS_REPLY_IS_ERROR(call, reply, q->credentialsError(data->accId))

    Secret secret =  reply.argumentAt<0>();
    QString secretData = secret.getValue();
    QStringList secretPairs = secretData.split("&");

    QHash<QString, QString> result;
    for (int i = 0; i < secretPairs.size(); ++i) {
        QStringList pair = secretPairs.at(i).split("=");
        if (pair.at(0) == KeyringPrivate::TOKEN_NAME)
            // TODO: Need to figure out how to actually use the
            // QUrl::fromPercentEncoding at this point in the code.
            result[pair.at(0)] = QString(pair.at(1)).replace("+", " ").replace("%40", "@");
        else
            result[pair.at(0)] = pair.at(1);
    }

    // assert that the info is present
    if (!result.contains(KeyringPrivate::TOKEN_NAME) ||
        !result.contains(KeyringPrivate::TOKEN_KEY) ||
        !result.contains(KeyringPrivate::TOKEN_SECRET_KEY) ||
        !result.contains(KeyringPrivate::CONSUMER_KEY) ||
        !result.contains(KeyringPrivate::CONSUMER_SECRET_KEY))
    {
        qCritical() << "Keyring::Missing data in stored token entry.";
        emit q->credentialsError(data->accId);
        call->deleteLater();
        return;
    }

    emit q->credentialsFound(data->accId,
                             result[KeyringPrivate::TOKEN_KEY],
                             result[KeyringPrivate::TOKEN_SECRET_KEY],
                             result[KeyringPrivate::CONSUMER_KEY],
                             result[KeyringPrivate::CONSUMER_SECRET_KEY],
                             true);
    qDebug() << "Keyring::Got credentials for account" << data->accId;
    call->deleteLater();
}

void KeyringPrivate::onCreateItem(QDBusPendingCallWatcher* call, QObject* obj)
{
    Q_Q(Keyring);
    AsyncCallData* data = (AsyncCallData*)obj;
    QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> reply = *call;

    ASSERT_DBUS_REPLY_IS_ERROR(call, reply, q->credentialsSet(data->accId, false))

    QDBusObjectPath prompt = reply.argumentAt<1>();
    // test that we do not need a prompt
    if (prompt.path() != KeyringPrivate::NO_PROMPT_REQUIRED)
    {
        // TODO: deal when the collection has to be unlocked
    }
    else
    {
        emit q->credentialsSet(data->accId, true);
        call->deleteLater();
        return;
    }
}

void KeyringPrivate::onUnlock(QDBusPendingCallWatcher* call, QObject* obj)
{
    AsyncCallData* data = (AsyncCallData*)obj;
    QDBusPendingReply<QList<QDBusObjectPath> , QDBusObjectPath> reply = *call;

    ASSERT_DBUS_REPLY_IS_ERROR(call, reply, data->unlockDbusError(data))

    // following the docs, if you get '/' it means that we do not need to perform the prompt
    QList<QDBusObjectPath> unlockedReplayObjects = reply.argumentAt<0>();
    QDBusObjectPath promptObjectPath = reply.argumentAt<1>();
    QString promptPath = promptObjectPath.path().trimmed();

    if (promptPath != KeyringPrivate::NO_PROMPT_REQUIRED)
        data->lockedItemsCb(promptPath, data);
    else
    {
        QList<QDBusObjectPath> newUnlocked;
        newUnlocked << data->unlocked << data->locked;
        data->unlocked = newUnlocked;
        data->locked = QList<QDBusObjectPath>();
        data->unlockedItemsCb(data);
    }
    call->deleteLater();
    return;

}

/**
 * Constructs a Keyring that can be used to access the users keyring via the
 * secrets API which is used to store the tokens used for the different
 * twitter accounts.
 */
Keyring::Keyring(QDBusConnection connection, QObject* parent, QString windowId) :
    QObject(parent),
    d_ptr(new KeyringPrivate(connection, this, windowId))
{
}

Keyring::~Keyring()
{
}

void Keyring::openSession()
{
    Q_D(Keyring);
    d->openSession();
}

void Keyring::getCredentials(QString id)
{
    Q_D(Keyring);
    d->getCredentials(id);
}

void Keyring::setCredentials(QString id, QString token, QString tokenSecret, QString consumer, QString consumerSecret)
{
    Q_D(Keyring);
    d->setCredentials(id, token, tokenSecret, consumer, consumerSecret);
}

void Keyring::deleteCredentials(QString id)
{
    Q_D(Keyring);
    d->deleteCredentials(id);
}

/**
 * \fn QString Keyring::getTokenName(QString id)
 *
 * Compute the token name for the token, from the id and hostname.
 */
QString Keyring::getTokenName(QString id)
{
    QString computer_name = QHostInfo::localHostName();
    computer_name.replace(TOKEN_SEP, TOKEN_SEP_REPLACEMENT);

    return id + TOKEN_SEP + computer_name;
}

} // keyring

#include "moc_keyring.cpp"
