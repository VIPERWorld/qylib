#ifndef QY_ASYNCSOCKET_H__
#define QY_ASYNCSOCKET_H__

#include "qysigslot.h"
#include "qysocket.h"

namespace qy 
{
	// Provides the ability to perform socket I/O asynchronously.

    class QyAsyncSocket : public QySocket , public sigslot::has_slots<>
	{
	public:
        virtual ~QyAsyncSocket() {}

        sigslot::signal1<QyAsyncSocket*> sigReadEvent;  // ready to read
        sigslot::signal1<QyAsyncSocket*> sigWriteEvent; // ready to write
        sigslot::signal1<QyAsyncSocket*> sigConnectEvent; // connected
        sigslot::signal2<QyAsyncSocket*,int> sigCloseEvent; // closed
	};

    class QyAsyncSocketAdapter : public QyAsyncSocket
    {
    public:
        QyAsyncSocketAdapter(QySocket * socket)
            : socket_(socket)
        {
        }
        QyAsyncSocketAdapter(QyAsyncSocket * socket) : socket_(socket)
        {
            socket->sigConnectEvent.connect(this, &QyAsyncSocketAdapter::onConnectEvent);
            socket->sigReadEvent.connect(this, &QyAsyncSocketAdapter::onReadEvent);
            socket->sigWriteEvent.connect(this, &QyAsyncSocketAdapter::onWriteEvent);
            socket->sigCloseEvent.connect(this, &QyAsyncSocketAdapter::onCloseEvent);
        }
        virtual ~QyAsyncSocketAdapter() { delete socket_; }

        virtual QySocketAddress localAddress() const { return socket_->localAddress(); }
        virtual QySocketAddress remoteAddress() const { return socket_->remoteAddress(); }

        virtual bool open(int type) { return socket_->open(type); }
        virtual int  bind(const QySocketAddress& addr) { return socket_->bind(addr); }
        virtual int  connect(const QySocketAddress& addr) {return socket_->connect(addr); }
        virtual int  send(const void *pv, size_t cb) { return socket_->send(pv, cb); }
        virtual int  sendTo(const void *pv, size_t cb, const QySocketAddress& addr) { return socket_->sendTo(pv, cb, addr); }
        virtual int  recv(void *pv, size_t cb) { return socket_->recv(pv, cb); }
        virtual int  recvFrom(void *pv, size_t cb, QySocketAddress *paddr) { return socket_->recvFrom(pv, cb, paddr); }
        virtual int  listen(int backlog) { return socket_->listen(backlog); }
        virtual int  close() { return socket_->close(); }
        virtual int  error() const { return socket_->error(); }
        virtual void setError(int error) { return socket_->setError(error); }
        virtual ConnState state() const { return socket_->state(); }
        virtual int estimateMTU(uint16* mtu) { return socket_->estimateMTU(mtu); }
        virtual int setOption(Option opt, int value) { return socket_->setOption(opt, value); }

    protected:
        virtual void onConnectEvent(QyAsyncSocket *) { sigConnectEvent(this); }
        virtual void onReadEvent(QyAsyncSocket * ) { sigReadEvent(this); }
        virtual void onWriteEvent(QyAsyncSocket * ) { sigWriteEvent(this); }
        virtual void onCloseEvent(QyAsyncSocket * , int err) { sigCloseEvent(this, err); }

        QySocket * socket_;
    };

} // namespace qy

#endif // QY_ASYNCSOCKET_H__
