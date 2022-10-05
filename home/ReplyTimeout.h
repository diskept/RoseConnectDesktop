#ifndef REPLYTIMEOUT_H
#define REPLYTIMEOUT_H

#include <QNetworkReply>
#include <QBasicTimer>
#include <QTimerEvent>

/*
class ReplyTimeout : public QObject {
  Q_OBJECT
  QBasicTimer m_timer;
public:
  ReplyTimeout(QNetworkReply* reply, const int timeout) : QObject(reply) {
    Q_ASSERT(reply);
    if (reply && reply->isRunning())
      m_timer.start(timeout, this);
  }
  static void set(QNetworkReply* reply, const int timeout) {
    new ReplyTimeout(reply, timeout);
  }
protected:
  void timerEvent(QTimerEvent * ev) {
    if (!m_timer.isActive() || ev->timerId() != m_timer.timerId())
      return;
    auto reply = static_cast<QNetworkReply*>(parent());
    if (reply->isRunning())
      reply->close();
    m_timer.stop();
  }
};
*/

class ReplyTimeout : public QObject {
    Q_OBJECT
public:
    enum HandleMethod { Abort, Close };
    ReplyTimeout(QNetworkReply* reply, const int timeout, HandleMethod method = Abort) :
        QObject(reply), m_method(method)
    {
        Q_ASSERT(reply);
        if (reply && reply->isRunning()) {
            m_timer.start(timeout, this);
            connect(reply, &QNetworkReply::finished, this, &QObject::deleteLater);
        }
    }
    static void set(QNetworkReply* reply, const int timeout, HandleMethod method = Abort)
    {
        new ReplyTimeout(reply, timeout, method);
    }
protected:
    QBasicTimer m_timer;
    HandleMethod m_method;
    void timerEvent(QTimerEvent * ev) {
        if (!m_timer.isActive() || ev->timerId() != m_timer.timerId())
            return;
        auto reply = static_cast<QNetworkReply*>(parent());
        if (reply->isRunning())
        {
            if (m_method == Close)
                reply->close();
            else if (m_method == Abort)
                reply->abort();
            m_timer.stop();
        }
    }
};
#endif // REPLYTIMEOUT_H
