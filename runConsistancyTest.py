import os

commad_string = "./MultiConsistancyTest"

for i in range(1):
    commad_string+=" & ./MultiConsistancyTest"

os.system(commad_string)