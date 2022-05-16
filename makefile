
tweets: tweets_generator.c linked_list.c markov_chain.c
	gcc -Wall -Wextra -Wvla -std=c99 $^ -o tweets_generator

snake: snakes_and_ladders.c linked_list.c markov_chain.c
	gcc -Wall -Wextra -Wvla -std=c99 $^ -o snakes_and_ladders
