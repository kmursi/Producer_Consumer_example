#-------------------------------------

SOURCE=pc.cpp
MYPROGRAM=pc
cc=g++
#-----------------------------------

all:$(MYPROGRAM)

$(MYPROGRAM) : $(SOURCE)
 
	$(cc) -w $(SOURCE) -lpthread -o $(MYPROGRAM) 

clean:

	rm -f $(MYPROGRAM)
