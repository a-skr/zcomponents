// copyright Alexandre Skrzyniarz 2011-2012, All Rights Reserved.

#include <pthread.h>
#include <oc2/ZReactor.hpp>
#include <unistd.h>

using namespace oc2;

void * thr_main (void * data)
{
  ZReactor * reactor = (ZReactor *)data;
  reactor->run_event_loop();
  return NULL;
}

class SubHandler : public ZCallback
{
public:
  SubHandler() : count(0) {};
  virtual ~SubHandler() {}
  virtual void execute (zmq::pollitem_t * item, zmq::socket_t * sock)
  {
    zmq::message_t msg;
    sock->recv (&msg);
    count++;
  };
  virtual void release(int, zmq::socket_t *) {};
  int count;
};

int main (int, char **)
{
  // init
  zmq::context_t context (1);
  SubHandler handler;
  ZReactor reactor ("inproc://reactor-0", &context);

  zmq::socket_t sub_sock (context, ZMQ_SUB);
  sub_sock.setsockopt (ZMQ_SUBSCRIBE, "", 0);
  sub_sock.bind ("inproc://subscriber-0");

  zmq::socket_t pub_sock (context, ZMQ_PUB);
  pub_sock.connect ("inproc://subscriber-0");
  
  pthread_t reactor_thread;
  int rc = pthread_create (&reactor_thread, NULL, thr_main, (void*)(&reactor));
  assert (rc == 0);
  
  // register
  reactor.register_events (&sub_sock, &handler);
  
  // stimulate
  zmq::message_t msg;
  pub_sock.send(msg);

  // check
  usleep (5000);
  assert ( handler.count == 1 );
 
  // stimulate
  pub_sock.send(msg);

  // check
  usleep (5000);
  assert ( handler.count == 2);

  // unregister
  reactor.unregister_events (& sub_sock);

  // stimulate
  pub_sock.send(msg);

  // check
  usleep (5000);
  assert ( handler.count == 2);
  
  // halt
  reactor.halt ();
  pthread_join (reactor_thread, NULL);
  
  return 0;
}
