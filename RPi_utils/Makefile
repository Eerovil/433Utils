
# Defines the RPI variable which is needed by rc-switch/RCSwitch.h
CXXFLAGS=-DRPI

all: send codesend RFRemote

send: ../rc-switch/RCSwitch.o send.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -pthread $+ -o $@ -lwiringPi
	
codesend: ../rc-switch/RCSwitch.o codesend.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -pthread $+ -o $@ -lwiringPi
	
RFSniffer: ../rc-switch/RCSwitch.o RFSniffer.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -pthread $+ -o $@ -lwiringPi

RFRemote: ../rc-switch/RCSwitch.o RFRemote.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -pthread $+ -o $@ -lwiringPi -lpython3.4m -lcurl

clean:
	$(RM) ../rc-switch/*.o *.o send codesend servo RFSniffer

