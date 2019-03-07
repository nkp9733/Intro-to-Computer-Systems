f = open('input.txt', 'w')
modulo = 1
l = range(1,100001)
for i in l:
	if i%modulo == 0:
		f.write(str(i)+"\n")
