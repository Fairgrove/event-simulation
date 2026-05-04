# event-simulation
Side project for practicing c++ :)

# some notes
- create processor class with event queue
- create a blocker when queue is full
- change the events to be a stream of events with events per second
- show '''''''real time ''''''' metrics
- define real time


## event stream
- device struct that uses Event generator class
- has attribute that determines variance of time between events

increases events per second by increasing amount of 'devices'


metrics 

```
=====================
events
       number of devices: 7
       events per second: xx
                    loss: xx

server load
                 servers: 12
      full server queues: xx
            total strain: xx%
=====================
```


##
the simulate work function   
$$\sum_{i=1}^{n} x+1$$
