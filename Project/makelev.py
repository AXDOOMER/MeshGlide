#!/usr/bin/python3

#./makelev.py > my.txt && rm my.txt.lnb && ../run.sh -level my.txt

print("#THING	TYPE	X	Y	Z 	ANGLE")
print("thing	spawn	1	1	2	0")
print("#POLY	INVISIBLE	BLC	DBS	ScX	ScY	AlX	AlY	Lit	X	Y	Z	X	Y	Z	X	Y	Z	X	Y	Z")

s = 5;

# First floor
for i in range(10):
	for j in range(10):
		if ( i == 1 and j == 2):
			print("poly	concr.jpg	1	0	1	1	0	0	1	" + str(i*s) + "\t" + str(j*s) + "\t0\t" + str(i*s+s) +'\t'+ str(j*s) + "\t0\t" + str(i*s+s) + '\t' + str(j*s+s) + "\t0\t" + str(i*s) + "\t" + str(j*s+s) + "\t0\t")
		else:
			print("poly	concr.jpg	0	0	1	1	0	0	1	" + str(i*s) + "\t" + str(j*s) + "\t0\t" + str(i*s+s) +'\t'+ str(j*s) + "\t0\t" + str(i*s+s) + '\t' + str(j*s+s) + "\t0\t" + str(i*s) + "\t" + str(j*s+s) + "\t0\t")


# Second floor
k = str(5);

for i in range(10):
	for j in range(10):
		if( not ( (j == 2 or j == 3) and i == 1) and not (j >= 8 and i >= 8)):
			print("poly	concr.jpg	0	1	1	1	0	0	1	" + str(i*s) + "\t" + str(j*s) + "\t"+k+"\t" + str(i*s+s) +'\t'+ str(j*s) + "\t"+k+"\t" + str(i*s+s) + '\t' + str(j*s+s) + "\t"+k+"\t" + str(i*s) + "\t" + str(j*s+s) + "\t"+k+"\t")


# Fall connectors
print("#POLY	INVISIBLE	BLC	DBS	ScX	ScY	AlX	AlY	Lit	X	Y	Z	X	Y	Z	X	Y	Z	X	Y	Z")
print("poly		INVISIBLE	0	1	1	1	0	0	1	50	40	5	50	40	0	45	40	0	45	40	5")
print("poly		INVISIBLE	0	1	1	1	0	0	1	45	40	5	45	40	0	40	40	0	40	40	5")

print("poly		INVISIBLE	0	1	1	1	0	0	1	40	50	5	40	50	0	40	45	0	40	45	5")
print("poly		INVISIBLE	0	1	1	1	0	0	1	40	45	5	40	45	0	40	40	0	40	40	5")


# Slope
print("#POLY	INVISIBLE	BLC	DBS	ScX	ScY	AlX	AlY	Lit	X	Y	Z	X	Y	Z	X	Y	Z	X	Y	Z")
print("poly		concr.jpg	0	1	1	2	0	0	1	5	10	0	10	10	0	10	20	5	5	20	5")


