;; Enrico Scala (enricos83@gmail.com) and Miquel Ramirez (miquel.ramirez@gmail.com)
(define (problem instance_20_5_2_1)
  (:domain mt-block-grouping)
  (:objects
    b1 b2 b3 b4 b5 - block
  )

  (:init
    (= (x b3) 17)
	(= (y b3) 14)
	(= (x b4) 17)
	(= (y b4) 8)
	(= (x b2) 19)
	(= (y b2) 5)
	(= (x b1) 20)
	(= (y b1) 14)
	(= (x b5) 5)
	(= (y b5) 5)
	(= (max_x) 20 )
	(= (min_x) 1 )
	(= (max_y) 20 )
	(= (min_y) 1 )
  )

  (:goal (and 
    (= (x b1) 19)
    (= (y b1) 13)
  ))

  
  

  
)
