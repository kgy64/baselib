#ifndef __MESSAGE_H__
#define __MESSAGE_H__

template <typename T> class MessageBox;

namespace _Message
{
    template <typename T>
    class MessageHandler
    {
     friend class MessageBox<T>;

     public:
        MessageHandler(void)
        {
        }

     private:
        static MessageBox<T> * msgbox_list; ///< The first messagebox for this type

        static void register_msgbox(MessageBox<T> & box)
        {
            box.next = msgbox_list;
            msgbox_list = &box;
        }

        static void send_message(T & msg)
        {
            for (MessageBox<T> * i = msgbox_list; i; i=i->next) {
                i->got_message(msg);
            }
        }
    };

    template <typename T>
    void _send_message(T & msg)
    {
        MessageHandler<T>::send_message(msg);
    }
}

template <typename T>
class MessageBox
{
 friend class _Message::MessageHandler<T>;

 public:
    MessageBox(void)
    {
        _Message::MessageHandler<T>::register_msgbox(*this);
    }

 private:
    MessageBox<T> * next;

    virtual void got_message(T & msg) = 0;
};

#define DEFINE_MESSAGE_CLASS(M) \
    namespace { \
        MessageBox<M> * _Message::MessageHandler<M>::msgbox_list = (MessageBox<M>*)0; \
    }

#define SEND_MESSAGE(M) \
    _Message::_send_message(M)

#endif /* __MESSAGE_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
