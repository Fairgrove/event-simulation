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

device chooses a server based on connection to that server   
get a random connection strength to each server and send events to that. this happens when the device is generated  
if server event handler queue is full then send the event to another server (with a risk of losing the event)   
event redirection  
 - does it know the device connection strength  
 - does it say to the device to try another server (will slow down traffic a lot because there will be a lot of back and forth)   
 - does it send the event to a random server without letting the device know (potentially sends the event to a server with really bad connection)   

increases events per second by increasing amount of 'devices'   


metrics 
```
=====================
events
       number of devices: 7
       events per second: xx
                    loss: xx
        redirection loss: xx

server load
                 servers: 12
      full server queues: xx
            total strain: xx%
=====================
```


##
the simulate work function   
$$\sum_{i=1}^{n} x+1$$
