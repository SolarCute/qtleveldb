#ifndef QLEVELDBBATCH_H
#define QLEVELDBBATCH_H


#include <QObject>
#include <QQmlParserStatus>
#include "qleveldbglobal.h"
#include "qleveldb.h"
#include <../3rdparty/leveldb/include/leveldb/write_batch.h>
#include <../3rdparty/leveldb/include/leveldb/db.h>

QT_BEGIN_NAMESPACE

class Q_LEVELDB_EXPORT QLevelDBBatch : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
public:
    explicit QLevelDBBatch(QWeakPointer<leveldb::DB> db, QObject *parent = 0);

    Q_INVOKABLE QLevelDBBatch* del(QString key);
    Q_INVOKABLE QLevelDBBatch* put(QString key, QVariant value);
    Q_INVOKABLE QLevelDBBatch* clear();
    //TODO: change it to LevelDB::Status
    Q_INVOKABLE bool write();
signals:
    void batchWritten(QSet<QString> keys);
protected:
    void classBegin();
    void componentComplete();

private:
    Q_DISABLE_COPY(QLevelDBBatch)
    QSharedPointer<leveldb::DB> m_levelDB;
    leveldb::WriteBatch m_writeBatch;
    QSet<QString> m_operations;
};

QT_END_NAMESPACE
#endif // QLEVELDBBATCH_H
