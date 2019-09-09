//
// Created by Sitan Chen on 2019/9/1.
//

#ifndef ADVANCEDGL_IRENDERABLE_H
#define ADVANCEDGL_IRENDERABLE_H


class IRenderable
{
private:
    const int m_queueID;

public:
    IRenderable( int queueID = 60)
            :m_queueID(queueID){};
    virtual ~IRenderable(){};

    virtual void attach(){}
    virtual void detach(){}
    virtual void render(){};

    int getQueueID() { return m_queueID; }
};

class DummyRenderable : public IRenderable
{
public:
    DummyRenderable( int queueID )
    :IRenderable(queueID){};
};


template <class T>
class DeletegateRendeable : public  IRenderable
{
private:
    T&  m_client;

public:
    DeletegateRendeable(T& client)
    :m_client( client ) {}

    void render() override
    {
        m_client.render();
    }
};


#endif //ADVANCEDGL_IRENDERABLE_H
