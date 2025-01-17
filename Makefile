# Makefile for the Bank Management System

# Variables
CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -Werror -pedantic -pthread
CXXLINK = $(CXX)
RM = rm -f

# Object files and the final executable
OBJS = main.o account.o atm.o bank.o logger.o procedure_handler.o error_handler.o vip_thread_pool.o vip_function.o thread_data.o
EXEC = bank

# Functions and Dependencies
$(EXEC): $(OBJS)
	$(CXXLINK) -pthread $(OBJS) -o $(EXEC)

main.o: main.cpp account.hpp atm.hpp bank.hpp logger.hpp procedure_handler.hpp 
	$(CXX) $(CXXFLAGS) -c -o main.o main.cpp

atm.o: atm.cpp atm.hpp logger.o bank.o procedure_handler.o
	$(CXX) $(CXXFLAGS) -c -o atm.o atm.cpp

bank.o: bank.cpp bank.hpp account.o atm.hpp logger.o procedure_handler.o vip_thread_pool.o thread_data.o
	$(CXX) $(CXXFLAGS) -c -o bank.o bank.cpp

logger.o: logger.cpp logger.hpp
	$(CXX) $(CXXFLAGS) -c -o logger.o logger.cpp

procedure_handler.o: procedure_handler.cpp procedure_handler.hpp error_handler.o
	$(CXX) $(CXXFLAGS) -c -o procedure_handler.o procedure_handler.cpp

error_handler.o: error_handler.cpp error_handler.hpp
	$(CXX) $(CXXFLAGS) -c -o error_handler.o error_handler.cpp

account.o: account.cpp account.hpp
	$(CXX) $(CXXFLAGS) -c -o account.o account.cpp

vip_thread_pool.o: vip_thread_pool.cpp vip_thread_pool.hpp vip_function.o
	$(CXX) $(CXXFLAGS) -c -o vip_thread_pool.o vip_thread_pool.cpp

vip_function.o: vip_function.cpp vip_function.hpp
	$(CXX) $(CXXFLAGS) -c -o vip_function.o vip_function.cpp

thread_data.o: thread_data.cpp thread_data.hpp 
	$(CXX) $(CXXFLAGS) -c -o thread_data.o thread_data.cpp

#status_snapshot.o: status_snapshot.cpp status_snapshot.hpp bank.hpp account.hpp
#$(CXX) $(CXXFLAGS) -c -o status_snapshot.o status_snapshot.cpp
# Clean up old files
clean:
	$(RM) $(EXEC) $(OBJS) *~ core.* 
