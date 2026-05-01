;;Setting seed to 1229
;; Enrico Scala (enricos83@gmail.com) and Miquel Ramirez (miquel.ramirez@gmail.com)
(define (problem instance_2_600_1229_ladder)
	(:domain farmland)
	(:objects
		farm0 farm1  - farm
	)
  (:init
		(= (x farm0) 600)
		(= (x farm1) 1)
		
		(adj farm0 farm1)
		(adj farm1 farm0)
		
		(= (cost) 0)
	)
	(:goal
		(and
			(>= (x farm1) 4)
		)
	)
)
