FIFO: a thread que chegou primeiro roda até terminar. Se a lenta entrou antes, ela trava o processador por bastante tempo e só depois a rápida roda

Round Robin: o processador divide o tempo em fatias e alterna entre as threads. Assim mesmo que a lenta tenha entrado antes, a rápida logo ganha sua vez e termina mais cedo