Some particle generators I made inspired by http://vimeo.com/36278748

* basic.c: constantly adds new square particles at the origin
* circles.c: click and drag to make circular particles
* faster_circles.c: an experiment to speed up circles.c
    * integrates moving and drawing logic in the same loop so particles aren't looped over twice each game loop
    * uses a circularly linked list that reuses nodes and should cut down on needed to do list resizes
    
Bugs

* Both circles.c and faster_circles.c: newly created particles don't align with the cursor
* faster_circles.c has some (less than before!) list processing errors. There is a hidden double free error I am looking for.
