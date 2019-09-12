//
// Created by Sitan Chen on 2019/9/1.
//

#ifndef ADVANCEDGL_IRENDERABLE_H
#define ADVANCEDGL_IRENDERABLE_H


class IRenderable
{
private:
    const int m_queueID;
    bool  m_enable = true;

public:
    IRenderable( int queueID = 60)
            :m_queueID(queueID){};
    virtual ~IRenderable(){};

    virtual void attach(){}
    virtual void detach(){}
    virtual void render(){};

    int getQueueID() { return m_queueID; }
    bool  isEnable() { return m_enable; }
    bool  setEnabel( bool enable ) { m_enable = enable; }
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
